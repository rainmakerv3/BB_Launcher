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
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_ShadSettings.h"

static std::vector<QString> m_physical_devices;

ShadSettings::ShadSettings(std::shared_ptr<IpcClient> ipc_client, bool game_specific,
                           QWidget* parent)
    : is_game_specific(game_specific), m_ipc_client(ipc_client), QDialog(parent),
      ui(new Ui::ShadSettings) {
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

    // hide old version settings for new versions
    if (!Config::isReleaseOlder(11)) {
        ui->oldVersionsGroupBox->setVisible(false);
    }

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Save)->setFocus();

    ui->consoleLanguageComboBox->addItems(languageNames);

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
            std::string filename = Common::game_serial + ".toml";
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
        toml::value data = toml::parse(Common::GetShadUserDir() / "config.toml");
        toml::value gs_data;
        is_game_specific ? gs_data = toml::parse(Common::GetShadUserDir() / "custom_configs" /
                                                 (Common::game_serial + ".toml"))
                         : gs_data = data;

        // reset real-time widgets to config value if not saved
        int sliderValue = toml::find_or<int>(gs_data, "General", "volumeSlider", 100);
        ui->volumeSlider->setValue(sliderValue);

        if (Config::GameRunning) {
            m_ipc_client->setFsr(toml::find_or<bool>(gs_data, "GPU", "fsrEnabled", true));
            m_ipc_client->setRcas(toml::find_or<bool>(gs_data, "GPU", "rcasEnabled", true));
            m_ipc_client->setRcasAttenuation(
                toml::find_or<int>(gs_data, "GPU", "rcasAttenuation", 250));
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
        QString initial_path;
        Common::PathToQString(initial_path, Config::externalSaveDir);
        QString save_data_path_string =
            QFileDialog::getExistingDirectory(this, "Directory to save data", initial_path);
        auto file_path = Common::PathFromQString(save_data_path_string);

        if (!file_path.empty()) {
            Config::externalSaveDir = file_path;
            ui->SavePathLineEdit->setText(save_data_path_string);

            Config::ShadSettings settings;
            settings.savePath = Config::externalSaveDir;
            Config::SaveShadSettings(settings);
        }
    });

    connect(ui->DlcPathButton, &QPushButton::clicked, this, [this]() {
        QString initial_path;
        Common::PathToQString(initial_path, Config::dlcDir);
        QString dlc_path_string =
            QFileDialog::getExistingDirectory(this, "Directory to dlc files", initial_path);
        auto file_path = Common::PathFromQString(dlc_path_string);

        if (!file_path.empty()) {
            Config::dlcDir = file_path;
            ui->DLCPathLineEdit->setText(dlc_path_string);

            Config::ShadSettings settings;
            settings.dlcPath = Config::dlcDir;
            Config::SaveShadSettings(settings);
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
        ui->consoleLanguageGroupBox->installEventFilter(this);
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
    std::string filename = Common::game_serial + ".toml";
    std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;
    std::filesystem::path shadConfigFile =
        is_game_specific ? gsConfig : Common::GetShadUserDir() / "config.toml";

    toml::value data;

    try {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ifs.open(shadConfigFile, std::ios_base::binary);
        data = toml::parse(ifs, std::string{fmt::UTF(shadConfigFile.filename().u8string()).data});
    } catch (std::exception& ex) {
        // handle ?
        return;
    }

    const QVector<int> languageIndexes = {21, 23, 14, 6, 18, 1, 12, 22, 2, 4,  25, 24, 29, 5,  0, 9,
                                          15, 16, 17, 7, 26, 8, 11, 20, 3, 13, 27, 10, 19, 30, 28};

    ui->consoleLanguageComboBox->setCurrentIndex(
        std::distance(languageIndexes.begin(),
                      std::find(languageIndexes.begin(), languageIndexes.end(),
                                toml::find_or<int>(data, "Settings", "consoleLanguage", 1))) %
        languageIndexes.size());
    ui->hideCursorComboBox->setCurrentIndex(toml::find_or<int>(data, "Input", "cursorState", 1));
    OnCursorStateChanged(toml::find_or<int>(data, "Input", "cursorState", 1));
    ui->idleTimeoutSpinBox->setValue(toml::find_or<int>(data, "Input", "cursorHideTimeout", 5));
    ui->widthSpinBox->setValue(toml::find_or<int>(data, "GPU", "screenWidth", 1280));
    ui->heightSpinBox->setValue(toml::find_or<int>(data, "GPU", "screenHeight", 720));
    ui->vblankSpinBox->setValue(toml::find_or<int>(data, "GPU", "vblankFrequency", 60));
    ui->ReadbacksCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "readbacks", false));
    ui->DMACheckBox->setChecked(toml::find_or<bool>(data, "GPU", "directMemoryAccess", false));
    ui->GPUBufferCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "copyGPUBuffers", false));
    ui->disableTrophycheckBox->setChecked(
        toml::find_or<bool>(data, "General", "isTrophyPopupDisabled", false));
    ui->popUpPosComboBox->setCurrentText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "sideTrophy", "right")));
    ui->popUpDurationSpinBox->setValue(
        toml::find_or<double>(data, "General", "trophyNotificationDuration", 6.0));

    ui->showSplashCheckBox->setChecked(toml::find_or<bool>(data, "General", "showSplash", false));
    ui->discordRPCCheckbox->setChecked(
        toml::find_or<bool>(data, "General", "enableDiscordRPC", false));
    ui->dmemSpinBox->setValue(toml::find_or<int>(data, "General", "extraDmemInMbytes", 0));
    ui->logTypeComboBox->setCurrentText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "logType", "sync")));
    ui->logFilterLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "logFilter", "")));
    ui->userNameLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "userName", "shadPS4")));
    ui->trophyKeyLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "Keys", "TrophyKey", "")));
    ui->trophyKeyLineEdit->setEchoMode(QLineEdit::Password);
    ui->FSRCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "fsrEnabled", true));
    ui->RCASCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "rcasEnabled", true));
    ui->RCASSlider->setValue(toml::find_or<int>(data, "GPU", "rcasAttenuation", 250));
    ui->RCASValue->setText(QString::number(ui->RCASSlider->value() / 1000.0, 'f', 3));
    ui->volumeSlider->setValue(toml::find_or<int>(data, "General", "volumeSlider", 100));
    ui->volumeValue->setText(QString::number(ui->volumeSlider->value()) + "%");
    ui->graphicsAdapterBox->setCurrentIndex(toml::find_or<int>(data, "Vulkan", "gpuId", -1) + 1);
    ui->pipelineCacheCheckBox->setChecked(
        toml::find_or<bool>(data, "Vulkan", "pipelineCacheEnable", false));
    ui->networkConnectedCheckBox->setChecked(
        toml::find_or<bool>(data, "General", "isConnectedToNetwork", false));
    ui->psnSignInCheckBox->setChecked(
        toml::find_or<bool>(data, "General", "isPSNSignedIn", false)
    );
    ui->httpHostOverrideEdit->setText(QString::fromStdString(
        toml::find_or<std::string>(data, "General", "httpHostOverride", "thehuntersdream.com")));

    QString translatedText_PresentMode = presentModeMap.key(
        QString::fromStdString(toml::find_or<std::string>(data, "GPU", "presentMode", "Mailbox")));
    ui->presentModeComboBox->setCurrentText(translatedText_PresentMode);

    QString save_data_path_string;
    Common::PathToQString(save_data_path_string, Config::externalSaveDir);
    ui->SavePathLineEdit->setText(save_data_path_string);

    QString dlc_path_string;
    Common::PathToQString(save_data_path_string, Config::dlcDir);
    ui->DLCPathLineEdit->setText(save_data_path_string);

    ui->motionControlsCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "isMotionControlsEnabled", false));
    ui->fullscreenModeComboBox->setCurrentText(QString::fromStdString(
        toml::find_or<std::string>(data, "GPU", "FullscreenMode", "Windowed")));
    ui->backgroundControllerCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "backgroundControllerInput", false));

    ui->vblankDividerSpinBox->setValue(toml::find_or<int>(data, "GPU", "vblankDivider", 1));
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
    toml::value data;
    std::string filename = Common::game_serial + ".toml";
    std::filesystem::path gsConfig = Common::GetShadUserDir() / "custom_configs" / filename;
    std::filesystem::path shadConfigFile =
        is_game_specific ? gsConfig : Common::GetShadUserDir() / "config.toml";

    std::error_code error;
    if (std::filesystem::exists(shadConfigFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(shadConfigFile, std::ios_base::binary);
            data =
                toml::parse(ifs, std::string{fmt::UTF(shadConfigFile.filename().u8string()).data});
        } catch (const std::exception& ex) {
            // handle
            return;
        }
    } else {
        if (error) {
            // handle
        }
    }

    // Save old release settings
    if (Config::isReleaseOlder(11)) {
        data["GPU"]["vblankDivider"] = ui->vblankDividerSpinBox->value();
    }

    if (is_game_specific) {
        data["General"]["extraDmemInMbytes"] = ui->dmemSpinBox->value();

        data["General"]["isConnectedToNetwork"] = ui->networkConnectedCheckBox->isChecked();
        data["General"]["isPSNSignedIn"] = ui->psnSignInCheckBox->isChecked();

        data["General"]["httpHostOverride"] = ui->httpHostOverrideEdit->text().toStdString();

        data["GPU"]["readbacks"] = ui->ReadbacksCheckBox->isChecked();
        data["GPU"]["directMemoryAccess"] = ui->DMACheckBox->isChecked();
        data["GPU"]["vblankFrequency"] = ui->vblankSpinBox->value();
    } else {
        data["General"]["enableDiscordRPC"] = ui->discordRPCCheckbox->isChecked();

        data["Keys"]["TrophyKey"] = ui->trophyKeyLineEdit->text().toStdString();

        Config::TrophyKey = ui->trophyKeyLineEdit->text().toStdString();
    }

    data["General"]["isTrophyPopupDisabled"] = ui->disableTrophycheckBox->isChecked();
    data["General"]["sideTrophy"] = ui->popUpPosComboBox->currentText().toStdString();
    data["General"]["trophyNotificationDuration"] = ui->popUpDurationSpinBox->value();
    data["General"]["logFilter"] = ui->logFilterLineEdit->text().toStdString();
    data["General"]["logType"] = ui->logTypeComboBox->currentText().toStdString();
    data["General"]["userName"] = ui->userNameLineEdit->text().toStdString();
    data["General"]["volumeSlider"] = ui->volumeSlider->value();
    data["General"]["showSplash"] = ui->showSplashCheckBox->isChecked();

    data["Input"]["cursorState"] = ui->hideCursorComboBox->currentIndex();
    data["Input"]["cursorHideTimeout"] = ui->idleTimeoutSpinBox->value();
    data["Input"]["isMotionControlsEnabled"] = ui->motionControlsCheckBox->isChecked();
    data["Input"]["backgroundControllerInput"] = ui->backgroundControllerCheckBox->isChecked();

    data["GPU"]["screenWidth"] = ui->widthSpinBox->value();
    data["GPU"]["screenHeight"] = ui->heightSpinBox->value();
    data["GPU"]["copyGPUBuffers"] = ui->GPUBufferCheckBox->isChecked();
    data["GPU"]["fsrEnabled"] = ui->FSRCheckBox->isChecked();
    data["GPU"]["rcasEnabled"] = ui->RCASCheckBox->isChecked();
    data["GPU"]["rcasAttenuation"] = ui->RCASSlider->value();
    data["GPU"]["FullscreenMode"] = ui->fullscreenModeComboBox->currentText().toStdString();
    data["GPU"]["presentMode"] =
        presentModeMap.value(ui->presentModeComboBox->currentText()).toStdString();

    bool isFullscreen = ui->fullscreenModeComboBox->currentText() != "Windowed";
    data["GPU"]["Fullscreen"] = isFullscreen;

    data["Vulkan"]["gpuId"] = ui->graphicsAdapterBox->currentIndex() - 1;
    data["Vulkan"]["pipelineCacheEnable"] = ui->pipelineCacheCheckBox->isChecked();

    data["Settings"]["consoleLanguage"] =
        languageIndexes[ui->consoleLanguageComboBox->currentIndex()];

    std::ofstream file(shadConfigFile, std::ios::binary);
    file << data;
    file.close();
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
        ui->DMACheckBox->setChecked(false);
        ui->dmemSpinBox->setValue(0);
        ui->networkConnectedCheckBox->setChecked(false);
        ui->psnSignInCheckBox->setChecked(false);
        ui->httpHostOverrideEdit->setText("thehuntersdream.com");
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
