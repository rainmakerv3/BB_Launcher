// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QDesktopServices>
#include <QMessageBox>

#include "SaveManager.h"
#include "modules/ui_SaveManager.h"
#include "settings/PSF/psf.h"
#include "settings/config.h"

SaveManager::SaveManager(QWidget* parent) : QDialog(parent), ui(new Ui::SaveManager) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    // Releases older than 0.9.0 will need to use the game serial as save folder
    std::string savePath =
        Config::isReleaseOlder(9) ? Common::game_serial : PSFdata::getSavePath(Common::installPath);
    ExactSaveDir = Common::GetSaveDir() / "1" / savePath / "SPRJ0005";

    if (!std::filesystem::exists(BackupsDir)) {
        std::filesystem::create_directories(BackupsDir);
    }

    for (auto& FolderEntry : std::filesystem::directory_iterator(BackupsDir)) {
        if (FolderEntry.is_directory()) {
            if (!std::filesystem::is_empty(FolderEntry)) {
                std::string Foldername = Common::PathToU8(FolderEntry.path().filename());
                ui->SelectSaveComboBox->addItem(QString::fromStdString(Foldername));
            }
        }
    }

    PopulateGameSaveSlots();

    ui->LinkLabel->setText("<a href=\"https://www.nexusmods.com/bloodborne/mods/108/\">BBLauncher "
                           "cannot edit saves, check out a great save editor here c/o Nox!</a>");
    ui->LinkLabel->setTextFormat(Qt::RichText);
    ui->LinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->LinkLabel->setOpenExternalLinks(true);

    connect(ui->ManualBackupButton, &QPushButton::pressed, this,
            &SaveManager::CreateManualBackupPressed);
    connect(ui->RestoreButton, &QPushButton::pressed, this, &SaveManager::RestoreBackupPressed);
    connect(ui->DeleteBackupButton, &QPushButton::pressed, this,
            &SaveManager::DeleteManualBackupPressed);
    connect(ui->RestoreFolderButton, &QPushButton::pressed, this,
            &SaveManager::RestoreBackupFolderPressed);

    connect(ui->SelectSaveComboBox, &QComboBox::currentIndexChanged, this,
            &SaveManager::OnSelectBackupSaveChanged);
    connect(ui->SaveSlotComboBox, &QComboBox::currentIndexChanged, this,
            &SaveManager::OnGameSaveSlotChanged);
    connect(ui->BackupSaveSlotComboBox, &QComboBox::currentIndexChanged, this,
            &SaveManager::OnBackupSaveSlotChanged);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(ui->saveFolderButton, &QPushButton::clicked, this, [this]() {
        if (!std::filesystem::exists(ExactSaveDir)) {
            QMessageBox::information(this, "Error", "Save folder does not exist");
            return;
        }

        QString QSavePath;
        Common::PathToQString(QSavePath, ExactSaveDir);
        QDesktopServices::openUrl(QUrl::fromLocalFile(QSavePath));
    });

    ui->SelectSaveComboBox->setCurrentIndex(0);
    OnSelectBackupSaveChanged();
}

void SaveManager::PopulateGameSaveSlots() {
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

    ui->OverwriteSlotComboBox->clear();
    ui->OverwriteSlotComboBox->addItems(SaveSlotList);

    ui->SaveSlotComboBox->clear();
    ui->SaveSlotComboBox->addItems(SaveSlotList);

    saveslot = "userdata0000";
    ui->SaveSlotComboBox->setCurrentText(QString::fromStdString(saveslot));
    Savefile = ExactSaveDir / saveslot;
    UpdateGameSaveValues();
}

