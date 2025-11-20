// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QProgressBar>
#include <nlohmann/json.hpp>
#include <qmicroz.h>

#include "Common.h"
#include "ModDownloader.h"
#include "settings/config.h"
#include "ui_ModDownloader.h"

using json = nlohmann::json;

ModDownloader::ModDownloader(QWidget* parent) : QDialog(parent), ui(new Ui::ModDownloader) {
    ui->setupUi(this);
    ui->downloadButton->setFocus();

    manager = new QNetworkAccessManager(this);
    ui->linkLabel->setText(
        "<a href=\"https://www.nexusmods.com/users/myaccount?tab=api%20access\">API Link</a>");
    ui->linkLabel->setOpenExternalLinks(true);
    ui->modDesc->setOpenLinks(false);

    apiKey = QString::fromStdString(Config::ApiKey);
    ui->apiLineEdit->setText(apiKey);

    modIDmap = {{0, 109},   // Vertex Explosion Mod
                {1, 70},    // 60 fps Cutscene Fix
                {2, 41},    // Sfx Fix Mods
                {3, 114},   // Cloth physics mods
                {4, 30},    // Xbox Controller Icons
                {5, 162},   // Disable FXAA
                {6, 102},   // Performance Drawparams
                {7, 223},   // Stake of Marika
                {8, 107},   // More Options at Lamps
                {9, 6},     // Great One Beast Restored
                {10, 206},  // Estus Vial and Bullet
                {11, 156},  // Jump on L3
                {12, 19},   // Bloodborne Enhanced
                {13, 182}}; // 4k Upscaled UI

    ui->modComboBox->addItem("Vertex Explosion Mod");
    ui->modComboBox->addItem("60 fps Cutscene Fix");
    ui->modComboBox->addItem("Sfx Fix Mods");
    ui->modComboBox->addItem("Cloth physics mods");
    ui->modComboBox->addItem("Xbox Controller Icons");
    ui->modComboBox->addItem("Disable FXAA");
    ui->modComboBox->addItem("Performance Drawparams");
    ui->modComboBox->addItem("Stake of Marika");
    ui->modComboBox->addItem("More Options at Lamps");
    ui->modComboBox->addItem("Great One Beast Restored");
    ui->modComboBox->addItem("Estus Vial and Bullet");
    ui->modComboBox->addItem("Jump on L3");
    ui->modComboBox->addItem("Bloodborne Enhanced");
    ui->modComboBox->addItem("4K Upscaled UI");

    if (apiKey.isEmpty()) {
        ui->validApiLabel->setStyleSheet("color: red;");
        ui->validApiLabel->setText("No Valid Nexus ModsAPI Key Set");
    } else {
        if (ValidateApi())
            LoadModInfo(109);
    }

    sevenzipPath = Config::SevenZipPath;
    if (sevenzipPath.empty()) {
        if (std::filesystem::exists("C:/Program Files/7-zip/7z.exe")) {
            sevenzipPath = "C:/Program Files/7-zip/7z.exe";
        } else if (std::filesystem::exists("/usr/bin/7z/7z")) {
            sevenzipPath = "/usr/bin/7z/7z";
        } else if (std::filesystem::exists("/usr/bin/p7zip/7z")) {
            sevenzipPath = "/usr/bin/p7zip/7z";
        } else if (std::filesystem::exists("/usr/local/bin/7z/7z")) {
            sevenzipPath = "/usr/local/bin/7z/7z";
        } else if (std::filesystem::exists("/usr/local/bin/p7zip/7z")) {
            sevenzipPath = "/usr/local/bin/p7zip/7z";
        }
    }

    if (sevenzipPath.empty()) {
        ui->zipLabel->setStyleSheet("color: red;");
    } else {
        ui->zipLabel->setText("Valid 7zip binary detected. 7z mod files can be downloaded");
        ui->zipLabel->setStyleSheet("color: green;");
    }

    connect(ui->zipButton, &QPushButton::pressed, this, &ModDownloader::SetSevenzipPath);

    connect(ui->modComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        int index = ui->modComboBox->currentIndex();
        LoadModInfo(modIDmap[index]);
    });

    connect(ui->setApiButton, &QPushButton::pressed, this, [this]() {
        if (ui->apiLineEdit->text().isEmpty())
            return;

        apiKey = ui->apiLineEdit->text();
        Config::ApiKey = apiKey.toStdString();
        Config::SaveLauncherSettings();

        if (ValidateApi())
            LoadModInfo(modIDmap[ui->modComboBox->currentIndex()]);
    });

    connect(ui->downloadButton, &QPushButton::pressed, this, [this]() {
        if (ui->fileListWidget->selectedItems().size() == 0) {
            QMessageBox::information(this, "No file selected", "Select a file for download first");
            return;
        }

        if (ui->fileListWidget->currentItem()->text().contains("Patcher Only")) {
            QMessageBox::information(this, "Non-compatible mod",
                                     "For Bloodborne Enhanced, use the full package file");
            return;
        }

        int modIndex = ui->modComboBox->currentIndex();
        int fileIndex = ui->fileListWidget->currentRow();
        QString modFilename = ui->fileListWidget->currentItem()->text();

        if (fileTypeList[fileIndex] == "7z" && sevenzipPath.empty()) {
            QMessageBox::warning(this, "Error",
                                 "Selected file is a 7z file. Valid 7zip must be set to "
                                 "download. Aborting...");
            return;
        }

        if (std::filesystem::exists(Common::ModPath / modFilename.toStdString())) {
            QMessageBox::information(this, tr("Mod already exists"),
                                     tr("%1 is already in your mod folder. Aborting...")
                                         .arg(QString::fromStdString(modFilename.toStdString())));
            return;
        }

        DownloadFile(fileIdList[fileIndex], modIDmap[modIndex], modFilename);
    });

    connect(ui->fileListWidget, &QListWidget::itemClicked, this, [this]() {
        int fileIndex = ui->fileListWidget->currentRow();
        fileDescList[fileIndex].isEmpty() ? ui->fileDesc->setText("No description")
                                          : ui->fileDesc->setHtml(fileDescList[fileIndex]);
    });
}

