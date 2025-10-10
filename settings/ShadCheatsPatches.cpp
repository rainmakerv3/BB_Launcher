// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QComboBox>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHoverEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QStringListModel>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "ShadCheatsPatches.h"
#include "modules/Common.h"

QImage icon;
QString gameVersion;
QString qserial;

CheatsPatches::CheatsPatches(std::shared_ptr<IpcClient> client, bool game_running, QWidget* parent)
    : m_ipc_client(client), is_game_running(game_running), QDialog(parent) {
    this->setModal(true);
    setupUI();
    resize(500, 400);
    setWindowTitle("Enable/Disable Patches for Bloodborne");
}

CheatsPatches::~CheatsPatches() {}

void CheatsPatches::setupUI() {
    readGameInfo();
    qserial = QString::fromStdString(Common::game_serial);
    defaultTextEdit = defaultTextEditMSG;
    defaultTextEdit.replace("\\n", "\n");

    QString CHEATS_DIR_QString;
    Common::PathToQString(CHEATS_DIR_QString, Common::GetShadUserDir() / "cheats");

    QString PATCHS_DIR_QString;
    Common::PathToQString(PATCHS_DIR_QString, Common::GetShadUserDir() / "patches");

    QString NameCheatJson = QString::fromStdString(Common::game_serial + "_01.09.json");
    m_cheatFilePath = CHEATS_DIR_QString + "/" + NameCheatJson;

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Create the game info group box
    QGroupBox* gameInfoGroupBox = new QGroupBox();
    QVBoxLayout* gameInfoLayout = new QVBoxLayout(gameInfoGroupBox);
    gameInfoLayout->setAlignment(Qt::AlignTop);

    QPixmap gameImage = QPixmap::fromImage(icon);
    QLabel* gameImageLabel = new QLabel();
    if (!gameImage.isNull()) {
        gameImageLabel->setPixmap(gameImage.scaled(275, 275, Qt::KeepAspectRatio));
    } else {
        gameImageLabel->setText(tr("No Image Available"));
    }
    gameImageLabel->setAlignment(Qt::AlignCenter);
    gameInfoLayout->addWidget(gameImageLabel, 0, Qt::AlignCenter);

    QLabel* gameNameLabel = new QLabel("Bloodborne");
    gameNameLabel->setAlignment(Qt::AlignLeft);
    gameNameLabel->setWordWrap(true);
    gameInfoLayout->addWidget(gameNameLabel);

    QLabel* gameSerialLabel =
        new QLabel(tr("Serial: ") + QString::fromStdString(Common::game_serial));
    gameSerialLabel->setAlignment(Qt::AlignLeft);
    gameInfoLayout->addWidget(gameSerialLabel);

    auto labelFont = font();
    labelFont.setBold(true);
    QLabel* gameVersionLabel =
        new QLabel("IMPORTANT: Patches only work properly with\nversion 1.09, please "
                   "update to version\n1.09 if you have not before using patches.");
    gameVersionLabel->setAlignment(Qt::AlignLeft);
    gameVersionLabel->setFont(labelFont);
    gameInfoLayout->addWidget(gameVersionLabel);

    // Add a text area for instructions and 'Patch' descriptions
    instructionsTextEdit = new QTextEdit();
    instructionsTextEdit->setText(defaultTextEdit);
    instructionsTextEdit->setReadOnly(true);
    instructionsTextEdit->setFixedHeight(290);
    gameInfoLayout->addWidget(instructionsTextEdit);

    // Create the tab widget
    QTabWidget* tabWidget = new QTabWidget();
    QWidget* cheatsTab = new QWidget();
    QWidget* patchesTab = new QWidget();

    // Layouts for the tabs
    QVBoxLayout* cheatsLayout = new QVBoxLayout();
    QVBoxLayout* patchesLayout = new QVBoxLayout();

    // Setup the cheats tab
    QGroupBox* cheatsGroupBox = new QGroupBox();
    rightLayout = new QVBoxLayout(cheatsGroupBox);
    rightLayout->setAlignment(Qt::AlignTop);

    cheatsGroupBox->setLayout(rightLayout);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(cheatsGroupBox);
    scrollArea->setMinimumHeight(490);
    cheatsLayout->addWidget(scrollArea);

    // QListView
    listView_selectFile = new QListView();
    listView_selectFile->setSelectionMode(QAbstractItemView::SingleSelection);
    listView_selectFile->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Add QListView to layout
    QVBoxLayout* fileListLayout = new QVBoxLayout();
    fileListLayout->addWidget(new QLabel(tr("Select Cheat File:")));
    fileListLayout->addWidget(listView_selectFile);
    cheatsLayout->addLayout(fileListLayout, 2);

    // Call the method to fill the list of cheat files
    populateFileListCheats();

    QLabel* repositoryLabel = new QLabel(tr("Repository:"));
    repositoryLabel->setAlignment(Qt::AlignLeft);
    repositoryLabel->setAlignment(Qt::AlignVCenter);

    // Add a combo box and a download button
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(repositoryLabel);
    controlLayout->setAlignment(Qt::AlignLeft);
    QComboBox* downloadComboBox = new QComboBox();

    downloadComboBox->addItem("GoldHEN", "GoldHEN");
    downloadComboBox->addItem("shadPS4", "shadPS4");

    controlLayout->addWidget(downloadComboBox);

    QPushButton* downloadButton = new QPushButton(tr("Download Cheats"));
    connect(downloadButton, &QPushButton::clicked, [this, downloadComboBox]() {
        QString source = downloadComboBox->currentData().toString();
        downloadCheats(source, QString::fromStdString(Common::game_serial), "01.09", true);
    });

    QPushButton* deleteCheatButton = new QPushButton(tr("Delete File"));
    connect(deleteCheatButton, &QPushButton::clicked, [this, CHEATS_DIR_QString]() {
        QStringListModel* model = qobject_cast<QStringListModel*>(listView_selectFile->model());
        if (!model) {
            return;
        }
        QItemSelectionModel* selectionModel = listView_selectFile->selectionModel();
        if (!selectionModel) {
            return;
        }
        QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
        if (selectedIndexes.isEmpty()) {
            QMessageBox::warning(
                this, tr("Delete File"),
                tr("No files selected.") + "\n" +
                    tr("You can delete the cheats you don't want after downloading them."));
            return;
        }
        QModelIndex selectedIndex = selectedIndexes.first();
        QString selectedFileName = model->data(selectedIndex).toString();

        int ret = QMessageBox::warning(
            this, tr("Delete File"),
            QString(tr("Do you want to delete the selected file?\\n%1").replace("\\n", "\n"))
                .arg(selectedFileName),
            QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            QString filePath = CHEATS_DIR_QString + "/" + selectedFileName;
            QFile::remove(filePath);
            populateFileListCheats();
        }
    });

    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QPushButton::clicked, [this]() { QWidget::close(); });

    controlLayout->addWidget(downloadButton);
    controlLayout->addWidget(deleteCheatButton);
    controlLayout->addWidget(closeButton);

    cheatsLayout->addLayout(controlLayout);
    cheatsTab->setLayout(cheatsLayout);

    // Setup the patches tab
    QGroupBox* patchesGroupBox = new QGroupBox();
    patchesGroupBoxLayout = new QVBoxLayout(patchesGroupBox);
    patchesGroupBoxLayout->setAlignment(Qt::AlignTop);
    patchesGroupBox->setLayout(patchesGroupBoxLayout);

    QScrollArea* patchesScrollArea = new QScrollArea();
    patchesScrollArea->setWidgetResizable(true);
    patchesScrollArea->setWidget(patchesGroupBox);
    patchesScrollArea->setMinimumHeight(490);
    patchesLayout->addWidget(patchesScrollArea);

    // List of files in patchesListView
    patchesListView = new QListView();
    patchesListView->setSelectionMode(QAbstractItemView::SingleSelection);
    patchesListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Add new label "Select Patch File:" above the QListView
    QVBoxLayout* patchFileListLayout = new QVBoxLayout();
    patchFileListLayout->addWidget(new QLabel(tr("Select Patch File:")));
    patchFileListLayout->addWidget(patchesListView);
    patchesLayout->addLayout(patchFileListLayout, 2);

    QStringListModel* patchesModel = new QStringListModel();
    patchesListView->setModel(patchesModel);

    QHBoxLayout* patchesControlLayout = new QHBoxLayout();

    QLabel* patchesRepositoryLabel = new QLabel(tr("Repository:"));
    patchesRepositoryLabel->setAlignment(Qt::AlignLeft);
    patchesRepositoryLabel->setAlignment(Qt::AlignVCenter);
    patchesControlLayout->addWidget(patchesRepositoryLabel);

    // Add the combo box with options
    patchesComboBox = new QComboBox();
    patchesComboBox->addItem("shadPS4", "shadPS4");
    patchesComboBox->addItem("GoldHEN", "GoldHEN");
    patchesControlLayout->addWidget(patchesComboBox);

    QPushButton* patchesButton = new QPushButton(tr("Download Patches"));
    connect(patchesButton, &QPushButton::clicked, [this]() {
        QString selectedOption = patchesComboBox->currentData().toString();
        downloadPatches(selectedOption, true);
    });
    patchesControlLayout->addWidget(patchesButton);

    QPushButton* deletePatchButton = new QPushButton(tr("Delete File"));
    connect(deletePatchButton, &QPushButton::clicked, [this, PATCHS_DIR_QString]() {
        QStringListModel* model = qobject_cast<QStringListModel*>(patchesListView->model());
        if (!model) {
            return;
        }
        QItemSelectionModel* selectionModel = patchesListView->selectionModel();
        if (!selectionModel) {
            return;
        }
        QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
        if (selectedIndexes.isEmpty()) {
            QMessageBox::warning(this, tr("Delete File"), tr("No files selected."));
            return;
        }
        QModelIndex selectedIndex = selectedIndexes.first();
        QString selectedFileName = model->data(selectedIndex).toString();

        int ret = QMessageBox::warning(
            this, tr("Delete File"),
            QString(tr("Do you want to delete the selected file?\\n%1").replace("\\n", "\n"))
                .arg(selectedFileName),
            QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            QString fileName = selectedFileName.split('|').first().trimmed();
            QString directoryName = selectedFileName.split('|').last().trimmed();
            QString filePath = PATCHS_DIR_QString + "/" + directoryName + "/" + fileName;

            QFile::remove(filePath);
            createFilesJson(directoryName);
            populateFileListPatches();
        }
    });

    QPushButton* saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &CheatsPatches::onSaveButtonClicked);

    patchesControlLayout->addWidget(deletePatchButton);
    patchesControlLayout->addWidget(saveButton);

    patchesLayout->addLayout(patchesControlLayout);
    patchesTab->setLayout(patchesLayout);

    tabWidget->addTab(cheatsTab, tr("Cheats"));
    tabWidget->addTab(patchesTab, "Patches");

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 1) {
            populateFileListPatches();
        }
    });

    mainLayout->addWidget(gameInfoGroupBox, 1);
    mainLayout->addWidget(tabWidget, 3);

    manager = new QNetworkAccessManager(this);

    setLayout(mainLayout);
}

