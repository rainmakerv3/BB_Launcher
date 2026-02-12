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

#ifndef USE_WEBENGINE
    QtWebView::initialize();
#endif
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");

// Check if CPU supports AVX2, show error and quit otherwise
#if defined(_MSC_VER)
    std::vector<int> cpuInfo(4);
    __cpuidex(cpuInfo.data(), 7, 0);
    const int AVX2_BIT = (1 << 5);
    bool avx2_available = (cpuInfo[1] & AVX2_BIT) != 0;

    if (!avx2_available) {
        QMessageBox::information(nullptr, "Unsupported CPU",
                                 "CPU does not support AVX2 instructions. BBLauncher and ShadPS4 "
                                 "both require CPU that support AVX2. Quitting application...");
        return 0;
    }
#elif defined(__clang__) || defined(__GNUC__)

#ifndef __APPLE__
    if (!__builtin_cpu_supports("avx2")) {
        QMessageBox::information(nullptr, "Unsupported CPU",
                                 "CPU does not support AVX2 instructions. BBLauncher and ShadPS4 "
                                 "both require CPU that support AVX2. Quitting application...");
        return 0;
    }
#endif

#endif

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
        return 0;
    }

    if (!noGUIset)
        main_window->show();

    if (!main_window->canLaunch) {
        return 0;
    }

    return a.exec();
}
