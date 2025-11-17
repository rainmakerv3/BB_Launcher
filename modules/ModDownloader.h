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
    Ui::ModDownloader* ui;

    QNetworkAccessManager* manager;
};
