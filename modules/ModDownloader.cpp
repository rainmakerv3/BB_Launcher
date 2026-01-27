// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QProgressBar>
#include <QTimer>
#include <QUuid>
#include <QWebSocket>
#include <QtConcurrent/QtConcurrentRun>
#include <nlohmann/json.hpp>
#include <qmicroz.h>

#include "ModDownloader.h"
#include "settings/config.h"
#include "ui_ModDownloader.h"

#ifndef USE_WEBENGINE
#include <QQuickItem>
#include <QQuickView>
#include <QtWebView>
#else
#include <QFutureWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebEngineProfile>
#include <QWebEngineView>
#endif

using json = nlohmann::json;

ModDownloader::ModDownloader(QWidget* parent) : QDialog(parent), ui(new Ui::ModDownloader) {

    ui->setupUi(this);
    ui->downloadButton->setFocus();
    this->setFixedSize(this->width(), this->height());
    manager = new QNetworkAccessManager(this);

    if (!std::filesystem::exists(Common::ModPath)) {
        std::filesystem::create_directories(Common::ModPath);
    }

    ui->RecModsLabel->setText("<a "
                              "href=\"https://docs.google.com/document/d/"
                              "1H5d-RrOE6q3lKTupkZxZRFKP2Q7zoWMTCL0hsPmYuAo\">Click here to see "
                              "my notes on these mods</a>");
    ui->RecModsLabel->setTextFormat(Qt::RichText);
    ui->RecModsLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->RecModsLabel->setOpenExternalLinks(true);

#ifdef USE_WEBENGINE
    QString storagePath;
    Common::PathToQString(storagePath, Common::GetBBLFilesPath() / "WebCache" / "Data");
    QString cachePath = QFileInfo(storagePath).absolutePath();
    if (!QDir(storagePath).exists()) {
        QDir().mkdir(storagePath);
    }

    profile = new QWebEngineProfile(QStringLiteral("MyPersistentProfile"), this);
    profile->setPersistentStoragePath(storagePath);
    profile->setCachePath(cachePath);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
#endif

    apiKey = QString::fromStdString(Config::ApiKey);
    if (apiKey.isEmpty())
        GetApiKey();

    modIDmap = {
        {0, 109},  // Vertex Explosion Mod
        {1, 70},   // 60 fps Cutscene Fix
        {2, 41},   // Sfx Fix Mods
        {3, 30},   // Xbox Controller Icons
        {4, 182},  // 4k Upscaled UI
        {5, 160},  // Bloodborne Visual Upgrade Mod
        {6, 430},  // SFX Texture Overhaul
        {7, 162},  // Disable FXAA
        {8, 114},  // Cloth physics mods
        {9, 102},  // Performance Drawparams
        {10, 206}, // Estus Vial and Bullet
        {11, 223}, // Stake of Marika
        {12, 257}, // Estus of Marika
        {13, 19},  // Bloodborne Enhanced
        {14, 107}, // More Options at Lamps
        {15, 6},   // Great One Beast Restored
        {16, 156}, // Jump on L3
        {17, 28},  // Boczekek's FPS boost
        {18, 168}, // 2B mod
        {19, 93},  // Debug Menu Restoration (needs debug menu patch enabled)
        {20, 224}, // Start with any Weapon
    };

    ui->modComboBox->addItem("Vertex Explosion Mod");
    ui->modComboBox->addItem("60 fps Cutscene Fix");
    ui->modComboBox->addItem("Sfx Fix Mods");
    ui->modComboBox->addItem("Xbox Controller Icons");
    ui->modComboBox->addItem("4K Upscaled UI");
    ui->modComboBox->addItem("Bloodborne Visual Upgrade Mod");
    ui->modComboBox->addItem("SFX Texture Overhaul");
    ui->modComboBox->addItem("Disable FXAA");
    ui->modComboBox->addItem("Cloth physics mods");
    ui->modComboBox->addItem("Performance Drawparams");
    ui->modComboBox->addItem("Estus Vial and Bullet");
    ui->modComboBox->addItem("Stake of Marika");
    ui->modComboBox->addItem("Estus of Marika");
    ui->modComboBox->addItem("Bloodborne Enhanced");
    ui->modComboBox->addItem("More Options at Lamps");
    ui->modComboBox->addItem("Great One Beast Restored");
    ui->modComboBox->addItem("Jump on L3");
    ui->modComboBox->addItem("Boczekek's FPS boost");
    ui->modComboBox->addItem("2B mod");
    ui->modComboBox->addItem("Debug Menu Restoration (needs debug menu patch enabled)");
    ui->modComboBox->addItem("Start with any Weapon");

    if (apiKey.isEmpty()) {
        ui->validApiLabel->setStyleSheet("color: red;");
        ui->validApiLabel->setText("No Valid Nexus Mods API Key Set");
    } else {
        if (ValidateApi())
            LoadModInfo(109);
    }

    sevenzipPath = Config::SevenZipPath;
    if (sevenzipPath.empty()) {
        if (std::filesystem::exists("C:/Program Files/7-zip/7z.exe")) {
            sevenzipPath = "C:/Program Files/7-zip/7z.exe";
        } else if (std::filesystem::exists("/usr/bin/7z")) {
            sevenzipPath = "/usr/bin/7z";
        } else if (std::filesystem::exists("/usr/bin/p7zip/7z")) {
            sevenzipPath = "/usr/bin/p7zip/7z";
        } else if (std::filesystem::exists("/usr/local/bin/7z")) {
            sevenzipPath = "/usr/local/bin/7z";
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
        GetApiKey();

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
        QString modName = ui->fileListWidget->currentItem()->text();

        if (fileNameList[fileIndex].right(3) != "zip" && sevenzipPath.empty()) {
            QMessageBox::warning(
                this, "Cannot download non-zip file",
                "Selected file is not a zip file. A detected 7-zip installation is required to "
                "download this file. To set the 7-zip installation, install 7-zip if you have not "
                "then select the installed 7-zip exe/binary using the proper button located "
                "towards the bottom part of the this interface.");
            return;
        }

        if (std::filesystem::exists(Common::ModPath / modName.toStdString()) ||
            std::filesystem::exists(ModActivePath / modName.toStdString())) {
            QMessageBox::information(
                this, tr("Mod already exists"),
                tr("%1 is already in your mod folder or active mod folder. Aborting...")
                    .arg(modName));
            return;
        }

        if (isApiKeyPremium) {
            DownloadFilePremium(fileIdList[fileIndex], modIDmap[modIndex], modName);
        } else {
#if defined Q_OS_LINUX and !defined USE_WEBENGINE
            QMessageBox::information(
                this, tr("Not Supported"),
                "Non-premium downloads not supported on this build. Download the Linux-Downloader "
                "build from Nexus Mods or Github for non-premium mod downloads on Linux.");
            return;
#endif
            DownloadFileRegular(fileIdList[fileIndex], modIDmap[modIndex], modName,
                                fileNameList[fileIndex]);
        }
    });

    connect(ui->fileListWidget, &QListWidget::itemClicked, this, [this]() {
        int fileIndex = ui->fileListWidget->currentRow();
        fileDescList[fileIndex].isEmpty() ? ui->fileDesc->setText("No description")
                                          : ui->fileDesc->setHtml(fileDescList[fileIndex]);
    });
}

