// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later
/*

#include <QDialog>

class CheckShadUpdate : public QDialog {
    Q_OBJECT

signals:
    void DownloadProgressed(int value);
    void UpdateComplete();

public:
    explicit CheckShadUpdate(const bool showMessage, QWidget* parent = nullptr);
    ~CheckShadUpdate();

private slots:
    void UpdateShad(bool isAutoupdate);
    void InstallUpdate(QString zipPath);
    void DownloadUpdate(const QString& downloadUrl);

private:
    void setupUI(const QString& downloadUrl, const QString& latestDate, const QString& latestRev,
                 const QString& currentDate, const QString& currentRev);

    QString updateDownloadUrl;
    QString latestVersion;
    QString updateChannel;
};
*/
