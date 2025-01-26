// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include "modules/RunGuard.h"
#include "modules/bblauncher.h"

void customMessageHandler(QtMsgType, const QMessageLogContext&, const QString&) {}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(customMessageHandler);
    std::cout << "SHADPS4 LOG WINDOW\n\n";
    QApplication a(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption noGui("n");
    parser.addOption(noGui);
    parser.process(a);
    bool noGUIset = parser.isSet(noGui);

    RunGuard guard("d8976skj86874hkj287960980lkjhfka1#Q$^&*");
    bool noinstancerunning = guard.tryToRun();

    BBLauncher* main_window = new BBLauncher(noGUIset, noinstancerunning);

    if (!noinstancerunning) {
        QMessageBox::warning(nullptr, "BB_Launcher already running",
                             "Only one instance of BB_Launcher can run at a time");
        return 0;
    }

    if (!noGUIset)
        main_window->show();

    if (main_window->canLaunch) {
        return a.exec();
    }
}
