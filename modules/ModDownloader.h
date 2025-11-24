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
    void DownloadFile(int fileId, int ModId, QString modFilename);
    void DownloadFileRegular(int fileId, int ModId, QString modFilename);
    void StartDownload(QString url, QString modFilename);
    void SetSevenzipPath();
    QString BbcodeToHtml(QString BbcodeString);
    void extract7z(QString inpath, QString outpath);

    Ui::ModDownloader* ui;
    QNetworkAccessManager* manager;
    QString apiKey;
    std::filesystem::path sevenzipPath{};

    QStringList fileList;
    std::vector<int> fileIdList;
    QStringList fileDescList;
    QStringList fileTypeList;
    QMap<int, int> modIDmap;

    QDialog* authorizationDialog;
    QDialog* downloadDialog;
};
