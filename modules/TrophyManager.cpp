// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QMessageBox>
#include <pugixml.hpp>

#include "Common.h"
#include "TrophyManager.h"
#include "modules/ui_TrophyManager.h"

TrophyViewer::TrophyViewer(QString trophyPath, QString gameTrpPath, QWidget* parent)
    : QDialog(parent), trophyFolder(trophyPath), ui(new Ui::TrophyViewer) {
    ui->setupUi(this);
    this->setWindowTitle(tr("Trophy Viewer"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModal(true);

    gameTrpPath_ = gameTrpPath;
    headers << "Unlocked"
            << "Trophy"
            << "Name"
            << "Description"
            << "ID"
            << "Hidden"
            << "Type"
            << "PID";

    if (!RefreshValues(trophyPath)) {
        QWidget::close();
        return;
    }
    PopulateTrophyWidget(trophyPath);

    ui->UnlockButton->setFocus();
    ui->TrophyIDBox->addItems(trpId);
    ui->TrophyIDBox->setCurrentIndex(0);

    TrophyIDChanged();
    UpdateStats();

    connect(ui->TrophyIDBox, &QComboBox::currentTextChanged, this, &TrophyViewer::TrophyIDChanged);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &TrophyViewer::TabChanged);

    connect(ui->UnlockButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::UnlockTrophy();
        RefreshValues(trophyFolder);
        UpdateStats();
    });

    connect(ui->LockButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::LockTrophy();
        RefreshValues(trophyFolder);
        UpdateStats();
    });

    connect(ui->LockAllButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::LockAllTrophies();
        RefreshValues(trophyFolder);
        UpdateStats();
    });
}

void TrophyViewer::PopulateTrophyWidget(QString title) {
    tableWidget = new QTableWidget(this);
    tableWidget->setShowGrid(false);
    tableWidget->setColumnCount(8);
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setRowCount(icons.size());

    for (int row = 0; auto& icon : icons) {
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setData(Qt::DecorationRole, icon);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(row, 1, item);

        std::string detailString = trophyDetails[row].toStdString();

        std::size_t newline_pos = 0;
        while ((newline_pos = detailString.find("\n", newline_pos)) != std::string::npos) {
            detailString.replace(newline_pos, 1, " ");
            ++newline_pos;
        }

        if (!trophyNames.isEmpty() && !trophyDetails.isEmpty()) {
            SetTableItem(tableWidget, row, 0, trpUnlocked[row]);
            SetTableItem(tableWidget, row, 2, trophyNames[row]);
            SetTableItem(tableWidget, row, 3, QString::fromStdString(detailString));
            SetTableItem(tableWidget, row, 4, trpId[row]);
            SetTableItem(tableWidget, row, 5, trpHidden[row]);
            SetTableItem(tableWidget, row, 6, GetTrpType(trpType[row].at(0)));
            SetTableItem(tableWidget, row, 7, trpPid[row]);
        }

        tableWidget->verticalHeader()->resizeSection(row, icon.height());
        row++;
    }

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    int width = 16;

    for (int i = 0; i < 8; i++) {
        width += tableWidget->horizontalHeader()->sectionSize(i);
    }

    QVBoxLayout* tablayout = new QVBoxLayout();
    tablayout->addWidget(tableWidget);
    ui->ViewerTab->setLayout(tablayout);
}

void TrophyViewer::SetTableItem(QTableWidget* parent, int row, int column, QString str) {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    QLabel* label = new QLabel(str);
    QTableWidgetItem* item = new QTableWidgetItem();
    label->setWordWrap(true);
    label->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");

    // Create shadow effect
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(5);               // Set the blur radius of the shadow
    shadowEffect->setColor(QColor(0, 0, 0, 160)); // Set the color and opacity of the shadow
    shadowEffect->setOffset(2, 2);                // Set the offset of the shadow

    label->setGraphicsEffect(shadowEffect); // Apply shadow effect to the QLabel

    layout->addWidget(label);
    if (column != 1 && column != 2 && column != 3)
        layout->setAlignment(Qt::AlignCenter);
    widget->setLayout(layout);
    parent->setItem(row, column, item);
    parent->setCellWidget(row, column, widget);
}

void TrophyViewer::TabChanged() {
    if (ui->tabWidget->currentIndex() == 1) {
        ui->UnlockButton->setFocus();
    }

    if (ui->tabWidget->currentIndex() == 0) {
        tableWidget->viewport()->update();
    }
}

void TrophyViewer::TrophyIDChanged() {
    QString indexstring = ui->TrophyIDBox->currentText();
    int index = indexstring.toInt();
    const auto trophyDir = Common::GetShadUserDir() / "game_data" / Common::game_serial /
                           "TrophyFiles" / "trophy00" / "Icons";
    QString trophyDirQt;
    Common::PathToQString(trophyDirQt, trophyDir);

    QString fileName = "TROP" + indexstring + ".PNG";
    QString iconsPath = trophyDirQt + "/" + fileName;

    ui->TrophyIcon->setPixmap(iconsPath);
    ui->TrophyNameLabel->setText("Trophy Name:\n" + trophyNames[index]);
    ui->LockStatusLabel->setText(trpUnlocked[index]);
}

