// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHoverEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include "ShadSettings.h"
#include "config.h"
#include "modules/Common.h"
#include "settings/ui_ShadSettings.h"
#include "user_settings.h"

static std::vector<QString> m_physical_devices;

// Normalize paths consistently for equality checks
static inline std::string NormalizePath(const std::filesystem::path& p) {
    // Convert to a normalized lexical path
    auto np = p.lexically_normal();

    // Convert to UTF-8 string
    auto u8 = np.generic_u8string();
    std::string s(u8.begin(), u8.end());

#ifdef _WIN32
    // Windows paths: drive letters are case-insensitive to normalize case
    // Example: "C:/Games" vs "c:/Games"
    if (s.size() >= 2 && s[1] == ':')
        s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
#endif

    return s;
}

// Equality operators
inline bool operator==(GameInstallDir const& a, GameInstallDir const& b) {
    return a.enabled == b.enabled && NormalizePath(a.path) == NormalizePath(b.path);
}

inline bool operator!=(GameInstallDir const& a, GameInstallDir const& b) {
    return !(a == b);
}

ShadSettings::ShadSettings(std::shared_ptr<IpcClient> ipc_client, QWidget* parent)
    : m_ipc_client(ipc_client), QDialog(parent), ui(new Ui::ShadSettings) {
    ui->setupUi(this);
    getPhysicalDevices();
    ui->tabWidgetSettings->setUsesScrollButtons(false);
    initialHeight = this->height();

    ui->tabWidgetSettings->setCurrentIndex(0);

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Save)->setFocus();

    QStringList locales;
    for (const QString& code : language_ids.keys()) {
        const QLocale locale(code);
        QString locale_name = locale.nativeLanguageName();

        if (locale.territory() != QLocale::AnyTerritory) {
            locale_name += " (" + locale.nativeTerritoryName() + ")";
        }

        if (!locales.contains(locale_name))
            locales.append(locale_name);
    }

    ui->consoleLanguageComboBox->addItems(locales);

    ui->fullscreenModeComboBox->addItem("Fullscreen (Borderless)");
    ui->fullscreenModeComboBox->addItem("Windowed");
    ui->fullscreenModeComboBox->addItem("Fullscreen");

    ui->hideCursorComboBox->addItem("Never");
    ui->hideCursorComboBox->addItem("Idle");
    ui->hideCursorComboBox->addItem("Always");

    ui->graphicsAdapterBox->addItem(tr("Auto Select")); // -1, auto selection
    for (const auto& device : m_physical_devices) {
        ui->graphicsAdapterBox->addItem(device);
    }

    gs_settings.Load(Common::game_serial);
    user = UserSettings.GetUserManager().GetUserByID(
        UserSettings.GetUserManager().GetDefaultUser().user_id);
    LoadValuesFromConfig();
    RefreshHostOverrideFileStatus();

    defaultTextEdit = "Point your mouse at an option to display its description.";
    ui->descriptionText->setText(defaultTextEdit);

    QPushButton* deleteButton = new QPushButton("Delete Game-specific config");
    ui->buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);

    connect(
        deleteButton, &QPushButton::pressed, this, [this]() {
            std::string filename = Common::game_serial + ".json";
            std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;

            if (QMessageBox::Yes ==
                QMessageBox::question(this, "Confirm deletion",
                                      "Are you sure you want to delete the game-specific config?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                std::filesystem::remove(gsConfig);
                QWidget::close();
            }
        });

    connect(this, &QDialog::rejected, this, [this]() {
        // reset real-time widgets to config value if not saved
        int sliderValue = EmulatorSettings.GetVolumeSlider();
        ui->volumeSlider->setValue(sliderValue);

        if (Config::GameRunning) {
            m_ipc_client->setFsr(EmulatorSettings.IsFsrEnabled());
            m_ipc_client->setRcas(EmulatorSettings.IsRcasEnabled());
            m_ipc_client->setRcasAttenuation(EmulatorSettings.GetRcasAttenuation());
        }

        QWidget::close();
    });

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button) {
        if (button == ui->buttonBox->button(QDialogButtonBox::Save)) {
            SaveSettings();
            QWidget::close();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
            SaveSettings();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
            SetDefaults();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
            QDialog::rejected();
        }
    });

    connect(ui->tabWidgetSettings, &QTabWidget::currentChanged, this,
            [this]() { ui->buttonBox->button(QDialogButtonBox::Cancel)->setFocus(); });

    connect(ui->hideCursorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) { OnCursorStateChanged(index); });

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->volumeValue->setText(QString::number(value) + "%");
        if (Config::GameRunning)
            m_ipc_client->adjustVol(value, true);
    });

    connect(ui->HomePathButton, &QPushButton::clicked, this, [this]() {
        QString initial_path;
        Common::PathToQString(initial_path, EmulatorSettings.GetHomeDir());

        QString home_path_string =
            QFileDialog::getExistingDirectory(this, "Set home folder", initial_path);
        auto file_path = Common::PathFromQString(home_path_string);

        if (!file_path.empty()) {
            ui->HomePathLineEdit->setText(home_path_string);
        }
    });

    connect(ui->DlcPathButton, &QPushButton::clicked, this, [this]() {
        QString initial_path;
        Common::PathToQString(initial_path, EmulatorSettings.GetAddonInstallDir());
        QString dlc_path_string =
            QFileDialog::getExistingDirectory(this, "Directory to dlc files", initial_path);
        auto file_path = Common::PathFromQString(dlc_path_string);

        if (!file_path.empty()) {
            ui->DLCPathLineEdit->setText(dlc_path_string);
            EmulatorSettings.SetAddonInstallDir(file_path);
            EmulatorSettings.Save();
        }
    });

    connect(ui->RCASSlider, &QSlider::valueChanged, this, [this](int value) {
        QString RCASValue = QString::number(value / 1000.0, 'f', 3);
        ui->RCASValue->setText(RCASValue);
    });

    connect(ui->deleteCacheButton, &QPushButton::clicked, this, [this]() {
        std::filesystem::path cachePath = Common::GetShadUserDir() / "cache" / Common::game_serial;
        if (!std::filesystem::exists(cachePath)) {
            QMessageBox::information(this, "Error",
                                     QString("No current shader cache for %1")
                                         .arg(QString::fromStdString(Common::game_serial)));
        } else {
            try {
                std::filesystem::remove_all(cachePath);
                QMessageBox::information(this, "Removal Completed",
                                         QString("Shader cache for %1 successfully removed.")
                                             .arg(QString::fromStdString(Common::game_serial)));
            } catch (std::exception& e) {
                QMessageBox::information(this, "Removal Completed",
                                         "Shader cache removal failed: " + QString(e.what()));
            }
        }
    });

    connect(ui->hostOverrideButton, &QPushButton::clicked, this, [this]() {
        std::string text =
            R"({
"https://ss4.scej-network.jp:20443" : "http://thehuntersdream.com"
})";

        std::filesystem::path filePath = Common::GetShadUserDir() / "host_overrides.json";

        if (std::filesystem::exists(filePath)) {
            if (QMessageBox::No == QMessageBox::question(this, "Confirm Overwrite",
                                                         "Host override file already exists. Do "
                                                         "you want to overwrite the existing file?",
                                                         QMessageBox::Yes | QMessageBox::No)) {
                return;
            }
        }

        std::ofstream file(filePath);
        if (file.is_open()) {
            file << text;
            file.close();
            QMessageBox::information(this, "File created",
                                     "Host override file successfully created");
        } else {
            QMessageBox::warning(this, "Error", "Could not create host override file");
        }

        RefreshHostOverrideFileStatus();
    });

    connect(ui->shadnetRegButton, &QPushButton::clicked, this, [this]() {
        QString link = "https://shadps4.net/shadnet/register/";
        QDesktopServices::openUrl(QUrl(link));
    });

    connect(ui->showShadNetPasswordCheckBox, &QCheckBox::checkStateChanged, this,
            [this](bool checked) {
                checked ? ui->shadnetPasswordLineEdit->setEchoMode(QLineEdit::Normal)
                        : ui->shadnetPasswordLineEdit->setEchoMode(QLineEdit::Password);
            });

    if (Config::GameRunning) {
        connect(ui->RCASSlider, &QSlider::valueChanged, this,
                [this](int value) { m_ipc_client->setRcasAttenuation(value); });

        connect(ui->FSRCheckBox, &QCheckBox::checkStateChanged, this,
                [this](Qt::CheckState state) { m_ipc_client->setFsr(state); });

        connect(ui->RCASCheckBox, &QCheckBox::checkStateChanged, this,
                [this](Qt::CheckState state) { m_ipc_client->setRcas(state); });
    }

    // Descriptions
    {
        ui->FullscreenModeGroupBox->installEventFilter(this);
        ui->readbacksModeComboBox->installEventFilter(this);
        ui->GPUBufferCheckBox->installEventFilter(this);
        ui->discordRPCCheckbox->installEventFilter(this);
        ui->trophyKeyLineEdit->installEventFilter(this);
        ui->logTypeGroupBox->installEventFilter(this);
        ui->logFilter->installEventFilter(this);
        ui->disableTrophycheckBox->installEventFilter(this);

        ui->hideCursorGroupBox->installEventFilter(this);
        ui->idleTimeoutGroupBox->installEventFilter(this);

        ui->widthGroupBox->installEventFilter(this);
        ui->heightGroupBox->installEventFilter(this);
        ui->heightDivider->installEventFilter(this);
        ui->motionControlsCheckBox->installEventFilter(this);
        ui->backgroundControllerCheckBox->installEventFilter(this);

        ui->FSRCheckBox->installEventFilter(this);
        ui->RCASCheckBox->installEventFilter(this);
        ui->RCASLabel->installEventFilter(this);
        ui->RCASValue->installEventFilter(this);
        ui->RCASSlider->installEventFilter(this);
        ui->presentModeGroupBox->installEventFilter(this);
    }
}

