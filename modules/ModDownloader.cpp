// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <nlohmann/json.hpp>

#include "ModDownloader.h"
#include "ui_ModDownloader.h"

using json = nlohmann::json;

ModDownloader::ModDownloader(QWidget* parent) : QDialog(parent), ui(new Ui::ModDownloader) {
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    ui->linkLabel->setText(
        "<a href=\"https://www.nexusmods.com/users/myaccount?tab=api%20access\">API Link</a>");
    ui->linkLabel->setOpenExternalLinks(true);

    connect(ui->testButton, &QPushButton::pressed, this, [this]() {
        QString url = "https://api.nexusmods.com/v1/games/bloodborne/mods/194/files.json";
        QNetworkRequest request(url);
        QString apiKey = "gOFUmUvwsVZ3X5gapSrNCyKJVJOHo6TfrfwhvV66oLw2N+sM--h5R8PvAl9rINTZ5q--"
                         "cX/d3M85/lL2zp1pVXrQMw==";
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

                json file_array = jsonDoc["files"];
                for (const auto& element : file_array) {
                    if (element.contains("category_name")) {
                        std::string catName = element.at("category_name").get<std::string>();
                        int fileId = element.at("file_id").get<int>();
                        if (catName == "MAIN") {
                            QMessageBox::information(this, "test", QString::number(fileId));
                        }
                    }
                }
            } else {
                QMessageBox::warning(this, tr("Error"),
                                     QString(tr("Network error:") + "\n" + reply->errorString()));
            }

            reply->deleteLater();
        });
    });

    connect(ui->downloadButton, &QPushButton::pressed, this, [this]() {
        QString url =
            "https://api.nexusmods.com/v1/games/bloodborne/mods/194/files/2166/download_link.json";
        QNetworkRequest request(url);
        QString apiKey = "gOFUmUvwsVZ3X5gapSrNCyKJVJOHo6TfrfwhvV66oLw2N+sM--h5R8PvAl9rINTZ5q--"
                         "cX/d3M85/lL2zp1pVXrQMw==";
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
                        std::string url = element.at("URI").get<std::string>();
                        QMessageBox::information(this, "test", QString::fromStdString(url));
                        break;
                    }
                }
            } else {
                QMessageBox::warning(this, tr("Error"),
                                     QString(tr("Network error:") + "\n" + reply->errorString()));
            }

            reply->deleteLater();
        });
    });

    // https://api.nexusmods.com/v1/users/validate.json
}

ModDownloader::~ModDownloader() {}