#ifndef USE_WEBENGINE
void ModDownloader::GetApiKey() {
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString(QUuid::WithoutBraces);
    QJsonValue jsonValueUuid(uuidString);

    QWebSocket* m_webSocket = new QWebSocket();
    QUrl socketUrl = QUrl("wss://sso.nexusmods.com");
    m_webSocket->open(QNetworkRequest(socketUrl));

    authorizationDialog = new QDialog();
    authorizationDialog->setModal(true);
    authorizationDialog->setWindowTitle("Get Authorization");

    QQuickView* webView = new QQuickView();
    QVBoxLayout* layout = new QVBoxLayout(authorizationDialog);

    connect(m_webSocket, &QWebSocket::textMessageReceived, m_webSocket,
            [this, m_webSocket](QString message) {
                QByteArray jsonData = message.toUtf8();
                QJsonDocument doc = QJsonDocument::fromJson(jsonData);

                if (!doc.isNull()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("data") && obj["data"].isObject()) {
                        QJsonObject userObject = obj["data"].toObject();
                        if (userObject.contains("api_key")) {
                            apiKey = userObject["api_key"].toString();
                            Config::ApiKey = apiKey.toStdString();
                            Config::SaveLauncherSettings();
                            authorizationDialog->close();
                            this->raise();
                        }
                    }
                } else {
                    qDebug() << "Failed to parse JSON message";
                }
            });

    connect(m_webSocket, &QWebSocket::connected, this,
            [this, jsonValueUuid, m_webSocket, uuidString, webView, layout]() {
                QJsonObject jsonObject;
                jsonObject["id"] = jsonValueUuid;
                jsonObject["appid"] = "rainmakerv2-bblauncher";
                jsonObject["protocol"] = 2;
                // jsonObject["token"] = NULL;

                QJsonDocument jsonDoc(jsonObject);
                QByteArray jsonByteArray = jsonDoc.toJson();
                m_webSocket->sendTextMessage(QString::fromUtf8(jsonByteArray));

                QTimer* m_pingTimer = new QTimer(this);
                m_pingTimer->setInterval(30000); // 30 seconds
                connect(m_pingTimer, &QTimer::timeout, m_webSocket,
                        [this, m_webSocket]() { m_webSocket->ping(); });
                m_pingTimer->start();

                QString link = "https://www.nexusmods.com/sso?id=" + uuidString +
                               "&application=rainmakerv2-bblauncher";

                QMessageBox::information(this, "Authorization required",
                                         "Click authorize to get your Api information with Nexus "
                                         "mods.");

#ifdef Q_OS_LINUX
                QDesktopServices::openUrl(link);
                QLabel* label =
                    new QLabel("Waiting for authorization (link opened in external browser)",
                               authorizationDialog);
                layout->addWidget(label);

#else
                webView->setSource(QUrl("qrc:/web.qml"));
                QObject* rootObject = webView->rootObject();
                QObject* webViewObject = rootObject->findChild<QObject*>("currentWebView");
                
                webView->setResizeMode(QQuickView::SizeRootObjectToView);
                webViewObject->setProperty("url", link);
                layout->addWidget(QWidget::createWindowContainer(webView));
                authorizationDialog->resize(1280, 720);
#endif
                authorizationDialog->setLayout(layout);
                authorizationDialog->show();
                authorizationDialog->raise();
            });

    QEventLoop authloop;
    connect(authorizationDialog, &QDialog::finished, &authloop, &QEventLoop::quit);
    authloop.exec();

    webView->close();
    webView->deleteLater();
    authorizationDialog->deleteLater();
}
#endif

