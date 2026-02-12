// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCheckBox>
#include <QDialog>

#include "modules/ipc/ipc_client.h"

namespace Ui {
class ShadSettings;
}

class ShadSettings : public QDialog {
    Q_OBJECT
public:
    explicit ShadSettings(std::shared_ptr<IpcClient> ipc_client, bool game_specific,
                          QWidget* parent = nullptr);
    ~ShadSettings();

    bool eventFilter(QObject* obj, QEvent* event) override;
    void updateNoteTextEdit(const QString& groupName);

private:
    void LoadValuesFromConfig();
    void OnCursorStateChanged(int index);
    void SaveSettings();
    void SetDefaults();
    void UpdateDialog();
    void getPhysicalDevices();

    std::unique_ptr<Ui::ShadSettings> ui;
    std::shared_ptr<IpcClient> m_ipc_client;

    bool is_game_specific;
    std::map<std::string, int> languages;
    QString defaultTextEdit;
    int initialHeight;

    const QMap<QString, QString> presentModeMap = {{tr("Mailbox (Vsync)"), "Mailbox"},
                                                   {tr("Fifo (Vsync)"), "Fifo"},
                                                   {tr("Immediate (No Vsync)"), "Immediate"}};

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
        "this to a language the game supports, which will vary by region.";
    const QString fullscreenModeGroupBoxtext =
        "Fullscreen Mode:\nChoose between windowed, borderless or exclusive fullscreen mode";
    const QString ReadbacksCheckBoxtext =
        "Enable GPU readbacks which fixed vertex explosions in Bloodborne without mods. This WIP "
        "implementation hits performance significantly.";
    const QString GPUBufferCheckBoxtext = "May help with PM4 Type 0 crashes";
    const QString discordRPCCheckboxtext =
        "Enable Discord Rich Presence:\nDisplays the emulator icon "
        "and relevant information on your Discord profile.";
    const QString userNametext =
        "Username:\nSets the PS4's account username, which may be displayed by some games.";
    const QString TrophyKeytext =
        "Trophy Key:\nKey used to decrypt trophies. Must be obtained from "
        "your jailbroken console.\nMust contain only hex characters.";
    const QString logTypeGroupBoxtext =
        "Log Type:\nSets whether to synchronize the output of the "
        "log window for performance. May have adverse effects on emulation.";
    const QString logFiltertext =
        "Log Filter:\nFilters the log to only print specific information.\nExamples: Core "
        " : Trace  Lib.Pad : Debug Common.Filesystem : Error* : Critical";
    const QString disableTrophycheckBoxtext =
        "Disable Trophy Pop-ups:\nDisable in-game trophy notifications. Trophy progress can still "
        "be tracked using the Trophy Viewer (right-click the game in the main window).";
    const QString motionControlsCheckBoxtext =
        "Disable motion controls (can cause random gesturing in Bloodborne).";
    const QString hideCursorGroupBoxtext =
        "Hide Cursor:\nChoose when the cursor will disappear:\nIdle: Set a time for it to "
        "disappear after being idle.\nNever: You will always see the mouse.\nAlways: you will "
        "never see the mouse.";
    const QString idleTimeoutGroupBoxtext =
        "Hide Idle Cursor Timeout:\nThe duration (seconds) after "
        "which the cursor that has been idle hides itself.";
    const QString resolutionLayouttext =
        "Width/Height:\nSets the size of the emulator window at launch, which can be resized "
        "during gameplay.\nThis is different from the in-game resolution.";
    const QString vblanktext =
        "Vblank Freqency:\nThe frame rate at which the emulator refreshes at (60hz is the "
        "baseline, whether the game runs at 30 or 60fps). Changing this may have adverse effects, "
        "such as increasing the game speed, or breaking critical game functionality that does not "
        "expect this to change!";
    const QString DevkitCheckBoxtext =
        "Enabled Devkit mode for shadPS4, this is required to use the 1440p patch as the game "
        "crashes otherwise";
    const QString backgroundControllerCheckBoxtext =
        "Enable Controller Background Input:\\nAllow shadPS4 to detect controller inputs when the "
        "game window is not in focus.";
    const QString FSRtext =
        "Applies AMD FSR1 when game resolution is lower than window resolution to upscale the "
        "rendered graphics. This tends to improve upscaled graphics without performance cost.";
    const QString RCAStext =
        "Attempts to enhance detail and sharpness of graphics upscaled using FSR.";
    const QString RCASAttenuationtext =
        "Adjusts the intensity of the sharpening filter used by RCAS. Higher settings tend to "
        "improve sharpness but may cause additional visual artifacts.";
    const QString PresentModetext =
        "Mailbox: Frames synchronize with your screen's refresh rate.  Reduces latency but may "
        "skip frames if running behind.\\nFifo: Frames synchronize with your screen's refresh "
        "rate. New frames will be queued. All frames are presented but may increase latency.\\n"
        "Immediate: Frames immediately present to your screen when ready. May result in "
        "tearing.";
};