void ShadSettings::LoadValuesFromConfig() {
    if (!gs_settings.Load(Common::game_serial)) {
        QMessageBox::information(this, "Error", "Unable to load game-specific settings");
        return;
    }

    if (!EmulatorSettings.Load()) {
        QMessageBox::information(this, "Error", "Unable to load settings");
        return;
    }

    // global settings - use Emulator Settings
    QString selected_locale = "American English (United States)";
    for (const QString& code : language_ids.keys()) {
        const QLocale locale(code);
        QString locale_name = locale.nativeLanguageName();

        if (locale.territory() != QLocale::AnyTerritory) {
            locale_name += " (" + locale.nativeTerritoryName() + ")";
        }

        if (language_ids[code] == EmulatorSettings.GetConsoleLanguage())
            selected_locale = locale_name;
    }
    ui->consoleLanguageComboBox->setCurrentText(selected_locale);

    QString home_path_string;
    Common::PathToQString(home_path_string, EmulatorSettings.GetHomeDir());
    ui->HomePathLineEdit->setText(home_path_string.replace("\\", "/"));

    QString dlc_path_string;
    Common::PathToQString(dlc_path_string, EmulatorSettings.GetAddonInstallDir());
    ui->DLCPathLineEdit->setText(dlc_path_string.replace("\\", "/"));

    ui->discordRPCCheckbox->setChecked(EmulatorSettings.IsDiscordRPCEnabled());

    // Other settings - gs_settings
    ui->hideCursorComboBox->setCurrentIndex(gs_settings.GetCursorState());
    OnCursorStateChanged(ui->hideCursorComboBox->currentIndex());
    ui->idleTimeoutSpinBox->setValue(gs_settings.GetCursorHideTimeout());
    ui->widthSpinBox->setValue(gs_settings.GetWindowWidth());
    ui->heightSpinBox->setValue(gs_settings.GetWindowHeight());
    ui->vblankSpinBox->setValue(gs_settings.GetVblankFrequency());
    ui->readbacksModeComboBox->setCurrentIndex(gs_settings.GetReadbacksMode());
    ui->DMACheckBox->setChecked(gs_settings.IsDirectMemoryAccessEnabled());
    ui->GPUBufferCheckBox->setChecked(gs_settings.IsCopyGpuBuffers());
    ui->disableTrophycheckBox->setChecked(gs_settings.IsTrophyPopupDisabled());
    ui->popUpPosComboBox->setCurrentText(
        QString::fromStdString(gs_settings.GetTrophyNotificationSide()));
    ui->popUpDurationSpinBox->setValue(gs_settings.GetTrophyNotificationDuration());

    ui->showSplashCheckBox->setChecked(gs_settings.IsShowSplash());
    ui->dmemSpinBox->setValue(gs_settings.GetExtraDmemInMBytes());
    ui->logTypeComboBox->setCurrentText(QString::fromStdString(gs_settings.GetLogType()));
    ui->logFilterLineEdit->setText(QString::fromStdString(gs_settings.GetLogFilter()));

    ui->FSRCheckBox->setChecked(gs_settings.IsFsrEnabled());
    ui->RCASCheckBox->setChecked(gs_settings.IsRcasEnabled());
    ui->RCASSlider->setValue(gs_settings.GetRcasAttenuation());
    ui->RCASValue->setText(QString::number(ui->RCASSlider->value() / 1000.0, 'f', 3));
    ui->volumeSlider->setValue(gs_settings.GetVolumeSlider());
    ui->volumeValue->setText(QString::number(ui->volumeSlider->value()) + "%");
    ui->graphicsAdapterBox->setCurrentIndex(gs_settings.GetGpuId() + 1);
    ui->pipelineCacheCheckBox->setChecked(gs_settings.IsPipelineCacheEnabled());

    QString translatedText_PresentMode =
        presentModeMap.key(QString::fromStdString(gs_settings.GetPresentMode()));
    ui->presentModeComboBox->setCurrentText(translatedText_PresentMode);

    ui->motionControlsCheckBox->setChecked(gs_settings.IsMotionControlsEnabled());
    ui->fullscreenModeComboBox->setCurrentText(
        QString::fromStdString(gs_settings.GetFullScreenMode()));
    ui->backgroundControllerCheckBox->setChecked(gs_settings.IsBackgroundControllerInput());

    ui->trophyKeyLineEdit->setText(QString::fromStdString(Config::TrophyKey));
    ui->trophyKeyLineEdit->setEchoMode(QLineEdit::Password);

    // Network Settings
    ui->shadnetCheckBox->setChecked(gs_settings.IsShadNetEnabled());
    ui->networkConnectedCheckBox->setChecked(gs_settings.IsConnectedToNetwork());
    ui->serverLineEdit->setText(QString::fromStdString(gs_settings.GetShadNetServer()));
    ui->servWebApiLineEdit->setText(QString::fromStdString(gs_settings.GetShadnetWebapiServer()));
    ui->upnpCheckBox->setChecked(gs_settings.IsUPnPEnabled());

    // User Settings
    ui->usernameLineEdit->setText(QString::fromStdString(user->user_name));
    ui->enableShadnetUserCheckBox->setChecked(user->shadnet_enabled);
    ui->shadnetIdLineEdit->setText(QString::fromStdString(user->shadnet_npid));
    ui->shadnetPasswordLineEdit->setText(QString::fromStdString(user->shadnet_password));
}

