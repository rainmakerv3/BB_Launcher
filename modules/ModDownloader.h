// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QWebEngineDownloadRequest>

namespace Ui {
class ModDownloader;
}

class ModDownloader : public QDialog {
    Q_OBJECT

public:
    explicit ModDownloader(QWidget* parent = nullptr);
    ~ModDownloader();

private:
    bool ValidateApi();
    void LoadModInfo(int modId);
    void GetModFiles(int modId);
    void GetModImage(QUrl url);
    void DownloadFileRegular(int fileId, int ModId, QString modFilename);
    void DownloadFilePremium(int fileId, int ModId, QString modFilename);
    void StartDownload(QString url, QString modFilename, bool isPremium);
    void SetSevenzipPath();
    QString BbcodeToHtml(QString BbcodeString);
    void extract7z(QString inpath, QString outpath);
    void processClickedDownload(QString idString, QString expiredString);

    Ui::ModDownloader* ui;
    QNetworkAccessManager* manager;
    QString apiKey;
    bool isApiKeyPremium = false;
    std::filesystem::path sevenzipPath{};

    QStringList fileList;
    std::vector<int> fileIdList;
    QStringList fileDescList;
    QStringList fileTypeList;
    QMap<int, int> modIDmap;
};
