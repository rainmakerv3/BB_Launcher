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

class CheatsPatches : public QDialog {
    Q_OBJECT

public:
    explicit CheatsPatches(QWidget* parent = nullptr);
    ~CheatsPatches();

    void downloadPatches(const QString repository, const bool showMessageBox);
    void createFilesJson(const QString& repository);
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
    void populateFileListPatches();

    void addCheatsToLayout(const QJsonArray& modsArray, const QJsonArray& creditsArray);
    void addPatchesToLayout(const QString& serial);

    void updateNoteTextEdit(const QString& patchName);
    void readGameInfo();

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
    QMap<QString, PatchInfo> m_patchInfos;

    // UI Elements
    QVBoxLayout* rightLayout;
    QVBoxLayout* patchesGroupBoxLayout;
    QGroupBox* patchesGroupBox;
    QVBoxLayout* patchesLayout;
    QTextEdit* instructionsTextEdit;
    QListView* listView_selectFile;
    QItemSelectionModel* selectionModel;
    QComboBox* patchesComboBox;
    QListView* patchesListView;
    QString defaultTextEdit;
    QString defaultTextEditMSG =
        "Cheats/Patches are experimental.\nUse with caution.\n\nDownload cheats individually by "
        "selecting the repository and clicking the download button.\nIn the Patches tab, you can "
        "download all patches at once, choose which ones you want to use, and save your "
        "selection.\n\nSince we do not develop the Cheats/Patches,\nplease report issues to the "
        "cheat author.\n\nCreated a new cheat? Visit:\nhttps://github.com/shadps4-emu/ps4_cheats";
};