void ShadSettings::OnCursorStateChanged(int index) {
    if (index == -1)
        return;
    if (index == 1) {
        ui->idleTimeoutGroupBox->show();
    } else {
        if (!ui->idleTimeoutGroupBox->isHidden()) {
            ui->idleTimeoutGroupBox->hide();
        }
    }
}

void ShadSettings::updateNoteTextEdit(const QString& elementName) {
    QString text; // put texts definition somewhere

    // General
    if (elementName == "consoleLanguageGroupBox") {
        text = consoleLanguageGroupBoxtext;
    } else if (elementName == "FullscreenModeGroupBox") {
        text = fullscreenModeGroupBoxtext;
    } else if (elementName == "ReadbacksCheckBox") {
        text = ReadbacksCheckBoxtext;
    } else if (elementName == "GPUBufferCheckBox") {
        text = GPUBufferCheckBoxtext;
    } else if (elementName == "discordRPCCheckbox") {
        text = discordRPCCheckboxtext;
    } else if (elementName == "trophyKeyLineEdit") {
        text = TrophyKeytext;
    } else if (elementName == "logTypeGroupBox") {
        text = logTypeGroupBoxtext;
    } else if (elementName == "logFilter") {
        text = logFiltertext;
    } else if (elementName == "disableTrophycheckBox") {
        text = disableTrophycheckBoxtext;
    } else if (elementName == "motionControlsCheckBox") {
        text = motionControlsCheckBoxtext;
    } else if (elementName == "DevkitCheckBox") {
        text = DevkitCheckBoxtext;
    }

    // Input
    if (elementName == "hideCursorGroupBox") {
        text = hideCursorGroupBoxtext;
    } else if (elementName == "idleTimeoutGroupBox") {
        text = idleTimeoutGroupBoxtext;
    } else if (elementName == "backgroundControllerCheckBox") {
        text = backgroundControllerCheckBoxtext;
    }

    // Graphics
    if (elementName == "widthGroupBox") {
        text = resolutionLayouttext;
    } else if (elementName == "heightGroupBox") {
        text = resolutionLayouttext;
    } else if (elementName == "heightDivider") {
        text = vblanktext;
    } else if (elementName == "FSRCheckBox") {
        text = FSRtext;
    } else if (elementName == "RCASCheckBox") {
        text = RCAStext;
    } else if (elementName == "RCASLabel" || elementName == "RCASValue" ||
               elementName == "RCASSlider") {
        text = RCASAttenuationtext;
    } else if (elementName == "presentModeGroupBox") {
        text = PresentModetext;
    }

    ui->descriptionText->setText(text.replace("\\n", "\n"));
}

