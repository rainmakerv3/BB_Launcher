// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <fstream>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QProgressBar>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <nlohmann/json.hpp>
#include <qmicroz.h>
#include <sys/stat.h>

#include "Common.h"
#include "settings/formatting.h"
#include "ui_version_dialog.h"
#include "version_dialog.h"

VersionDialog::VersionDialog(QWidget* parent) : QDialog(parent), ui(new Ui::VersionDialog) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    ui->FolderLabel->setText(QString::fromStdString(Config::DefaultFolderString));
    ui->checkOnStartupCheckBox->setChecked(Config::AutoUpdateShadEnabled);
    ui->showChangelogCheckBox->setChecked(Config::ShowChangeLog);
    ui->versionListUpdateCheckBox->setChecked(Config::AutoUpdateVersionsEnabled);

    connect(ui->versionListUpdateCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        Config::AutoUpdateVersionsEnabled = checked;
        Config::SaveLauncherSettings();
    });

    connect(ui->checkOnStartupCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        Config::AutoUpdateShadEnabled = checked;
        Config::SaveLauncherSettings();
    });

    connect(ui->showChangelogCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        Config::ShowChangeLog = checked;
        Config::SaveLauncherSettings();
    });

    networkManager = new QNetworkAccessManager(this);

    GetBuildInfo();
    LoadInstalledList();

    QStringList cachedVersions = LoadDownloadCache();
    if (!cachedVersions.isEmpty()) {
        PopulateDownloadTree(cachedVersions);
    } else {
        CheckVersionsList(false);
    }

    connect(ui->addLocalVersionButton, &QPushButton::clicked, this, [this]() {
        QString defaultPath;
        if (QDir(ui->FolderLabel->text()).exists()) {
            defaultPath = ui->FolderLabel->text();
        } else {
            defaultPath = QDir::currentPath();
        }

        QString exePath;
#ifdef _WIN32
        exePath = QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadPS4.exe)",
                                               defaultPath, "Executables (*.exe)");
#elif defined(__linux__)

    exePath = QFileDialog::getOpenFileName(
        this,
        "Select ShadPS4 executable (ex. /usr/bin/shadps4, "
        "Shadps4-qt.AppImage, etc.)",
        QDir::homePath(),
        "AppImages (*.AppImage);;non-AppImage (shadps4*)",
        0, QFileDialog::DontUseNativeDialog);

#elif defined(__APPLE__)
    exePath = QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadps4*)",
                                           QDir::homePath(), "Shadps4 App (shadps4*)");
#endif

        if (exePath.isEmpty()) {
            return;
        }

        bool ok;
        QString version_name = QInputDialog::getText(
            this, "Version name", tr("Enter a name for this build."), QLineEdit::Normal, "", &ok);
        if (!ok || version_name.trimmed().isEmpty())
            return;
        version_name = version_name.trimmed();

        std::filesystem::path path = Common::PathFromQString(exePath);
        Config::Build build;
        build.path = std::string{fmt::UTF(path.u8string()).data};
        build.type = "Local";
        build.id = version_name.toStdString();
        build.modified = Config::GetLastModifiedString(path);
        buildInfo.push_back(build);

        LoadInstalledList();
        SaveBuilds();

        QMessageBox::information(this, tr("Success"), tr("Local build added successfully."));
    });

    connect(ui->removeVersionButton, &QPushButton::clicked, this, [this]() { RemoveItem(false); });

    connect(ui->deleteVersionButton, &QPushButton::clicked, this, [this]() { RemoveItem(true); });

    connect(ui->FolderSelectButton, &QPushButton::clicked, this, [this]() {
        QString DefaultFolder = QFileDialog::getExistingDirectory(
            this, "Set Default Path for Version Installation", QDir::currentPath());
        if (DefaultFolder != "") {
            ui->FolderLabel->setText(DefaultFolder);
            Config::DefaultFolderString = DefaultFolder.toStdString();
            Config::SaveLauncherSettings();
        }
    });

    connect(ui->loadJsonButton, &QPushButton::clicked, this, [this]() { loadJson(); });

    connect(ui->downloadVersionButton, &QPushButton::clicked, this,
            [this]() { InstallSelectedVersion(); });

    connect(ui->checkVersionDownloadButton, &QPushButton::clicked, this,
            [this]() { CheckVersionsList(true); });

    connect(ui->installedTreeWidget, &QTreeWidget::itemChanged, this,
            &VersionDialog::onItemChanged);

    connect(ui->updateButton, &QPushButton::clicked, this, [this]() { checkUpdatePre(true); });
};

VersionDialog::~VersionDialog() {
    delete ui;
}