#ifdef USE_WEBENGINE
void ModDownloader::GetApiKey() {
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString(QUuid::WithoutBraces);
    QJsonValue jsonValueUuid(uuidString);

    QWebSocket* m_webSocket = new QWebSocket();
    QUrl socketUrl = QUrl("wss://sso.nexusmods.com");
    m_webSocket->open(QNetworkRequest(socketUrl));

    authorizationDialog = new QDialog();
    authorizationDialog->setModal(true);
    authorizationDialog->setWindowTitle("Get Authorization");

    connect(m_webSocket, &QWebSocket::textMessageReceived, m_webSocket,
            [this, m_webSocket](QString message) {
                QByteArray jsonData = message.toUtf8();
                QJsonDocument doc = QJsonDocument::fromJson(jsonData);

                if (!doc.isNull()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("data") && obj["data"].isObject()) {
                        QJsonObject userObject = obj["data"].toObject();
                        if (userObject.contains("api_key")) {
                            apiKey = userObject["api_key"].toString();
                            authorizationDialog->close();
                            this->raise();
                        }
                    }
                } else {
                    qDebug() << "Failed to parse JSON message";
                }
            });

    connect(m_webSocket, &QWebSocket::connected, this,
            [this, jsonValueUuid, m_webSocket, uuidString]() {
                QJsonObject jsonObject;
                jsonObject["id"] = jsonValueUuid;
                jsonObject["appid"] = "rainmakerv2-bblauncher";
                jsonObject["protocol"] = 2;
                // jsonObject["token"] = NULL;

                QJsonDocument jsonDoc(jsonObject);
                QByteArray jsonByteArray = jsonDoc.toJson();
                m_webSocket->sendTextMessage(QString::fromUtf8(jsonByteArray));

                QTimer* m_pingTimer = new QTimer(this);
                m_pingTimer->setInterval(30000); // 30 seconds
                connect(m_pingTimer, &QTimer::timeout, m_webSocket,
                        [this, m_webSocket]() { m_webSocket->ping(); });
                m_pingTimer->start();

                QString link = "https://www.nexusmods.com/sso?id=" + uuidString +
                               "&application=rainmakerv2-bblauncher";

                QWebEngineView* webView = new QWebEngineView(profile, authorizationDialog);
                webView->setUrl(QUrl(link));

                QVBoxLayout* layout = new QVBoxLayout(authorizationDialog);
                layout->addWidget(webView);
                authorizationDialog->setLayout(layout);

                authorizationDialog->resize(1280, 720);
                authorizationDialog->show();
                authorizationDialog->raise();
            });

    QEventLoop authloop;
    connect(authorizationDialog, &QDialog::finished, &authloop, &QEventLoop::quit);
    authloop.exec();

    authorizationDialog->deleteLater();
}
#endif

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
                        ui->validApiLabel->setStyleSheet("color: green;");
                        ui->validApiLabel->setText("Valid Non-Premium Nexus Mods API Key Set");
                        isApiKeyPremium = false;
                    } else {
                        ui->validApiLabel->setStyleSheet("color: green;");
                        ui->validApiLabel->setText("Valid premium Api key set");
                        isApiKeyPremium = true;
                    }
                    break;
                }
            }
            valid = true;
        } else {
            ui->validApiLabel->setStyleSheet("color: red;");
            ui->validApiLabel->setText("No Valid Nexus Mods API Key Set");
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
                if (item.key() == "status") {
                    std::string status = item.value().get<std::string>();
                    if (status != "published") {
                        QMessageBox::information(this, "Mod unavailable",
                                                 "This mod is currently not published. It may have "
                                                 "been removed or hidden by the mod owner.");
                        return;
                    }
                }

                if (item.key() == "name") {
                    ui->modNameLabel->setText(
                        "Name: " + QString::fromStdString(item.value().get<std::string>()));
                }

                if (item.key() == "updated_timestamp") {
                    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(item.value().get<int>());
                    QString formattedDateTime = dateTime.toString("yyyy-MM-dd");
                    ui->modUpdatedLabel->setText("Updated: " + formattedDateTime);
                }

                if (item.key() == "author") {
                    ui->modAuthorLabel->setText(
                        "Author: " + QString::fromStdString(item.value().get<std::string>()));
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

            GetModFiles(modId);
        } else {
            ui->validApiLabel->setStyleSheet("color: red;");
            ui->validApiLabel->setText("No Valid Nexus Mods API Key Set");
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
        }

        reply->deleteLater();
    });
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
    fileNameList.clear();

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
                        fileNameList.push_back(
                            QString::fromStdString(element.at("file_name").get<std::string>()));
                    }
                }
            }

            ui->fileListWidget->clear();
            ui->fileListWidget->addItems(fileList);
            ui->fileDesc->setText("Selected File Description");
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Network error:") + "\n" + reply->errorString()));
        }

        reply->deleteLater();
    });
}

