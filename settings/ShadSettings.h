// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCheckBox>
#include <QDialog>

#include "modules/ipc/ipc_client.h"
#include "settings/emulator_settings.h"

namespace Ui {
class ShadSettings;
}

class ShadSettings : public QDialog {
    Q_OBJECT
public:
    explicit ShadSettings(std::shared_ptr<EmulatorSettings> emu_settings,
                          std::shared_ptr<IpcClient> ipc_client, bool game_specific,
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

    bool IsSettingOverrideable(const char* setting_key, const QString& setting_group) const;
    void MapUIControls();

    std::unique_ptr<Ui::ShadSettings> ui;
    std::shared_ptr<IpcClient> m_ipc_client;
    std::shared_ptr<EmulatorSettings> m_game_specific_settings;
    std::shared_ptr<EmulatorSettings> m_emu_settings;
    std::shared_ptr<EmulatorSettings> m_original_settings;

    bool is_game_specific;
    // GameInfo m_current_game;   // Add current game info
    // std::string m_game_serial; // Game serial number

    QString defaultTextEdit;
    int initialHeight;

    // Map UI controls to their setting keys
    QMap<QObject*, std::pair<const char*, QString>> m_uiSettingMap;

    const QMap<QString, QString> presentModeMap = {{tr("Mailbox (Vsync)"), "Mailbox"},
                                                   {tr("Fifo (Vsync)"), "Fifo"},
                                                   {tr("Immediate (No Vsync)"), "Immediate"}};

    const QMap<QString, HideCursorState> cursorStateMap = {{tr("Never"), HideCursorState::Never},
                                                           {tr("Idle"), HideCursorState::Idle},
                                                           {tr("Always"), HideCursorState::Always}};

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

    enum OrbisSystemParamLanguage : s32 {
        ORBIS_SYSTEM_PARAM_LANG_JAPANESE = 0,
        ORBIS_SYSTEM_PARAM_LANG_ENGLISH_US = 1,
        ORBIS_SYSTEM_PARAM_LANG_FRENCH = 2,
        ORBIS_SYSTEM_PARAM_LANG_SPANISH = 3,
        ORBIS_SYSTEM_PARAM_LANG_GERMAN = 4,
        ORBIS_SYSTEM_PARAM_LANG_ITALIAN = 5,
        ORBIS_SYSTEM_PARAM_LANG_DUTCH = 6,
        ORBIS_SYSTEM_PARAM_LANG_PORTUGUESE_PT = 7,
        ORBIS_SYSTEM_PARAM_LANG_RUSSIAN = 8,
        ORBIS_SYSTEM_PARAM_LANG_KOREAN = 9,
        ORBIS_SYSTEM_PARAM_LANG_CHINESE_T = 10,
        ORBIS_SYSTEM_PARAM_LANG_CHINESE_S = 11,
        ORBIS_SYSTEM_PARAM_LANG_FINNISH = 12,
        ORBIS_SYSTEM_PARAM_LANG_SWEDISH = 13,
        ORBIS_SYSTEM_PARAM_LANG_DANISH = 14,
        ORBIS_SYSTEM_PARAM_LANG_NORWEGIAN = 15,
        ORBIS_SYSTEM_PARAM_LANG_POLISH = 16,
        ORBIS_SYSTEM_PARAM_LANG_PORTUGUESE_BR = 17,
        ORBIS_SYSTEM_PARAM_LANG_ENGLISH_GB = 18,
        ORBIS_SYSTEM_PARAM_LANG_TURKISH = 19,
        ORBIS_SYSTEM_PARAM_LANG_SPANISH_LA = 20,
        ORBIS_SYSTEM_PARAM_LANG_ARABIC = 21,
        ORBIS_SYSTEM_PARAM_LANG_FRENCH_CA = 22,
        ORBIS_SYSTEM_PARAM_LANG_CZECH = 23,
        ORBIS_SYSTEM_PARAM_LANG_HUNGARIAN = 24,
        ORBIS_SYSTEM_PARAM_LANG_GREEK = 25,
        ORBIS_SYSTEM_PARAM_LANG_ROMANIAN = 26,
        ORBIS_SYSTEM_PARAM_LANG_THAI = 27,
        ORBIS_SYSTEM_PARAM_LANG_VIETNAMESE = 28,
        ORBIS_SYSTEM_PARAM_LANG_INDONESIAN = 29,
        ORBIS_SYSTEM_PARAM_LANG_UKRAINIAN = 30,
    };

    const QMap<QString, s32> language_ids = {
        {"ja", ORBIS_SYSTEM_PARAM_LANG_JAPANESE},
        {"ja-JP", ORBIS_SYSTEM_PARAM_LANG_JAPANESE},
        {"en", ORBIS_SYSTEM_PARAM_LANG_ENGLISH_US},
        {"en-US", ORBIS_SYSTEM_PARAM_LANG_ENGLISH_US},
        {"en-GB", ORBIS_SYSTEM_PARAM_LANG_ENGLISH_GB},
        {"fr", ORBIS_SYSTEM_PARAM_LANG_FRENCH},
        {"es", ORBIS_SYSTEM_PARAM_LANG_SPANISH},
        {"es-ES", ORBIS_SYSTEM_PARAM_LANG_SPANISH},
        {"de", ORBIS_SYSTEM_PARAM_LANG_GERMAN},
        {"it", ORBIS_SYSTEM_PARAM_LANG_ITALIAN},
        {"nl", ORBIS_SYSTEM_PARAM_LANG_DUTCH},
        {"pt", ORBIS_SYSTEM_PARAM_LANG_PORTUGUESE_PT},
        {"pt-PT", ORBIS_SYSTEM_PARAM_LANG_PORTUGUESE_PT},
        {"pt-BR", ORBIS_SYSTEM_PARAM_LANG_PORTUGUESE_BR},
        {"ru", ORBIS_SYSTEM_PARAM_LANG_RUSSIAN},
        {"ko", ORBIS_SYSTEM_PARAM_LANG_KOREAN},
        {"zh", ORBIS_SYSTEM_PARAM_LANG_CHINESE_T},
        {"zh-HANT", ORBIS_SYSTEM_PARAM_LANG_CHINESE_T},
        {"zh-HANS", ORBIS_SYSTEM_PARAM_LANG_CHINESE_S},
        {"fi", ORBIS_SYSTEM_PARAM_LANG_FINNISH},
        {"sv", ORBIS_SYSTEM_PARAM_LANG_SWEDISH},
        {"da", ORBIS_SYSTEM_PARAM_LANG_DANISH},
        {"no", ORBIS_SYSTEM_PARAM_LANG_NORWEGIAN},
        {"nn", ORBIS_SYSTEM_PARAM_LANG_NORWEGIAN},
        {"nb", ORBIS_SYSTEM_PARAM_LANG_NORWEGIAN},
        {"pl", ORBIS_SYSTEM_PARAM_LANG_POLISH},
        {"tr", ORBIS_SYSTEM_PARAM_LANG_TURKISH},
        {"tr-TR", ORBIS_SYSTEM_PARAM_LANG_TURKISH},
        {"es-419", ORBIS_SYSTEM_PARAM_LANG_SPANISH_LA},
        {"ar-AE", ORBIS_SYSTEM_PARAM_LANG_ARABIC},
        {"ar", ORBIS_SYSTEM_PARAM_LANG_ARABIC},
        {"fr-CA", ORBIS_SYSTEM_PARAM_LANG_FRENCH_CA},
        {"cs", ORBIS_SYSTEM_PARAM_LANG_CZECH},
        {"cs-CZ", ORBIS_SYSTEM_PARAM_LANG_CZECH},
        {"hu-HU", ORBIS_SYSTEM_PARAM_LANG_HUNGARIAN},
        {"hu", ORBIS_SYSTEM_PARAM_LANG_HUNGARIAN},
        {"el-GR", ORBIS_SYSTEM_PARAM_LANG_GREEK},
        {"el", ORBIS_SYSTEM_PARAM_LANG_GREEK},
        {"ro-RO", ORBIS_SYSTEM_PARAM_LANG_ROMANIAN},
        {"ro", ORBIS_SYSTEM_PARAM_LANG_ROMANIAN},
        {"th-TH", ORBIS_SYSTEM_PARAM_LANG_THAI},
        {"th", ORBIS_SYSTEM_PARAM_LANG_THAI},
        {"vi-VN", ORBIS_SYSTEM_PARAM_LANG_VIETNAMESE},
        {"vi", ORBIS_SYSTEM_PARAM_LANG_VIETNAMESE},
        {"id-ID", ORBIS_SYSTEM_PARAM_LANG_INDONESIAN},
        {"id", ORBIS_SYSTEM_PARAM_LANG_INDONESIAN},
        {"uk", ORBIS_SYSTEM_PARAM_LANG_UKRAINIAN},
    };
};