void VersionDialog::onItemChanged(QTreeWidgetItem* item, int column) {
    if (column == 0) {
        if (item->checkState(0) == Qt::Checked) {
            QString fullPath = item->text(3);
            Common::shadPs4Executable = Common::PathFromQString(fullPath);
            Config::SaveLauncherSettings();

            for (int row = 0; row < ui->installedTreeWidget->topLevelItemCount(); ++row) {
                QTreeWidgetItem* topItem = ui->installedTreeWidget->topLevelItem(row);
                if (topItem != item) {
                    topItem->setCheckState(0, Qt::Unchecked);
                    topItem->setSelected(false);
                }
            }
            item->setSelected(true);
        } else {
            item->setSelected(false);
        }
    }
}

void VersionDialog::loadJson() {
    QString jsonPath =
        QFileDialog::getOpenFileName(this, "Select ShadPS4 versions.json file", QDir::homePath(),
                                     "shadPS4 versions json file (versions.json)");

    if (jsonPath.isEmpty())
        return;

    nlohmann::json data;
    std::ifstream ifs(Common::PathFromQString(jsonPath));

    try {
        ifs >> data;
    } catch (const std::exception& ex) {
        fmt::print(stderr, "VersionManager: Failed to parse JSON: {}\n", ex.what());
        return;
    }

    if (!data.is_array()) {
        fmt::print(stderr, "VersionManager: Invalid JSON format (expected array)\n");
        return;
    }

    bool newBuildFound = false;
    for (const auto& entry : data) {
        if (!entry.is_object()) {
            fmt::print(stderr, "VersionManager: Skipping invalid entry (not an object)\n");
            continue;
        }

        bool alreadyExists = false;
        bool conflictingPrerelease = false;
        std::string name = entry.value("name", std::string("no name"));
        std::string codename = entry.value("codename", std::string("no tag found"));
        std::filesystem::path buildPath = entry.value("path", std::string(""));
        const int size = buildInfo.size();

        std::string type;
        if (entry.value("type", -1) == 0) {
            type = "Release";
        } else if (entry.value("type", -1) == 1) {
            type = "Pre-release";
        } else if (entry.value("type", -1) == 2) {
            type = "Local";
        }

        if (type == "Pre-release") {
            for (auto build : buildInfo) {
                if (build.type == "Pre-release") {
                    QMessageBox::information(
                        this, "Pre-release already exists.",
                        "The build " + QString::fromStdString(buildPath.string()) +
                            " is a pre-release, but a pre-release already exists. BB_Launcher can "
                            "only set one pre-release for auto-updates to function. Other builds "
                            "can be set as local builds instead.");
                    conflictingPrerelease = true;
                    break;
                }
            }
        }

        if (!buildInfo.empty()) {
            for (auto build : buildInfo) {
                if (build.path == buildPath) {
                    QMessageBox::information(this, "Build already added.",
                                             "The build " +
                                                 QString::fromStdString(buildPath.string()) +
                                                 " is already loaded. It will not be added.");
                    alreadyExists = true;
                    break;
                }
            }
        }

        if (alreadyExists || conflictingPrerelease) {
            continue;
        } else {
            newBuildFound = true;
        }

        std::string modifiedString = "unknown";
        if (std::filesystem::exists(buildPath)) {
            modifiedString = Config::GetLastModifiedString(buildPath);
        }

        std::string id;
        if (type == "Release" || type == "Local") {
            id = name;
        } else if (type == "Pre-release") {
            std::string folderName = buildPath.parent_path().string();
            id = codename;
        }

        if (std::filesystem::exists(buildPath)) {
            Config::Build b{
                .path = std::string{fmt::UTF(buildPath.u8string()).data},
                .type = type,
                .id = id,
                .modified = modifiedString,
            };
            buildInfo.push_back(b);
        }
    }

    if (newBuildFound) {
        LoadInstalledList();
        SaveBuilds();
    }
}

