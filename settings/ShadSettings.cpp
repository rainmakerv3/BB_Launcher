// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFileDialog>
#include <QHoverEvent>
#include <QMessageBox>
#include <QStandardPaths>

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include "ShadSettings.h"
#include "config.h"
#include "modules/Common.h"
#include "settings/ui_ShadSettings.h"

static std::vector<QString> m_physical_devices;
using json = nlohmann::json;

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

ShadSettings::ShadSettings(std::shared_ptr<EmulatorSettings> emu_settings,
                           std::shared_ptr<IpcClient> ipc_client, bool game_specific,
                           QWidget* parent)
    : m_emu_settings(std::move(emu_settings)), is_game_specific(game_specific),
      m_ipc_client(ipc_client), QDialog(parent), ui(new Ui::ShadSettings) {

    if (is_game_specific) {
        std::string filename = Common::game_serial + ".json";
        std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;

        if (!std::filesystem::exists(gsConfig)) {
            if (QMessageBox::Yes ==
                QMessageBox::question(this, "No game-specific config file found",
                                      QString::fromStdString(gsConfig.string()) +
                                          " not found. Do you want to create it?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                const std::filesystem::path cfgDir = Common::GetShadUserDir() / "custom_configs";
                std::filesystem::create_directories(cfgDir);
                const std::filesystem::path path = cfgDir / (Common::game_serial + ".json");

                m_emu_settings->Load();
                m_emu_settings->Save(Common::game_serial);

            } else {
                return;
            }
        }
    }

    ui->setupUi(this);
    getPhysicalDevices();
    ui->tabWidgetSettings->setUsesScrollButtons(false);
    initialHeight = this->height();

    ui->tabWidgetSettings->setCurrentIndex(0);

    if (game_specific) {
        ui->tabWidgetSettings->setTabVisible(2, false);
    } else {
        ui->tabWidgetSettings->setTabVisible(1, false);
    }

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Save)->setFocus();

    // ui->consoleLanguageComboBox->addItems(languageNames);

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

    LoadValuesFromConfig();

    defaultTextEdit = "Point your mouse at an option to display its description.";
    ui->descriptionText->setText(defaultTextEdit);

    if (game_specific) {
        QPushButton* deleteButton = new QPushButton("Delete Game-specific config");
        ui->buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);

        connect(deleteButton, &QPushButton::pressed, this, [this]() {
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
    }

    connect(this, &QDialog::rejected, this, [this]() {
        // reset real-time widgets to config value if not saved
        int sliderValue = m_emu_settings->GetVolumeSlider();
        ui->volumeSlider->setValue(sliderValue);

        if (Config::GameRunning) {
            m_ipc_client->setFsr(m_emu_settings->IsFsrEnabled());
            m_ipc_client->setRcas(m_emu_settings->IsRcasEnabled());
            m_ipc_client->setRcasAttenuation(m_emu_settings->GetRcasAttenuation());
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
            m_ipc_client->adjustVol(value, is_game_specific);
    });

    connect(ui->SavePathButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Not implemented yet",
                                 "Recent shadPS4 changes require a new implementation for this, "
                                 "disabled in the meantime");
        return;

        QString initial_path;
        Common::PathToQString(initial_path, Config::externalSaveDir);
        QString save_data_path_string =
            QFileDialog::getExistingDirectory(this, "Directory to save data", initial_path);
        auto file_path = Common::PathFromQString(save_data_path_string);
        if (!file_path.empty()) {
            Config::externalSaveDir = file_path;
            ui->SavePathLineEdit->setText(save_data_path_string);

            // TODO: transfer savedir to emu settings
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
            std::filesystem::remove_all(cachePath);
            QMessageBox::information(this, "Removal Completed",
                                     QString("Shader cache for %1 successfully removed.")
                                         .arg(QString::fromStdString(Common::game_serial)));
        }
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
        ui->ReadbacksCheckBox->installEventFilter(this);
        ui->GPUBufferCheckBox->installEventFilter(this);
        ui->discordRPCCheckbox->installEventFilter(this);
        ui->userName->installEventFilter(this);
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
        ui->DevkitCheckBox->installEventFilter(this);
        ui->backgroundControllerCheckBox->installEventFilter(this);
    }
}

void ShadSettings::LoadValuesFromConfig() {
    if (is_game_specific) {
        if (!m_emu_settings->Load(Common::game_serial)) {
            QMessageBox::information(this, "Error", "Unable to load settings");
            return;
        }
    } else {
        if (!m_emu_settings->Load()) {
            QMessageBox::information(this, "Error", "Unable to load settings");
            return;
        }
    }

    ui->hideCursorComboBox->setCurrentIndex(m_emu_settings->GetCursorState());
    OnCursorStateChanged(ui->hideCursorComboBox->currentIndex());
    ui->idleTimeoutSpinBox->setValue(m_emu_settings->GetCursorHideTimeout());
    ui->widthSpinBox->setValue(m_emu_settings->GetWindowWidth());
    ui->heightSpinBox->setValue(m_emu_settings->GetWindowHeight());
    ui->vblankSpinBox->setValue(m_emu_settings->GetVblankFrequency());
    ui->ReadbacksCheckBox->setChecked(m_emu_settings->IsReadbacksEnabled());
    ui->GPUBufferCheckBox->setChecked(m_emu_settings->IsCopyGpuBuffers());
    ui->disableTrophycheckBox->setChecked(m_emu_settings->IsTrophyPopupDisabled());
    ui->popUpPosComboBox->setCurrentText(
        QString::fromStdString(m_emu_settings->GetTrophyNotificationSide()));
    ui->popUpDurationSpinBox->setValue(m_emu_settings->GetTrophyNotificationDuration());

    ui->showSplashCheckBox->setChecked(m_emu_settings->IsShowSplash());
    ui->discordRPCCheckbox->setChecked(m_emu_settings->IsDiscordRPCEnabled());
    ui->DevkitCheckBox->setChecked(m_emu_settings->IsDevKit());
    ui->dmemSpinBox->setValue(m_emu_settings->GetExtraDmemInMBytes());
    ui->logTypeComboBox->setCurrentText(QString::fromStdString(m_emu_settings->GetLogType()));
    ui->logFilterLineEdit->setText(QString::fromStdString(m_emu_settings->GetLogFilter()));

    // TODO username
    //  ui->userNameLineEdit->setText(QString::fromStdString(m_emu_settings->getuse));

    // TODO trophy key
    // ui->trophyKeyLineEdit->setText(QString::fromStdString(m_emu_settings->getuse));
    ui->trophyKeyLineEdit->setEchoMode(QLineEdit::Password);

    ui->FSRCheckBox->setChecked(m_emu_settings->IsFsrEnabled());
    ui->RCASCheckBox->setChecked(m_emu_settings->IsRcasEnabled());
    ui->RCASSlider->setValue(m_emu_settings->GetRcasAttenuation());
    ui->RCASValue->setText(QString::number(ui->RCASSlider->value() / 1000.0, 'f', 3));
    ui->volumeSlider->setValue(m_emu_settings->GetVolumeSlider());
    ui->volumeValue->setText(QString::number(ui->volumeSlider->value()) + "%");
    ui->graphicsAdapterBox->setCurrentIndex(m_emu_settings->GetGpuId() + 1);
    ui->pipelineCacheCheckBox->setChecked(m_emu_settings->IsPipelineCacheEnabled());

    QString translatedText_PresentMode =
        presentModeMap.key(QString::fromStdString(m_emu_settings->GetPresentMode()));
    ui->presentModeComboBox->setCurrentText(translatedText_PresentMode);

    QString save_data_path_string;
    Common::PathToQString(save_data_path_string, Config::externalSaveDir);
    ui->SavePathLineEdit->setText(save_data_path_string);

    ui->motionControlsCheckBox->setChecked(m_emu_settings->IsMotionControlsEnabled());
    ui->fullscreenModeComboBox->setCurrentText(
        QString::fromStdString(m_emu_settings->GetFullScreenMode()));
    ui->backgroundControllerCheckBox->setChecked(m_emu_settings->IsBackgroundControllerInput());
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
    } else if (elementName == "userName") {
        text = userNametext;
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
    m_emu_settings->SetShowSplash(ui->showSplashCheckBox->isChecked());
    m_emu_settings->SetVolumeSlider(ui->volumeSlider->value());
    m_emu_settings->SetTrophyPopupDisabled(ui->disableTrophycheckBox->isChecked());
    m_emu_settings->SetTrophyNotificationDuration(ui->popUpDurationSpinBox->value());

    std::string trophy_loc = ui->popUpPosComboBox->currentText().toStdString();
    m_emu_settings->SetTrophyNotificationSide(trophy_loc);

    m_emu_settings->SetDiscordRPCEnabled(ui->discordRPCCheckbox->isChecked());

    // ------------------ Graphics tab --------------------------------------------------------
    bool isFullscreen = ui->fullscreenModeComboBox->currentText() != tr("Windowed");
    m_emu_settings->SetFullScreen(isFullscreen);
    m_emu_settings->SetPresentMode(
        presentModeMap.value(ui->presentModeComboBox->currentText()).toStdString());
    m_emu_settings->SetFullScreenMode(ui->fullscreenModeComboBox->currentText().toStdString());

    m_emu_settings->SetWindowHeight(ui->heightSpinBox->value());
    m_emu_settings->SetWindowWidth(ui->widthSpinBox->value());

    m_emu_settings->SetFsrEnabled(ui->FSRCheckBox->isChecked());
    m_emu_settings->SetRcasEnabled(ui->RCASCheckBox->isChecked());
    m_emu_settings->SetRcasAttenuation(ui->RCASSlider->value());

    // First options is auto selection -1, so gpuId on the GUI will always have to subtract 1
    // when setting and add 1 when getting to select the correct gpu in Qt
    m_emu_settings->SetGpuId(ui->graphicsAdapterBox->currentIndex() - 1);

    // ------------------ Input tab --------------------------------------------------------
    m_emu_settings->SetCursorState(cursorStateMap.value(ui->hideCursorComboBox->currentText()));
    m_emu_settings->SetCursorHideTimeout(ui->idleTimeoutSpinBox->value());
    m_emu_settings->SetMotionControlsEnabled(ui->motionControlsCheckBox->isChecked());
    m_emu_settings->SetBackgroundControllerInput(ui->backgroundControllerCheckBox->isChecked());

    // ------------------ Log tab --------------------------------------------------------
    m_emu_settings->SetLogFilter(ui->logFilterLineEdit->text().toStdString());
    m_emu_settings->SetLogType(ui->logTypeComboBox->currentText().toStdString());

    // ------------------ Debug tab --------------------------------------------------------
    m_emu_settings->SetCopyGpuBuffers(ui->GPUBufferCheckBox->isChecked());

    // ------------------ Experimental tab --------------------------------------------------------
    m_emu_settings->SetReadbacksEnabled(ui->ReadbacksCheckBox->isChecked());
    m_emu_settings->SetDevKit(ui->DevkitCheckBox->isChecked());

    // m_emu_settings->SetPSNSignedIn(ui->psnSignInCheckBox->isChecked());
    // m_emu_settings->SetConnectedToNetwork(ui->networkConnectedCheckBox->isChecked());

    m_emu_settings->SetPipelineCacheEnabled(ui->pipelineCacheCheckBox->isChecked());
    m_emu_settings->SetExtraDmemInMBytes(ui->dmemSpinBox->value());
    m_emu_settings->SetVblankFrequency(ui->vblankSpinBox->value());

    if (is_game_specific) {
        if (!m_emu_settings->Save(Common::game_serial)) {
            QMessageBox::information(this, "Error", "Unable to save settings");
            return;
        }
    } else {
        if (!m_emu_settings->Save()) {
            QMessageBox::information(this, "Error", "Unable to save settings");
            return;
        }
    }
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
    ui->userNameLineEdit->setText("shadPS4");
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

    if (is_game_specific) {
        ui->vblankSpinBox->setValue(60);
        ui->ReadbacksCheckBox->setChecked(false);
        ui->DevkitCheckBox->setChecked(false);
        ui->dmemSpinBox->setValue(0);
    } else {
        ui->discordRPCCheckbox->setChecked(true);
    }
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

ShadSettings::~ShadSettings() {}
