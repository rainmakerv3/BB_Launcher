// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>
#include <QNetworkAccessManager>
#include "Common.h"

#ifdef USE_WEBENGINE
#include <QWebEngineProfile>
#endif

namespace Ui {
class ModDownloader;
}

class ModDownloader : public QDialog {
    Q_OBJECT

public:
    explicit ModDownloader(QWidget* parent = nullptr);
    ~ModDownloader();

signals:
    void FileExtracted(int extracted);
    void ExtractionDone();

private:
    void GetApiKey();
    bool ValidateApi();
    void LoadModInfo(int modId);
    void GetModFiles(int modId);
    void GetModImage(QUrl url);
    void DownloadFilePremium(int fileId, int ModId, QString modName);
    void DownloadFileRegular(int fileId, int ModId, QString modName, QString modFileName);
    void StartDownload(QString url, QString modNamee, bool isPremium);
    QString BbcodeToHtml(QString BbcodeString);
    void ExtractArchive(QString inpath, QString outpath);
    bool GetOption(QStringList options, QString& modName, std::string& option);

    Ui::ModDownloader* ui;
    QNetworkAccessManager* manager;
    QString apiKey;
    bool isApiKeyPremium = false;

    QStringList fileList;
    std::vector<int> fileIdList;
    QStringList fileDescList;
    QStringList fileNameList;
    QMap<int, int> modIDmap;

    QDialog* authorizationDialog;
    QDialog* downloadDialog;

    const std::filesystem::path ModActivePath =
        Common::GetBBLFilesPath() / "Mods-Active (DO NOT DELETE)";

#ifdef USE_WEBENGINE
    QWebEngineProfile* profile;
#endif
};