void VersionDialog::CheckVersionsList(const bool showMessage) {
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QUrl url("https://api.github.com/repos/shadps4-emu/shadPS4/tags");
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, showMessage]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            if (doc.isArray()) {
                QJsonArray tags = doc.array();
                ui->downloadTreeWidget->clear();

                QTreeWidgetItem* preReleaseItem = nullptr;
                QList<QTreeWidgetItem*> otherItems;
                bool foundPreRelease = false;

                //  > v.0.5.0
                auto isVersionGreaterThan_0_5_0 = [](const QString& tagName) -> bool {
                    QRegularExpression versionRegex(R"(v\.?(\d+)\.(\d+)\.(\d+))");
                    QRegularExpressionMatch match = versionRegex.match(tagName);
                    if (match.hasMatch()) {
                        int major = match.captured(1).toInt();
                        int minor = match.captured(2).toInt();
                        int patch = match.captured(3).toInt();

                        if (major > 0)
                            return true;
                        if (major == 0 && minor >= 5)
                            return true;
                        if (major == 0 && minor == 5 && patch > 0)
                            return true;
                    }
                    return false;
                };

                for (const QJsonValue& value : tags) {
                    QJsonObject tagObj = value.toObject();
                    QString tagName = tagObj["name"].toString();

                    if (tagName.startsWith("Pre-release", Qt::CaseInsensitive)) {
                        if (!foundPreRelease) {
                            preReleaseItem = new QTreeWidgetItem();
                            preReleaseItem->setText(0, "Pre-release");
                            foundPreRelease = true;
                        }
                        continue;
                    }
                    if (!isVersionGreaterThan_0_5_0(tagName)) {
                        continue;
                    }

                    QTreeWidgetItem* item = new QTreeWidgetItem();
                    item->setText(0, tagName);
                    otherItems.append(item);
                }

                // If you didn't find Pre-release, add it manually
                if (!foundPreRelease) {
                    preReleaseItem = new QTreeWidgetItem();
                    preReleaseItem->setText(0, "Pre-release");
                }

                // Add Pre-release first
                if (preReleaseItem) {
                    ui->downloadTreeWidget->addTopLevelItem(preReleaseItem);
                }

                // Add the others
                for (QTreeWidgetItem* item : otherItems) {
                    ui->downloadTreeWidget->addTopLevelItem(item);
                }

                // Assemble the current list
                QStringList versionList;
                for (int i = 0; i < ui->downloadTreeWidget->topLevelItemCount(); ++i) {
                    QTreeWidgetItem* item = ui->downloadTreeWidget->topLevelItem(i);
                    if (item)
                        versionList.append(item->text(0));
                }

                QStringList cachedVersions = LoadDownloadCache();
                if (cachedVersions == versionList) {
                    if (showMessage) {
                        QMessageBox::information(
                            this, tr("Version list update"),
                            tr("No news, the downloadable version list is already updated."));
                    }
                } else {
                    SaveDownloadCache(versionList);
                    QMessageBox::information(
                        this, tr("Version list update"),
                        tr("New shadPS4 versions added to the downloadable versions list."));
                }
            }
        } else {
            QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                 tr("Error accessing GitHub API") +
                                     QString(":\n%1").arg(reply->errorString()));
        }
        reply->deleteLater();
    });
}

