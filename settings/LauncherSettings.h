// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>
#include <QNetworkAccessManager>
#include <SDL3/SDL_gamepad.h>

namespace Ui {
class LauncherSettings;
}

class LauncherSettings : public QDialog {
    Q_OBJECT

public:
    explicit LauncherSettings(QWidget* parent = nullptr);
    ~LauncherSettings();

public slots:

private slots:
    void SetLauncherDefaults();
    void SaveSettings();

private:
    Ui::LauncherSettings* ui;
    void OnBackupStateChanged();
    void CreateShortcut();
    bool convertPngToIco(const QString& pngFilePath, const QString& icoFilePath);

#ifdef Q_OS_WIN
    bool createShortcutWin(const QString& linkPath, const QString& iconPath,
                           const QString& exePath);
#else
    bool createShortcutLinux(const QString& linkPath, const std::string& name,
                             const QString& iconPath);
#endif

    const QStringList BackupNumList = {"1", "2", "3", "4", "5"};
    const QStringList BackupFreqList = {"5", "10", "15", "20", "25", "30"};
};

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
    QNetworkAccessManager* networkManager;
};
