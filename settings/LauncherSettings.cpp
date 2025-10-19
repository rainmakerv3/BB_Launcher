// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QStandardPaths>
#include <qmicroz.h>
#include <sys/stat.h>

#include "LauncherSettings.h"
#include "config.h"
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_LauncherSettings.h"
#include "settings/updater/CheckUpdate.h"

#ifdef Q_OS_WIN
#include <ShlObj.h>
#include <Windows.h>
#include <objbase.h>
#include <shlguid.h>
#include <shobjidl.h>
#include <wrl/client.h>
#endif

#if __APPLE__
#include <date/date.h>
#include <date/tz.h>
#include <date/tz_private.h>
#endif

LauncherSettings::LauncherSettings(QWidget* parent)
    : QDialog(parent), ui(new Ui::LauncherSettings) {
    ui->setupUi(this);
    using namespace Config;

    ui->BackupIntervalComboBox->addItems(BackupFreqList);
    ui->BackupNumberComboBox->addItems(BackupNumList);

    if (theme == "Dark") {
        ui->DarkThemeRadioButton->setChecked(true);
    } else {
        ui->LightThemeRadioButton->setChecked(true);
    }

    ui->shadChannelComboBox->setCurrentText(QString::fromStdString(UpdateChannel));
    ui->shadUpdateCheckBox->setChecked(AutoUpdateShadEnabled);
    ui->UpdateCheckBox->setChecked(AutoUpdateEnabled);
    ui->PortableSettingsCheckBox->setChecked(CheckPortableSettings);
    ui->SoundFixCheckBox->setChecked(SoundFixEnabled);
    ui->BackupSaveCheckBox->setChecked(BackupSaveEnabled);
    ui->BackupIntervalComboBox->setCurrentText(QString::number(BackupInterval));
    ui->BackupNumberComboBox->setCurrentText(QString::number(BackupNumber));
    OnBackupStateChanged();

    connect(ui->UpdateButton, &QPushButton::clicked, this, []() {
        auto checkUpdate = new CheckUpdate(true);
        checkUpdate->exec();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::pressed, this, [this]() {
        SaveSettings();
        QWidget::close();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this,
            &LauncherSettings::SaveSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed, this,
            &LauncherSettings::SetLauncherDefaults);

#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    connect(ui->BackupSaveCheckBox, &QCheckBox::stateChanged, this,
            &LauncherSettings::OnBackupStateChanged);

#else
    connect(ui->BackupSaveCheckBox, &QCheckBox::checkStateChanged, this,
            &LauncherSettings::OnBackupStateChanged);
#endif

    connect(ui->shortcutButton, &QPushButton::clicked, this, &LauncherSettings::CreateShortcut);

    connect(ui->shadChannelComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        Config::UpdateChannel = ui->shadChannelComboBox->currentText().toStdString();
    });

    connect(ui->shadUpdateButton, &QPushButton::pressed, this, [this]() {
        if (Config::GameRunning) {
            QMessageBox::warning(this, "Cannot update",
                                 "Cannot update shadPS4 while game is running");
            return;
        }

        ui->shadUpdateButton->setEnabled(false);
        ui->buttonBox->setEnabled(false);
        SaveLauncherSettings();

        CheckShadUpdate* shadUpdateWindow = new CheckShadUpdate(false);

        QObject::connect(shadUpdateWindow, &CheckShadUpdate::DownloadProgressed,
                         ui->DownloadProgressBar, &QProgressBar::setValue);

        QObject::connect(shadUpdateWindow, &CheckShadUpdate::UpdateComplete, this, [this]() {
            ui->shadUpdateButton->setEnabled(true);
            ui->buttonBox->setEnabled(true);
        });

        shadUpdateWindow->exec();
    });
}

