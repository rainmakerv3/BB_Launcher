// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <chrono>
#include <QFileDialog>
#include <QHoverEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <qmicroz.h>
#include <sys/stat.h>

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include "ShadSettings.h"
#include "config.h"
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_ShadSettings.h"

#if __APPLE__
#include <date/date.h>
#include <date/tz.h>
#include <date/tz_private.h>
#endif

static std::vector<QString> m_physical_devices;

ShadSettings::ShadSettings(std::shared_ptr<IpcClient> ipc_client, bool game_specific,
                           QWidget* parent)
    : is_game_specific(game_specific), m_ipc_client(ipc_client), QDialog(parent),
      ui(new Ui::ShadSettings) {
    ui->setupUi(this);
    getPhysicalDevices();
    ui->tabWidgetSettings->setUsesScrollButtons(false);
    initialHeight = this->height();

    if (game_specific) {
        ui->tabWidgetSettings->setTabVisible(2, false);
    } else {
        ui->tabWidgetSettings->setTabVisible(1, false);
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

    ui->updaterGroupBox->setVisible(true);

    connect(ui->hideCursorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) { OnCursorStateChanged(index); });

    connect(ui->updateComboBox, &QComboBox::currentIndexChanged, this,
            [this]() { Config::UpdateChannel = ui->updateComboBox->currentText().toStdString(); });

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->volumeValue->setText(QString::number(value) + "%");
        if (Config::GameRunning)
            m_ipc_client->adjustVol(value, is_game_specific);
    });

    connect(ui->checkUpdateButton, &QPushButton::pressed, this, [this]() {
        if (Config::GameRunning) {
            QMessageBox::warning(this, "Cannot update",
                                 "Cannot update shadPS4 while game is running");
            return;
        }

        ui->checkUpdateButton->setEnabled(false);
        ui->buttonBox->setEnabled(false);
        SaveUpdateSettings();

        CheckShadUpdate* shadUpdateWindow = new CheckShadUpdate(false);
        QObject::connect(shadUpdateWindow, &CheckShadUpdate::DownloadProgressed,
                         ui->DownloadProgressBar, &QProgressBar::setValue);
        QObject::connect(shadUpdateWindow, &CheckShadUpdate::UpdateComplete, this, [this]() {
            ui->checkUpdateButton->setEnabled(true);
            ui->buttonBox->setEnabled(true);
        });
        shadUpdateWindow->exec();
    });

    connect(ui->SavePathButton, &QPushButton::clicked, this, [this]() {
        QString initial_path;
        Common::PathToQString(initial_path, Common::SaveDir);
        QString save_data_path_string =
            QFileDialog::getExistingDirectory(this, "Directory to save data", initial_path);
        auto file_path = Common::PathFromQString(save_data_path_string);
        if (!file_path.empty()) {
            Common::SaveDir = file_path;
            ui->SavePathLineEdit->setText(save_data_path_string);

            std::filesystem::path shadConfigFile = Common::GetShadUserDir() / "config.toml";
            toml::value shadData;
            try {
                std::ifstream ifs;
                ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                ifs.open(shadConfigFile, std::ios_base::binary);
                shadData = toml::parse(
                    ifs, std::string{fmt::UTF(shadConfigFile.filename().u8string()).data});
            } catch (std::exception& ex) {
                QMessageBox::critical(NULL, "Filesystem error", ex.what());
                return;
            }

            shadData["GUI"]["saveDataPath"] = std::string{Common::PathToU8(Common::SaveDir)};
            std::ofstream file(Common::GetShadUserDir() / "config.toml", std::ios::binary);
            file << shadData;
            file.close();
        }
    });

    connect(ui->RCASSlider, &QSlider::valueChanged, this, [this](int value) {
        QString RCASValue = QString::number(value / 1000.0, 'f', 3);
        ui->RCASValue->setText(RCASValue);
    });

    if (Config::GameRunning) {
        connect(ui->RCASSlider, &QSlider::valueChanged, this,
                [this](int value) { m_ipc_client->setRcasAttenuation(value); });

#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
        connect(ui->FSRCheckBox, &QCheckBox::stateChanged, this,
                [this](int state) { m_ipc_client->setFsr(state); });

        connect(ui->RCASCheckBox, &QCheckBox::stateChanged, this,
                [this](int state) { m_ipc_client->setRcas(state); });
#else
        connect(ui->FSRCheckBox, &QCheckBox::checkStateChanged, this,
                [this](Qt::CheckState state) { m_ipc_client->setFsr(state); });

        connect(ui->RCASCheckBox, &QCheckBox::checkStateChanged, this,
                [this](Qt::CheckState state) { m_ipc_client->setRcas(state); });
#endif
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
        ui->updaterComboBox->installEventFilter(this);
        ui->checkUpdateButton->installEventFilter(this);
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
    ui->GPUBufferCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "copyGPUBuffers", false));
    ui->disableTrophycheckBox->setChecked(
        toml::find_or<bool>(data, "General", "isTrophyPopupDisabled", false));
    ui->popUpPosComboBox->setCurrentText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "sideTrophy", "right")));
    ui->popUpDurationSpinBox->setValue(
        toml::find_or<double>(data, "General", "trophyNotificationDuration", 6.0));

    ui->discordRPCCheckbox->setChecked(
        toml::find_or<bool>(data, "General", "enableDiscordRPC", false));
    ui->DevkitCheckBox->setChecked(toml::find_or<bool>(data, "General", "isDevKit", false));
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

    QString translatedText_PresentMode = presentModeMap.key(
        QString::fromStdString(toml::find_or<std::string>(data, "GPU", "presentMode", "Mailbox")));
    ui->presentModeComboBox->setCurrentText(translatedText_PresentMode);

    QString save_data_path_string;
    Common::PathToQString(save_data_path_string, Common::SaveDir);
    ui->SavePathLineEdit->setText(save_data_path_string);

    ui->motionControlsCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "isMotionControlsEnabled", false));
    ui->fullscreenModeComboBox->setCurrentText(QString::fromStdString(
        toml::find_or<std::string>(data, "GPU", "FullscreenMode", "Windowed")));
    ui->backgroundControllerCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "backgroundControllerInput", false));

    ui->autoUpdateCheckBox->setChecked(Config::AutoUpdateShadEnabled);
    ui->updateComboBox->setCurrentText(QString::fromStdString(Config::UpdateChannel));
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
    } else if (elementName == "updaterComboBox") {
        text = updaterComboBoxtext;
    } else if (elementName == "checkUpdateButton") {
        text = checkUpdateButtontext;
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

    if (is_game_specific) {
        data["General"]["isDevKit"] = ui->DevkitCheckBox->isChecked();
        data["General"]["extraDmemInMbytes"] = ui->dmemSpinBox->value();

        data["GPU"]["readbacks"] = ui->ReadbacksCheckBox->isChecked();
        data["GPU"]["vblankFrequency"] = ui->vblankSpinBox->value();
    } else {
        data["General"]["enableDiscordRPC"] = ui->discordRPCCheckbox->isChecked();

        data["Keys"]["TrophyKey"] = ui->trophyKeyLineEdit->text().toStdString();

        Config::TrophyKey = ui->trophyKeyLineEdit->text().toStdString();
        SaveUpdateSettings();
    }

    data["General"]["isTrophyPopupDisabled"] = ui->disableTrophycheckBox->isChecked();
    data["General"]["sideTrophy"] = ui->popUpPosComboBox->currentText().toStdString();
    data["General"]["trophyNotificationDuration"] = ui->popUpDurationSpinBox->value();
    data["General"]["logFilter"] = ui->logFilterLineEdit->text().toStdString();
    data["General"]["logType"] = ui->logTypeComboBox->currentText().toStdString();
    data["General"]["userName"] = ui->userNameLineEdit->text().toStdString();
    data["General"]["volumeSlider"] = ui->volumeSlider->value();

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
    ui->vblankSpinBox->setValue(60);
    ui->disableTrophycheckBox->setChecked(false);
    ui->popUpPosComboBox->setCurrentText("right");
    ui->popUpDurationSpinBox->setValue(6.0);
    ui->discordRPCCheckbox->setChecked(false);
    ui->ReadbacksCheckBox->setChecked(false);
    ui->DevkitCheckBox->setChecked(false);
    ui->GPUBufferCheckBox->setChecked(false);
    ui->logTypeComboBox->setCurrentText("sync");
    ui->logFilterLineEdit->setText("");
    ui->userNameLineEdit->setText("shadPS4");
    ui->updateComboBox->setCurrentText("Nightly");
    ui->autoUpdateCheckBox->setChecked(false);
    ui->motionControlsCheckBox->setChecked(false);
    ui->backgroundControllerCheckBox->setChecked(false);
    ui->fullscreenModeComboBox->setCurrentText("Windowed");
    ui->FSRCheckBox->setChecked(true);
    ui->RCASCheckBox->setChecked(true);
    ui->RCASSlider->setValue(250);
    ui->volumeSlider->setValue(100);
    ui->presentModeComboBox->setCurrentText("Mailbox");
    ui->dmemSpinBox->setValue(0);
}

