// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QTextBrowser>
#include <QTreeWidget>

#include "settings/config.h"

namespace Ui {
class VersionDialog;
}

class VersionDialog : public QDialog {
    Q_OBJECT

public:
    explicit VersionDialog(QWidget* parent = nullptr);
    ~VersionDialog();
    void onItemChanged(QTreeWidgetItem* item, int column);
    void checkUpdatePre(const bool showMessage);
    void CheckVersionsList(const bool showMessage);
    void InstallSelectedVersion();

private:
    Ui::VersionDialog* ui;
    QNetworkAccessManager* networkManager;

    void GetBuildInfo();
    void SaveBuilds();
    void LoadInstalledList();
    void RemoveItem(bool alsoDelete);
    QStringList LoadDownloadCache();
    void SaveDownloadCache(const QStringList& versions);
    void PopulateDownloadTree(const QStringList& versions);
    void showPreReleaseUpdateDialog(const QString& localHash, const QString& latestHash,
                                    const QString& latestTag);
    void requestChangelog(const QString& localHash, const QString& latestHash,
                          const QString& latestTag, QTextBrowser* outputView);
    void installPreReleaseByTag(const QString& tagName);
    void showDownloadDialog(const QString& tagName, const QString& downloadUrl);

    std::vector<Config::Build> buildInfo = {};
    QString preReleaseFolder;
    bool hasPreRelease = false;
    int preReleaseIndex;
};