void CheatsPatches::onSaveButtonClicked() {
    // Get the name of the selected folder in the patchesListView
    QString selectedPatchName;
    QModelIndexList selectedIndexes = patchesListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Error", "No patch selected.");
        return;
    }
    selectedPatchName = patchesListView->model()->data(selectedIndexes.first()).toString();
    int separatorIndex = selectedPatchName.indexOf(" | ");
    selectedPatchName = selectedPatchName.mid(separatorIndex + 3);

    QString patchDir;
    Common::PathToQString(patchDir, Common::GetShadUserDir() / "patches");
    patchDir += "/" + selectedPatchName;

    QString filesJsonPath = patchDir + "/files.json";
    QFile jsonFile(filesJsonPath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Unable to open files.json for reading.");
        return;
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = jsonDoc.object();

    QString selectedFileName;
    QString serial = QString::fromStdString(Common::game_serial);

    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it) {
        QString filePath = it.key();
        QJsonArray idsArray = it.value().toArray();

        if (idsArray.contains(QJsonValue(serial))) {
            selectedFileName = filePath;
            break;
        }
    }

    if (selectedFileName.isEmpty()) {
        QMessageBox::critical(this, "Error", "No patch file found for the current serial.");
        return;
    }

    QString filePath = patchDir + "/" + selectedFileName;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Unable to open the file for reading.");
        return;
    }

    QByteArray xmlData = file.readAll();
    file.close();

    QString newXmlData;
    QXmlStreamWriter xmlWriter(&newXmlData);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    QXmlStreamReader xmlReader(xmlData);
    bool insideMetadata = false;

    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
            if (xmlReader.name() == QStringLiteral("Metadata")) {
                insideMetadata = true;
                xmlWriter.writeStartElement(xmlReader.name().toString());

                QString name = xmlReader.attributes().value("Name").toString();
                bool isEnabled = false;
                bool hasIsEnabled = false;
                bool foundPatchInfo = false;

                // Check and update the isEnabled attribute
                for (const QXmlStreamAttribute& attr : xmlReader.attributes()) {
                    if (attr.name() == QStringLiteral("isEnabled")) {
                        hasIsEnabled = true;
                        auto it = m_patchInfos.find(name);
                        if (it != m_patchInfos.end()) {
                            QCheckBox* checkBox = findCheckBoxByName(it->name);
                            if (checkBox) {
                                foundPatchInfo = true;
                                isEnabled = checkBox->isChecked();
                                xmlWriter.writeAttribute("isEnabled", isEnabled ? "true" : "false");
                            }
                        }
                        if (!foundPatchInfo) {
                            auto maskIt = m_patchInfos.find(name + " (any version)");
                            if (maskIt != m_patchInfos.end()) {
                                QCheckBox* checkBox = findCheckBoxByName(maskIt->name);
                                if (checkBox) {
                                    foundPatchInfo = true;
                                    isEnabled = checkBox->isChecked();
                                    xmlWriter.writeAttribute("isEnabled",
                                                             isEnabled ? "true" : "false");
                                }
                            }
                        }

                    } else {
                        xmlWriter.writeAttribute(attr.name().toString(), attr.value().toString());
                    }
                }

                if (!hasIsEnabled) {
                    auto it = m_patchInfos.find(name);
                    if (it != m_patchInfos.end()) {
                        QCheckBox* checkBox = findCheckBoxByName(it->name);
                        if (checkBox) {
                            foundPatchInfo = true;
                            isEnabled = checkBox->isChecked();
                        }
                    }
                    if (!foundPatchInfo) {
                        auto maskIt = m_patchInfos.find(name + " (any version)");
                        if (maskIt != m_patchInfos.end()) {
                            QCheckBox* checkBox = findCheckBoxByName(maskIt->name);
                            if (checkBox) {
                                foundPatchInfo = true;
                                isEnabled = checkBox->isChecked();
                            }
                        }
                    }
                    xmlWriter.writeAttribute("isEnabled", isEnabled ? "true" : "false");
                }
            } else {
                xmlWriter.writeStartElement(xmlReader.name().toString());
                for (const QXmlStreamAttribute& attr : xmlReader.attributes()) {
                    xmlWriter.writeAttribute(attr.name().toString(), attr.value().toString());
                }
            }
        } else if (xmlReader.isEndElement()) {
            if (xmlReader.name() == QStringLiteral("Metadata")) {
                insideMetadata = false;
            }
            xmlWriter.writeEndElement();
        } else if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
            xmlWriter.writeCharacters(xmlReader.text().toString());
        }
    }

    xmlWriter.writeEndDocument();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Unable to open the file for writing.");
        return;
    }

    QTextStream textStream(&file);
    textStream << newXmlData;
    file.close();

    if (xmlReader.hasError()) {
        QMessageBox::critical(this, "Error",
                              tr("Failed to parse XML: ") + "\n" + xmlReader.errorString());
    } else {
        QMessageBox::information(this, "Success", "Options saved successfully.");
    }
}

