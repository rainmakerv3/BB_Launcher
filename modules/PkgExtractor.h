// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <filesystem>
#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>

class PkgExtractor : public QDialog {
    Q_OBJECT

public:
    explicit PkgExtractor(QWidget* parent = nullptr);
    ~PkgExtractor();

private:
    void setupUI();
    void browseOutput();
    void browsePkg();
    void ExtractPkg();
    std::optional<std::filesystem::path> FindGameByID(const std::filesystem::path& dir,
                                                      const std::string& game_id, int max_depth);
    std::vector<std::string> SplitString(const std::string& str, char delimiter);

    QLineEdit* pkgLineEdit;
    QLineEdit* outputLineEdit;
    QCheckBox* separateUpdateCheckBox;
};
