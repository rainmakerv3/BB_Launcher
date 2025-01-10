#include <filesystem>
#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include "modules/bblauncher.h"

void customMessageHandler(QtMsgType, const QMessageLogContext&, const QString&) {}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(customMessageHandler);
    std::cout << "SHADPS4 UPDATE WINDOW\n\n";

    QApplication a(argc, argv);
    BBLauncher w;
    w.show();

#ifdef _WIN32
    if (!std::filesystem::exists(std::filesystem::current_path() / "shadPS4.exe")) {
        QMessageBox::warning(
            nullptr, "No shadPS4.exe found",
            "No shadPS4.exe found. Move BB_Launcher.exe next to shadPS4.exe.\n\nMove all other "
            "files/folders in BB_Launcher folder to shadPS4 folder only if you are using a non-QT "
            "(no GUI) version of shadPS4.");
    } else {
        return a.exec();
    }
#elif defined(__linux__)
    if (!std::filesystem::exists(std::filesystem::current_path() / "Shadps4-qt.AppImage") &&
        !std::filesystem::exists(std::filesystem::current_path() / "Shadps4-sdl.AppImage") &&
        !std::filesystem::exists(std::filesystem::current_path() / "Shadps4")) {
        QMessageBox::warning(nullptr, "No Shadps4 App or AppImage found",
                             "No Shadps4 App or AppImage found. Move BB_Launcher app next to "
                             "shadPS4 App or AppImage");
    } else {
        return a.exec();
    }

#elif defined(__APPLE__)
    if (!std::filesystem::exists(std::filesystem::current_path() / "shadps4.app")) {
        QMessageBox::warning(nullptr, "No shadps4 app found",
                             "No shadPS4 app found. Move BB_Launcher app next to shadPS4 install.");
    } else if (!std::filesystem::exists(std::filesystem::current_path() / "shadps4.")) {
        QMessageBox::warning(
            nullptr, "No shadps4 app found",
            "No shadPS4 app found. Move BB_Launcher app and other files next to shadPS4 install");
    } else {
        return a.exec();
    }
#endif
}
