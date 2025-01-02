
#include <filesystem>
#include "bblauncher.h"
#include "ui_bblauncher.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);
    BBLauncher w;
    w.show();

    if (!std::filesystem::exists(std::filesystem::current_path() / "shadPS4.exe")) {
        QMessageBox::warning(
            nullptr, "No shadPS4.exe found",
            "No shadPS4.exe found. Move BB_Launcher.exe next to shadPS4.exe.\nMove all other "
            "files/folders in BB_Launcher folder to shadPS4 folder only if you are not using a QT "
            "version of shadPS4.");
    } else {
        return a.exec();
    }
}
