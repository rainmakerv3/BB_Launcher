// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

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
    bool ValidateApi();
    void LoadModInfo(int modId);
    void GetModFiles(int modId);
    void GetModImage(QUrl url);
    void DownloadFile(int fileId, int ModId, QString modFilename);
    void StartDownload(QString url, QString modFilename);
    QString BbcodeToHtml(QString BbcodeString);

    Ui::ModDownloader* ui;
    QNetworkAccessManager* manager;
    QString apiKey;
    QStringList fileList;
    std::vector<int> fileIdList;

    QMap<int, int> modIDmap;
};