bool ModDownloader::ValidateApi() {
    QString url = "https://api.nexusmods.com/v1/users/validate.json";
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("apikey", apiKey.toUtf8());

    QNetworkReply* reply = manager->get(request);
    bool valid = false;

    connect(reply, &QNetworkReply::finished, [this, &valid, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            std::string json_string(jsonData.constData(), jsonData.length());

            json jsonDoc;
            if (json::accept(json_string)) {
                jsonDoc = json::parse(json_string);
            }

            for (auto& item : jsonDoc.items()) {
                if (item.key() == "is_premium") {
                    bool isPremium = jsonDoc["is_premium"].get<bool>();
                    if (!isPremium) {
                        QMessageBox::information(this, "Premium key required",
                                                 "Due to nexus Mods Api guidelines, a premium Api "
                                                 "key is required for donwloading");
                        ui->validApiLabel->setStyleSheet("color: red;");
                        ui->validApiLabel->setText("No Valid Nexus ModsAPI Key Set");
                    } else {
                        ui->validApiLabel->setStyleSheet("color: green;");
                        ui->validApiLabel->setText("Valid premium Api key set");
                        valid = true;
                    }
                    break;
                }
            }
        } else {
            ui->validApiLabel->setStyleSheet("color: red;");
            ui->validApiLabel->setText("No Valid Nexus ModsAPI Key Set");
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
        }

        reply->deleteLater();
    });

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    return valid;
}

