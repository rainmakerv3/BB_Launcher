// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDockWidget>
#include <QMessageBox>
#include <pugixml.hpp>

#include "Common.h"
#include "Log.h"
#include "TrophyManager.h"
#include "modules/ui_TrophyManager.h"
#include "settings/config.h"
#include "settings/emulator_settings.h"

bool useEuropeanDateFormat = true;

// PS4 system language IDs map directly to TROP00.XML .. TROP30.XML.
// Index = OrbisSystemServiceParamId language value reported by the system.

static constexpr std::array<std::string_view, 31> s_language_xml_names = {
    "TROP_00.XML", // 00 Japanese
    "TROP_01.XML", // 01 English (US)
    "TROP_02.XML", // 02 French
    "TROP_03.XML", // 03 Spanish (ES)
    "TROP_04.XML", // 04 German
    "TROP_05.XML", // 05 Italian
    "TROP_06.XML", // 06 Dutch
    "TROP_07.XML", // 07 Portuguese (PT)
    "TROP_08.XML", // 08 Russian
    "TROP_09.XML", // 09 Korean
    "TROP_10.XML", // 10 Traditional Chinese
    "TROP_11.XML", // 11 Simplified Chinese
    "TROP_12.XML", // 12 Finnish
    "TROP_13.XML", // 13 Swedish
    "TROP_14.XML", // 14 Danish
    "TROP_15.XML", // 15 Norwegian
    "TROP_16.XML", // 16 Polish
    "TROP_17.XML", // 17 Portuguese (BR)
    "TROP_18.XML", // 18 English (GB)
    "TROP_19.XML", // 19 Turkish
    "TROP_20.XML", // 20 Spanish (LA)
    "TROP_21.XML", // 21 Arabic
    "TROP_22.XML", // 22 French (CA)
    "TROP_23.XML", // 23 Czech
    "TROP_24.XML", // 24 Hungarian
    "TROP_25.XML", // 25 Greek
    "TROP_26.XML", // 26 Romanian
    "TROP_27.XML", // 27 Thai
    "TROP_28.XML", // 28 Vietnamese
    "TROP_29.XML", // 29 Indonesian
    "TROP_30.XML", // 30 Unkrainian
};

static std::filesystem::path GetTrophyXmlPath(const std::filesystem::path& xml_dir,
                                              int system_language) {
    // Try the exact language file first.
    if (system_language >= 0 && system_language < static_cast<int>(s_language_xml_names.size())) {
        auto lang_path = xml_dir / s_language_xml_names[system_language];
        if (std::filesystem::exists(lang_path)) {
            return lang_path;
        }
    }
    // Final fallback: master TROP.XML (always present).
    return xml_dir / "TROP.XML";
}