void LauncherSettings::SetLauncherDefaults() {
    ui->shadChannelComboBox->setCurrentText("Nightly");
    ui->shadUpdateCheckBox->setChecked(false);
    ui->PortableSettingsCheckBox->setChecked(true);
    ui->UpdateCheckBox->setChecked(false);
    ui->DarkThemeRadioButton->setChecked(true);
    ui->SoundFixCheckBox->setChecked(true);
    ui->BackupIntervalComboBox->setCurrentText("10");
    ui->BackupNumberComboBox->setCurrentText("2");
}

void LauncherSettings::SaveSettings() {
    using namespace Config;
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(SettingsFile.filename().u8string()).data});
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

    if (ui->DarkThemeRadioButton->isChecked()) {
        theme = "Dark";
    } else {
        theme = "Light";
    }

    SoundFixEnabled = ui->SoundFixCheckBox->isChecked();
    AutoUpdateEnabled = ui->UpdateCheckBox->isChecked();
    CheckPortableSettings = ui->PortableSettingsCheckBox->isChecked();

    BackupSaveEnabled = ui->BackupSaveCheckBox->isChecked();
    BackupInterval = ui->BackupIntervalComboBox->currentText().toInt();
    BackupNumber = ui->BackupNumberComboBox->currentText().toInt();

    UpdateChannel = ui->shadChannelComboBox->currentText().toStdString();
    AutoUpdateShadEnabled = ui->shadUpdateCheckBox->isChecked();

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();

    Config::SetTheme(theme);
}

void LauncherSettings::OnBackupStateChanged() {
    if (ui->BackupSaveCheckBox->isChecked()) {
        ui->BackupIntervalLabel->show();
        ui->BackupIntervalComboBox->show();
        ui->BackupNumberLabel->show();
        ui->BackupNumberComboBox->show();
    } else {
        ui->BackupIntervalLabel->hide();
        ui->BackupIntervalComboBox->hide();
        ui->BackupNumberLabel->hide();
        ui->BackupNumberComboBox->hide();
    }
}

