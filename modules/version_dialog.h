// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QTextBrowser>
#include <QTreeWidget>

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
    void DownloadListVersion();
    void InstallSelectedVersion();

private:
    struct Build {
        std::string path;
        std::string type;
        std::string id;
        std::string modified;
        int index;
    };

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

    std::vector<Build> buildInfo = {};
    QString preReleasePath;
    bool hasPreRelease = false;
    int preReleaseIndex;
};
