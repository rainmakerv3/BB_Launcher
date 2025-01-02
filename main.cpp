
#include <filesystem>
#include "bblauncher.h"
#include "ui_bblauncher.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[]) {

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
    if (!std::filesystem::exists(std::filesystem::current_path() / "Shadps4-qt.AppImage")) {
        QMessageBox::warning(
            nullptr, "No Shadps4 AppImage found",
            "No Shadps4 AppImage found. Move BB_Launcher app next to shadPS4 install");
    } else if (!std::filesystem::exists(std::filesystem::current_path() / "Shadps4-sdl.AppImage")) {
        QMessageBox::warning(
            nullptr, "No Shadps4 AppImage found",
            "No Shadps4 AppImage found. Move BB_Launcher and all its files next to shadPS4 "
            "install");
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