void LauncherSettings::CreateShortcut() {

#ifdef Q_OS_APPLE
    QMessageBox::critical(this, "Not currently supported",
                          "Shortcut creation not currently enabled on MacOS");
    return;
#endif

    if (!std::filesystem::exists(Common::shadPs4Executable) || Common::shadPs4Executable.empty()) {
        QMessageBox::critical(this, "Error", "Set valid shadPS4 path first.");
        return;
    }

    // Path to shortcut/link
    QString linkPath;

    // Eboot path
    QString targetPath;
    Common::PathToQString(targetPath, Common::installPath);
    QString ebootPath = targetPath + "/eboot.bin";

    // Get the full path to the icon
    QString iconPath;
    Common::PathToQString(iconPath, Common::installPath / "sce_sys" / "icon0.png");

    QFileInfo iconFileInfo(iconPath);
    QString icoPath = iconFileInfo.absolutePath() + "/" + iconFileInfo.baseName() + ".ico";

    const QString name = "Bloodborne (skip GUI)";

    QString exePath;
#ifdef Q_OS_WIN
    linkPath =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + name + ".lnk";

    exePath = QCoreApplication::applicationFilePath().replace("\\", "/");
#else
    linkPath =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + name + ".desktop";
#endif

    // Convert the icon to .ico if necessary
    if (iconFileInfo.suffix().toLower() == "png") {
        // Convert icon from PNG to ICO
        if (convertPngToIco(iconPath, icoPath)) {

#ifdef Q_OS_WIN
            if (createShortcutWin(linkPath, icoPath, exePath)) {
#else
            if (createShortcutLinux(linkPath, name.toStdString(), iconPath)) {
#endif
                QMessageBox::information(
                    nullptr, tr("Shortcut creation"),
                    QString(tr("Shortcut created successfully!") + "\n%1").arg(linkPath));
            } else {
                QMessageBox::critical(
                    nullptr, tr("Error"),
                    QString(tr("Error creating shortcut!") + "\n%1").arg(linkPath));
            }
        } else {
            QMessageBox::critical(nullptr, tr("Error"), tr("Failed to convert icon."));
        }

        // If the icon is already in ICO format, we just create the shortcut
    } else {
#ifdef Q_OS_WIN
        if (createShortcutWin(linkPath, iconPath, exePath)) {
#else
        if (createShortcutLinux(linkPath, name.toStdString(), iconPath)) {
#endif
            QMessageBox::information(
                nullptr, tr("Shortcut creation"),
                QString(tr("Shortcut created successfully!") + "\n%1").arg(linkPath));
        } else {
            QMessageBox::critical(nullptr, tr("Error"),
                                  QString(tr("Error creating shortcut!") + "\n%1").arg(linkPath));
        }
    }
}

bool LauncherSettings::convertPngToIco(const QString& pngFilePath, const QString& icoFilePath) {
    // Load the PNG image
    QImage image(pngFilePath);
    if (image.isNull()) {
        return false;
    }

    // Scale the image to the default icon size (256x256 pixels)
    QImage scaledImage =
        image.scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Convert the image to QPixmap
    QPixmap pixmap = QPixmap::fromImage(scaledImage);

    // Save the pixmap as an ICO file
    if (pixmap.save(icoFilePath, "ICO")) {
        return true;
    } else {
        return false;
    }
}

#ifdef Q_OS_WIN
bool LauncherSettings::createShortcutWin(const QString& linkPath, const QString& iconPath,
                                         const QString& exePath) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Create the ShellLink object
    Microsoft::WRL::ComPtr<IShellLink> pShellLink;
    HRESULT hres =
        CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pShellLink));
    if (SUCCEEDED(hres)) {
        // Defines the path to the program executable
        pShellLink->SetPath((LPCWSTR)exePath.utf16());

        // Sets the home directory ("Start in")
        pShellLink->SetWorkingDirectory((LPCWSTR)QFileInfo(exePath).absolutePath().utf16());

        // Set arguments, eboot.bin file location
        QString arguments;
        arguments = QString("-n");
        pShellLink->SetArguments((LPCWSTR)arguments.utf16());

        // Set the icon for the shortcut
        pShellLink->SetIconLocation((LPCWSTR)iconPath.utf16(), 0);

        // Save the shortcut
        Microsoft::WRL::ComPtr<IPersistFile> pPersistFile;
        hres = pShellLink.As(&pPersistFile);
        if (SUCCEEDED(hres)) {
            hres = pPersistFile->Save((LPCWSTR)linkPath.utf16(), TRUE);
        }
    }

    CoUninitialize();

    return SUCCEEDED(hres);
}
#else
bool LauncherSettings::createShortcutLinux(const QString& linkPath, const std::string& name,
                                           const QString& iconPath) {
    QFile shortcutFile(linkPath);
    if (!shortcutFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error",
                              QString("Error creating shortcut!\n %1").arg(linkPath));
        return false;
    }

    char* appdir_env = std::getenv("APPDIR");
    QString appImagePath;

    if (appdir_env != nullptr) {
        appImagePath = QProcessEnvironment::systemEnvironment().value(QStringLiteral("APPIMAGE"));
    } else {
        appImagePath = QCoreApplication::applicationFilePath();
    }

    QTextStream out(&shortcutFile);
    out << "[Desktop Entry]\n";
    out << "Version=1.0\n";
    out << "Name=" << QString::fromStdString(name) << "\n";
    out << "Exec=" << appImagePath << " -n\n";
    out << "Icon=" << iconPath << "\n";
    out << "Terminal=false\n";
    out << "Type=Application\n";
    shortcutFile.close();

    return true;
}
#endif

LauncherSettings::~LauncherSettings() {
    delete ui;
}

CheckShadUpdate::CheckShadUpdate(const bool isAutoupdate, QWidget* parent) : QDialog(parent) {
    setFixedSize(0, 0);
    CheckShadUpdate::UpdateShad(isAutoupdate);

    connect(this, &CheckShadUpdate::UpdateComplete, this, &QDialog::close);
}