void ModDownloader::DownloadFilePremium(int fileId, int ModId, QString modName) {
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
                    StartDownload(downloadUrl, modName, true);
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

void ModDownloader::StartDownload(QString url, QString m_modName, bool isPremium) {
    QNetworkRequest downloadRequest(url);
    if (isPremium)
        downloadRequest.setRawHeader("apikey", apiKey.toUtf8());

    QNetworkAccessManager* downloadManager = new QNetworkAccessManager(this);
    QNetworkReply* downloadReply = downloadManager->get(downloadRequest);

    QDialog* progressDialog = new QDialog(this);
    progressDialog->setWindowTitle(tr("Downloading %1 , please wait...").arg(m_modName));
    progressDialog->setFixedSize(400, 80);

    QLabel* label = new QLabel("Initializing", progressDialog);
    QVBoxLayout* layout = new QVBoxLayout(progressDialog);
    QProgressBar* progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 100);

    layout->addWidget(progressBar);
    layout->addWidget(label);
    progressDialog->setLayout(layout);
    progressDialog->show();

    connect(downloadReply, &QNetworkReply::downloadProgress, this,
            [progressBar, label](qint64 bytesReceived, qint64 bytesTotal) {
                if (bytesTotal > 0)
                    progressBar->setValue(static_cast<int>((bytesReceived * 100) / bytesTotal));
                label->setText(QString("%1 / %2 bytes downloaded")
                                   .arg(QString::number(bytesReceived))
                                   .arg(bytesTotal));
            });

    QString zipPath;
    Common::PathToQString(zipPath, Common::GetBBLFilesPath() / "Temp");
    zipPath = zipPath + "/" + downloadReply->request().url().fileName();

    QString extractPath;
    Common::PathToQString(extractPath, Common::GetBBLFilesPath() / "Temp" / "Download");
    std::filesystem::remove_all(Common::GetBBLFilesPath() / "Temp");
    std::filesystem::create_directories(Common::GetBBLFilesPath() / "Temp" / "Download");

    QFile* file = new QFile(zipPath);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save file."));
        file->deleteLater();
        downloadReply->deleteLater();
        return;
    }

    connect(progressDialog, &QDialog::rejected, this,
            [zipPath, file, downloadReply]() { downloadReply->abort(); });

    connect(downloadReply, &QNetworkReply::readyRead, this,
            [file, downloadReply]() { file->write(downloadReply->readAll()); });

    connect(downloadReply, &QNetworkReply::finished, [=, this]() {
        QString modName = m_modName;
        if (downloadReply->error() == QNetworkReply::NoError) {
            file->flush();
            file->close();
            downloadReply->deleteLater();
            progressDialog->close();
            progressDialog->deleteLater();

            bool isZip = zipPath.right(3) == "zip";
            isZip ? ExtractZip(zipPath, extractPath) : Extract7z(zipPath, extractPath);

            std::filesystem::path sourcePath = Common::PathFromQString(extractPath);
            std::filesystem::path optionsSourcePath;
            const QStringList modsWithOptions = {
                "Jump on L3",
                "Performance Drawparams",
            };

            const std::vector<std::string> BBFolders = {
                "dvdroot_ps4", "action", "chr",    "event", "facegen", "map",   "menu",
                "movie",       "msg",    "mtd",    "obj",   "other",   "param", "paramdef",
                "parts",       "remo",   "script", "sfx",   "shader",  "sound", "font"};

            //  source folder for mods with options
            for (const auto& entry : std::filesystem::recursive_directory_iterator(sourcePath)) {
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();
                    if (std::find(BBFolders.begin(), BBFolders.end(), folderName) !=
                        BBFolders.end()) {

                        if (entry.path().parent_path().filename().string() == "dvdroot_ps4") {
                            optionsSourcePath =
                                entry.path().parent_path().parent_path().parent_path();
                        } else {
                            optionsSourcePath = entry.path().parent_path().parent_path();
                        }
                        break;
                    }
                }
            }

            // get options and show dialog for user to select, then set folder path
            std::filesystem::path folderPath = "";
            for (const QString& name : modsWithOptions) {
                if (modName.contains(name)) {
                    QStringList options;
                    for (const auto& entry :
                         std::filesystem::directory_iterator(optionsSourcePath)) {
                        if (entry.is_directory() && entry.path() != optionsSourcePath &&
                            entry.path().filename().string() != "Backup" &&
                            // LOD Maps and model versions are empty for some reason
                            !entry.path().filename().string().contains("LOD Maps and Models")) {

                            for (const auto& subEntry :
                                 std::filesystem::recursive_directory_iterator(entry.path())) {

                                std::string folderName = subEntry.path().filename().string();
                                if (std::find(BBFolders.begin(), BBFolders.end(), folderName) !=
                                    BBFolders.end()) {

                                    options.append(
                                        QString::fromStdString(entry.path().filename().string()));
                                    break;
                                }
                            }
                        }
                    }

                    std::string option;
                    if (GetOption(options, modName, option)) {
                        folderPath = optionsSourcePath / option;
                    } else {
                        QDir(extractPath).removeRecursively();
                        QFile::remove(zipPath);
                        return;
                    }
                }
            }

            if (folderPath.empty()) {
                for (const auto& entry :
                     std::filesystem::recursive_directory_iterator(sourcePath)) {
                    if (entry.is_directory()) {
                        std::string folderName = entry.path().filename().string();
                        if (std::find(BBFolders.begin(), BBFolders.end(), folderName) !=
                            BBFolders.end()) {

                            folderPath = entry.path().parent_path();
                            break;
                        }
                    }
                }
            }

            // Boczekek: rename mapstudio (maybe case sensitive on non-Windows), insert map folder
            if (modName == "Boczekek's FPS boost") {
                std::filesystem::rename(sourcePath / "MapStudio", sourcePath / "mapstudio");
                std::filesystem::path tempPath =
                    Common::GetBBLFilesPath() / "Temp" / modName.toStdString();
                std::filesystem::create_directory(tempPath);
                std::filesystem::rename(sourcePath, tempPath / "map");
                std::filesystem::rename(tempPath, sourcePath);
                folderPath = sourcePath;
            } else if (modName == "Boczekek's FPS boost Lite") {
                std::filesystem::rename(sourcePath / "map" / "MapStudio",
                                        sourcePath / "map" / "mapstudio");
            }

            std::filesystem::path newPath = Common::ModPath / modName.toStdString();
            std::filesystem::rename(folderPath, newPath);
            QMessageBox::information(
                this, tr("Confirm Download"),
                tr("%1 has been downloaded. You can activate it using the mod manager")
                    .arg(modName));

            QDir(extractPath).removeRecursively();
            QFile::remove(zipPath);
        } else {
            QMessageBox::warning(
                this, tr("Download incomplete"),
                QString(tr("Download incomplete:") + "\n" + downloadReply->errorString()));

            file->flush();
            file->close();
            progressDialog->close();
            progressDialog->deleteLater();
            QFile::remove(zipPath);
        }
    });
}