QCheckBox* CheatsPatches::findCheckBoxByName(const QString& name) {
    for (int i = 0; i < patchesGroupBoxLayout->count(); ++i) {
        QLayoutItem* item = patchesGroupBoxLayout->itemAt(i);
        if (item) {
            QWidget* widget = item->widget();
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
            if (checkBox) {
                const auto patchName = checkBox->property("patchName");
                if (patchName.isValid() && patchName.toString().toStdString().find(
                                               name.toStdString()) != std::string::npos) {
                    return checkBox;
                }
            }
        }
    }
    return nullptr;
}

void CheatsPatches::populateFileListPatches() {
    QLayoutItem* item;
    while ((item = patchesGroupBoxLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_patchInfos.clear();
        QString patchesDir;
        Common::PathToQString(patchesDir, Common::GetShadUserDir() / "patches");
        QDir dir(patchesDir);

        QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        QStringList matchingFiles;
        QString shadPS4entry = "";

        foreach (const QString& folder, folders) {
            QString folderPath = dir.filePath(folder);
            QDir subDir(folderPath);

            QString filesJsonPath = subDir.filePath("files.json");
            QFile file(filesJsonPath);

            if (file.open(QIODevice::ReadOnly)) {
                QByteArray fileData = file.readAll();
                file.close();

                QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));
                QJsonObject jsonObj = jsonDoc.object();

                for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
                    QString fileName = it.key();
                    QJsonArray serials = it.value().toArray();

                    if (serials.contains(QJsonValue(qserial))) {
                        QString fileEntry = fileName + " | " + folder;
                        if (!matchingFiles.contains(fileEntry)) {
                            if (folder == "shadPS4") {
                                shadPS4entry = fileEntry;
                            }
                            matchingFiles << fileEntry;
                        }
                    }
                }
            }
        }
        QStringListModel* model = new QStringListModel(matchingFiles, this);
        if (shadPS4entry != "") {
            QModelIndexList matches =
                model->match(model->index(0, 0), Qt::DisplayRole, shadPS4entry, 1,
                             Qt::MatchExactly | Qt::MatchCaseSensitive);
            QModelIndex shadPS4Index = matches.first();
            model->moveRow(QModelIndex(), shadPS4Index.row(), QModelIndex(), 0);
        }
        patchesListView->setModel(model);

        connect(
            patchesListView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
       [this]() { QModelIndexList selectedIndexes =
       patchesListView->selectionModel()->selectedIndexes(); if (!selectedIndexes.isEmpty()) {
                    QString selectedText = selectedIndexes.first().data().toString();
                    addPatchesToLayout(selectedText);
                }
            });

        if (!matchingFiles.isEmpty()) {
            QModelIndex firstIndex = model->index(0, 0);
            patchesListView->selectionModel()->select(firstIndex, QItemSelectionModel::Select |
                                                                      QItemSelectionModel::Rows);
            patchesListView->setCurrentIndex(firstIndex);
        }
}