void TrophyViewer::UpdateStats() {
    int UnlockedCount = 0;
    int LockedCount = 0;

    for (const auto& i : trpUnlocked) {
        if (i == "locked")
            LockedCount = LockedCount + 1;
        if (i == "unlocked")
            UnlockedCount = UnlockedCount + 1;
    }

    ui->LockedNumLabel->setText(QString::number(LockedCount));
    ui->UnlockedNumLabel->setText(QString::number(UnlockedCount));

    float AchievementRate = (UnlockedCount / 40.0) * 100.0;
    ui->AchivementLabel->setText(QString::number(round(AchievementRate)) + "%");
}

void TrophyViewer::UnlockTrophy() {
    int ID = ui->TrophyIDBox->currentText().toInt();

    const auto trophy_dir =
        Common::GetShadUserDir() / "game_data" / Common::game_serial / "TrophyFiles";
    auto trophy_file = trophy_dir / "trophy00" / "Xml" / "TROP.XML";

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(trophy_file.native().c_str());

    if (!result) {
        QMessageBox::critical(this, "Failed to parse trophy xml",
                              QString::fromStdString(result.description()));
        return;
    }

    // *platinumId = ORBIS_NP_TROPHY_INVALID_TROPHY_ID;
    pugi::xml_node platinum_node;
    auto trophyconf = doc.child("trophyconf");

    for (pugi::xml_node& node : trophyconf.children()) {
        int current_trophy_id = node.attribute("id").as_int();
        bool current_trophy_unlockstate = node.attribute("unlockstate").as_bool();
        const char* current_trophy_name = node.child("name").text().as_string();
        std::string_view current_trophy_description = node.child("detail").text().as_string();
        std::string_view current_trophy_type = node.attribute("ttype").value();

        if (std::string_view(node.name()) == "trophy") {
            if (current_trophy_id == ID) {
                if (current_trophy_unlockstate) {
                    QMessageBox::information(this, "Trophy already unlocked",
                                             "Trophy already unlocked");
                    return;
                } else {
                    if (node.attribute("unlockstate").empty()) {
                        node.append_attribute("unlockstate") = "true";
                    } else {
                        node.attribute("unlockstate").set_value("true");
                    }

                    int64_t timestamp = duration_cast<std::chrono::milliseconds>(
                                            std::chrono::system_clock::now().time_since_epoch())
                                            .count();

                    if (node.attribute("timestamp").empty()) {
                        node.append_attribute("timestamp") = std::to_string(timestamp).c_str();
                    } else {
                        node.attribute("timestamp").set_value(std::to_string(timestamp).c_str());
                    }
                }
            }
        }
    }
    doc.save_file((trophy_dir / "trophy00" / "Xml" / "TROP.XML").native().c_str());
    SetTableItem(tableWidget, ID, 0, "unlocked");
    ui->LockStatusLabel->setText("unlocked");
    QMessageBox::information(this, "Trophy Unlocked", trophyNames[ID] + " unlocked");
}

void TrophyViewer::LockTrophy() {
    int ID = ui->TrophyIDBox->currentText().toInt();

    const auto trophy_dir =
        Common::GetShadUserDir() / "game_data" / Common::game_serial / "TrophyFiles";
    auto trophy_file = trophy_dir / "trophy00" / "Xml" / "TROP.XML";

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(trophy_file.native().c_str());

    if (!result) {
        QMessageBox::critical(this, "Failed to parse trophy xml",
                              QString::fromStdString(result.description()));
        return;
    }

    pugi::xml_node platinum_node;
    auto trophyconf = doc.child("trophyconf");

    for (pugi::xml_node& node : trophyconf.children()) {
        int current_trophy_id = node.attribute("id").as_int();
        bool current_trophy_unlockstate = node.attribute("unlockstate").as_bool();
        const char* current_trophy_name = node.child("name").text().as_string();
        std::string_view current_trophy_description = node.child("detail").text().as_string();
        std::string_view current_trophy_type = node.attribute("ttype").value();

        if (current_trophy_type == "P")
            platinum_node = node;

        if (std::string_view(node.name()) == "trophy") {
            if (current_trophy_id == ID) {
                if (!current_trophy_unlockstate) {
                    QMessageBox::information(this, "Trophy already locked",
                                             "Trophy already locked");
                    return;
                } else {
                    if (node.attribute("unlockstate").empty()) {
                        node.append_attribute("unlockstate") = "false";
                    } else {
                        node.attribute("unlockstate").set_value("false");
                    }

                    if (!node.attribute("timestamp").empty()) {
                        node.remove_attribute("timestamp");
                    }
                }
            }
        }
    }
    doc.save_file((trophy_dir / "trophy00" / "Xml" / "TROP.XML").native().c_str());
    SetTableItem(tableWidget, ID, 0, "locked");
    ui->LockStatusLabel->setText("locked");
    QMessageBox::information(this, "Trophy Locked", trophyNames[ID] + " locked");
}

