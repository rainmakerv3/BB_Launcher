// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class ModDownloader;
}

class ModDownloader : public QDialog {
    Q_OBJECT

public:
    explicit ModDownloader(QWidget* parent = nullptr);
    ~ModDownloader();

private:
    void GetApiKey();
    bool ValidateApi();
    void LoadModInfo(int modId);
    void GetModFiles(int modId);
    void GetModImage(QUrl url);
    void DownloadFilePremium(int fileId, int ModId, QString modName);
    void DownloadFileRegular(int fileId, int ModId, QString modName, QString modFileName);
    void StartDownload(QString url, QString modNamee, bool isPremium);
    void SetSevenzipPath();
    QString BbcodeToHtml(QString BbcodeString);
    void extract7z(QString inpath, QString outpath);

    Ui::ModDownloader* ui;
    QNetworkAccessManager* manager;
    QString apiKey;
    bool isApiKeyPremium = false;
    std::filesystem::path sevenzipPath{};

    QStringList fileList;
    std::vector<int> fileIdList;
    QStringList fileDescList;
    QStringList fileNameList;
    QMap<int, int> modIDmap;

    QDialog* authorizationDialog;
    QDialog* downloadDialog;
};