void ModDownloader::LoadModInfo(int modId) {
    QString url =
        "https://api.nexusmods.com/v1/games/bloodborne/mods/" + QString::number(modId) + ".json";
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("apikey", apiKey.toUtf8());

    QString name;
    QString author;
    int timestamp;
    QString version;

    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            std::string json_string(jsonData.constData(), jsonData.length());

            json jsonDoc;
            if (json::accept(json_string)) {
                jsonDoc = json::parse(json_string);
            }

            for (auto& item : jsonDoc.items()) {
                if (item.key() == "name") {
                    ui->modNameLabel->setText(
                        "Name: " +
                        QString::fromStdString(item.value().get<std::string>()).left(45));
                }

                if (item.key() == "updated_timestamp") {
                    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(item.value().get<int>());
                    QString formattedDateTime = dateTime.toString("yyyy-MM-dd");
                    ui->modUpdatedLabel->setText("Updated: " + formattedDateTime);
                }

                if (item.key() == "author") {
                    ui->modAuthorLabel->setText(
                        "Author: " +
                        QString::fromStdString(item.value().get<std::string>()).left(45));
                }

                if (item.key() == "version") {
                    ui->modVersionLabel->setText(
                        "Version: " + QString::fromStdString(item.value().get<std::string>()));
                }

                if (item.key() == "picture_url") {
                    QString url2 = QString::fromStdString(item.value().get<std::string>());
                    QUrl imageUrl(url2);
                    GetModImage(url2);
                }

                if (item.key() == "description") {
                    QString desc = QString::fromStdString(item.value().get<std::string>());
                    ui->modDesc->setHtml(BbcodeToHtml(desc));
                }
            }
        } else {
            ui->validApiLabel->setStyleSheet("color: red;");
            ui->validApiLabel->setText("No Valid Nexus ModsAPI Key Set");
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
        }

        reply->deleteLater();
    });

    GetModFiles(modId);
}

void ModDownloader::GetModImage(QUrl url) {
    QNetworkRequest request2(url);
    manager->get(request2);

    QNetworkReply* reply2 = manager->get(request2);
    connect(reply2, &QNetworkReply::finished, [=, this]() {
        if (reply2->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply2->readAll();
            QPixmap pixmap;
            if (pixmap.loadFromData(imageData)) {
                ui->modPicLabel->setPixmap(pixmap.scaled(480, 270));
            } else {
                QMessageBox::warning(this, tr("Error"),
                                     QString(tr("Error creating mod image file")));
            }
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply2->errorString()));
        }

        reply2->deleteLater();
    });
}

void ModDownloader::GetModFiles(int modId) {
    QString url = "https://api.nexusmods.com/v1/games/bloodborne/mods/" + QString::number(modId) +
                  "/files.json";
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("apikey", apiKey.toUtf8());

    fileIdList.clear();
    fileList.clear();
    fileDescList.clear();

    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            std::string json_string(jsonData.constData(), jsonData.length());

            json jsonDoc;
            if (json::accept(json_string)) {
                jsonDoc = json::parse(json_string);
            }

            json file_array = jsonDoc["files"];
            for (const auto& element : file_array) {
                if (element.contains("category_name") && !element.at("category_name").is_null()) {
                    std::string catName = element.at("category_name").get<std::string>();
                    int fileId = element.at("file_id").get<int>();
                    if (catName == "MAIN") {
                        fileList.push_back(
                            QString::fromStdString(element.at("name").get<std::string>()));
                        fileIdList.push_back(element.at("file_id").get<int>());
                        fileDescList.push_back(BbcodeToHtml(
                            QString::fromStdString(element.at("description").get<std::string>())));

                        if (element.at("name").get<std::string>().ends_with("zip")) {
                            fileTypeList.push_back("zip");
                        } else if (element.at("name").get<std::string>().ends_with("7z")) {
                            fileTypeList.push_back("7z");
                        } else {
                            fileTypeList.push_back("unknown");
                        }
                    }
                }
            }

            ui->fileListWidget->clear();
            ui->fileListWidget->addItems(fileList);
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
        }

        reply->deleteLater();
    });
}

void ModDownloader::DownloadFile(int fileId, int ModId, QString modFilename) {
    QString url = "https://api.nexusmods.com/v1/games/bloodborne/mods/" + QString::number(ModId) +
                  "/files/" + QString::number(fileId) + "/download_link.json";
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("apikey", apiKey.toUtf8());

    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            std::string json_string(jsonData.constData(), jsonData.length());

            json jsonDoc;
            if (json::accept(json_string)) {
                jsonDoc = json::parse(json_string);
            }

            for (const auto& element : jsonDoc) {
                if (element.contains("URI")) {
                    QString downloadUrl =
                        QString::fromStdString(element.at("URI").get<std::string>());
                    StartDownload(downloadUrl, modFilename);
                    break;
                }
            }
            reply->deleteLater();
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
            reply->deleteLater();
        }
    });
}

