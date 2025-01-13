// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>

namespace Ui {
class SaveManager;
}

class SaveManager : public QDialog {
    Q_OBJECT

public:
    explicit SaveManager(QWidget* parent = nullptr);
    ~SaveManager();

private:
    Ui::SaveManager* ui;
};