void SaveManager::UpdateGameSaveValues() {
    if (saveslot == "") {
        return;
    }

    Savefile = ExactSaveDir / saveslot;

    // searches for 40 F0 FF FF - offsets are always the same distance from this value
    std::ifstream file(Savefile, std::ios::in | std::ios::binary);
    uint filesize = std::filesystem::file_size(Savefile);
    uint offset;
    uint invsearch;
    for (uint i = 10000; i < filesize; i++) { // probably safe to start at 10k
        file.seekg(i);
        file.read(reinterpret_cast<char*>(&invsearch), 4);
        if (invsearch == 4294963264) { // converted 40 F0 FF FF
            offset = i;
            break;
        }
    }

    // Playtime
    file.seekg(8);
    uint time;
    file.read(reinterpret_cast<char*>(&time), 4);
    int hours = time / 3600000;
    int remainder = time % 3600000;
    int minutes = remainder / 60000;

    QString Qhours = QString::number(hours);
    QString Qminutes = QString("%1").arg(minutes, 2, 10, QChar('0'));
    ui->TimeLineEdit->setText(Qhours + ":" + Qminutes);

    //  Name
    file.seekg(offset - 468);
    std::string name(sizeof(name), '\0');
    file.read(&name[0], sizeof(name));
    name.erase(std::remove(name.begin(), name.end(), '\0'), name.end());
    ui->NameLineEdit->setText(QString::fromStdString(name));

    // Str
    file.seekg(offset - 548);
    uint str;
    file.read(reinterpret_cast<char*>(&str), 4);
    ui->StrValLabel->setText(QString::number(str));

    // Skl
    file.seekg(offset - 540);
    uint skl;
    file.read(reinterpret_cast<char*>(&skl), 4);
    ui->SklValLabel->setText(QString::number(skl));

    // Vit
    file.seekg(offset - 572);
    uint vit;
    file.read(reinterpret_cast<char*>(&vit), 4);
    ui->VitValLabel->setText(QString::number(vit));

    // End
    file.seekg(offset - 564);
    uint end;
    file.read(reinterpret_cast<char*>(&end), 4);
    ui->EndValLabel->setText(QString::number(end));

    // Bloodtinge
    file.seekg(offset - 532);
    uint btg;
    file.read(reinterpret_cast<char*>(&btg), 4);
    ui->BTValLabel->setText(QString::number(btg));

    // Arcane
    file.seekg(offset - 524);
    uint arc;
    file.read(reinterpret_cast<char*>(&arc), 4);
    ui->ArcValLabel->setText(QString::number(arc));

    // Level
    file.seekg(offset - 492);
    uint lvl;
    file.read(reinterpret_cast<char*>(&lvl), sizeof(lvl));
    ui->LevelValueLabel->setText(QString::number(lvl));

    // NG
    file.seekg(offset + 68362);
    uint NGnum;
    file.read(reinterpret_cast<char*>(&NGnum), 4);
    ui->NGValueLabel->setText(QString::number(NGnum));
    file.close();
}

void SaveManager::UpdateBackupSaveValues() {
    if (backupsaveslot == "" || ui->SelectSaveComboBox->currentText().isEmpty()) {
        return;
    }

    if (ui->SelectSaveComboBox->currentText() == "MANUAL") {
        Savefile = BackupsDir / "MANUAL" / backupsaveslot;
    } else {
        std::string BackupFolder = ui->SelectSaveComboBox->currentText().toStdString();
        Savefile = BackupsDir / BackupFolder / "SPRJ0005" / backupsaveslot;
    }

    // searches for 40 F0 FF FF - offsets are always the same distance from this value
    std::ifstream file(Savefile, std::ios::in | std::ios::binary);
    uint filesize = std::filesystem::file_size(Savefile);
    uint offset;
    uint invsearch;
    for (uint i = 10000; i < filesize; i++) { // probably safe to start at 10k
        file.seekg(i);
        file.read(reinterpret_cast<char*>(&invsearch), 4);
        if (invsearch == 4294963264) { // converted 40 F0 FF FF
            offset = i;
            break;
        }
    }

    // Playtime
    file.seekg(8);
    uint time;
    file.read(reinterpret_cast<char*>(&time), 4);
    int hours = time / 3600000;
    int remainder = time % 3600000;
    int minutes = remainder / 60000;

    QString Qhours = QString::number(hours);
    QString Qminutes = QString("%1").arg(minutes, 2, 10, QChar('0'));
    ui->BackupTimeLineEdit->setText(Qhours + ":" + Qminutes);

    //  Name
    file.seekg(offset - 468);
    std::string name(sizeof(name), '\0');
    file.read(&name[0], sizeof(name));
    name.erase(std::remove(name.begin(), name.end(), '\0'), name.end());
    ui->BackupNameLineEdit->setText(QString::fromStdString(name));

    // Str
    file.seekg(offset - 548);
    uint str;
    file.read(reinterpret_cast<char*>(&str), 4);
    ui->BackupStrValLabel->setText(QString::number(str));

    // Skl
    file.seekg(offset - 540);
    uint skl;
    file.read(reinterpret_cast<char*>(&skl), 4);
    ui->BackupSklValLabel->setText(QString::number(skl));

    // Vit
    file.seekg(offset - 572);
    uint vit;
    file.read(reinterpret_cast<char*>(&vit), 4);
    ui->BackupVitValLabel->setText(QString::number(vit));

    // End
    file.seekg(offset - 564);
    uint end;
    file.read(reinterpret_cast<char*>(&end), 4);
    ui->BackupEndValLabel->setText(QString::number(end));

    // Bloodtinge
    file.seekg(offset - 532);
    uint btg;
    file.read(reinterpret_cast<char*>(&btg), 4);
    ui->BackupBTValLabel->setText(QString::number(btg));

    // Arcane
    file.seekg(offset - 524);
    uint arc;
    file.read(reinterpret_cast<char*>(&arc), 4);
    ui->BackupArcValLabel->setText(QString::number(arc));

    // Level
    file.seekg(offset - 492);
    uint lvl;
    file.read(reinterpret_cast<char*>(&lvl), sizeof(lvl));
    ui->BackupLevelValueLabel->setText(QString::number(lvl));

    // NG
    file.seekg(offset + 68362);
    uint NGnum;
    file.read(reinterpret_cast<char*>(&NGnum), 4);
    ui->BackupNGValueLabel->setText(QString::number(NGnum));
    file.close();
}

