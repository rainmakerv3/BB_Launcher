// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

/*

#include <QDir>
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

#include "ShadUpdate.h"
#include "config.h"

#if __APPLE__
#include <date/date.h>
#include <date/tz.h>
#include <date/tz_private.h>
#endif

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

*/