TrophyViewer::TrophyViewer(QString gameTrpPath) : QMainWindow(), ui(new Ui::TrophyViewer) {
    ui->setupUi(this);
    this->setWindowTitle(tr("Trophy Viewer"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowModality(Qt::ApplicationModal);

    gameTrpPath_ = gameTrpPath;
    auto basepath = Common::PathFromQString(gameTrpPath_);
    std::filesystem::path npbindPath = basepath / "sce_sys" / "npbind.dat";
    NPBindFile npbind;
    if (!npbind.Load(npbindPath.string())) {
        std::string msg = "Failed to load npbind.dat file";
        LogError(msg);
    } else {
        std::vector<std::string> npCommIds = npbind.GetNpCommIds();
        if (npCommIds.empty()) {
            std::string msg = "No NPComm IDs found in npbind.dat";
            LogWarning(msg);
        } else {
            npCommId = npCommIds[0];
        }
    }

    headers << "Unlocked"
            << "Trophy"
            << "Name"
            << "Description"
            << "Time Unlocked"
            << "Type"
            << "ID"
            << "Hidden";

    if (!RefreshValues()) {
        QWidget::close();
        return;
    }

    PopulateTrophyWidget();

    QDockWidget* trophyInfoDock = new QDockWidget("", this);
    QWidget* dockWidget = new QWidget(trophyInfoDock);
    QVBoxLayout* dockLayout = new QVBoxLayout(dockWidget);
    dockLayout->setAlignment(Qt::AlignTop);

    trophyInfoLabel = new QLabel(tr("Progress") + ": 0% (0/0)", dockWidget);
    trophyInfoLabel->setStyleSheet(
        "font-weight: bold; font-size: 16px; color: white; background: #333; padding: 5px;");
    dockLayout->addWidget(trophyInfoLabel);

    // Creates QCheckBox to filter trophies
    showEarnedCheck = new QCheckBox(tr("Show Earned Trophies"), dockWidget);
    showNotEarnedCheck = new QCheckBox(tr("Show Not Earned Trophies"), dockWidget);
    showHiddenCheck = new QCheckBox(tr("Show Hidden Trophies"), dockWidget);
    saveSettings = new QPushButton(tr("Save Settings"), dockWidget);

    // Defines the initial states (all checked)
    showEarnedCheck->setChecked(Config::ShowEarnedTrophy);
    showNotEarnedCheck->setChecked(Config::ShowNotEarnedTrophy);
    showHiddenCheck->setChecked(Config::ShowHiddenTrophy);

    // Adds checkboxes to the layout
    dockLayout->addWidget(showEarnedCheck);
    dockLayout->addWidget(showNotEarnedCheck);
    dockLayout->addWidget(showHiddenCheck);
    dockLayout->addWidget(saveSettings);

    dockWidget->setLayout(dockLayout);
    trophyInfoDock->setWidget(dockWidget);

    // Adds the dock to the left area
    this->addDockWidget(Qt::LeftDockWidgetArea, trophyInfoDock);

    // Connects checkbox signals to update trophy display
    connect(showEarnedCheck, &QCheckBox::checkStateChanged, this,
            &TrophyViewer::updateTableFilters);
    connect(showNotEarnedCheck, &QCheckBox::checkStateChanged, this,
            &TrophyViewer::updateTableFilters);
    connect(showHiddenCheck, &QCheckBox::checkStateChanged, this,
            &TrophyViewer::updateTableFilters);

    connect(saveSettings, &QPushButton::pressed, this, [this]() {
        Config::ShowEarnedTrophy = showEarnedCheck->isChecked();
        Config::ShowNotEarnedTrophy = showNotEarnedCheck->isChecked();
        Config::ShowHiddenTrophy = showHiddenCheck->isChecked();
        Config::SaveLauncherSettings();

        QMessageBox::information(this, "Save Successful", "Trophy settings saved");
    });

    updateTrophyInfo();
    updateTableFilters();

    ui->UnlockButton->setFocus();
    ui->TrophyIDBox->addItems(trpId);
    ui->TrophyIDBox->setCurrentIndex(0);

    TrophyIDChanged();
    UpdateStats();

    connect(ui->TrophyIDBox, &QComboBox::currentTextChanged, this, &TrophyViewer::TrophyIDChanged);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &TrophyViewer::TabChanged);

    connect(ui->UnlockButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::UnlockTrophy();
        UpdateStats();
    });

    connect(ui->LockButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::LockTrophy();
        UpdateStats();
    });

    connect(ui->LockAllButton, &QPushButton::clicked, this, [this] {
        TrophyViewer::LockAllTrophies();
        UpdateStats();
    });
}

void TrophyViewer::updateTrophyInfo() {
    int total = 0;
    int unlocked = 0;

    total += tableWidget->rowCount();
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        if (trpUnlocked[row] == "unlocked")
            unlocked++;
    }

    int AchievementRate = std::round((unlocked * 100.0) / total);
    int progress = (total > 0) ? AchievementRate : 0;
    trophyInfoLabel->setText(
        QString(tr("Progress") + ": %1% (%2/%3)").arg(progress).arg(unlocked).arg(total));
}

void TrophyViewer::updateTableFilters() {
    bool showEarned = showEarnedCheck->isChecked();
    bool showNotEarned = showNotEarnedCheck->isChecked();
    bool showHidden = showHiddenCheck->isChecked();

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        bool visible = true;
        if (trpUnlocked[row] == "unlocked" && !showEarned)
            visible = false;
        if (trpUnlocked[row] == "locked" && !showNotEarned)
            visible = false;
        if (trpHidden[row] == "yes" && !showHidden && trpUnlocked[row] == "locked")
            visible = false;

        tableWidget->setRowHidden(row, !visible);
    }
}