void VersionDialog::InstallSelectedVersion() {
    if (ui->downloadTreeWidget->selectedItems().empty()) {
        QMessageBox::information(this, "Error", "Select a version first");
        return;
    }

    QTreeWidgetItem* item = ui->downloadTreeWidget->selectedItems().first();
    QString versionName = item->text(0);
    QString apiUrl;
    if (versionName == "Pre-release") {
        apiUrl = "https://api.github.com/repos/shadps4-emu/shadPS4/releases";
        for (auto build : buildInfo) {
            if (build.type == "Pre-release") {
                QMessageBox::information(
                    this, "Pre-release already downloaded.",
                    "A pre-release version has already been downloaded. Only one designated "
                    "pre-release can be present for auto-updater to function correctly. Other "
                    "builds can be downloaded manually and added as a local build.");
                return;
            }
        }
    } else {
        apiUrl = QString("https://api.github.com/repos/shadps4-emu/"
                         "shadPS4/releases/tags/%1")
                     .arg(versionName);
        for (auto build : buildInfo) {
            if (build.id == versionName.toStdString()) {
                QMessageBox::information(this, "Error",
                                         "This version has already been downloaded.");
                return;
            }
        }
    }

    QString defaultPath;
    if (QDir(ui->FolderLabel->text()).exists()) {
        defaultPath = ui->FolderLabel->text();
    } else {
        defaultPath = QDir::currentPath();
    }

    QString downloadFolder =
        QFileDialog::getExistingDirectory(this, "Select Download Folder", defaultPath);
    if (downloadFolder.isEmpty()) {
        QMessageBox::information(this, "Error", "Download folder must be selected");
        return;
    }

    { // Message yes/no
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Confirm Download"),
                                      tr("Do you want to download the version") +
                                          QString(": %1 ?").arg(versionName),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(apiUrl);
    QNetworkReply* reply = manager->get(request);
    QString platform;
#ifdef Q_OS_WIN
    platform = "win64-sdl";
#elif defined(Q_OS_LINUX)
    platform = "linux-sdl";
#elif defined(Q_OS_MAC)
    platform = "macos-sdl";
#endif

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, platform, versionName, downloadFolder]() {
                if (reply->error() != QNetworkReply::NoError) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"), reply->errorString());
                    reply->deleteLater();
                    return;
                }
                QByteArray response = reply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(response);

                QJsonArray assets;
                QJsonObject release;

                if (versionName == "Pre-release") {
                    QJsonArray releases = doc.array();
                    for (const QJsonValue& val : releases) {
                        QJsonObject obj = val.toObject();
                        if (obj["prerelease"].toBool()) {
                            release = obj;
                            assets = obj["assets"].toArray();
                            break;
                        }
                    }
                } else {
                    release = doc.object();
                    assets = release["assets"].toArray();
                }

                QString downloadUrl;
                for (const QJsonValue& val : assets) {
                    QJsonObject obj = val.toObject();
                    QString name = obj["name"].toString();
                    if (name.contains(platform)) {
                        downloadUrl = obj["browser_download_url"].toString();
                        break;
                    }
                }
                if (downloadUrl.isEmpty()) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                         tr("No files available for this platform."));
                    reply->deleteLater();
                    return;
                }

                QString fileName = "temp_download_update.zip";
                QString zipPath = downloadFolder + "/" + fileName;

                QNetworkAccessManager* downloadManager = new QNetworkAccessManager(this);
                QNetworkRequest downloadRequest(downloadUrl);
                QNetworkReply* downloadReply = downloadManager->get(downloadRequest);

                QDialog* progressDialog = new QDialog(this);
                progressDialog->setWindowTitle(
                    tr("Downloading %1 , please wait...").arg(versionName));
                progressDialog->setFixedSize(400, 80);
                progressDialog->setWindowFlags(progressDialog->windowFlags() &
                                               ~Qt::WindowCloseButtonHint);
                QVBoxLayout* layout = new QVBoxLayout(progressDialog);
                QProgressBar* progressBar = new QProgressBar(progressDialog);
                progressBar->setRange(0, 100);
                layout->addWidget(progressBar);
                progressDialog->setLayout(layout);
                progressDialog->show();

                connect(downloadReply, &QNetworkReply::downloadProgress, this,
                        [progressBar](qint64 bytesReceived, qint64 bytesTotal) {
                            if (bytesTotal > 0)
                                progressBar->setValue(
                                    static_cast<int>((bytesReceived * 100) / bytesTotal));
                        });

                QFile* file = new QFile(zipPath);
                if (!file->open(QIODevice::WriteOnly)) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                         tr("Could not save file."));
                    file->deleteLater();
                    downloadReply->deleteLater();
                    return;
                }

                connect(downloadReply, &QNetworkReply::readyRead, this,
                        [file, downloadReply]() { file->write(downloadReply->readAll()); });

                connect(downloadReply, &QNetworkReply::finished, this,
                        [this, file, downloadReply, progressDialog, release, zipPath, versionName,
                         downloadFolder]() {
                            file->flush();
                            file->close();
                            file->deleteLater();
                            downloadReply->deleteLater();

                            QString buildId;
                            QString folderName;
                            if (versionName == "Pre-release") {
                                folderName = "Pre-release";
                                buildId = release["tag_name"].toString().right(40);
                            } else {
                                folderName = release["tag_name"].toString();
                                buildId = release["tag_name"].toString();
                            }

                            QString destPath = downloadFolder + "/" + folderName;
                            QMicroz::extract(zipPath, destPath);
                            std::filesystem::remove(Common::PathFromQString(zipPath));

                            QMessageBox::information(
                                this, tr("Confirm Download"),
                                tr("Version %1 has been downloaded.").arg(versionName));

                            std::string type =
                                versionName == "Pre-release" ? "Pre-release" : "Release";
                            QString exeName;
#ifdef Q_OS_WIN
                            exeName = "/shadPS4.exe";
#elif defined(Q_OS_LINUX)
                            exeName = "/Shadps4-sdl.AppImage";
#elif defined(Q_OS_MACOS)
                            exeName = "/shadps4";
#endif

                            QString fullExePath = destPath + exeName;
                            std::filesystem::remove(Common::PathFromQString(zipPath));
#ifndef Q_OS_WIN
                            mode_t permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
                            if (chmod(fullExePath.toStdString().c_str(), permissions) != 0) {
                                QMessageBox::information(this, "Error setting permissions",
                                                         "Could not set access permissions for " + fullExePath +
                                                             ". Set permissions manually before launching.");
                            }
#endif


                            Config::Build build;
                            build.path = fullExePath.toStdString();
                            build.type = type;
                            build.id = buildId.toStdString();
                            build.modified =
                                Config::GetLastModifiedString(Common::PathFromQString(fullExePath));
                            buildInfo.push_back(build);
                            LoadInstalledList();
                            SaveBuilds();

                            progressDialog->close();
                            progressDialog->deleteLater();
                        });
                reply->deleteLater();
            });
}

