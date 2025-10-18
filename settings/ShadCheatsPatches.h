// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListView>
#include <QMap>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include "modules/ipc/ipc_client.h"

class CheatsPatches : public QDialog {
    Q_OBJECT

public:
    explicit CheatsPatches(std::shared_ptr<IpcClient> client, bool game_running,
                           QWidget* parent = nullptr);
    ~CheatsPatches();

    void downloadCheats(const QString& source, const QString& m_gameSerial,
                        const QString& m_gameVersion, bool showMessageBox);
    void downloadPatches(const QString repository, const bool showMessageBox);
    void createFilesJson(const QString& repository);
    void clearListCheats();
    void compatibleVersionNotice(const QString repository);

signals:
    void downloadFinished();

private:
    // UI Setup and Event Handlers
    void setupUI();
    void onSaveButtonClicked();
    QCheckBox* findCheckBoxByName(const QString& name);
    bool eventFilter(QObject* obj, QEvent* event);
    void onPatchCheckBoxHovered(QCheckBox* checkBox, bool hovered);

    // Cheat and Patch Management
    void populateFileListCheats();
    void populateFileListPatches();

    void addCheatsToLayout(const QJsonArray& modsArray, const QJsonArray& creditsArray);
    void addPatchesToLayout(const QString& serial);

    void applyCheat(const QString& modName, bool enabled);

    void uncheckAllCheatCheckBoxes();

    void updateNoteTextEdit(const QString& patchName);
    std::string getGameVersion();
    void SceUpdateChecker(const std::string sceItem, std::filesystem::path& gameItem,
                          std::filesystem::path& update_folder, std::filesystem::path& patch_folder,
                          std::filesystem::path& game_folder);

    // Network Manager
    QNetworkAccessManager* manager;

    // Patch Info Structures
    struct MemoryMod {
        QString offset;
        QString on;
        QString off;
    };

    struct Cheat {
        QString name;
        QString type;
        bool hasHint;
        QVector<MemoryMod> memoryMods;
    };

    struct PatchInfo {
        QString name;
        QString author;
        QString note;
        QJsonArray linesArray;
        QString serial;
    };

    // Members
    bool is_game_running;
    std::shared_ptr<IpcClient> m_ipc_client;
    QMap<QString, PatchInfo> m_patchInfos;

    // UI Elements
    QLabel* gameVersionLabel;
    QVBoxLayout* rightLayout;
    QVBoxLayout* patchesGroupBoxLayout;
    QGroupBox* patchesGroupBox;
    QVBoxLayout* patchesLayout;
    QTextEdit* instructionsTextEdit;
    QListView* listView_selectFile;
    QItemSelectionModel* selectionModel;
    QComboBox* patchesComboBox;
    QListView* patchesListView;
    QMap<QString, Cheat> m_cheats;
    QVector<QCheckBox*> m_cheatCheckBoxes;
    QString m_cheatFilePath;
    QString defaultTextEdit;
    QString defaultTextEditMSG =
        "Cheats/Patches are experimental.\nUse with caution.\n\nDownload cheats individually by "
        "selecting the repository and clicking the download button.\nIn the Patches tab, you can "
        "download all patches at once, choose which ones you want to use, and save your "
        "selection.\n\nSince we do not develop the Cheats/Patches,\nplease report issues to the "
        "cheat author.\n\nCreated a new cheat? Visit:\nhttps://github.com/shadps4-emu/ps4_cheats";

    QString CheatsNotFound_MSG =
        "No Cheats found for this game in this version of the selected repository,try another "
        "repository or a different version of the game.";

    QString CheatsDownloadedSuccessfully_MSG =
        "You have successfully downloaded the cheats for this version of the game from the "
        "selected repository. You can try downloading from another repository, if it is available "
        "it will also be possible to use it by selecting the file from the list.";

    QString cheatsLabelText = "IMPORTANT: Cheats only work with the latest\nshadPS4 SDL version "
                              "(not Qt) only. Also, some\ncheats may not work or may crash.";
    QString gameVersionLabelText =
        "IMPORTANT: Patches only work properly with\nversion 1.09, please "
        "update to version 1.09\nif you have not before using patches.";
};