void CheckShadUpdate::UpdateShad(bool isAutoupdate) {
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QUrl url;

    bool checkName = true;
    while (checkName) {
        updateChannel = QString::fromStdString(Config::UpdateChannel);
        if (updateChannel == "Nightly") {
            url = QUrl("https://api.github.com/repos/shadps4-emu/shadPS4/releases");
            checkName = false;
        } else if (updateChannel == "Release") {
            url = QUrl("https://api.github.com/repos/shadps4-emu/shadPS4/releases/latest");
            checkName = false;
        }
    }

    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, isAutoupdate, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Network error:") + "\n" +
                                     reply->errorString());
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(response));

        if (jsonDoc.isNull()) {
            QMessageBox::warning(this, "Error", "Failed to parse update information.");
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        QString downloadUrl;
        QString latestRev;
        QString latestDate;
        QString platformString;

#ifdef Q_OS_WIN
        platformString = "win64-sdl";
#elif defined(Q_OS_LINUX)
        platformString = "linux-sdl";
#elif defined(Q_OS_MAC)
        platformString = "macos-sdl";
#endif
        QJsonObject jsonObj;
        if (updateChannel == "Nightly") {
            QJsonArray jsonArray = jsonDoc.array();
            for (const QJsonValue& value : jsonArray) {
                jsonObj = value.toObject();
                if (jsonObj.contains("prerelease") && jsonObj["prerelease"].toBool()) {
                    break;
                }
            }
            if (!jsonObj.isEmpty()) {
                latestVersion = jsonObj["tag_name"].toString();
            } else {
                QMessageBox::warning(this, "Error", "No pre-releases found.");
                reply->deleteLater();
                emit UpdateComplete();
                return;
            }
        } else {
            jsonObj = jsonDoc.object();
            if (jsonObj.contains("tag_name")) {
                latestVersion = jsonObj["tag_name"].toString();
            } else {
                QMessageBox::warning(this, "Error", "Invalid release data.");
                reply->deleteLater();
                emit UpdateComplete();
                return;
            }
        }

        QJsonArray assets = jsonObj["assets"].toArray();
        bool found = false;

        for (const QJsonValue& assetValue : assets) {
            QJsonObject assetObj = assetValue.toObject();
            if (assetObj["name"].toString().contains(platformString)) {
                downloadUrl = assetObj["browser_download_url"].toString();
                found = true;
                break;
            }
        }

        if (!found) {
            QMessageBox::warning(this, "Error", "No download URL found for the specified asset.");
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        if (Config::LastBuildHash != "") {
            bool isUpdated;
            if (updateChannel == "Release") {
                isUpdated = latestVersion.toStdString() == Config::LastBuildBranch;
            } else {
                isUpdated = latestVersion.right(40).toStdString() == Config::LastBuildHash;
            }

            if (!isUpdated) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(this, "Update confirmation",
                                          "New shadPS4 build detected. Proceed with Update?",
                                          QMessageBox::Yes | QMessageBox::No)) {
                    DownloadUpdate(downloadUrl);
                } else {
                    emit UpdateComplete();
                }
                return;
            }

            if (!isAutoupdate) {
                QMessageBox::information(this, "BBLauncher", "Current build is already updated.");
            }

            emit UpdateComplete();
            return;
        }

        std::chrono::time_point shadWriteTime =
            std::filesystem::last_write_time(Common::shadPs4Executable);

#if __APPLE__
        auto sec =
            std::chrono::duration_cast<std::chrono::seconds>(shadWriteTime.time_since_epoch());
        auto time = date::sys_time<std::chrono::seconds>{sec};
        auto zonedt = date::make_zoned(date::current_zone(), time);
        std::string shadModifiedDateString = date::format("{:%F}", zonedt);
        std::string shadModifiedDateStringUTC = date::format("{:%F}", time);