void ShadSettings::SaveUpdateSettings() {
    Config::AutoUpdateShadEnabled = ui->autoUpdateCheckBox->isChecked();

    std::filesystem::path guiConfig = Common::GetShadUserDir() / "qt_ui.ini";
    std::fstream guiFile(guiConfig);
    std::string line;
    std::vector<std::string> lines;
    int lineCount = 0;

    while (std::getline(guiFile, line)) {
        lineCount++;

        if (line.contains("updateChannel")) {
            lines.push_back("updateChannel=" + ui->updateComboBox->currentText().toStdString());
            continue;
        }

        if (line.contains("checkForUpdates")) {
            std::string updatesEnabled = Config::AutoUpdateShadEnabled ? "true" : "false";
            lines.push_back("checkForUpdates=" + updatesEnabled);
            continue;
        }
        lines.push_back(line);
    }
    guiFile.close();

    std::ofstream output_file(guiConfig);
    for (auto const& line : lines) {
        output_file << line << '\n';
    }
    output_file.close();
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

CheckShadUpdate::CheckShadUpdate(const bool isAutoupdate, QWidget* parent) : QDialog(parent) {
    setFixedSize(0, 0);
    CheckShadUpdate::UpdateShad(isAutoupdate);

    connect(this, &CheckShadUpdate::UpdateComplete, this, &QDialog::close);
}

void CheckShadUpdate::UpdateShad(bool isAutoupdate) {
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QUrl url;

    bool checkName = true;
    while (checkName) {
        updateChannel = QString::fromStdString(Config::UpdateChannel);
        if (updateChannel == "Nightly") {
            url = QUrl("https://api.github.com/repos/shadps4-emu/shadPS4/releases");
            checkName = false;
        } else if (updateChannel == "Release") {
            url = QUrl("https://api.github.com/repos/shadps4-emu/shadPS4/releases/latest");
            checkName = false;
        }
    }

    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, isAutoupdate, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Network error:") + "\n" +
                                     reply->errorString());
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(response));

        if (jsonDoc.isNull()) {
            QMessageBox::warning(this, "Error", "Failed to parse update information.");
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        QString downloadUrl;
        QString latestRev;
        QString latestDate;
        QString platformString;

#ifdef Q_OS_WIN
        platformString = "win64-sdl";
#elif defined(Q_OS_LINUX)
        platformString = "linux-sdl";
#elif defined(Q_OS_MAC)
        platformString = "macos-sdl";
#endif
        QJsonObject jsonObj;
        if (updateChannel == "Nightly") {
            QJsonArray jsonArray = jsonDoc.array();
            for (const QJsonValue& value : jsonArray) {
                jsonObj = value.toObject();
                if (jsonObj.contains("prerelease") && jsonObj["prerelease"].toBool()) {
                    break;
                }
            }
            if (!jsonObj.isEmpty()) {
                latestVersion = jsonObj["tag_name"].toString();
            } else {
                QMessageBox::warning(this, "Error", "No pre-releases found.");
                reply->deleteLater();
                emit UpdateComplete();
                return;
            }
        } else {
            jsonObj = jsonDoc.object();
            if (jsonObj.contains("tag_name")) {
                latestVersion = jsonObj["tag_name"].toString();
            } else {
                QMessageBox::warning(this, "Error", "Invalid release data.");
                reply->deleteLater();
                emit UpdateComplete();
                return;
            }
        }

        QJsonArray assets = jsonObj["assets"].toArray();
        bool found = false;

        for (const QJsonValue& assetValue : assets) {
            QJsonObject assetObj = assetValue.toObject();
            if (assetObj["name"].toString().contains(platformString)) {
                downloadUrl = assetObj["browser_download_url"].toString();
                found = true;
                break;
            }
        }

        if (!found) {
            QMessageBox::warning(this, "Error", "No download URL found for the specified asset.");
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

        if (Config::LastBuildHash != "") {
            bool isUpdated;
            if (updateChannel == "Release") {
                isUpdated = latestVersion.toStdString() == Config::LastBuildBranch;
            } else {
                isUpdated = latestVersion.right(40).toStdString() == Config::LastBuildHash;
            }

            if (!isUpdated) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(this, "Update confirmation",
                                          "New shadPS4 build detected. Proceed with Update?",
                                          QMessageBox::Yes | QMessageBox::No)) {
                    DownloadUpdate(downloadUrl);
                } else {
                    emit UpdateComplete();
                }
                return;
            }

            if (!isAutoupdate) {
                QMessageBox::information(this, "BBLauncher", "Current build is already updated.");
            }

            emit UpdateComplete();
            return;
        }

        std::chrono::time_point shadWriteTime =
            std::filesystem::last_write_time(Common::shadPs4Executable);