void VersionDialog::LoadInstalledList() {
    ui->installedTreeWidget->clear();

    std::sort(buildInfo.begin(), buildInfo.end(), [](const auto& a, const auto& b) {
        auto getOrder = [](std::string type) {
            if (type == "Pre-release") {
                return 0;
            } else if (type == "Release") {
                return 1;
            } else if (type == "Local") {
                return 2;
            }
            return 3;
        };

        int orderA = getOrder(a.type);
        int orderB = getOrder(b.type);

        if (orderA != orderB)
            return orderA < orderB;

        if (a.type == "Release") {
            static QRegularExpression versionRegex("^v\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
            QRegularExpressionMatch matchA = versionRegex.match(QString::fromStdString(a.id));
            QRegularExpressionMatch matchB = versionRegex.match(QString::fromStdString(b.id));

            if (matchA.hasMatch() && matchB.hasMatch()) {
                int majorA = matchA.captured(1).toInt();
                int minorA = matchA.captured(2).toInt();
                int patchA = matchA.captured(3).toInt();
                int majorB = matchB.captured(1).toInt();
                int minorB = matchB.captured(2).toInt();
                int patchB = matchB.captured(3).toInt();

                if (majorA != majorB)
                    return majorA > majorB;
                if (minorA != minorB)
                    return minorA > minorB;
                return patchA > patchB;
            }
        }

        return QString::fromStdString(a.id).compare(QString::fromStdString(b.id),
                                                    Qt::CaseInsensitive) < 0;
    });

    for (int i = 0; i < buildInfo.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->installedTreeWidget);

        QString id;
        if (buildInfo[i].type == "Release") {
            id = QString::fromStdString(buildInfo[i].id).left(8);
        } else if (buildInfo[i].type == "Pre-release") {
            id = QString::fromStdString(buildInfo[i].id).left(7);
        } else {
            id = QString::fromStdString(buildInfo[i].id);
        }

        item->setText(1, QString::fromStdString(buildInfo[i].type));
        item->setText(2, id);
        item->setText(3, QString::fromStdString(buildInfo[i].path));
        item->setText(4, QString::fromStdString(buildInfo[i].modified));

        if (buildInfo[i].path == Common::shadPs4Executable.string()) {
            item->setCheckState(0, Qt::Checked);
        } else {
            item->setCheckState(0, Qt::Unchecked);
        }
    }

    ui->installedTreeWidget->setColumnCount(5);
    ui->installedTreeWidget->setColumnHidden(4, true);
    ui->installedTreeWidget->resizeColumnToContents(0);
    ui->installedTreeWidget->resizeColumnToContents(1);
    ui->installedTreeWidget->resizeColumnToContents(2);
    ui->installedTreeWidget->setColumnWidth(1, ui->installedTreeWidget->columnWidth(1) + 10);
    ui->installedTreeWidget->setColumnWidth(2, ui->installedTreeWidget->columnWidth(2) + 10);
}

QStringList VersionDialog::LoadDownloadCache() {
    QStringList cachedVersions;
    QString BBLPath;
    Common::PathToQString(BBLPath, Common::GetBBLFilesPath());
    QString cachePath = BBLPath + "/cache.version";

    QFile file(cachePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd())
            cachedVersions.append(in.readLine().trimmed());
    }
    return cachedVersions;
}

void VersionDialog::SaveDownloadCache(const QStringList& versions) {
    QString BBLPath;
    Common::PathToQString(BBLPath, Common::GetBBLFilesPath());
    QString cachePath = BBLPath + "/cache.version";
    QFile file(cachePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& v : versions)
            out << v << "\n";
    }
}

void VersionDialog::PopulateDownloadTree(const QStringList& versions) {
    ui->downloadTreeWidget->clear();

    QTreeWidgetItem* preReleaseItem = nullptr;
    QList<QTreeWidgetItem*> otherItems;
    bool foundPreRelease = false;

    for (const QString& tagName : versions) {
        if (tagName.startsWith("Pre-release", Qt::CaseInsensitive)) {
            if (!foundPreRelease) {
                preReleaseItem = new QTreeWidgetItem();
                preReleaseItem->setText(0, "Pre-release");
                foundPreRelease = true;
            }
            continue;
        }
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, tagName);
        otherItems.append(item);
    }

    if (!foundPreRelease) {
        preReleaseItem = new QTreeWidgetItem();
        preReleaseItem->setText(0, "Pre-release");
    }

    if (preReleaseItem)
        ui->downloadTreeWidget->addTopLevelItem(preReleaseItem);
    for (QTreeWidgetItem* item : otherItems)
        ui->downloadTreeWidget->addTopLevelItem(item);
}

