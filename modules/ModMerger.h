// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>
#include <QFuture>
#include <QListWidget>
#include <QTextBrowser>

#include "modules/Common.h"

namespace Ui {
class ModMerger;
}

class ModMerger : public QDialog {
    Q_OBJECT

signals:
    void CleanUpRequested(bool aborted);
    void LogRequested(QString msg);

public:
    explicit ModMerger(QWidget* parent = nullptr);
    ~ModMerger();

    enum class ModPriority : int { NotSet, Mod1, Mod2 };
    enum class Format : int { Default, Yellow, BoldRed, BoldGreen };

    std::string Mod1Name();
    std::string Mod2Name();
    ModPriority GetModPriority();
    void OpenPriorityDialog();

    QString FormatTextForBrowser(QString input, Format format);
    void Log(QString msg, Format = Format::Default);
    void Log(QString msg, int format);

private:
    void AttemptMerge();
    void GetConflictedFiles();

    void CombineModFiles();
    bool GetMergeFiles(std::filesystem::path mod1Base, std::filesystem::path mod2Base);
    std::string GetFileType(const std::vector<char>& filedata);
    bool ChooseBaseFile(std::filesystem::path targetFile, std::filesystem::path mod1File,
                        std::filesystem::path mod2File);

    bool IsFolderSupported(std::filesystem::path filePath);
    std::filesystem::path StandardizeBasePath(std::filesystem::path basePath);
    std::filesystem::path GetUpdatedFile(std::filesystem::path relative_path);

    void EnforceTwoItemLimit();
    void RefreshModList();

    Ui::ModMerger* ui;
    QList<QListWidgetItem*> selectedHistory;
    std::vector<std::string> conflictedFiles;
    QFuture<void> activeMerge;

    std::string mod1Name = "";
    std::string mod2Name = "";
    ModPriority currentPriority = ModPriority::NotSet;

    const std::filesystem::path modPath = Common::GetBBLFilesPath() / "Mods";
    const std::filesystem::path baseTempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "merged";
    const std::filesystem::path mod1TempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "1";
    const std::filesystem::path mod2TempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "2";

    const std::vector<std::string> BBFolders = {
        "dvdroot_ps4", "action", "adhoc", "chr",    "event", "facegen", "map",      "menu",
        "movie",       "msg",    "mtd",   "obj",    "other", "param",   "paramdef", "parts",
        "remo",        "script", "sfx",   "shader", "sound", "font"};
};
