// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>
#include "Common.h"

namespace Ui {
class SaveManager;
}

class SaveManager : public QDialog {
    Q_OBJECT

public:
    explicit SaveManager(QWidget* parent = nullptr);
    ~SaveManager();

private slots:
    void OnGameSaveSlotChanged();
    void OnBackupSaveSlotChanged();
    void OnSelectBackupSaveChanged();
    void CreateManualBackupPressed();
    void DeleteManualBackupPressed();
    void RestoreBackupPressed();
    void RestoreBackupFolderPressed();
    void PopulateGameSaveSlots();

private:
    void UpdateGameSaveValues();
    void UpdateBackupSaveValues();
    void PopulateBackupSlots();

    std::filesystem::path ExactSaveDir;
    std::filesystem::path Savefile;
    std::string saveslot = "";
    std::string backupsaveslot = "";

    Ui::SaveManager* ui;
    const std::filesystem::path BackupsDir = Common::GetBBLFilesPath() / "SaveBackups";
};
