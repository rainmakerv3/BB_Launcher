// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#ifndef USE_WEBENGINE
#include <QtWebView>
#endif

#include "modules/RunGuard.h"
#include "modules/bblauncher.h"

void customMessageHandler(QtMsgType, const QMessageLogContext&, const QString&) {}

int main(int argc, char* argv[]) {
#ifdef Q_OS_MAC
    qputenv("QT_WEBVIEW_PLUGIN", "native");
#endif

#ifndef USE_WEBENGINE
    QtWebView::initialize();
#endif

    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");

    QCommandLineParser parser;
    QCommandLineOption noGui("n");
    parser.addOption(noGui);
    parser.process(a);
    bool noGUIset = parser.isSet(noGui);

    RunGuard guard("d8976skj86874hkj287960980lkjhfka1#Q$^&*");
    bool noinstancerunning = guard.tryToRun();

    BBLauncher* main_window = new BBLauncher(noGUIset, noinstancerunning, nullptr);

    bool hasInstance = false;
    if (!noinstancerunning) {
        QMessageBox::warning(nullptr, "BB_Launcher already running",
                             "Only one instance of BB_Launcher can run at a time");
        main_window->close();
        return 0;
    }

    noGUIset ? main_window->hide() : main_window->show();

    if (!main_window->canLaunch) {
        return 0;
    }

    return a.exec();
}