void VersionDialog::checkUpdatePre(const bool showMessage) {
    hasPreRelease = false;
    preReleaseFolder = "";
    QString localHash = "";

    for (auto build : buildInfo) {
        if (build.type == "Pre-release") {
            hasPreRelease = true;
            std::filesystem::path preReleasePath = build.path;
            std::filesystem::path preReleaseParentPath = preReleasePath.parent_path();
            preReleaseFolder = QString::fromStdString(preReleaseParentPath.string());
            localHash = QString::fromStdString(build.id);
            break;
        }
    }

    QString localFolderName;

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.github.com/repos/shadps4-emu/shadPS4/releases"));
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, localHash, localFolderName, showMessage]() {
                if (reply->error() != QNetworkReply::NoError) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"), reply->errorString());
                    reply->deleteLater();
                    return;
                }

                QByteArray resp = reply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(resp);
                if (!doc.isArray()) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                         tr("The GitHub API response is not a valid JSON array."));
                    reply->deleteLater();
                    return;
                }

                QJsonArray arr = doc.array();
                QString latestHash;
                QString latestTag;

                for (const QJsonValue& val : arr) {
                    QJsonObject obj = val.toObject();
                    if (obj["prerelease"].toBool()) {
                        QString tag = obj["tag_name"].toString();
                        latestTag = tag;
                        int idx = tag.lastIndexOf('-');
                        if (idx != -1 && idx + 1 < tag.length()) {
                            latestHash = tag.mid(idx + 1);
                        }
                        break;
                    }
                }

                if (!hasPreRelease) {
                    QMessageBox::StandardButton reply =
                        QMessageBox::question(this, tr("No pre-release found"),
                                              // clang-format off
                        tr("You don't have any pre-release installed yet.\nWould you like to download it now?"),
                                              // clang-format on
                                              QMessageBox::Yes | QMessageBox::No);
                    if (reply == QMessageBox::Yes) {
                        QString defaultPath;
                        if (QDir(ui->FolderLabel->text()).exists()) {
                            defaultPath = ui->FolderLabel->text();
                        } else {
                            defaultPath = QDir::currentPath();
                        }

                        QString path = QFileDialog::getExistingDirectory(
                            this, "Select Download Folder", defaultPath);
                        if (path == "") {
                            QMessageBox::warning(this, "ShadPS4 Updater Error",
                                                 "No download folder selected");
                            return;
                        }

                        preReleaseFolder = path + "/Pre-release";
                        installPreReleaseByTag(latestTag);
                    }
                    return;
                }

                if (latestHash.isEmpty()) {
                    QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                         tr("Unable to get hash of latest pre-release"));
                    reply->deleteLater();
                    return;
                }

                if (latestHash == localHash) {
                    if (showMessage) {
                        QMessageBox::information(
                            this, tr("Auto Updater - Emulator"),
                            tr("You already have the latest pre-release version."));
                    }
                } else {
                    showPreReleaseUpdateDialog(localHash, latestHash, latestTag);
                }
                reply->deleteLater();
            });
}

void VersionDialog::showPreReleaseUpdateDialog(const QString& localHash, const QString& latestHash,
                                               const QString& latestTag) {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Auto Updater - Emulator"));

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* imageLabel = new QLabel(&dialog);
    QPixmap pixmap(":/images/shadps4.png");
    imageLabel->setPixmap(pixmap);
    imageLabel->setScaledContents(true);
    imageLabel->setFixedSize(50, 50);

    QLabel* titleLabel = new QLabel("<h2>" + tr("Update Available (Emulator)") + "</h2>", &dialog);

    headerLayout->addWidget(imageLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(1);

    mainLayout->addLayout(headerLayout);

    QString labelText = QString("<table>"
                                "<tr><td><b>%1:</b></td><td>%2</td></tr>"
                                "<tr><td><b>%3:</b></td><td>%4</td></tr>"
                                "</table>")
                            .arg(tr("Current Version"), localHash.left(7), tr("Latest Version"),
                                 latestHash.left(7));
    QLabel* infoLabel = new QLabel(labelText, &dialog);
    mainLayout->addWidget(infoLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QLabel* questionLabel = new QLabel(tr("Do you want to update?"), &dialog);
    QPushButton* btnUpdate = new QPushButton(tr("Update"), &dialog);
    QPushButton* btnCancel = new QPushButton(tr("No"), &dialog);

    btnLayout->addWidget(questionLabel);
    btnLayout->addStretch(1);
    btnLayout->addWidget(btnUpdate);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    // Changelog
    QTextBrowser* changelogView = new QTextBrowser(&dialog);
    changelogView->setReadOnly(true);
    changelogView->setVisible(false);
    changelogView->setFixedWidth(500);
    changelogView->setFixedHeight(200);
    mainLayout->addWidget(changelogView);

    QPushButton* toggleButton = new QPushButton(tr("Show Changelog"), &dialog);
    mainLayout->addWidget(toggleButton);

    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    connect(btnUpdate, &QPushButton::clicked, this, [this, &dialog, latestTag]() {
        installPreReleaseByTag(latestTag);
        dialog.accept();
    });

    connect(toggleButton, &QPushButton::clicked, this,
            [this, changelogView, toggleButton, &dialog, localHash, latestHash, latestTag]() {
                if (!changelogView->isVisible()) {
                    requestChangelog(localHash, latestHash, latestTag, changelogView);
                    changelogView->setVisible(true);
                    toggleButton->setText(tr("Hide Changelog"));
                    dialog.adjustSize();
                } else {
                    changelogView->setVisible(false);
                    toggleButton->setText(tr("Show Changelog"));
                    dialog.adjustSize();
                }
            });
    if (Config::ShowChangeLog) {
        requestChangelog(localHash, latestHash, latestTag, changelogView);
        changelogView->setVisible(true);
        toggleButton->setText(tr("Hide Changelog"));
        dialog.adjustSize();
    }

    dialog.exec();
}

void VersionDialog::requestChangelog(const QString& localHash, const QString& latestHash,
                                     const QString& latestTag, QTextBrowser* outputView) {
    QString compareUrlString =
        QString("https://api.github.com/repos/shadps4-emu/shadPS4/compare/%1...%2")
            .arg(localHash, latestHash);

    QUrl compareUrl(compareUrlString);
    QNetworkRequest req(compareUrl);
    QNetworkReply* reply = networkManager->get(req);

    connect(
        reply, &QNetworkReply::finished, this, [this, reply, localHash, latestHash, outputView]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                     tr("Network error while fetching changelog") + ":\n" +
                                         reply->errorString());
                reply->deleteLater();
                return;
            }
            QByteArray resp = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(resp);
            QJsonObject obj = doc.object();
            QJsonArray commits = obj["commits"].toArray();

            QString changesHtml;
            for (const QJsonValue& cval : commits) {
                QJsonObject cobj = cval.toObject();
                QString msg = cobj["commit"].toObject()["message"].toString();
                int newlinePos = msg.indexOf('\n');
                if (newlinePos != -1) {
                    msg = msg.left(newlinePos);
                }
                if (!changesHtml.isEmpty()) {
                    changesHtml += "<br>";
                }
                changesHtml += "&nbsp;&nbsp;&nbsp;&nbsp;• " + msg;
            }

            // PR number as link ( #123 )
            QRegularExpression re("\\(\\#(\\d+)\\)");
            QString newText;
            int last = 0;
            auto it = re.globalMatch(changesHtml);
            while (it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                newText += changesHtml.mid(last, m.capturedStart() - last);
                QString num = m.captured(1);
                newText +=
                    QString("(<a href=\"https://github.com/shadps4-emu/shadPS4/pull/%1\">#%1</a>)")
                        .arg(num);
                last = m.capturedEnd();
            }
            newText += changesHtml.mid(last);
            changesHtml = newText;

            outputView->setOpenExternalLinks(true);
            outputView->setHtml("<h3>" + tr("Changes") + ":</h3>" + changesHtml);
            reply->deleteLater();
        });
}