void CheatsPatches::downloadPatches(const QString repository, const bool showMessageBox) {
    QString url;
    if (repository == "shadPS4") {
        url = "https://api.github.com/repos/shadps4-emu/ps4_cheats/contents/PATCHES";
    }
    if (repository == "GoldHEN") {
        url = "https://api.github.com/repos/illusion0001/PS4-PS5-Game-Patch/contents/patches/xml";
    }
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [=, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            reply->deleteLater();

            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
            QJsonArray itemsArray = jsonDoc.array();

            if (itemsArray.isEmpty()) {
                if (showMessageBox) {
                    QMessageBox::warning(this, "Error", "Failed to parse JSON data from HTML.");
                }
                return;
            }

            QDir dir(Common::GetShadUserDir() / "patches");
            QString fullPath = dir.filePath(repository);
            if (!dir.exists(fullPath)) {
                dir.mkpath(fullPath);
            }
            dir.setPath(fullPath);

            foreach (const QJsonValue& value, itemsArray) {
                QJsonObject fileObj = value.toObject();
                QString fileName = fileObj["name"].toString();
                QString filePath = fileObj["path"].toString();
                QString downloadUrl = fileObj["download_url"].toString();

                if (fileName.endsWith(".xml")) {
                    QNetworkRequest fileRequest(downloadUrl);
                    QNetworkReply* fileReply = manager->get(fileRequest);

                    connect(fileReply, &QNetworkReply::finished, [=, this]() {
                        if (fileReply->error() == QNetworkReply::NoError) {
                            QByteArray fileData = fileReply->readAll();
                            QFile localFile(dir.filePath(fileName));
                            if (localFile.open(QIODevice::WriteOnly)) {
                                localFile.write(fileData);
                                localFile.close();
                            } else {
                                if (showMessageBox) {
                                    QMessageBox::warning(
                                        this, "Error",
                                        QString(tr("Failed to save:") + "\n%1").arg(fileName));
                                }
                            }
                        } else {
                            if (showMessageBox) {
                                QMessageBox::warning(
                                    this, "Error",
                                    QString(tr("Failed to download:") + "\n%1").arg(downloadUrl));
                            }
                        }
                        fileReply->deleteLater();
                    });
                }
            }
            if (showMessageBox) {
                QMessageBox::information(this, "Download Complete",
                                         "Download Succesfully Completed");
            }
            // Create the files.json file with the identification of which file to open
            createFilesJson(repository);
            populateFileListPatches();
            compatibleVersionNotice(repository);
        } else {
            if (showMessageBox) {
                QMessageBox::warning(this, "Error",
                                     QString(tr("Failed to retrieve HTML page.") + "\n%1")
                                         .arg(reply->errorString()));
            }
        }
        emit downloadFinished();
    });
}

