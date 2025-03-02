// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QCheckBox>
#include <QDir>
#include <QFileInfoList>
#include <QGraphicsBlurEffect>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "modules/TrophyDeps/trp.h"

namespace Ui {
class TrophyViewer;
}

class TrophyViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit TrophyViewer(QString trophyPath, QString gameTrpPath);
    ~TrophyViewer();
    void updateTrophyInfo();
    void updateTableFilters();

private slots:
    void TabChanged();
    void TrophyIDChanged();
    void UnlockTrophy();
    void LockTrophy();
    void LockAllTrophies();

private:
    Ui::TrophyViewer* ui;
    void PopulateTrophyWidget(QString title);
    void SetTableItem(QTableWidget* parent, int row, int column, QString str);
    void UpdateStats();
    bool RefreshValues(QString title);

    QLabel* trophyInfoLabel;
    QCheckBox* showEarnedCheck;
    QCheckBox* showNotEarnedCheck;
    QCheckBox* showHiddenCheck;

    QTableWidget* tableWidget;
    QTabWidget* tabWidget = nullptr;
    QStringList headers;
    QString gameTrpPath_;
    TRP trp;

    QStringList trpId;
    QStringList trpHidden;
    QStringList trpUnlocked;
    QStringList trpType;
    QStringList trpPid;
    QStringList trophyNames;
    QStringList trophyDetails;
    QStringList trpTimeUnlocked;
    std::vector<QImage> icons;

    QString trophyFolder;

    std::string GetTrpType(const QChar trp_) {
        switch (trp_.toLatin1()) {
        case 'B':
            return "bronze.png";
        case 'S':
            return "silver.png";
        case 'G':
            return "gold.png";
        case 'P':
            return "platinum.png";
        }
        return "Unknown";
    }
};