void VersionDialog::installPreReleaseByTag(const QString& tagName) {

    QString apiUrl =
        QString("https://api.github.com/repos/shadps4-emu/shadPS4/releases/tags/%1").arg(tagName);

    QNetworkAccessManager* mgr = new QNetworkAccessManager(this);
    QNetworkRequest req(apiUrl);
    QNetworkReply* reply = mgr->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, tagName]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, tr("Error"), reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray bytes = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        QJsonObject obj = doc.object();
        QJsonArray assets = obj["assets"].toArray();

        QString downloadUrl;
        QString platformStr;
#ifdef Q_OS_WIN
        platformStr = "win64-sdl";
#elif defined(Q_OS_LINUX)
        platformStr = "linux-sdl";
#elif defined(Q_OS_MAC)
        platformStr = "macos-sdl";
#endif
        for (const QJsonValue& av : assets) {
            QJsonObject aobj = av.toObject();
            if (aobj["name"].toString().contains(platformStr)) {
                downloadUrl = aobj["browser_download_url"].toString();
                break;
            }
        }
        if (downloadUrl.isEmpty()) {
            QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                 tr("No download URL found for the specified asset."));
            reply->deleteLater();
            return;
        }
        showDownloadDialog(tagName, downloadUrl);

        reply->deleteLater();
    });
}

void VersionDialog::showDownloadDialog(const QString& tagName, const QString& downloadUrl) {
    QDialog* dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Downloading Pre‑release, please wait..."));
    QVBoxLayout* lay = new QVBoxLayout(dlg);

    QProgressBar* progressBar = new QProgressBar(dlg);
    progressBar->setRange(0, 100);
    lay->addWidget(progressBar);

    dlg->setLayout(lay);
    dlg->resize(400, 80);
    dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowCloseButtonHint);
    dlg->show();

    QNetworkRequest req(downloadUrl);
    QNetworkAccessManager* mgr = new QNetworkAccessManager(dlg);
    QNetworkReply* reply = mgr->get(req);

    connect(reply, &QNetworkReply::downloadProgress, this, [progressBar](qint64 rec, qint64 tot) {
        if (tot > 0) {
            int perc = static_cast<int>((rec * 100) / tot);
            progressBar->setValue(perc);
        }
    });

    connect(reply, &QNetworkReply::finished, this, [=, this]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                 tr("Network error while downloading") + ":\n" +
                                     reply->errorString());
            reply->deleteLater();
            dlg->close();
            dlg->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QFileInfo fileInfo(preReleaseFolder);
        QString zipPath = QDir(preReleaseFolder).filePath("temp_pre_release_download.zip");

        if (!fileInfo.exists())
            std::filesystem::create_directory(preReleaseFolder.toStdString());

        QFile zipFile(zipPath);
        if (!zipFile.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ShadPS4 Updater Error"),
                                 tr("Failed to save download file") + ":\n" + zipPath);
            reply->deleteLater();
            return;
        }

        zipFile.write(data);
        zipFile.close();

        QString exeName;