void TrophyViewer::LockAllTrophies() {
    if (QMessageBox::No ==
        QMessageBox::question(this, "Confirm reset",
                              "This will reset all trophies to locked status.\n\nProceed?",
                              QMessageBox::Yes | QMessageBox::No)) {
        return;
    }

    const auto trophy_dir =
        Common::GetShadUserDir() / "game_data" / Common::game_serial / "TrophyFiles";
    auto trophy_file = trophy_dir / "trophy00" / "Xml" / "TROP.XML";

    pugi::xml_document doc;
    pugi::xml_parse_result result =
        doc.load_file(trophy_file.native().c_str(), pugi::parse_minimal);

    if (!result) {
        QMessageBox::critical(this, "Failed to parse trophy xml",
                              QString::fromStdString(result.description()));
        return;
    }

    auto trophyconf = doc.child("trophyconf");

    for (pugi::xml_node& node : trophyconf.children()) {
        int current_trophy_id = node.attribute("id").as_int();
        bool current_trophy_unlockstate = node.attribute("unlockstate").as_bool();
        const char* current_trophy_name = node.child("name").text().as_string();
        std::string_view current_trophy_description = node.child("detail").text().as_string();
        std::string_view current_trophy_type = node.attribute("ttype").value();
        if (std::string_view(node.name()) == "trophy") {
            if (current_trophy_unlockstate) {
                if (node.attribute("unlockstate").empty()) {
                    node.append_attribute("unlockstate") = "false";
                } else {
                    node.attribute("unlockstate").set_value("false");
                }

                if (!node.attribute("timestamp").empty()) {
                    node.remove_attribute("timestamp");
                }
            }
        }
    }
    doc.save_file((trophy_dir / "trophy00" / "Xml" / "TROP.XML").native().c_str());

    for (int i = 0; i < 40; i++) {
        SetTableItem(tableWidget, i, 0, "locked");
    }
    ui->LockStatusLabel->setText("locked");
    QMessageBox::information(this, "Trophies Reset", "All trophies locked");
}

bool TrophyViewer::RefreshValues(QString title) {
    trpId.clear();
    trpHidden.clear();
    trpUnlocked.clear();
    trpType.clear();
    trpPid.clear();
    trophyNames.clear();
    trophyDetails.clear();
    icons.clear();

    const auto trophyDir =
        Common::GetShadUserDir() / "game_data" / Common::game_serial / "TrophyFiles";
    QString trophyDirQt;
    Common::PathToQString(trophyDirQt, trophyDir);

    QDir dir(trophyDirQt);
    if (!dir.exists() || dir.isEmpty()) {
        std::filesystem::path path = Common::PathFromQString(gameTrpPath_);
        if (!trp.Extract(path, title.toStdString())) {
            QMessageBox::warning(this, "Error",
                                 "Error extracting trophy files, a Trophy Key may be required "
                                 "(check shadPS4 settings)");
            return false;
        }
    }

    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& dirInfo : dirList) {
        QString fileName = dirInfo.fileName();
        QString trpDir = trophyDirQt + "/" + fileName;

        QString iconsPath = trpDir + "/Icons";
        QDir iconsDir(iconsPath);
        QFileInfoList iconDirList = iconsDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

        for (const QFileInfo& iconInfo : iconDirList) {
            QImage icon =
                QImage(iconInfo.absoluteFilePath())
                    .scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            icons.push_back(icon);
        }

        QString xmlPath = trpDir + "/Xml/TROP.XML";
        QFile file(xmlPath);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Error", "Error opening Trophy XML file");
            return false;
        }

        QXmlStreamReader reader(&file);
        while (!reader.atEnd() && !reader.hasError()) {
            reader.readNext();
            if (reader.isStartElement() && reader.name().toString() == "trophy") {
                trpId.append(reader.attributes().value("id").toString());
                trpHidden.append(reader.attributes().value("hidden").toString());
                trpType.append(reader.attributes().value("ttype").toString());
                trpPid.append(reader.attributes().value("pid").toString());
                if (reader.attributes().hasAttribute("unlockstate")) {
                    if (reader.attributes().value("unlockstate").toString() == "true") {
                        trpUnlocked.append("unlocked");
                    } else {
                        trpUnlocked.append("locked");
                    }
                } else {
                    trpUnlocked.append("locked");
                }
            }

            if (reader.name().toString() == "name" && !trpId.isEmpty()) {
                trophyNames.append(reader.readElementText());
            }

            if (reader.name().toString() == "detail" && !trpId.isEmpty()) {
                trophyDetails.append(reader.readElementText());
            }
        }
    }
    return true;
}

TrophyViewer::~TrophyViewer() {
    delete ui;
}