void CheatsPatches::compatibleVersionNotice(const QString repository) {
    QDir patchesDir(Common::GetShadUserDir() / "patches");
    QDir dir = patchesDir.filePath(repository);

    QStringList xmlFiles = dir.entryList(QStringList() << "*.xml", QDir::Files);
    QSet<QString> appVersionsSet;

    foreach (const QString& xmlFile, xmlFiles) {
        QFile file(dir.filePath(xmlFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "ERROR",
                                 QString(tr("Failed to open file:") + "\n%1").arg(xmlFile));
            continue;
        }

        QXmlStreamReader xmlReader(&file);
        bool foundMatchingID = false;

        while (!xmlReader.atEnd() && !xmlReader.hasError()) {
            QXmlStreamReader::TokenType token = xmlReader.readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (xmlReader.name() == QStringLiteral("ID")) {
                    QString id = xmlReader.readElementText();
                    if (id == qserial) {
                        foundMatchingID = true;
                    }
                } else if (xmlReader.name() == QStringLiteral("Metadata")) {
                    if (foundMatchingID) {
                        QString appVer = xmlReader.attributes().value("AppVer").toString();
                        if (!appVer.isEmpty()) {
                            appVersionsSet.insert(appVer);
                        }
                    }
                }
            }
        }

        if (xmlReader.hasError()) {
            QMessageBox::warning(this, "ERROR",
                                 QString(tr("XML ERROR:") + "\n%1").arg(xmlReader.errorString()));
        }

        if (foundMatchingID) {
            QStringList incompatibleVersions;
            bool hasMatchingVersion = false;

            foreach (const QString& appVer, appVersionsSet) {
                if (appVer != gameVersion) {
                    incompatibleVersions.append(appVer);
                } else {
                    hasMatchingVersion = true;
                }
            }

            if (!incompatibleVersions.isEmpty() ||
                (hasMatchingVersion && incompatibleVersions.isEmpty())) {
                QString message;

                if (!incompatibleVersions.isEmpty()) {
                    QString versionsList = incompatibleVersions.join(", ");
                    message += QString(tr("The game is in version: %1")).arg(gameVersion) + "\n" +
                               QString(tr("The downloaded patch only works on version: %1"))
                                   .arg(versionsList);

                    if (hasMatchingVersion) {
                        message += QString(", %1").arg(gameVersion);
                    }
                    message += QString("\n" + tr("You may need to update your game."));
                }

                if (!message.isEmpty()) {
                    QMessageBox::information(this, "Incompatibility Notice", message);
                }
            }
        }
    }
}

void CheatsPatches::createFilesJson(const QString& repository) {
    QDir dir(Common::GetShadUserDir() / "patches");
    QString fullPath = dir.filePath(repository);
    if (!dir.exists(fullPath)) {
        dir.mkpath(fullPath);
    }
    dir.setPath(fullPath);

    QJsonObject filesObject;
    QStringList xmlFiles = dir.entryList(QStringList() << "*.xml", QDir::Files);

    foreach (const QString& xmlFile, xmlFiles) {
        QFile file(dir.filePath(xmlFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "ERROR",
                                 QString(tr("Failed to open file:") + "\n%1").arg(xmlFile));
            continue;
        }

        QXmlStreamReader xmlReader(&file);
        QJsonArray titleIdsArray;

        while (!xmlReader.atEnd() && !xmlReader.hasError()) {
            QXmlStreamReader::TokenType token = xmlReader.readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (xmlReader.name() == QStringLiteral("ID")) {
                    titleIdsArray.append(xmlReader.readElementText());
                }
            }
        }

        if (xmlReader.hasError()) {
            QMessageBox::warning(this, "ERROR",
                                 QString(tr("XML ERROR:") + "\n%1").arg(xmlReader.errorString()));
        }
        filesObject[xmlFile] = titleIdsArray;
    }

    QFile jsonFile(dir.absolutePath() + "/files.json");
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "ERROR", tr("Failed to open files.json for writing"));
        return;
    }

    QJsonDocument jsonDoc(filesObject);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}