#if __APPLE__
        auto sec =
            std::chrono::duration_cast<std::chrono::seconds>(shadWriteTime.time_since_epoch());
        auto time = date::sys_time<std::chrono::seconds>{sec};
        auto zonedt = date::make_zoned(date::current_zone(), time);
        std::string shadModifiedDateString = date::format("{:%F}", zonedt);
        std::string shadModifiedDateStringUTC = date::format("{:%F}", time);

#else
        auto shadTimePoint = std::chrono::clock_cast<std::chrono::system_clock>(shadWriteTime);
        const std::chrono::zoned_time zonedt{std::chrono::current_zone()->name(), shadTimePoint};
        std::string shadModifiedDateString = std::format("{:%F}", zonedt);
        std::string shadModifiedDateStringUTC = std::format("{:%F}", shadWriteTime);
#endif

        std::string latestVerDateString =
            jsonObj["published_at"].toString().toStdString().substr(0, 10);

        if (shadModifiedDateString == latestVerDateString ||
            shadModifiedDateStringUTC == latestVerDateString) {
            if (isAutoupdate) {
                emit UpdateComplete();
                return;
            } else {
                if (QMessageBox::No ==
                    QMessageBox::question(this, "Update confirmation",
                                          "Latest shadPS4 build has the same date as the currently "
                                          "installed build. Proceed with update?",
                                          QMessageBox::Yes | QMessageBox::No)) {
                    emit UpdateComplete();
                    return;
                }
            }
        } else {
            if (QMessageBox::No ==
                QMessageBox::question(this, "Update confirmation",
                                      "New shadPS4 build detected. Proceed with Update?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                emit UpdateComplete();
                return;
            }
        }

        DownloadUpdate(downloadUrl);
    });
}

