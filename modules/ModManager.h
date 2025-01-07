#include <filesystem>
#include <QDialog>

namespace Ui {
class ModManager;
}

const std::filesystem::path ModPath = std::filesystem::current_path() / "BBLauncher" / "Mods";
const std::filesystem::path ModBackupPath =
    std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP";
const std::filesystem::path ModUniquePath =
    std::filesystem::current_path() / "BBLauncher" / "Mods-BACKUP" / "Mods-UNIQUEFILES";
const std::vector<std::string> BBFolders = {"dvdroot_ps4", "action", "chr",   "event",    "facegen",
                                            "map",         "menu",   "movie", "msg",      "mtd",
                                            "obj",         "other",  "param", "paramdef", "parts",
                                            "remo",        "script", "sfx",   "shader",   "sound"};

class ModManager : public QDialog {
    Q_OBJECT

public:
    explicit ModManager(QWidget* parent = nullptr);
    ~ModManager();

signals:
    void progressChanged(int value);

private slots:
    void ActivateButton_isPressed();
    void DeactivateButton_isPressed();

private:
    Ui::ModManager* ui;

    void RefreshLists();
    int getFileCount(std::filesystem::path ModFolder);
    void ActiveModRemove(std::string ModName);
    void ConflictAdd(std::string ModName);
    void ConflictRemove(std::string ModName);
    std::string PathToU8(const std::filesystem::path& path);
};
