// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>
#include <QListWidget>
#include <pugixml.hpp>

#include "modules/Common.h"

namespace Ui {
class ModMerger;
}

class ModMerger : public QDialog {
    Q_OBJECT

signals:
    void WitchyBndFinished();

public:
    explicit ModMerger(QWidget* parent = nullptr);
    ~ModMerger();

private:
    enum class ModPriority { NotSet, Mod1, Mod2 };
    enum class Format { Yellow, BoldRed, BoldGreen };

    void AttemptMerge();
    void GetConflictedFiles();
    void ExamineConflictedFiles();
    void CombineModFiles();
    void SetModPriority();
    void SetWitchyPath();

    void HandleConflict(std::filesystem::path targetFile, std::filesystem::path file1,
                        std::filesystem::path file2);
    bool UnextractrableFileProcessed(std::filesystem::path targetFile,
                                     std::filesystem::path mod1File,
                                     std::filesystem::path mod2File);

    void ProcessParamConflicts(pugi::xml_document& baseDoc, pugi::xml_document& mod1Doc,
                               pugi::xml_document& mod2Doc);
    void ProcessFmgConflicts(pugi::xml_document& baseDoc, pugi::xml_document& mod1Doc,
                             pugi::xml_document& mod2Doc);

    void ExtractFile(std::filesystem::path file);
    void RepackItem(std::filesystem::path itemPath);

    std::filesystem::path StandardizeBasePath(std::filesystem::path basePath);
    std::filesystem::path GetUpdatedFile(std::filesystem::path relative_path);
    bool AreFilesIdentical(const std::filesystem::path& path1, const std::filesystem::path& path2);
    void EnforceTwoItemLimit();
    void RefreshModList();
    QString FormatTextForBrowser(QString input, Format format);

    Ui::ModMerger* ui;
    QList<QListWidgetItem*> selectedHistory;
    std::vector<std::string> conflictedFiles;

    std::string mod1Name;
    std::string mod2Name;
    ModPriority currentPriority = ModPriority::NotSet;
    bool abortAttempt = false;

    const std::filesystem::path modPath = Common::GetBBLFilesPath() / "Mods";
    const std::filesystem::path baseTempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "merged";
    const std::filesystem::path mod1TempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "1";
    const std::filesystem::path mod2TempPath =
        Common::GetBBLFilesPath() / "Temp" / "ModMerge" / "2";
    QString witchyPath;

    const std::vector<std::string> BBFolders = {
        "dvdroot_ps4", "action", "adhoc", "chr",    "event", "facegen", "map",      "menu",
        "movie",       "msg",    "mtd",   "obj",    "other", "param",   "paramdef", "parts",
        "remo",        "script", "sfx",   "shader", "sound", "font"};
};