void CheckShadUpdate::DownloadUpdate(const QString& downloadUrl) {
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QNetworkRequest request(downloadUrl);
    QNetworkReply* reply = networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this](qint64 bytesReceived, qint64 bytesTotal) {
                if (bytesTotal > 0) {
                    int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
                    emit DownloadProgressed(percentage);
                }
            });

    connect(reply, &QNetworkReply::finished, this, [this, reply, downloadUrl]() {
        if (reply->error() != QNetworkReply::NoError) {
            QString errmsg = ("Network error occurred while trying to access the URL:\n" +
                              downloadUrl + "\n" + reply->errorString());
            QMessageBox::warning(this, "Error", errmsg);
            reply->deleteLater();
            emit UpdateComplete();
            return;
        }

#ifdef Q_OS_WIN
        QString tempDownloadPath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/Temp/temp_download_update";
#else
        QString userPath;
        Common::PathToQString(userPath, Common::GetShadUserDir());
        QString tempDownloadPath = userPath + "/temp_download_update";
#endif

        QDir dir(tempDownloadPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString downloadPath = tempDownloadPath + "/temp_download_update.zip";
        QFile file(downloadPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            InstallUpdate(downloadPath);
            emit UpdateComplete();
        } else {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Failed to save the update file at") +
                                     ":\n" + downloadPath);
            emit DownloadProgressed(0);
            emit UpdateComplete();
        }

        reply->deleteLater();
    });
}

void CheckShadUpdate::InstallUpdate(QString zipPath) {
    QString shadPath;
    Common::PathToQString(shadPath, Common::shadPs4Executable.parent_path());

    std::filesystem::remove(Common::shadPs4Executable);
    QMicroz::extract(zipPath, shadPath);

#ifndef Q_OS_WIN
    QString shadFullPath;
    Common::PathToQString(shadPath, Common::shadPs4Executable);
    mode_t permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    std::string exe = Common::shadPs4Executable.string();

    if (chmod(exe.c_str(), permissions) != 0) {
        QMessageBox::information(this, "Error setting permissions",
                                 "Could not set access permissions for " + shadFullPath +
                                     ". Set permissions manually before launching.");
    }
#endif

    updateChannel == "Release"
        ? Config::SaveBuild(latestVersion.toStdString(), latestVersion.toStdString(),
                            Config::GetLastModifiedString(Common::shadPs4Executable))
        : Config::SaveBuild(latestVersion.right(40).toStdString(),
                            latestVersion.right(40).toStdString(),
                            Config::GetLastModifiedString(Common::shadPs4Executable));

    std::filesystem::remove(Common::PathFromQString(zipPath));
    QMessageBox::information(this, "Update Complete", "Update process completed");
}

CheckShadUpdate::~CheckShadUpdate() {}