#ifndef USE_WEBENGINE
void ModDownloader::DownloadFileRegular(int fileId, int ModId, QString modName,
                                        QString modFilename) {
    downloadDialog = new QDialog();
    downloadDialog->setModal(true);
    downloadDialog->setWindowTitle("Download Selected Mod");
    QMessageBox::information(this, "Instructions",
                             "Click the Slow Download button to proceed with the download.");

    QQuickView* webView = new QQuickView();
    webView->setSource(QUrl("qrc:/web.qml"));
    webView->setResizeMode(QQuickView::SizeRootObjectToView);

    QString redirectUrl = "";
    QString fileUrl = "https://www.nexusmods.com/bloodborne/mods/" + QString::number(ModId) +
                      "?tab=files&file_id=" + QString::number(fileId);
    QObject* rootObject = webView->rootObject();
    QObject* webViewObject = rootObject->findChild<QObject*>("currentWebView");
    webViewObject->setProperty("url", fileUrl);

    // Check url every .05 second (lol)
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this,
                     [this, modFilename, webViewObject, fileUrl, &redirectUrl]() {
                         QString url = webViewObject->property("url").toString();
                         QByteArray utf8ByteArray = url.toUtf8();
                         QString urlUtf8 = QString::fromUtf8(utf8ByteArray);

                         if (urlUtf8.contains(modFilename)) {
                             redirectUrl = url;
                             downloadDialog->close();
                         }
                     });

    QVBoxLayout* layout = new QVBoxLayout(downloadDialog);
    layout->addWidget(QWidget::createWindowContainer(webView));
    downloadDialog->setLayout(layout);

    downloadDialog->resize(1280, 720);
    downloadDialog->show();
    downloadDialog->raise();

    QTimer::singleShot(500, this, [timer]() { timer->start(50); });
    QEventLoop downloadloop;
    connect(downloadDialog, &QDialog::finished, &downloadloop, &QEventLoop::quit);
    downloadloop.exec();

    timer->stop();
    downloadDialog->deleteLater();
    webView->close();
    webView->deleteLater();

    if (!redirectUrl.isEmpty())
        StartDownload(redirectUrl, modName, false);
}
#endif