#ifdef Q_OS_WIN
        exeName = "/shadPS4.exe";
#elif defined(Q_OS_LINUX)
        exeName = "/Shadps4-sdl.AppImage";
#elif defined(Q_OS_MACOS)
        exeName = "/shadps4";
#endif

        QString fullExePath = preReleaseFolder + exeName;
        std::filesystem::remove(Common::PathFromQString(fullExePath));
        QMicroz::extract(zipPath, preReleaseFolder);
        std::filesystem::remove(Common::PathFromQString(zipPath));

#ifndef Q_OS_WIN
        mode_t permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
        if (chmod(fullExePath.toStdString().c_str(), permissions) != 0) {
            QMessageBox::information(this, "Error setting permissions",
                                     "Could not set access permissions for " + fullExePath +
                                         ". Set permissions manually before launching.");
        }
#endif

        if (hasPreRelease)
            buildInfo.erase(buildInfo.begin());

        Config::Build build;
        build.path = fullExePath.toStdString();
        build.type = "Pre-release";
        build.id = tagName.right(40).toStdString();
        build.modified = Config::GetLastModifiedString(build.path);

        buildInfo.insert(buildInfo.begin(), build);
        LoadInstalledList();
        SaveBuilds();

        QMessageBox::information(this, tr("Complete installation"),
                                 tr("Pre-release updated successfully") + ":\n" + tagName);

        reply->deleteLater();
        dlg->close();
        dlg->deleteLater();
    });
}

void VersionDialog::SaveBuilds() {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(Config::SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(Config::SettingsFile, std::ios_base::binary);
            data = toml::parse(
                ifs, std::string{fmt::UTF(Config::SettingsFile.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(this, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(this, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    if (data.contains("Builds")) {
        data.as_table().erase("Builds");
    }

    for (int i = 0; i < buildInfo.size(); i++) {
        toml::array arr = toml::array{buildInfo[i].path, buildInfo[i].type, buildInfo[i].id,
                                      buildInfo[i].modified};
        data["Builds"][std::to_string(i + 1)] = arr;
    }

    std::ofstream file(Config::SettingsFile, std::ios::binary);
    file << data;
    file.close();
}

void VersionDialog::GetBuildInfo() {
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(Config::SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(Config::SettingsFile, std::ios_base::binary);
            data = toml::parse(
                ifs, std::string{fmt::UTF(Config::SettingsFile.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(this, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(this, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    if (!data.contains("Builds"))
        return;

    bool missingBuild = false;
    int buildCounter = 1;
    while (true) {
        const auto arr =
            toml::find_or<toml::array>(data.at("Builds"), std::to_string(buildCounter), {});

        if (arr.empty())
            break;

        Config::Build build;
        build.path = arr[0].as_string();
        build.type = arr[1].as_string();
        build.id = arr[2].as_string();
        build.modified = arr[3].as_string();

        if (std::filesystem::exists(build.path)) {
            buildInfo.push_back(build);
        } else {
            QMessageBox::information(this, "Build not found",
                                     "Saved build " + QString::fromStdString(arr[0].as_string()) +
                                         " cannot be found, it may have been moved or deleted. "
                                         "This will be removed from the build list.");
            missingBuild = true;
        }

        buildCounter += 1;
    }

    if (missingBuild)
        SaveBuilds();
}

void VersionDialog::RemoveItem(bool alsoDelete) {
    QTreeWidgetItem* selectedItem = ui->installedTreeWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, tr("Error"),
                             tr("No version selected. Please choose one from the list to delete."));
        return;
    }

    QString fullPath = selectedItem->text(3);
    QString msg;

    if (!alsoDelete) {
        msg = tr("Do you want to remove the build") + QString(" \"%1\" ?").arg(fullPath);
    } else {
        msg = tr("Do you want to remove the build and the delete the folder containing") +
              QString(" \"%1\" ?").arg(fullPath);
    }

    auto reply =
        QMessageBox::question(this, tr("Remove build"), msg, QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (alsoDelete) {
            QFileInfo fileInfo(fullPath);
            QString folderPath = fileInfo.absolutePath();
            if (QFileInfo::exists(fileInfo.absolutePath())) {
                QDir dir(folderPath);
                dir.removeRecursively();
            }
        }

        if (selectedItem->checkState(0) == Qt::Checked) {
            Common::shadPs4Executable = "";
            Config::SaveLauncherSettings();
        }

        buildInfo.erase(buildInfo.begin() + ui->installedTreeWidget->currentIndex().row());
        LoadInstalledList();
        SaveBuilds();
    }
}
