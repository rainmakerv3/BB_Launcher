// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>
#include "Common.h"

namespace Ui {
class ModManager;
}

class ModManager : public QDialog {
    Q_OBJECT

public:
    explicit ModManager(QWidget* parent = nullptr);
    ~ModManager();

signals:
    void progressChanged(int value);

private slots:
    void ActivateButton_isPressed();
    void DeactivateButton_isPressed();

private:
    Ui::ModManager* ui;

    void RefreshLists();
    int getFileCount(std::filesystem::path ModFolder);
    void ActiveModRemove(std::string ModName);
    void ConflictAdd(std::string ModName);
    void ConflictRemove(std::string ModName);

    const std::filesystem::path ModBackupPath = Common::BBLFilesPath / "Mods-BACKUP";
    const std::filesystem::path ModUniquePath = ModBackupPath / "Mods-UNIQUEFILES";
    const std::vector<std::string> BBFolders = {
        "dvdroot_ps4", "action", "chr",    "event", "facegen", "map",   "menu",
        "movie",       "msg",    "mtd",    "obj",   "other",   "param", "paramdef",
        "parts",       "remo",   "script", "sfx",   "shader",  "sound"};
};