void TrophyViewer::PopulateTrophyWidget() {
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
    tableWidget->setSortingEnabled(true);

    for (int row = 0; auto& icon : icons) {
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setData(Qt::DecorationRole, icon);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(row, 1, item);

        const std::string filename = GetTrpType(trpType[row].at(0));
        const QString typeIcon = ":/" + QString::fromStdString(filename);

        QTableWidgetItem* typeitem = new QTableWidgetItem();
        QImage type_icon =
            QImage(typeIcon).scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        typeitem->setData(Qt::DecorationRole, type_icon);
        typeitem->setFlags(typeitem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(row, 5, typeitem);

        QImage lock_icon;
        trpUnlocked[row] == "unlocked" ? lock_icon = QImage(":/unlocked.png")
                                       : lock_icon = QImage(":/locked.png");
        SetTableIcon(tableWidget, row, 0, lock_icon);

        std::string detailString = trophyDetails[row].toStdString();
        std::size_t newline_pos = 0;
        while ((newline_pos = detailString.find("\n", newline_pos)) != std::string::npos) {
            detailString.replace(newline_pos, 1, " ");
            ++newline_pos;
        }

        if (!trophyNames.isEmpty() && !trophyDetails.isEmpty()) {

            SetTableItem(tableWidget, row, 2, trophyNames[row]);
            SetTableItem(tableWidget, row, 3, QString::fromStdString(detailString));
            SetTableItem(tableWidget, row, 4, trpTimeUnlocked[row]);
            SetTableItem(tableWidget, row, 6, trpId[row]);
            SetTableItem(tableWidget, row, 7, trpHidden[row]);
        }

        tableWidget->verticalHeader()->resizeSection(row, icon.height());
        row++;
    }

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    int width = 16;

    for (int i = 0; i < 9; i++) {
        width += tableWidget->horizontalHeader()->sectionSize(i);
    }

    tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    tableWidget->setColumnWidth(3, 275);

    QVBoxLayout* tablayout = new QVBoxLayout();
    tablayout->addWidget(tableWidget);
    ui->ViewerTab->setLayout(tablayout);
}

void TrophyViewer::SetTableIcon(QTableWidget* parent, int row, int column, QImage icon) {
    QImage resized_icon = icon.scaled(QSize(64, 64), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QLabel* lbl_item = new QLabel();
    lbl_item->setPixmap(QPixmap::fromImage(resized_icon));
    lbl_item->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    parent->setCellWidget(row, column, lbl_item);
}

void TrophyViewer::SetTableItem(QTableWidget* parent, int row, int column, QString str) {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    QLabel* label = new QLabel(str);
    QTableWidgetItem* item = new QTableWidgetItem();
    label->setWordWrap(true);

    if (Config::theme == "Dark") {
        label->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");
    } else {
        label->setStyleSheet("color: black; font-size: 15px; font-weight: bold;");
    }

    // Create shadow effect
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(5);               // Set the blur radius of the shadow
    shadowEffect->setColor(QColor(0, 0, 0, 160)); // Set the color and opacity of the shadow
    shadowEffect->setOffset(2, 2);                // Set the offset of the shadow

    if (Config::theme == "Dark")
        label->setGraphicsEffect(shadowEffect); // Apply shadow effect to the QLabel
    if (column == 4)
        label->setAlignment(Qt::AlignCenter);

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

    auto trophyDir = Common::GetShadUserDir() / "trophy" / npCommId / "Icons";
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
    trophyInfoLabel->setText(QString(tr("Progress") + ": %1% (%2/%3)")
                                 .arg(QString::number(round(AchievementRate)))
                                 .arg(UnlockedCount)
                                 .arg(UnlockedCount + LockedCount));
}

void TrophyViewer::UnlockTrophy() {
    int ID = ui->TrophyIDBox->currentText().toInt();
    std::string filename = npCommId + ".xml";
    auto trophy_file = Common::GetTrophyDir() / filename;

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

                    int64_t timestamp = QDateTime::currentSecsSinceEpoch();

                    if (node.attribute("timestamp").empty()) {
                        node.append_attribute("timestamp") = std::to_string(timestamp).c_str();
                    } else {
                        node.attribute("timestamp").set_value(std::to_string(timestamp).c_str());
                    }
                }
            }
        }
    }

    doc.save_file(trophy_file.c_str());
    RefreshValues();

    QImage unlocked_icon = QImage(":/unlocked.png");
    SetTableIcon(tableWidget, ID, 0, unlocked_icon);
    SetTableItem(tableWidget, ID, 4, trpTimeUnlocked[ID]);

    updateTableFilters();
    ui->LockStatusLabel->setText("unlocked");
    QMessageBox::information(this, "Trophy Unlocked", trophyNames[ID] + " unlocked");
}

void TrophyViewer::LockTrophy() {
    int ID = ui->TrophyIDBox->currentText().toInt();
    std::string filename = npCommId + ".xml";
    auto trophy_file = Common::GetTrophyDir() / filename;

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

    doc.save_file(trophy_file.c_str());
    RefreshValues();

    QImage locked_icon = QImage(":/locked.png");
    SetTableIcon(tableWidget, ID, 0, locked_icon);
    SetTableItem(tableWidget, ID, 4, "");

    updateTableFilters();
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

    std::string filename = npCommId + ".xml";
    auto trophy_file = Common::GetTrophyDir() / filename;

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
        bool current_trophy_unlockstate = node.attribute("unlockstate").as_bool();
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

    doc.save_file(trophy_file.c_str());
    RefreshValues();

    QImage locked_icon = QImage(":/locked.png");
    for (int i = 0; i < 40; i++) {
        SetTableIcon(tableWidget, i, 0, locked_icon);
        SetTableItem(tableWidget, i, 4, "");
    }

    updateTableFilters();
    ui->LockStatusLabel->setText("locked");
    QMessageBox::information(this, "Trophies Reset", "All trophies locked");
}