void ModDownloader::StartDownload(QString url, QString modFilename) {
    QNetworkRequest downloadRequest(url);
    downloadRequest.setRawHeader("apikey", apiKey.toUtf8());
    QNetworkAccessManager* downloadManager = new QNetworkAccessManager(this);
    QNetworkReply* downloadReply = downloadManager->get(downloadRequest);

    QDialog* progressDialog = new QDialog(this);
    progressDialog->setWindowTitle(tr("Downloading %1 , please wait...").arg(modFilename));
    progressDialog->setFixedSize(400, 80);
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
    QVBoxLayout* layout = new QVBoxLayout(progressDialog);
    QProgressBar* progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 100);
    layout->addWidget(progressBar);
    progressDialog->setLayout(layout);
    progressDialog->show();

    connect(downloadReply, &QNetworkReply::downloadProgress, this,
            [progressBar](qint64 bytesReceived, qint64 bytesTotal) {
                if (bytesTotal > 0)
                    progressBar->setValue(static_cast<int>((bytesReceived * 100) / bytesTotal));
            });

    QString zipPath;
    Common::PathToQString(zipPath, Common::GetBBLFilesPath() / "Temp" / "downloaded_mod.tmp");

    QString tempPath;
    Common::PathToQString(tempPath, Common::GetBBLFilesPath() / "Temp" / "Download");
    std::filesystem::create_directories(Common::GetBBLFilesPath() / "Temp" / "Download");

    QFile* file = new QFile(zipPath);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save file."));
        file->deleteLater();
        downloadReply->deleteLater();
        return;
    }

    connect(downloadReply, &QNetworkReply::readyRead, this,
            [file, downloadReply]() { file->write(downloadReply->readAll()); });

    connect(downloadReply, &QNetworkReply::finished, [=, this]() {
        if (downloadReply->error() == QNetworkReply::NoError) {
            file->flush();
            file->close();
            downloadReply->deleteLater();

            QString newZipPath;
            Common::PathToQString(newZipPath,
                                  Common::GetBBLFilesPath() / "Temp" / "downloaded_mod.tmp");

            bool is7z = false;
            QString filenameFromUrl = downloadReply->request().url().fileName();
            if (filenameFromUrl.right(3) == "zip") {
                newZipPath = newZipPath.replace("tmp", "zip");
            } else if (filenameFromUrl.right(2) == "7z") {
                newZipPath = newZipPath.replace("tmp", "7z");
                is7z = true;
            }

            QFile::rename(zipPath, newZipPath);
            if (is7z) {
                extract7z(newZipPath, tempPath);
            } else {
                QMicroz::extract(newZipPath, tempPath);
            }

            std::string folderName = modFilename.toStdString();
            std::filesystem::path folderPath = "";
            const std::vector<std::string> BBFolders = {
                "dvdroot_ps4", "action", "chr",    "event", "facegen", "map",   "menu",
                "movie",       "msg",    "mtd",    "obj",   "other",   "param", "paramdef",
                "parts",       "remo",   "script", "sfx",   "shader",  "sound"};

            for (const auto& entry :
                 std::filesystem::recursive_directory_iterator(Common::PathFromQString(tempPath))) {
                if (entry.is_directory()) {
                    folderName = entry.path().filename().string();
                    folderPath = entry.path().parent_path();
                    if (std::find(BBFolders.begin(), BBFolders.end(), folderName) !=
                        BBFolders.end()) {
                        break;
                    }
                }
            }

            std::filesystem::path newPath = Common::ModPath / modFilename.toStdString();
            std::filesystem::rename(folderPath, newPath);
            QMessageBox::information(
                this, tr("Confirm Download"),
                tr("%1 has been downloaded. You can activate it using the mod manager")
                    .arg(modFilename.toStdString()));

            progressDialog->close();
            progressDialog->deleteLater();
            QDir(tempPath).removeRecursively();
            QFile::remove(newZipPath);
        } else {
            QMessageBox::warning(
                this, tr("Error"),
                QString(tr("Network error:") + "\n" + downloadReply->errorString()));

            progressDialog->close();
            progressDialog->deleteLater();
            QFile::remove(zipPath);
        }
    });
}