#ifdef USE_WEBENGINE
void ModDownloader::DownloadFileRegular(int fileId, int ModId, QString modName,
                                        QString modFilename) {
    downloadDialog = new QDialog();
    downloadDialog->setModal(true);
    downloadDialog->setWindowTitle("Download Selected Mod");
    QMessageBox::information(this, "Instructions",
                             "Click the Slow Download button to proceed with the download.");

    QWebEngineView* webView = new QWebEngineView(profile, downloadDialog);
    QString fileUrl = "https://www.nexusmods.com/bloodborne/mods/" + QString::number(ModId) +
                      "?tab=files&file_id=" + QString::number(fileId);
    webView->setUrl(QUrl(fileUrl));

    QString downloadUrl = "";
    QObject::connect(profile, &QWebEngineProfile::downloadRequested, this,
                     [this, modFilename, &downloadUrl](QWebEngineDownloadRequest* download) {
                         QString url = download->url().toString();
                         QByteArray utf8ByteArray = url.toUtf8();
                         QString urlUtf8 = QString::fromUtf8(utf8ByteArray);

                         if (urlUtf8.contains(modFilename)) {
                             downloadDialog->close();
                             downloadUrl = url;
                         }
                     });

    QVBoxLayout* layout = new QVBoxLayout(downloadDialog);
    layout->addWidget(webView);
    downloadDialog->setLayout(layout);

    downloadDialog->resize(1280, 720);
    downloadDialog->show();

    QEventLoop downloadloop;
    connect(downloadDialog, &QDialog::finished, &downloadloop, &QEventLoop::quit);
    downloadloop.exec();
    downloadDialog->deleteLater();

    if (!downloadUrl.isEmpty())
        StartDownload(downloadUrl, modName, false);
}
#endif

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

