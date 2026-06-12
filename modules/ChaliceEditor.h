// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>
#include <QLabel>
#include <nlohmann/json.hpp>

namespace Ui {
class ChaliceEditor;
}

class ChaliceEditor : public QDialog {
    Q_OBJECT

public:
    explicit ChaliceEditor(QWidget* parent = nullptr);
    ~ChaliceEditor();

private:
    void LoadChalicesFromSave();
    void PopulateCategory(std::string category);
    void SearchDungeons();
    void SaveDungeon();
    void CreateManualBackup();

    Ui::ChaliceEditor* ui;

    std::filesystem::path ExactSaveDir;
    std::filesystem::path Savefile;
    std::vector<uint8_t> fileData;
    uint currentOffset;

    nlohmann::json allDungeons;
    std::vector<nlohmann::json> flatDungeons;
    std::vector<std::vector<uint8_t>> dungeonListData;

    QMap<int, QLabel*> GlyphLabelMap;
    QMap<int, QLabel*> GlyphDescMap;
};
