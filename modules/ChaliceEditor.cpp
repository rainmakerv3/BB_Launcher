// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QInputDialog>
#include <QMessageBox>

#include "ChaliceEditor.h"
#include "ChaliceInfo.h"
#include "settings/PSF/psf.h"
#include "ui_ChaliceEditor.h"

using json = nlohmann::json;

ChaliceEditor::ChaliceEditor(QWidget* parent) : QDialog(parent), ui(new Ui::ChaliceEditor) {
    ui->setupUi(this);
    allDungeons = json::parse(chaliceInfo);
    for (auto& it : allDungeons.items()) {
        for (auto& item : it.value()) {
            flatDungeons.push_back(item);
        }
    }

    GlyphLabelMap = {
        {0, ui->GlyphLabel1}, {1, ui->GlyphLabel2}, {2, ui->GlyphLabel3},
        {3, ui->GlyphLabel4}, {4, ui->GlyphLabel5}, {5, ui->GlyphLabel6},
    };

    GlyphDescMap = {
        {0, ui->DescLabel1}, {1, ui->DescLabel2}, {2, ui->DescLabel3},
        {3, ui->DescLabel4}, {4, ui->DescLabel5}, {5, ui->DescLabel6},
    };

    std::string savePath = PSFdata::getSavePath(Common::installPath);
    ExactSaveDir = Common::GetSaveDir() / savePath / "SPRJ0005";

    QStringList SaveSlotList;
    for (int i = 0; i < 10; i++) {
        std::string slot = "userdata000" + std::to_string(i);
        Savefile = ExactSaveDir / slot;
        std::ifstream file(Savefile, std::ios::in | std::ios::binary);

        file.seekg(8);
        uint time;
        file.read(reinterpret_cast<char*>(&time), 4);
        file.close();

        if (time != 0) {
            SaveSlotList.append(QString::fromStdString(slot));
        }
    }

    ui->saveSlotComboBox->addItems(SaveSlotList);
    ui->altarComboBox->addItem("First Ritual Altar");
    ui->altarComboBox->addItem("Second Ritual Altar");
    ui->altarComboBox->addItem("Third Ritual Altar");
    ui->altarComboBox->addItem("Fourth Ritual Altar");
    ui->altarComboBox->addItem("Fifth Ritual Altar");
    ui->altarComboBox->addItem("Final Ritual Altar");

    LoadChalicesFromSave();

    connect(ui->saveSlotComboBox, &QComboBox::currentIndexChanged, this,
            &ChaliceEditor::LoadChalicesFromSave);

    connect(ui->searchButton, &QPushButton::clicked, this, &ChaliceEditor::SearchDungeons);
    connect(ui->searchLineEdit, &QLineEdit::returnPressed, ui->searchButton, &QPushButton::click);

    connect(ui->farmingButton, &QPushButton::clicked, this,
            [this]() { PopulateCategory("farming"); });
    connect(ui->equipmentButton, &QPushButton::clicked, this,
            [this]() { PopulateCategory("equipment"); });
    connect(ui->gemsButton, &QPushButton::clicked, this,
            [this]() { PopulateCategory("bloodgems"); });
    connect(ui->testingButton, &QPushButton::clicked, this,
            [this]() { PopulateCategory("testing"); });

    connect(ui->dungeonsList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem* current) {
                if (current) {
                    QString itemText = current->text();
                    QString glyph = itemText.left(itemText.indexOf(':'));
                    ui->selectedGlyphLabel->setText("SELECTED GLYPH: " + glyph);
                }
            });

    connect(ui->backupButton, &QPushButton::clicked, this, &ChaliceEditor::CreateManualBackup);
    connect(ui->saveButton, &QPushButton::clicked, this, &ChaliceEditor::SaveDungeon);
}

void ChaliceEditor::LoadChalicesFromSave() {
    std::string slot = ui->saveSlotComboBox->currentText().toStdString();
    if (slot.empty()) {
        QMessageBox::information(this, "Error", "No valid saves detected. Aborting...");
        return;
    }

    Savefile = ExactSaveDir / slot;

    // searches for 40 F0 FF FF - offsets are always the same distance from this value
    std::ifstream file(Savefile, std::ios::in | std::ios::binary);
    fileData.clear();
    fileData = std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>());

    uint filesize = std::filesystem::file_size(Savefile);
    uint invsearch;
    for (uint i = 10000; i < filesize; i++) { // probably safe to start at 10k
        file.seekg(i);
        file.read(reinterpret_cast<char*>(&invsearch), 4);
        if (invsearch == 4294963264) { // converted 40 F0 FF FF
            currentOffset = i;
            break;
        }
    }

    //  Name
    file.seekg(currentOffset - 468);
    std::string name(sizeof(name), '\0');
    file.read(&name[0], sizeof(name));
    name.erase(std::remove(name.begin(), name.end(), '\0'), name.end());
    ui->NameLabel->setText("Character Name: " + QString::fromStdString(name));

    uint8_t empty_flag[] = {
        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    size_t start = currentOffset + 88328;
    for (int i = 0; i < 6; i++) {
        size_t current_offset = start + i * 0x7d;
        if (std::memcmp(&fileData[current_offset], empty_flag, 124) == 0) {
            GlyphLabelMap[i]->setText("Glyph: No Chalice Placed");
            GlyphDescMap[i]->setText("Description: n/a");
            continue;
        }

        bool found = false;
        for (const auto& x : flatDungeons) {
            std::vector<uint8_t> x_bytes;
            for (auto& b : x["bytes"]) {
                x_bytes.push_back(std::stoi(b.get<std::string>(), nullptr, 16));
            }
            if (std::memcmp(&fileData[current_offset], x_bytes.data(), 124) == 0) {
                GlyphLabelMap[i]->setText("Glyph: " +
                                          QString::fromStdString(x["glyph"].get<std::string>()));
                GlyphDescMap[i]->setText("Description: " +
                                         QString::fromStdString(x["desc"].get<std::string>()));
                found = true;
                break;
            }
        }

        if (!found) {
            GlyphLabelMap[i]->setText("Glyph: Story or unknown glyph");
            GlyphDescMap[i]->setText("Description: unknown");
        }
    }
}