void ModDownloader::Extract7z(QString inpath, QString outpath) {
    QString processPath;
    Common::PathToQString(processPath, sevenzipPath);
    int fileCount = 0;
    QProcess* infoProcess = new QProcess(this);
    connect(infoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [infoProcess, &fileCount](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    QByteArray output = infoProcess->readAllStandardOutput();
                    QString outputStr(output);

                    QRegularExpression regex("(\\d+)\\s+(?:files)\\b");
                    QRegularExpressionMatch match = regex.match(outputStr);

                    if (match.hasMatch()) {
                        QString numberString = match.captured(1);
                        fileCount = numberString.toInt();
                    }
                }
                infoProcess->deleteLater();
            });

    QStringList infoArgs;
    infoArgs << "l" << inpath;

    infoProcess->start(processPath, infoArgs);
    infoProcess->waitForFinished();

    QProcess* extractProcess = new QProcess(this);
    QStringList extractArgs;
    extractArgs << "x";
    extractArgs << QDir::toNativeSeparators(inpath);
    extractArgs << "-o" + QDir::toNativeSeparators(outpath);
    extractArgs << "-bso0";
    extractArgs << "-bsp1";

    QDialog* progressDialog = new QDialog(this);
    progressDialog->setWindowTitle(tr("Extracting compressed archive"));
    progressDialog->setFixedSize(400, 80);
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);

    QLabel* label = new QLabel("Initializing", progressDialog);
    QVBoxLayout* layout = new QVBoxLayout(progressDialog);
    QProgressBar* progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 100);

    layout->addWidget(progressBar);
    if (fileCount != 0)
        layout->addWidget(label);
    progressDialog->setLayout(layout);
    progressDialog->show();

    connect(extractProcess, &QProcess::readyRead, this,
            [this, extractProcess, progressBar, label, fileCount]() {
                QString output = extractProcess->readAllStandardOutput();
                QRegularExpression percentageMessage("^(\\s*)(\\d+)(%.*)$");
                QRegularExpressionMatch match = percentageMessage.match(output);
                if (match.hasMatch()) {
                    const int percentageExported = match.captured(2).toInt();
                    progressBar->setValue(percentageExported);
                    if (fileCount != 0)
                        label->setText(QString("%1 / %2 files extracted")
                                           .arg(QString::number(static_cast<int>(
                                               fileCount * percentageExported / 100.f)))
                                           .arg(fileCount));
                }
            });

    extractProcess->start(processPath, extractArgs);

    QEventLoop extractloop;
    connect(extractProcess, &QProcess::finished, &extractloop, &QEventLoop::quit);
    extractloop.exec();

    progressDialog->close();
    progressDialog->deleteLater();
}