QString ModDownloader::BbcodeToHtml(QString BbcodeString) {
    BbcodeString.replace("[b]", "<b>");
    BbcodeString.replace("[/b]", "</b>");

    BbcodeString.replace("[i]", "<i>");
    BbcodeString.replace("[/i]", "</i>");

    BbcodeString.replace("[u]", "<u>");
    BbcodeString.replace("[/u]", "</u>");

    BbcodeString.replace("[center]", "<center>");
    BbcodeString.replace("[/center]", "</center>");

    QRegularExpression attributedUrlRegex("\\[url=(.*?)\\](.*?)\\[/url\\]");
    BbcodeString.replace(attributedUrlRegex, "<a href=\"\\1\">\\2</a>");

    QRegularExpression openSizeRegex("\\[size=(\\d+)\\]");
    BbcodeString.replace(openSizeRegex, "<font size=\\1>");

    QRegularExpression closeSizeRegex("\\[/size\\]");
    BbcodeString.replace(closeSizeRegex, "</font>");

    QRegularExpression colorRegex("\\[color=(.*?)\\](.*?)\\[\\/color\\]");
    BbcodeString.replace(colorRegex, "<span style=\"color: \\1;\">\\2</span>");

    QRegularExpression color2RegexOpen("\\[color=(.*?)\\]");
    BbcodeString.replace(color2RegexOpen, "<span style=\"color:\\1\">");

    QRegularExpression color2RegexClose("\\[\\/color\\]");
    BbcodeString.replace(color2RegexClose, "</span>");

    QRegularExpression itemRegex("\\[\\*\\]");
    BbcodeString.replace(itemRegex, "<li>");
    BbcodeString.replace(QRegularExpression("\\[list\\]"), "<ul>");
    BbcodeString.replace(QRegularExpression("\\[/list\\]"), "</ul>");
    BbcodeString.replace(QRegularExpression("<li>(.*?)(?=<li>|</ul>|$)",
                                            QRegularExpression::DotMatchesEverythingOption),
                         "<li>\\1</li>");

    QRegularExpression imgRegex("\\[img\\](.+?)\\[\\/img\\]");
    BbcodeString.replace(imgRegex, "<img src=\"\\1\">");

    return BbcodeString;
}

void ModDownloader::extract7z(QString inpath, QString outpath) {
    QProcess* sevenZipProcess = new QProcess(this);

    QString processPath;
    Common::PathToQString(processPath, sevenzipPath);

    QStringList arguments;
    arguments << "x";
    arguments << QDir::toNativeSeparators(inpath);
    arguments << "-o" + QDir::toNativeSeparators(outpath);

    sevenZipProcess->start(processPath, arguments);
    sevenZipProcess->waitForFinished();
}

void ModDownloader::SetSevenzipPath() {
    QString exePath;
#ifdef _WIN32
    exePath = QFileDialog::getOpenFileName(this, "Select ShadPS4 executable (ex. shadPS4.exe)",
                                           QDir::currentPath(), "Executables (7z.exe)");
#else
    exePath = QFileDialog::getOpenFileName(this, "Select 7-zip binary", QDir::homePath(),
                                           "7z Binary (7z*)");
#endif

    if (exePath.isEmpty()) {
        return;
    } else {
        sevenzipPath = Common::PathFromQString(exePath);
        ui->zipLabel->setText("Valid 7zip binary detected. 7z mod files can be downloaded");
        ui->zipLabel->setStyleSheet("color: green;");
        Config::SevenZipPath = sevenzipPath;
        Config::SaveLauncherSettings();
    }
}

ModDownloader::~ModDownloader() {}