bool ShadSettings::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
        if (qobject_cast<QWidget*>(obj)) {
            bool hovered = (event->type() == QEvent::Enter);
            QString elementName = obj->objectName();

            if (hovered) {
                updateNoteTextEdit(elementName);
            } else {
                ui->descriptionText->setText(defaultTextEdit);
            }
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void ShadSettings::SaveSettings() {
    // Other Settings - use gs_settings
    gs_settings.SetShowSplash(ui->showSplashCheckBox->isChecked(), true);
    gs_settings.SetVolumeSlider(ui->volumeSlider->value(), true);
    gs_settings.SetTrophyPopupDisabled(ui->disableTrophycheckBox->isChecked(), true);
    gs_settings.SetTrophyNotificationDuration(ui->popUpDurationSpinBox->value(), true);

    std::string trophy_loc = ui->popUpPosComboBox->currentText().toStdString();
    gs_settings.SetTrophyNotificationSide(trophy_loc, true);

    // ------------------ Graphics tab --------------------------------------------------------
    bool isFullscreen = ui->fullscreenModeComboBox->currentText() != tr("Windowed");
    gs_settings.SetFullScreen(isFullscreen, true);
    gs_settings.SetPresentMode(
        presentModeMap.value(ui->presentModeComboBox->currentText()).toStdString(), true);
    gs_settings.SetFullScreenMode(ui->fullscreenModeComboBox->currentText().toStdString(), true);

    gs_settings.SetWindowHeight(ui->heightSpinBox->value(), true);
    gs_settings.SetWindowWidth(ui->widthSpinBox->value(), true);

    gs_settings.SetFsrEnabled(ui->FSRCheckBox->isChecked(), true);
    gs_settings.SetRcasEnabled(ui->RCASCheckBox->isChecked(), true);
    gs_settings.SetRcasAttenuation(ui->RCASSlider->value(), true);

    // First options is auto selection -1, so gpuId on the GUI will always have to subtract 1
    // when setting and add 1 when getting to select the correct gpu in Qt
    gs_settings.SetGpuId(ui->graphicsAdapterBox->currentIndex() - 1, true);

    // ------------------ Input tab --------------------------------------------------------
    gs_settings.SetCursorState(cursorStateMap.value(ui->hideCursorComboBox->currentText()), true);
    gs_settings.SetCursorHideTimeout(ui->idleTimeoutSpinBox->value(), true);
    gs_settings.SetMotionControlsEnabled(ui->motionControlsCheckBox->isChecked(), true);
    gs_settings.SetBackgroundControllerInput(ui->backgroundControllerCheckBox->isChecked(), true);

    // ------------------ Log tab --------------------------------------------------------
    gs_settings.SetLogFilter(ui->logFilterLineEdit->text().toStdString(), true);
    gs_settings.SetLogType(ui->logTypeComboBox->currentText().toStdString(), true);

    // ------------------ Debug tab --------------------------------------------------------
    gs_settings.SetCopyGpuBuffers(ui->GPUBufferCheckBox->isChecked(), true);

    gs_settings.SetReadbacksMode(ui->readbacksModeComboBox->currentIndex(), true);
    gs_settings.SetPipelineCacheEnabled(ui->pipelineCacheCheckBox->isChecked(), true);
    gs_settings.SetExtraDmemInMBytes(ui->dmemSpinBox->value(), true);
    gs_settings.SetDirectMemoryAccessEnabled(ui->DMACheckBox->isChecked(), true);
    gs_settings.SetVblankFrequency(ui->vblankSpinBox->value(), true);

    // Network Settings
    gs_settings.SetShadNetEnabled(ui->shadnetCheckBox->isChecked(), true);
    gs_settings.SetConnectedToNetwork(ui->networkConnectedCheckBox->isChecked(), true);
    gs_settings.SetShadNetServer(ui->serverLineEdit->text().toStdString(), true);
    gs_settings.SetShadnetWebapiServer(ui->servWebApiLineEdit->text().toStdString(), true);
    gs_settings.SetUPnPEnabled(ui->upnpCheckBox->isChecked(), true);

    // Global Settings - use EmulatorSettings
    EmulatorSettings.SetDiscordRPCEnabled(ui->discordRPCCheckbox->isChecked());
    EmulatorSettings.SetHomeDir(ui->HomePathLineEdit->text().toStdString());
    EmulatorSettings.SetAddonInstallDir(ui->DLCPathLineEdit->text().toStdString());

    QString code;
    QString currentLocale = ui->consoleLanguageComboBox->currentText();
    QList<QLocale> allLocales =
        QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for (int iLocale = 0; iLocale < allLocales.count(); iLocale++) {

        QLocale locale = allLocales.at(iLocale);
        QString locale_name = locale.nativeLanguageName();

        if (locale.territory() != QLocale::AnyTerritory) {
            locale_name += " (" + locale.nativeTerritoryName() + ")";
        }

        if (locale_name == currentLocale) {
            code = allLocales.at(iLocale).bcp47Name();
        }
    }

    EmulatorSettings.SetConsoleLanguage(language_ids[code]);

    QString key = ui->trophyKeyLineEdit->text();
    Config::TrophyKey = key.toStdString();
    Config::SaveTrophyKey(Config::TrophyKey);

    if (!gs_settings.Save(Common::game_serial)) {
        QMessageBox::information(this, "Error", "Unable to save game-specific settings");
        return;
    }

    if (!EmulatorSettings.Save()) {
        QMessageBox::information(this, "Error", "Unable to save settings");
        return;
    }

    // User settings
    user->user_name = ui->usernameLineEdit->text().toStdString();
    user->shadnet_enabled = ui->enableShadnetUserCheckBox->isChecked();
    user->shadnet_npid = ui->shadnetIdLineEdit->text().toStdString();
    user->shadnet_password = ui->shadnetPasswordLineEdit->text().toStdString();
    UserSettings.GetUserManager().Save();
}

void ShadSettings::SetDefaults() {
    ui->hideCursorComboBox->setCurrentIndex(1);
    OnCursorStateChanged(1);
    ui->idleTimeoutSpinBox->setValue(5);
    ui->widthSpinBox->setValue(1280);
    ui->heightSpinBox->setValue(720);
    ui->disableTrophycheckBox->setChecked(false);
    ui->popUpPosComboBox->setCurrentText("right");
    ui->popUpDurationSpinBox->setValue(6.0);
    ui->discordRPCCheckbox->setChecked(false);
    ui->GPUBufferCheckBox->setChecked(false);
    ui->logTypeComboBox->setCurrentText("sync");
    ui->logFilterLineEdit->setText("");
    ui->motionControlsCheckBox->setChecked(false);
    ui->backgroundControllerCheckBox->setChecked(false);
    ui->fullscreenModeComboBox->setCurrentText("Windowed");
    ui->FSRCheckBox->setChecked(true);
    ui->RCASCheckBox->setChecked(true);
    ui->RCASSlider->setValue(250);
    ui->volumeSlider->setValue(100);
    ui->presentModeComboBox->setCurrentText("Mailbox");
    ui->dmemSpinBox->setValue(0);
    ui->showSplashCheckBox->setChecked(false);
    ui->pipelineCacheCheckBox->setChecked(false);

    ui->vblankSpinBox->setValue(60);
    ui->readbacksModeComboBox->setCurrentIndex(0);
    ui->DMACheckBox->setChecked(false);
    ui->dmemSpinBox->setValue(0);

    ui->discordRPCCheckbox->setChecked(true);

    ui->shadnetCheckBox->setChecked(false);
    ui->networkConnectedCheckBox->setChecked(false);
    ui->serverLineEdit->setText("srv.shadps4.net:31313");
    ui->servWebApiLineEdit->setText("http://srv.shadps4.net:31315");
    ui->upnpCheckBox->setChecked(true);
}

void ShadSettings::getPhysicalDevices() {
    if (volkInitialize() != VK_SUCCESS) {
        qWarning() << "Failed to initialize Volk.";
        return;
    }

    // Create Vulkan instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "shadPS4QtLauncher";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&instInfo, nullptr, &instance) != VK_SUCCESS) {
        qWarning() << "Failed to create Vulkan instance.";
        return;
    }

    // Load instance-based function pointers
    volkLoadInstance(instance);

    // Enumerate devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        qWarning() << "No Vulkan physical devices found.";
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    m_physical_devices.clear();
    for (uint32_t i = 0; i < deviceCount; ++i) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);
        QString name = QString::fromUtf8(props.deviceName, -1);
        m_physical_devices.push_back(name);
    }

    vkDestroyInstance(instance, nullptr);
}

void ShadSettings::RefreshHostOverrideFileStatus() {
    if (std::filesystem::exists(Common::GetShadUserDir() / "host_overrides.json")) {
        ui->hostFileStatusLabel->setText("Host override file has been created");
        ui->hostFileStatusLabel->setStyleSheet("font-weight: bold; color: green;");
    } else {
        ui->hostFileStatusLabel->setText("Host override file does not exist");
        ui->hostFileStatusLabel->setStyleSheet("font-weight: bold; color: red;");
    }
}

ShadSettings::~ShadSettings() {
    // Clean up game-specific settings when dialog closes
    EmulatorSettings.Load();
}