void ChaliceEditor::PopulateCategory(std::string category) {
    ui->dungeonsList->clear();
    dungeonListData.clear();

    json choices = allDungeons[category].get<std::vector<json>>();
    for (size_t i = 0; i < choices.size(); ++i) {
        ui->dungeonsList->addItem(QString::fromStdString(choices[i]["glyph"]) + QString(": ") +
                                  QString::fromStdString(choices[i]["desc"]));
        std::vector<uint8_t> dungeonBytes;
        for (auto& b : choices[i]["bytes"]) {
            dungeonBytes.push_back(std::stoi(b.get<std::string>(), nullptr, 16));
        }

        dungeonListData.push_back(dungeonBytes);
    }
}

void ChaliceEditor::SearchDungeons() {
    if (ui->searchLineEdit->text().isEmpty()) {
        QMessageBox::information(this, "Search Term Required",
                                 "Input a search term to search for dungeons");
        return;
    }

    std::string searchTerm = ui->searchLineEdit->text().toStdString();
    std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);

    ui->dungeonsList->clear();
    dungeonListData.clear();

    for (const auto& x : flatDungeons) {
        std::string desc = x["desc"].get<std::string>();
        std::string glyph = x["glyph"].get<std::string>();
        std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
        std::transform(glyph.begin(), glyph.end(), glyph.begin(), ::tolower);

        if (desc.find(searchTerm) != std::string::npos ||
            glyph.find(searchTerm) != std::string::npos) {
            ui->dungeonsList->addItem(QString::fromStdString(x["glyph"]) + QString(": ") +
                                      QString::fromStdString(x["desc"]));

            std::vector<uint8_t> dungeonBytes;
            for (auto& b : x["bytes"]) {
                dungeonBytes.push_back(std::stoi(b.get<std::string>(), nullptr, 16));
            }
            dungeonListData.push_back(dungeonBytes);
        }
    }
}

void ChaliceEditor::CreateManualBackup() {
    bool ok;
    QString saveName =
        QInputDialog::getText(this, "Save Name", "Input Save Name", QLineEdit::Normal, "", &ok);
    if (!ok || saveName.isEmpty()) {
        QMessageBox::warning(this, "No Save Name", "Save name required");
        return;
    }

    const std::filesystem::path BackupsDir = Common::GetBBLFilesPath() / "SaveBackups";
    if (!std::filesystem::exists(BackupsDir / "MANUAL")) {
        std::filesystem::create_directories(BackupsDir / "MANUAL");
    }

    if (std::filesystem::exists(BackupsDir / "MANUAL" / saveName.toStdString())) {
        if (QMessageBox::No ==
            QMessageBox::question(this, "Backup File Exists",
                                  QString("A backup with this name already exists.\n") +
                                      "Do you want to overwrite this backup?",
                                  QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }

    std::string slot = ui->saveSlotComboBox->currentText().toStdString();
    Savefile = ExactSaveDir / slot;
    std::filesystem::path savePath = BackupsDir / "MANUAL" / saveName.toStdString();

    QString qpath;
    Common::PathToQString(qpath, savePath);

    std::filesystem::copy_file(Savefile, savePath,
                               std::filesystem::copy_options::overwrite_existing);
    QMessageBox::information(
        this, "Save Successful",
        "Backup save created: " + qpath +
            "\n\nYou can restore this backup using the Save and Backup Manager");
}

void ChaliceEditor::SaveDungeon() {
    int altarIndex = ui->altarComboBox->currentIndex();
    if (!GlyphLabelMap[altarIndex]->text().contains("No Chalice Placed")) {
        QMessageBox::warning(this, "Cannot Save", "Please select an empty chalice slot to save");
        return;
    }

    if (ui->selectedGlyphLabel->text().contains("no selection")) {
        QMessageBox::warning(this, "No Selected Glyph", "Select a dungeon to save");
        return;
    }

    if (QMessageBox::No ==
        QMessageBox::question(this, "Warning",
                              "This will modify your selected save slot/altar, making a backup "
                              "beforehand is highly recommended, particularly before trying "
                              "experimental dungeons.\n\nProceed with save?",
                              QMessageBox::Yes | QMessageBox::No)) {
        return;
    }

    int dungeonIndex = ui->dungeonsList->currentRow();
    std::vector<uint8_t> dungeonBytes = dungeonListData[dungeonIndex];

    size_t start = currentOffset + 88328 + 0x7d * (altarIndex);
    for (size_t i = 0; i < dungeonBytes.size(); i++) {
        fileData[start + i] = dungeonBytes[i];
    }

    size_t flags_offset = currentOffset + 102704 + 0x7d * (altarIndex);
    uint8_t flag[] = {0x30, 0x00, 0x03, 0xe8, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    for (size_t i = 0; i < sizeof(flag); i++) {
        fileData[flags_offset + i] = flag[i];
    }

    std::string slot = ui->saveSlotComboBox->currentText().toStdString();
    Savefile = ExactSaveDir / slot;
    std::ofstream out(Savefile, std::ios::binary);
    out.write((char*)fileData.data(), fileData.size());

    LoadChalicesFromSave();
}

ChaliceEditor::~ChaliceEditor() {
    delete ui;
}