void ModDownloader::ExtractZip(QString inpath, QString outpath) {
    QMicroz qmz(inpath);
    qmz.setOutputFolder(outpath);
    if (!qmz)
        return;

    QDialog* progressDialog = new QDialog(this);
    progressDialog->setWindowTitle(tr("Extracting zip archive"));
    progressDialog->setFixedSize(400, 80);
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);

    QLabel* label = new QLabel("Initializing", progressDialog);
    QVBoxLayout* layout = new QVBoxLayout(progressDialog);
    QProgressBar* progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 100);

    layout->addWidget(progressBar);
    layout->addWidget(label);
    progressDialog->setLayout(layout);
    progressDialog->show();

    connect(this, &ModDownloader::FileExtracted, progressBar,
            [this, &qmz, progressBar, label](int extracted) {
                progressBar->setValue(static_cast<int>((extracted * 100.f) / qmz.count()));
                label->setText(QString("%1 / %2 files extracted").arg(extracted).arg(qmz.count()));
            });

    QFuture<void> future = QtConcurrent::run([this, &qmz, progressBar, label]() {
        for (int i = 0; i < qmz.count(); ++i) {
            qmz.extractIndex(i);
            emit FileExtracted(i);
        }
    });

    QFutureWatcher<void> watcher;
    watcher.setFuture(future);

    QEventLoop extractloop;
    connect(&watcher, &QFutureWatcher<void>::finished, &extractloop, &QEventLoop::quit);
    extractloop.exec();

    progressDialog->close();
    progressDialog->deleteLater();
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

bool ModDownloader::GetOption(QStringList options, QString& modName, std::string& option) {
    bool selected = true;
    QString oldModName = modName;
    QString item = QInputDialog::getItem(this, "Select mod Option",
                                         "Please choose a version of the downloaded mod:", options,
                                         0, false, &selected);

    if (!selected) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Confirm Cancel", "Are you sure you want to cancel download of this mod?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        reply == QMessageBox::Yes ? selected = false
                                  : selected = GetOption(options, modName, option);
    } else {
        modName = modName + QString(" (%1)").arg(item);
        option = item.toStdString();
    }

    if (selected && std::filesystem::exists(Common::ModPath / modName.toStdString()) ||
        std::filesystem::exists(ModActivePath / modName.toStdString())) {
        QMessageBox::information(this, tr("Mod already exists"),
                                 tr("%1 is already in your mod folder or active mod folder. Select "
                                    "another option or cancel")
                                     .arg(modName));

        modName = oldModName;
        selected = GetOption(options, modName, option);
    }

    return selected;
}

ModDownloader::~ModDownloader() {}