void CheatsPatches::addPatchesToLayout(const QString& filePath) {
    if (filePath == "") {
        return;
    }
    QString folderPath = filePath.section(" | ", 1, 1);

    // Clear existing layout items
    QLayoutItem* item;
    while ((item = patchesGroupBoxLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_patchInfos.clear();

    QDir dir(Common::GetShadUserDir() / "patches");
    QString fullPath = dir.filePath(folderPath);

    if (!dir.exists(fullPath)) {
        QMessageBox::warning(this, "ERROR",
                             QString(tr("Directory does not exist:") + "\n%1").arg(fullPath));
        return;
    }
    dir.setPath(fullPath);

    QString filesJsonPath = dir.filePath("files.json");

    QFile jsonFile(filesJsonPath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "ERROR", tr("Failed to open files.json for reading."));
        return;
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = jsonDoc.object();

    bool patchAdded = false;

    // Iterate over each entry in the JSON file
    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it) {
        QString xmlFileName = it.key();
        QJsonArray idsArray = it.value().toArray();

        // Check if the serial is in the ID list
        if (idsArray.contains(QJsonValue(qserial))) {
            QString xmlFilePath = dir.filePath(xmlFileName);
            QFile xmlFile(xmlFilePath);

            if (!xmlFile.open(QIODevice::ReadOnly)) {
                QMessageBox::warning(
                    this, tr("ERROR"),
                    QString(tr("Failed to open file:") + "\n%1").arg(xmlFile.fileName()));
                continue;
            }
            QXmlStreamReader xmlReader(&xmlFile);
            QString patchName;
            QString patchAuthor;
            QString patchNote;
            QJsonArray patchLines;
            bool isEnabled = false;

            while (!xmlReader.atEnd() && !xmlReader.hasError()) {
                xmlReader.readNext();

                if (xmlReader.tokenType() == QXmlStreamReader::StartElement) {
                    if (xmlReader.name() == QStringLiteral("Metadata")) {
                        QXmlStreamAttributes attributes = xmlReader.attributes();
                        QString appVer = attributes.value("AppVer").toString();
                        if (appVer == gameVersion) {
                            patchName = attributes.value("Name").toString();
                            patchAuthor = attributes.value("Author").toString();
                            patchNote = attributes.value("Note").toString();
                            isEnabled =
                                attributes.value("isEnabled").toString() == QStringLiteral("true");
                        }
                        if (appVer == "mask") {
                            patchName = attributes.value("Name").toString() + " (any version)";
                            patchAuthor = attributes.value("Author").toString();
                            patchNote = attributes.value("Note").toString();
                            isEnabled =
                                attributes.value("isEnabled").toString() == QStringLiteral("true");
                        }
                    } else if (xmlReader.name() == QStringLiteral("PatchList")) {
                        QJsonArray linesArray;
                        while (!xmlReader.atEnd() &&
                               !(xmlReader.tokenType() == QXmlStreamReader::EndElement &&
                                 xmlReader.name() == QStringLiteral("PatchList"))) {
                            xmlReader.readNext();
                            if (xmlReader.tokenType() == QXmlStreamReader::StartElement &&
                                xmlReader.name() == QStringLiteral("Line")) {
                                QXmlStreamAttributes attributes = xmlReader.attributes();
                                QJsonObject lineObject;
                                lineObject["Type"] = attributes.value("Type").toString();
                                lineObject["Address"] = attributes.value("Address").toString();
                                lineObject["Value"] = attributes.value("Value").toString();
                                linesArray.append(lineObject);
                            }
                        }
                        patchLines = linesArray;
                    }
                }

                if (!patchName.isEmpty() && !patchLines.isEmpty()) {
                    QCheckBox* patchCheckBox = new QCheckBox(patchName);
                    patchCheckBox->setProperty("patchName", patchName);
                    patchCheckBox->setChecked(isEnabled);
                    patchesGroupBoxLayout->addWidget(patchCheckBox);

                    PatchInfo patchInfo;
                    patchInfo.name = patchName;
                    patchInfo.author = patchAuthor;
                    patchInfo.note = patchNote;
                    patchInfo.linesArray = patchLines;
                    patchInfo.serial = qserial;
                    m_patchInfos[patchName] = patchInfo;

                    patchCheckBox->installEventFilter(this);

                    patchName.clear();
                    patchAuthor.clear();
                    patchNote.clear();
                    patchLines = QJsonArray();
                    patchAdded = true;
                }
            }
            xmlFile.close();
        }
    }

    // Remove the item from the list view if no patches were added
    // (the game has patches, but not for the current version)
    if (!patchAdded) {
        QStringListModel* model = qobject_cast<QStringListModel*>(patchesListView->model());
        if (model) {
            QStringList items = model->stringList();
            int index = items.indexOf(filePath);
            if (index != -1) {
                items.removeAt(index);
                model->setStringList(items);
            }
        }
    }
}

void CheatsPatches::updateNoteTextEdit(const QString& patchName) {
    if (m_patchInfos.contains(patchName)) {
        const PatchInfo& patchInfo = m_patchInfos[patchName];
        QString text = QString(tr("Name:") + " %1\n" + tr("Author: ") + "%2\n\n%3")
                           .arg(patchInfo.name)
                           .arg(patchInfo.author)
                           .arg(patchInfo.note);

        foreach (const QJsonValue& value, patchInfo.linesArray) {
            QJsonObject lineObject = value.toObject();
            QString type = lineObject["Type"].toString();
            QString address = lineObject["Address"].toString();
            QString patchValue = lineObject["Value"].toString();
        }
        text.replace("\\n", "\n");
        instructionsTextEdit->setText(text);
    }
}

bool showErrorMessage = true;

bool CheatsPatches::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(obj);
        if (checkBox) {
            bool hovered = (event->type() == QEvent::HoverEnter);
            onPatchCheckBoxHovered(checkBox, hovered);
            return true;
        }
    }
    // Pass the event on to base class
    return QWidget::eventFilter(obj, event);
}

void CheatsPatches::onPatchCheckBoxHovered(QCheckBox* checkBox, bool hovered) {
    if (hovered) {
        const auto patchName = checkBox->property("patchName");
        if (patchName.isValid()) {
            updateNoteTextEdit(patchName.toString());
        }
    } else {
        instructionsTextEdit->setText(defaultTextEdit);
    }
}

void CheatsPatches::readGameInfo() {
    std::filesystem::path sce_folder_path = Common::installPath / "sce_sys" / "param.sfo";
    std::filesystem::path game_update_path = Common::installPath;
    std::filesystem::path game_patch_path = Common::installPath;
    game_update_path += "-UPDATE";
    game_patch_path += "-patch";

    if (std::filesystem::exists(game_update_path / "sce_sys" / "param.sfo")) {
        sce_folder_path = game_update_path / "sce_sys" / "param.sfo";
    } else if (std::filesystem::exists(game_patch_path / "sce_sys" / "param.sfo")) {
        sce_folder_path = game_patch_path / "sce_sys" / "param.sfo";
    }

    std::filesystem::path icon_path = Common::installPath / "sce_sys" / "icon0.png";
    QString iconpath;
    Common::PathToQString(iconpath, icon_path);
    icon = QImage(iconpath);

    std::ifstream sfofile(sce_folder_path, std::ios::in | std::ios::binary);
    sfofile.seekg(0x351);
    std::string version(4, '\0');
    sfofile.read(&version[0], 4);
    // gameVersion = "0" + QString::fromStdString(version);
    gameVersion = "01.09";
}

void CheatsPatches::clearListCheats() {
    QLayoutItem* item;
    while ((item = rightLayout->takeAt(0)) != nullptr) {
        QWidget* widget = item->widget();
        if (widget) {
            delete widget;
        } else {
            QLayout* layout = item->layout();
            if (layout) {
                QLayoutItem* innerItem;
                while ((innerItem = layout->takeAt(0)) != nullptr) {
                    QWidget* innerWidget = innerItem->widget();
                    if (innerWidget) {
                        delete innerWidget;
                    }
                    delete innerItem;
                }
                delete layout;
            }
        }
    }
    m_cheats.clear();
    m_cheatCheckBoxes.clear();
}