bool TrophyViewer::RefreshValues() {
    trpId.clear();
    trpHidden.clear();
    trpUnlocked.clear();
    trpType.clear();
    trpPid.clear();
    trophyNames.clear();
    trophyDetails.clear();
    trpTimeUnlocked.clear();
    icons.clear();

    const auto baseTrophyDir = Common::GetShadUserDir() / "trophy" / npCommId;
    QString baseTrophyDirQt;
    Common::PathToQString(baseTrophyDirQt, baseTrophyDir);

    if (!std::filesystem::exists(baseTrophyDir) || std::filesystem::is_empty(baseTrophyDir)) {
        std::filesystem::path path = Common::PathFromQString(gameTrpPath_);
        if (!trp.Extract(path, 0, npCommId, baseTrophyDir)) {
            QMessageBox::warning(this, "Error",
                                 "Error extracting trophy files, a Trophy Key may be required "
                                 "(check shadPS4 global settings)");
            return false;
        }
    }

    std::string filename = npCommId + ".xml";
    auto user_trophy_file = Common::GetTrophyDir() / filename;
    if (!std::filesystem::exists(user_trophy_file)) {
        std::error_code discard;
        std::filesystem::copy_file(baseTrophyDir / "Xml" / "TROPCONF.XML", user_trophy_file,
                                   discard);
    }

    QString iconsPath = baseTrophyDirQt + "/Icons";
    QDir iconsDir(iconsPath);
    QFileInfoList iconDirList = iconsDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo& iconInfo : iconDirList) {
        // Skip files that doesn't start with "trop" or "TROP"
        QString fileName = iconInfo.fileName();
        if (!fileName.startsWith("trop", Qt::CaseInsensitive))
            continue;

        QImage icon = QImage(iconInfo.absoluteFilePath())
                          .scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        icons.push_back(icon);
    }

    QString xmlPath;
    Common::PathToQString(xmlPath, user_trophy_file);
    if (!std::filesystem::exists(user_trophy_file)) {
        std::filesystem::path path = Common::PathFromQString(gameTrpPath_);
        if (!trp.Extract(path, 0, npCommId, baseTrophyDir)) {
            QMessageBox::warning(this, "Error",
                                 "Error extracting trophy files, a Trophy Key may be required "
                                 "(check shadPS4 global settings)");
            return false;
        }
    }

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
                if (reader.attributes().hasAttribute("timestamp")) {
                    QString ts = reader.attributes().value("timestamp").toString();
                    if (ts.length() > 10)
                        trpTimeUnlocked.append("unknown");
                    else {
                        bool ok;
                        qint64 timestampInt = ts.toLongLong(&ok);
                        if (ok) {
                            QDateTime dt = QDateTime::fromSecsSinceEpoch(timestampInt);
                            QString format = useEuropeanDateFormat ? "dd/MM/yyyy\n" : "MM/dd/yyy\n";
                            QString format2 = "h:mm ap";
                            trpTimeUnlocked.append(dt.toString(format) + dt.toString(format2));
                        } else {
                            trpTimeUnlocked.append("unknown");
                        }
                    }
                } else {
                    trpTimeUnlocked.append("");
                }
            } else {
                trpUnlocked.append("locked");
                trpTimeUnlocked.append("");
            }
        }
    }

    auto base_trophy_file = GetTrophyXmlPath(Common::GetShadUserDir() / "trophy" / npCommId / "Xml",
                                             EmulatorSettings.GetConsoleLanguage());
    QString baseXmlPath;
    Common::PathToQString(baseXmlPath, base_trophy_file);

    QFile baseFile(baseXmlPath);
    if (!baseFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Error opening Trophy XML file");
        return false;
    }

    QXmlStreamReader reader2(&baseFile);
    while (!reader2.atEnd() && !reader2.hasError()) {
        reader2.readNext();
        if (reader2.name().toString() == "name" && !trpId.isEmpty()) {
            trophyNames.append(reader2.readElementText());
        }
        if (reader2.name().toString() == "detail" && !trpId.isEmpty()) {
            trophyDetails.append(reader2.readElementText());
        }
    }

    return true;
}

TrophyViewer::~TrophyViewer() {
    delete ui;
}
