// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SaveManager.h"
#include "modules/ui_SaveManager.h"

SaveManager::SaveManager(QWidget* parent) : QDialog(parent), ui(new Ui::SaveManager) {
    ui->setupUi(this);
}

SaveManager::~SaveManager() {
    delete ui;
}