#else
        auto shadTimePoint = std::chrono::clock_cast<std::chrono::system_clock>(shadWriteTime);
        const std::chrono::zoned_time zonedt{std::chrono::current_zone()->name(), shadTimePoint};
        std::string shadModifiedDateString = std::format("{:%F}", zonedt);
        std::string shadModifiedDateStringUTC = std::format("{:%F}", shadWriteTime);
#endif

        std::string latestVerDateString =
            jsonObj["published_at"].toString().toStdString().substr(0, 10);

        if (shadModifiedDateString == latestVerDateString ||
            shadModifiedDateStringUTC == latestVerDateString) {
            if (isAutoupdate) {
                emit UpdateComplete();
                return;
            } else {
                if (QMessageBox::No ==
                    QMessageBox::question(this, "Update confirmation",
                                          "Latest shadPS4 build has the same date as the currently "
                                          "installed build. Proceed with update?",
                                          QMessageBox::Yes | QMessageBox::No)) {
                    emit UpdateComplete();
                    return;
                }
            }
        } else {
            if (QMessageBox::No ==
                QMessageBox::question(this, "Update confirmation",
                                      "New shadPS4 build detected. Proceed with Update?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                emit UpdateComplete();
                return;
            }
        }

        DownloadUpdate(downloadUrl);
    });
}

void CheckShadUpdate::DownloadUpdate(const QString& downloadUrl) {
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QNetworkRequest request(downloadUrl);
    QNetworkReply* reply = networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this](qint64 bytesReceived, qint64 bytesTotal) {
                if (bytesTotal > 0) {
                    int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
                    emit DownloadProgressed(percentage);
                }
            });

    connect(reply, &QNetworkReply::finished, this, [this, reply, downloadUrl]() {
        if (reply->error() != QNetworkReply::NoError) {
            QString errmsg = ("Network error occurred while trying to access the URL:\n" +
                              downloadUrl + "\n" + reply->errorString());
            QMessageBox::warning(this, "Error", errmsg);
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

#ifdef Q_OS_WIN
        QString tempDownloadPath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/Temp/temp_download_update";
#else
        QString userPath;
        Common::PathToQString(userPath, Common::GetShadUserDir());
        QString tempDownloadPath = userPath + "/temp_download_update";
#endif

        QDir dir(tempDownloadPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString downloadPath = tempDownloadPath + "/temp_download_update.zip";
        QFile file(downloadPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            InstallUpdate(downloadPath);
            emit UpdateComplete();
        } else {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Failed to save the update file at") +
                                     ":\n" + downloadPath);
            emit DownloadProgressed(0);
            emit UpdateComplete();
        }

        reply->deleteLater();
    });
}

void CheckShadUpdate::InstallUpdate(QString zipPath) {
    QString shadPath;
    Common::PathToQString(shadPath, Common::shadPs4Executable.parent_path());

    std::filesystem::remove(Common::shadPs4Executable);
    QMicroz::extract(zipPath, shadPath);

#ifndef Q_OS_WIN
    QString shadFullPath;
    Common::PathToQString(shadPath, Common::shadPs4Executable);
    mode_t permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    std::string exe = Common::shadPs4Executable.string();

    if (chmod(exe.c_str(), permissions) != 0) {
        QMessageBox::information(this, "Error setting permissions",
                                 "Could not set access permissions for " + shadFullPath +
                                     ". Set permissions manually before launching.");
    }
#endif

    if (updateChannel == "Release") {
        Config::LastBuildHash = "unknown";
        Config::LastBuildBranch = latestVersion.toStdString();
    } else {
        Config::LastBuildHash = latestVersion.right(40).toStdString();
        Config::LastBuildBranch = "main";
    }
    Config::LastBuildModified = Config::GetLastModifiedString(Common::shadPs4Executable);
    Config::SaveLauncherSettings();

    std::filesystem::remove(Common::PathFromQString(zipPath));
    QMessageBox::information(this, "Update Complete", "Update process completed");
}

CheckShadUpdate::~CheckShadUpdate() {}