void CheatsPatches::populateFileListCheats() {
    clearListCheats();

    QString cheatsDir;
    Common::PathToQString(cheatsDir, Common::GetShadUserDir() / "cheats");

    QString fullGameVersion = "01.09";
    QString modifiedGameVersion = fullGameVersion.mid(1);

    QString patternWithFirstChar =
        QString::fromStdString(Common::game_serial) + "_" + fullGameVersion + "*.json";
    QString patternWithoutFirstChar =
        QString::fromStdString(Common::game_serial) + "_" + modifiedGameVersion + "*.json";

    QDir dir(cheatsDir);
    QStringList filters;
    filters << patternWithFirstChar << patternWithoutFirstChar;
    dir.setNameFilters(filters);

    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    QStringList fileNames;

    for (const QFileInfo& fileInfo : fileList) {
        fileNames << fileInfo.fileName();
    }

    QStringListModel* model = new QStringListModel(fileNames, this);
    listView_selectFile->setModel(model);

    connect(listView_selectFile->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this]() {
                QModelIndexList selectedIndexes =
                    listView_selectFile->selectionModel()->selectedIndexes();
                if (!selectedIndexes.isEmpty()) {

                    QString selectedFileName = selectedIndexes.first().data().toString();
                    QString cheatsDir;
                    Common::PathToQString(cheatsDir, Common::GetShadUserDir() / "cheats");

                    QFile file(cheatsDir + "/" + selectedFileName);
                    if (file.open(QIODevice::ReadOnly)) {
                        QByteArray jsonData = file.readAll();
                        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
                        QJsonObject jsonObject = jsonDoc.object();
                        QJsonArray modsArray = jsonObject["mods"].toArray();
                        QJsonArray creditsArray = jsonObject["credits"].toArray();
                        addCheatsToLayout(modsArray, creditsArray);
                    }
                }
            });

    if (!fileNames.isEmpty()) {
        QModelIndex firstIndex = model->index(0, 0);
        listView_selectFile->selectionModel()->select(firstIndex, QItemSelectionModel::Select |
                                                                      QItemSelectionModel::Rows);
        listView_selectFile->setCurrentIndex(firstIndex);
    }
}

void CheatsPatches::downloadCheats(const QString& source, const QString& gameSerial,
                                   const QString& gameVersion, const bool showMessageBox) {

    QString cheatsDir;
    Common::PathToQString(cheatsDir, Common::GetShadUserDir() / "cheats");

    QDir dir(cheatsDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString url;
    if (source == "GoldHEN") {
        url = "https://raw.githubusercontent.com/GoldHEN/GoldHEN_Cheat_Repository/main/json.txt";
    } else if (source == "shadPS4") {
        url = "https://raw.githubusercontent.com/shadps4-emu/ps4_cheats/main/CHEATS_JSON.txt";
    } else {
        QMessageBox::warning(this, tr("Invalid Source"),
                             QString(tr("The selected source is invalid.") + "\n%1").arg(source));
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [=, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jsonData = reply->readAll();
            bool foundFiles = false;

            if (source == "GoldHEN" || source == "shadPS4") {
                QString textContent(jsonData);
                QRegularExpression regex(
                    QString("%1_%2[^=]*\\.json").arg(gameSerial).arg(gameVersion));
                QRegularExpressionMatchIterator matches = regex.globalMatch(textContent);
                QString baseUrl;

                if (source == "GoldHEN") {
                    baseUrl = "https://raw.githubusercontent.com/GoldHEN/GoldHEN_Cheat_Repository/"
                              "main/json/";
                } else {
                    baseUrl = "https://raw.githubusercontent.com/shadps4-emu/ps4_cheats/"
                              "main/CHEATS/";
                }

                while (matches.hasNext()) {
                    QRegularExpressionMatch match = matches.next();
                    QString fileName = match.captured(0);

                    if (!fileName.isEmpty()) {
                        QString newFileName = fileName;
                        int dotIndex = newFileName.lastIndexOf('.');
                        if (dotIndex != -1) {

                            if (source == "GoldHEN") {
                                newFileName.insert(dotIndex, "_GoldHEN");
                            } else {
                                newFileName.insert(dotIndex, "_shadPS4");
                            }
                        }
                        QString fileUrl = baseUrl + fileName;
                        QString localFilePath = dir.filePath(newFileName);

                        if (QFile::exists(localFilePath) && showMessageBox) {
                            QMessageBox::StandardButton reply;
                            reply = QMessageBox::question(
                                this, tr("File Exists"),
                                tr("File already exists. Do you want to replace it?") + "\n" +
                                    newFileName,
                                QMessageBox::Yes | QMessageBox::No);
                            if (reply == QMessageBox::No) {
                                continue;
                            }
                        }
                        QNetworkRequest fileRequest(fileUrl);
                        QNetworkReply* fileReply = manager->get(fileRequest);

                        connect(fileReply, &QNetworkReply::finished, [=, this]() {
                            if (fileReply->error() == QNetworkReply::NoError) {
                                QByteArray fileData = fileReply->readAll();
                                QFile localFile(localFilePath);
                                if (localFile.open(QIODevice::WriteOnly)) {
                                    localFile.write(fileData);
                                    localFile.close();
                                } else {
                                    QMessageBox::warning(
                                        this, tr("Error"),
                                        QString(tr("Failed to save file:") + "\n%1")
                                            .arg(localFilePath));
                                }
                            } else {
                                QMessageBox::warning(this, tr("Error"),
                                                     QString(tr("Failed to download file:") +
                                                             "%1\n\n" + tr("Error") + ":%2")
                                                         .arg(fileUrl)
                                                         .arg(fileReply->errorString()));
                            }
                            fileReply->deleteLater();
                        });

                        foundFiles = true;
                    }
                }
                if (!foundFiles && showMessageBox) {
                    QMessageBox::warning(this, tr("Cheats Not Found"), CheatsNotFound_MSG);
                }
            }
            if (foundFiles && showMessageBox) {
                QMessageBox::information(this, tr("Cheats Downloaded Successfully"),
                                         CheatsDownloadedSuccessfully_MSG);
                populateFileListCheats();
            }

        } else {
            if (showMessageBox) {
                QMessageBox::warning(this, tr("Cheats Not Found"), CheatsNotFound_MSG);
            }
        }
        reply->deleteLater();
        emit downloadFinished();
    });

    connect(reply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError code) {
        if (showMessageBox)
            QMessageBox::warning(this, "Download Error",
                                 QString("Error in response: %1").arg(reply->errorString()));
    });
}