void SaveManager::OnGameSaveSlotChanged() {
    saveslot = ui->SaveSlotComboBox->currentText().toStdString();
    if (saveslot == "") {
        ui->NameLineEdit->setText("-");
        ui->TimeLineEdit->setText("-");
        ui->StrValLabel->setText("-");
        ui->SklValLabel->setText("-");
        ui->VitValLabel->setText("-");
        ui->EndValLabel->setText("-");
        ui->BTValLabel->setText("-");
        ui->ArcValLabel->setText("-");
        ui->LevelValueLabel->setText("-----");
        ui->NGValueLabel->setText("-----");
    } else {
        UpdateGameSaveValues();
    }
}

void SaveManager::OnBackupSaveSlotChanged() {
    backupsaveslot = ui->BackupSaveSlotComboBox->currentText().toStdString();
    if (backupsaveslot == "" || ui->SelectSaveComboBox->currentText().isEmpty()) {
        ui->BackupNameLineEdit->setText("-");
        ui->BackupTimeLineEdit->setText("-");
        ui->BackupStrValLabel->setText("-");
        ui->BackupSklValLabel->setText("-");
        ui->BackupVitValLabel->setText("-");
        ui->BackupEndValLabel->setText("-");
        ui->BackupBTValLabel->setText("-");
        ui->BackupArcValLabel->setText("-");
        ui->BackupLevelValueLabel->setText("-----");
        ui->BackupNGValueLabel->setText("-----");
    } else {
        UpdateBackupSaveValues();
    }
}

void SaveManager::OnSelectBackupSaveChanged() {
    if (ui->SelectSaveComboBox->currentText() == "MANUAL") {
        ui->RestoreButton->show();
        ui->RestoreFolderButton->hide();
        ui->OverwriteSlotLabel->show();
        ui->OverwriteSlotComboBox->show();
        ui->DeleteBackupButton->show();
    } else {
        ui->RestoreButton->show();
        ui->RestoreFolderButton->show();
        ui->OverwriteSlotLabel->show();
        ui->OverwriteSlotComboBox->show();
        ui->DeleteBackupButton->hide();
    }

    ui->BackupSaveSlotComboBox->clear();
    QStringList BackupSlotList;
    if (ui->SelectSaveComboBox->currentText() == "MANUAL") {
        if (std::filesystem::exists(BackupsDir / "MANUAL")) {
            QString Slot;
            for (auto& FileEntry : std::filesystem::directory_iterator(BackupsDir / "MANUAL")) {
                QString Slot = QString::fromStdString(FileEntry.path().filename().string());
                BackupSlotList.append(Slot);
            }
        }
        ui->BackupSaveSlotComboBox->addItems(BackupSlotList);
    } else {
        for (int i = 0; i < 10; i++) {
            std::string BackupFolder = ui->SelectSaveComboBox->currentText().toStdString();
            std::string slot = "userdata000" + std::to_string(i);

            Savefile = BackupsDir / BackupFolder / "SPRJ0005" / slot;
            std::ifstream file(Savefile, std::ios::in | std::ios::binary);
            file.seekg(8);
            uint time;
            file.read(reinterpret_cast<char*>(&time), 4);
            file.close();
            if (time != 0) {
                BackupSlotList.append(QString::fromStdString(slot));
            }
        }
        ui->BackupSaveSlotComboBox->addItems(BackupSlotList);
    }

    ui->BackupSaveSlotComboBox->setCurrentIndex(0);
    backupsaveslot = ui->BackupSaveSlotComboBox->currentText().toStdString();
    UpdateBackupSaveValues();
}

