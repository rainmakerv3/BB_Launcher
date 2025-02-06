// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDialog>

namespace Ui {
class ShadSettings;
}

class ShadSettings : public QDialog {
    Q_OBJECT
public:
    explicit ShadSettings(QWidget* parent = nullptr);
    ~ShadSettings();

    bool eventFilter(QObject* obj, QEvent* event) override;
    void updateNoteTextEdit(const QString& groupName);

private:
    void LoadValuesFromConfig();
    void OnCursorStateChanged(int index);
    void SaveSettings();
    void SetDefaults();
    void UpdateShad();
    void InstallUpdate();
    void DownloadUpdate(const QString& downloadUrl);
    void UpdateDialog();

    std::unique_ptr<Ui::ShadSettings> ui;

    std::map<std::string, int> languages;
    QString defaultTextEdit;
    int initialHeight;

    const QVector<int> languageIndexes = {21, 23, 14, 6, 18, 1, 12, 22, 2, 4,  25, 24, 29, 5,  0, 9,
                                          15, 16, 17, 7, 26, 8, 11, 20, 3, 13, 27, 10, 19, 30, 28};

    const QStringList languageNames = {"Arabic",
                                       "Czech",
                                       "Danish",
                                       "Dutch",
                                       "English (United Kingdom)",
                                       "English (United States)",
                                       "Finnish",
                                       "French (Canada)",
                                       "French (France)",
                                       "German",
                                       "Greek",
                                       "Hungarian",
                                       "Indonesian",
                                       "Italian",
                                       "Japanese",
                                       "Korean",
                                       "Norwegian (Bokmaal)",
                                       "Polish",
                                       "Portuguese (Brazil)",
                                       "Portuguese (Portugal)",
                                       "Romanian",
                                       "Russian",
                                       "Simplified Chinese",
                                       "Spanish (Latin America)",
                                       "Spanish (Spain)",
                                       "Swedish",
                                       "Thai",
                                       "Traditional Chinese",
                                       "Turkish",
                                       "Ukrainian",
                                       "Vietnamese"};

    // Descriptions
    const QString consoleLanguageGroupBoxtext =
        "Console Language:\nSets the language that the PS4 game uses.\nIt's recommended to set "
        "this to "
        "a language the game supports, which will vary by region.";
    const QString fullscreenCheckBoxtext = "Enable Full Screen:\nAutomatically puts the game "
                                           "window into full-screen mode.\nThis can be "
                                           "toggled by pressing the F11 key.";
    const QString fullscreenModeGroupBoxtext =
        "Fullscreen Mode:\nChoose between borderless or exclusive fullscreen mode";
    const QString separateUpdatesCheckBoxtext =
        "Enable Separate Update Folder:\nEnables installing game updates into a separate folder "
        "for easy management.\nThis can be manually created by adding the extracted update to the "
        "game folder with the name CUSA00000-UPDATE where the CUSA ID matches the game's ID.";
    const QString GPUBufferCheckBoxtext = "Helps with PM4 Type 0 crashes";
    const QString discordRPCCheckboxtext =
        "Enable Discord Rich Presence:\nDisplays the emulator icon "
        "and relevant information on your Discord profile.";
    const QString userNametext =
        "Username:\nSets the PS4's account username, which may be displayed by some games.";
    const QString TrophyKeytext =
        "Trophy Key:\nKey used to decrypt trophies. Must be obtained from "
        "your jailbroken console.\nMust contain only hex characters.";
    const QString logTypeGroupBoxtext = "Log Type:\nSets whether to synchronize the output of the "
                                        "log window for performance. May have "
                                        "adverse effects on emulation.";
    const QString logFiltertext =
        "Log Filter:\nFilters the log to only print specific information.\nExamples: Core "
        " : Trace  Lib.Pad : Debug Common.Filesystem : Error*"
        " : Critical\nLevels: Trace, Debug, Info, Warning, Error, Critical - in this order, a "
        "specific "
        "level silences all levels preceding it in the list and logs every level after it.";
    const QString updaterComboBoxtext = "Update:\nRelease: Official versions released every month "
                                        "that may be very outdated, but are "
                                        "more reliable and tested.\nNightly: Development versions "
                                        "that have all the latest features "
                                        "and fixes, but may contain bugs and are less stable.";
    const QString checkUpdateButtontext =
        "Updates to the latest version of shadPS4.\n\n ***NOTE*** BBLauncher cannot detect whether "
        "current version is the same as the latest. It will update regardless.";
    const QString disableTrophycheckBoxtext =
        "Disable Trophy Pop-ups:\nDisable in-game trophy notifications. Trophy progress can still "
        "be "
        "tracked using the Trophy Viewer (right-click the game in the main window).";
    const QString motionControlsCheckBoxtext =
        "Disable motion controls (can cause random gesturing in Bloodborne).";
    const QString hideCursorGroupBoxtext =
        "Hide Cursor:\nChoose when the cursor will disappear:\nIdle: Set a time for it to "
        "disappear "
        "after being idle.\nNever: You will always see the mouse.\nAlways: you will never see "
        "the mouse.";
    const QString idleTimeoutGroupBoxtext =
        "Hide Idle Cursor Timeout:\nThe duration (seconds) after "
        "which the cursor that has been idle hides itself.";
    const QString backButtonBehaviorGroupBoxtext =
        "Back Button Behavior:\nSets the controller's back button to emulate tapping the specified "
        "position on the PS4 touchpad.";
    const QString resolutionLayouttext =
        "Width/Height:\nSets the size of the emulator window at launch, which can be resized "
        "during "
        "gameplay.\nThis is different from the in-game resolution.";
    const QString heightDividertext =
        "Vblank Divider:\nThe frame rate at which the emulator refreshes at is multiplied by this "
        "number. Changing this may have adverse effects, such as increasing the game speed, or "
        "breaking critical game functionality that does not expect this to change!";
};