void CheatsPatches::addCheatsToLayout(const QJsonArray& modsArray, const QJsonArray& creditsArray) {
    clearListCheats();
    int maxWidthButton = 0;

    for (const QJsonValue& modValue : modsArray) {
        QJsonObject modObject = modValue.toObject();
        QString modName = modObject["name"].toString();
        QString modType = modObject["type"].toString();

        Cheat cheat;
        cheat.name = modName;
        cheat.type = modType;

        QJsonArray memoryArray = modObject["memory"].toArray();
        for (const QJsonValue& memoryValue : memoryArray) {
            QJsonObject memoryObject = memoryValue.toObject();
            MemoryMod memoryMod;
            memoryMod.offset = memoryObject["offset"].toString();
            memoryMod.on = memoryObject["on"].toString();
            memoryMod.off = memoryObject["off"].toString();
            cheat.memoryMods.append(memoryMod);
        }

        // Check for the presence of 'hint' field
        cheat.hasHint = modObject.contains("hint");

        m_cheats[modName] = cheat;

        if (modType == "checkbox") {
            QCheckBox* cheatCheckBox = new QCheckBox(modName);
            rightLayout->addWidget(cheatCheckBox);
            m_cheatCheckBoxes.append(cheatCheckBox);
            connect(cheatCheckBox, &QCheckBox::toggled, [this, modName](bool checked) {
                if (!is_game_running && checked) {
                    QMessageBox::critical(this, tr("Error"),
                                          tr("Can't apply cheats before the game is started"));
                    uncheckAllCheatCheckBoxes();
                    return;
                }

                applyCheat(modName, checked);
            });
        } else if (modType == "button") {
            QPushButton* cheatButton = new QPushButton(modName);
            cheatButton->adjustSize();
            int buttonWidth = cheatButton->sizeHint().width();
            if (buttonWidth > maxWidthButton) {
                maxWidthButton = buttonWidth;
            }

            // Create a horizontal layout for buttons
            QHBoxLayout* buttonLayout = new QHBoxLayout();
            buttonLayout->setContentsMargins(0, 0, 0, 0);
            buttonLayout->addWidget(cheatButton);
            buttonLayout->addStretch();

            rightLayout->addLayout(buttonLayout);
            connect(cheatButton, &QPushButton::clicked,
                    [this, modName]() { applyCheat(modName, true); });
        }
    }

    // Set minimum and fixed size for all buttons + 20
    for (int i = 0; i < rightLayout->count(); ++i) {
        QLayoutItem* layoutItem = rightLayout->itemAt(i);
        QWidget* widget = layoutItem->widget();
        if (widget) {
            QPushButton* button = qobject_cast<QPushButton*>(widget);
            if (button) {
                button->setMinimumWidth(maxWidthButton);
                button->setFixedWidth(maxWidthButton + 20);
            }
        } else {
            QLayout* layout = layoutItem->layout();
            if (layout) {
                for (int j = 0; j < layout->count(); ++j) {
                    QLayoutItem* innerItem = layout->itemAt(j);
                    QWidget* innerWidget = innerItem->widget();
                    if (innerWidget) {
                        QPushButton* button = qobject_cast<QPushButton*>(innerWidget);
                        if (button) {
                            button->setMinimumWidth(maxWidthButton);
                            button->setFixedWidth(maxWidthButton + 20);
                        }
                    }
                }
            }
        }
    }

    // Set credits label
    QLabel* creditsLabel = new QLabel();
    QString creditsText = tr("Author: ");
    if (!creditsArray.isEmpty()) {
        QStringList authors;
        for (const QJsonValue& credit : creditsArray) {
            authors << credit.toString();
        }
        creditsText += authors.join(", ");
    }
    creditsLabel->setText(creditsText);
    creditsLabel->setAlignment(Qt::AlignLeft);
    rightLayout->addWidget(creditsLabel);
}

void CheatsPatches::applyCheat(const QString& modName, bool enabled) {
    if (!m_cheats.contains(modName))
        return;

    Cheat cheat = m_cheats[modName];

    for (const MemoryMod& memoryMod : cheat.memoryMods) {
        QString value = enabled ? memoryMod.on : memoryMod.off;

        std::string modNameStr = modName.toStdString();
        std::string offsetStr = memoryMod.offset.toStdString();
        std::string valueStr = value.toStdString();

        // Determine if the hint field is present
        bool isHintPresent = m_cheats[modName].hasHint;

        m_ipc_client->sendMemoryPatches(modNameStr, offsetStr, valueStr, "", "", !isHintPresent,
                                        false, MemoryPatcher::PatchMask::None, 0);
    }
}

void CheatsPatches::uncheckAllCheatCheckBoxes() {
    for (auto& cheatCheckBox : m_cheatCheckBoxes) {
        cheatCheckBox->setChecked(false);
    }
    showErrorMessage = true;
}
