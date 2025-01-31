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
    void OnSaveSlotChanged();
    void OnSelectSaveChanged();
    void ManualBackupPressed();
    void DeleteBackupPressed();
    void RestoreBackupPressed();
    void RestoreBackupFolderPressed();

private:
    void UpdateValues();
    void PopulateBackupSlots();
    std::filesystem::path ExactSaveDir;
    std::filesystem::path Savefile;
    std::string saveslot;
    QStringList SaveSlotList;

    Ui::SaveManager* ui;
    const std::filesystem::path BackupsDir = Common::BBLFilesPath / "SaveBackups";
};