void SaveManager::CreateManualBackupPressed() {
    if (ui->ManualSaveLineEdit->text() == "") {
        QMessageBox::warning(this, "No Slot Name",
                             "Input a file name for your manual backup save.");
        return;
    }

    if (!std::filesystem::exists(BackupsDir / "MANUAL")) {
        std::filesystem::create_directories(BackupsDir / "MANUAL");
    }

    std::string savename = ui->ManualSaveLineEdit->text().toStdString();
    Savefile = ExactSaveDir / saveslot;
    std::filesystem::copy_file(Savefile, BackupsDir / "MANUAL" / savename);
    QMessageBox::information(this, "Save Successful",
                             "Backup save created: " + QString::fromStdString(savename));

    if (ui->SelectSaveComboBox->findText("MANUAL") == -1)
        ui->SelectSaveComboBox->addItem("MANUAL");

    if (ui->SelectSaveComboBox->currentText().isEmpty())
        ui->SelectSaveComboBox->setCurrentText("MANUAL");
}

void SaveManager::DeleteManualBackupPressed() {
    QString savename = ui->BackupSaveSlotComboBox->currentText();
    if (QMessageBox::Yes == QMessageBox::question(this, "Delete Save Confirmation",
                                                  "This will permanently delete the save named " +
                                                      savename + ".\n\nProceed with deletion?",
                                                  QMessageBox::Yes | QMessageBox::No)) {
        std::filesystem::remove(BackupsDir / "MANUAL" / savename.toStdString());
        QMessageBox::information(this, "Save Deleted", "Backup save deleted: " + savename);
    }

    OnSelectBackupSaveChanged();
    if (ui->BackupSaveSlotComboBox->count() == 0) {
        ui->SelectSaveComboBox->removeItem(ui->SelectSaveComboBox->findText("MANUAL"));
    }

    OnBackupSaveSlotChanged();
}

void SaveManager::RestoreBackupPressed() {
    std::string overwriteslot = ui->OverwriteSlotComboBox->currentText().toStdString();
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Overwrite Save Confirmation",
                              "WARNING:This will permanently overwrite your active save in " +
                                  QString::fromStdString(overwriteslot) +
                                  ".\n\nProceed with backup restore?",
                              QMessageBox::Yes | QMessageBox::No)) {

        std::string selectedsave = ui->SelectSaveComboBox->currentText().toStdString();
        std::filesystem::path BackupFile;
        if (selectedsave == "MANUAL") {
            BackupFile = BackupsDir / "MANUAL" / backupsaveslot;
        } else {
            BackupFile = BackupsDir / selectedsave / "SPRJ0005" / backupsaveslot;
        }

        std::filesystem::copy(BackupFile, ExactSaveDir / overwriteslot,
                              std::filesystem::copy_options::overwrite_existing);
        QMessageBox::information(this, "Save Restored",
                                 "Backup save restored: " + QString::fromStdString(backupsaveslot));
    }

    PopulateGameSaveSlots();
}

void SaveManager::RestoreBackupFolderPressed() {
    std::string selectedsave = ui->SelectSaveComboBox->currentText().toStdString();
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Overwrite Save Confirmation",
                              QString("WARNING: This will permanently overwrite your existing save "
                                      "folder with %1.\n\nProceed with backup restore?")
                                  .arg(QString::fromStdString(selectedsave)),
                              QMessageBox::Yes | QMessageBox::No)) {

        std::filesystem::path BackupFolder = BackupsDir / selectedsave / "SPRJ0005";
        std::filesystem::copy(BackupFolder, ExactSaveDir,
                              std::filesystem::copy_options::overwrite_existing |
                                  std::filesystem::copy_options::recursive);
        QMessageBox::information(this, "Backup Folder Restored",
                                 "Backup Folder restored: " + QString::fromStdString(selectedsave));
    }

    PopulateGameSaveSlots();
}

SaveManager::~SaveManager() {
    delete ui;
}
