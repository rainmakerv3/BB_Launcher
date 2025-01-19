// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include "modules/bblauncher.h"

void customMessageHandler(QtMsgType, const QMessageLogContext&, const QString&) {}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(customMessageHandler);
    std::cout << "SHADPS4 UPDATE WINDOW\n\n";
    QApplication a(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption noGui("n");
    parser.addOption(noGui);
    parser.process(a);
    bool noGUIset = parser.isSet(noGui);

    BBLauncher* main_window = new BBLauncher(noGUIset);

    if (!noGUIset)
        main_window->show();

    if (main_window->canLaunch) {
        return a.exec();
    }
}
