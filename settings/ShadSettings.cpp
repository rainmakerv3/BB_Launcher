// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include "user_manager_dialog.h"

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

ShadSettings::ShadSettings(std::shared_ptr<IpcClient> ipc_client, bool game_specific,
                           QWidget* parent)
    : is_game_specific(game_specific), m_ipc_client(ipc_client), QDialog(parent),
      ui(new Ui::ShadSettings) {

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

                EmulatorSettings.Load(Common::game_serial);
                EmulatorSettings.Save(Common::game_serial);
            } else {
                return;
            }
        }
    }

    std::filesystem::path keysJson = Common::GetShadUserDir() / "keys.json";
    Common::PathToQString(keysJsonPath, keysJson);

    if (!std::filesystem::exists(keysJson)) {
        CreateKeysJson();
    }

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

    if (is_game_specific) {
        // We need to load game-specific settings
        EmulatorSettings.Load(Common::game_serial);

        this->setWindowTitle(
            tr("Custom Settings for %1").arg(QString::fromStdString(Common::game_serial)));
        ui->tabWidgetSettings->setTabVisible(2, false);

        MapUIControls();
    } else {
        this->setWindowTitle(tr("Global Settings"));
        ui->tabWidgetSettings->setTabVisible(1, false);
    }

    ui->HomeFolderGroupBox->setVisible(false);
    ui->userGroupBox->setVisible(false);

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
            m_ipc_client->adjustVol(value, is_game_specific);
    });

    connect(ui->HomePathButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Not implemented yet",
                                 "Recent shadPS4 changes require a new implementation for this, "
                                 "disabled in the meantime");
        return;

        /*
        QString initial_path;
        Common::PathToQString(initial_path, Config::externalHomeDir);
        QString home_path_string =
            QFileDialog::getExistingDirectory(this, "Set home folder", initial_path);
        auto file_path = Common::PathFromQString(home_path_string);
        if (!file_path.empty()) {
            Config::externalHomeDir = file_path;
            ui->HomePathLineEdit->setText(home_path_string);
        }
        */
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
            std::filesystem::remove_all(cachePath);
            QMessageBox::information(this, "Removal Completed",
                                     QString("Shader cache for %1 successfully removed.")
                                         .arg(QString::fromStdString(Common::game_serial)));
        }
    });

    connect(ui->usersButton, &QPushButton::clicked, this, [this]() {
        UserManagerDialog* userManager = new UserManagerDialog(this);
        userManager->exec();
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
    if (is_game_specific) {
        if (!EmulatorSettings.Load(Common::game_serial)) {
            QMessageBox::information(this, "Error", "Unable to load settings");
            return;
        }
    } else {
        if (!EmulatorSettings.Load()) {
            QMessageBox::information(this, "Error", "Unable to load settings");
            return;
        }
    }

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

    ui->hideCursorComboBox->setCurrentIndex(EmulatorSettings.GetCursorState());
    OnCursorStateChanged(ui->hideCursorComboBox->currentIndex());
    ui->idleTimeoutSpinBox->setValue(EmulatorSettings.GetCursorHideTimeout());
    ui->widthSpinBox->setValue(EmulatorSettings.GetWindowWidth());
    ui->heightSpinBox->setValue(EmulatorSettings.GetWindowHeight());
    ui->vblankSpinBox->setValue(EmulatorSettings.GetVblankFrequency());
    ui->readbacksModeComboBox->setCurrentIndex(EmulatorSettings.GetReadbacksMode());
    ui->GPUBufferCheckBox->setChecked(EmulatorSettings.IsCopyGpuBuffers());
    ui->disableTrophycheckBox->setChecked(EmulatorSettings.IsTrophyPopupDisabled());
    ui->popUpPosComboBox->setCurrentText(
        QString::fromStdString(EmulatorSettings.GetTrophyNotificationSide()));
    ui->popUpDurationSpinBox->setValue(EmulatorSettings.GetTrophyNotificationDuration());
    ui->psnSignInCheckBox->setChecked(EmulatorSettings.IsPSNSignedIn());
    ui->networkConnectedCheckBox->setChecked(EmulatorSettings.IsConnectedToNetwork());

    ui->showSplashCheckBox->setChecked(EmulatorSettings.IsShowSplash());
    ui->discordRPCCheckbox->setChecked(EmulatorSettings.IsDiscordRPCEnabled());
    ui->dmemSpinBox->setValue(EmulatorSettings.GetExtraDmemInMBytes());
    ui->logTypeComboBox->setCurrentText(QString::fromStdString(EmulatorSettings.GetLogType()));
    ui->logFilterLineEdit->setText(QString::fromStdString(EmulatorSettings.GetLogFilter()));

    ui->FSRCheckBox->setChecked(EmulatorSettings.IsFsrEnabled());
    ui->RCASCheckBox->setChecked(EmulatorSettings.IsRcasEnabled());
    ui->RCASSlider->setValue(EmulatorSettings.GetRcasAttenuation());
    ui->RCASValue->setText(QString::number(ui->RCASSlider->value() / 1000.0, 'f', 3));
    ui->volumeSlider->setValue(EmulatorSettings.GetVolumeSlider());
    ui->volumeValue->setText(QString::number(ui->volumeSlider->value()) + "%");
    ui->graphicsAdapterBox->setCurrentIndex(EmulatorSettings.GetGpuId() + 1);
    ui->pipelineCacheCheckBox->setChecked(EmulatorSettings.IsPipelineCacheEnabled());

    QString translatedText_PresentMode =
        presentModeMap.key(QString::fromStdString(EmulatorSettings.GetPresentMode()));
    ui->presentModeComboBox->setCurrentText(translatedText_PresentMode);

    QString home_path_string;
    Common::PathToQString(home_path_string, EmulatorSettings.GetHomeDir());
    ui->HomePathLineEdit->setText(home_path_string);

    QString dlc_path_string;
    Common::PathToQString(dlc_path_string, EmulatorSettings.GetAddonInstallDir());
    ui->DLCPathLineEdit->setText(dlc_path_string);

    ui->motionControlsCheckBox->setChecked(EmulatorSettings.IsMotionControlsEnabled());
    ui->fullscreenModeComboBox->setCurrentText(
        QString::fromStdString(EmulatorSettings.GetFullScreenMode()));
    ui->backgroundControllerCheckBox->setChecked(EmulatorSettings.IsBackgroundControllerInput());

    QFile file(keysJsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // handle
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        // handle
    }

    QString key;
    QJsonObject obj = doc.object();
    QJsonValue nameValue = obj.value("TrophyKeySet");
    if (nameValue.isObject()) {
        QJsonObject releaseObj = nameValue.toObject();
        key = releaseObj.value("ReleaseTrophyKey").toString();
    }

    ui->trophyKeyLineEdit->setText(key);
    ui->trophyKeyLineEdit->setEchoMode(QLineEdit::Password);
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
    EmulatorSettings.SetShowSplash(ui->showSplashCheckBox->isChecked(), is_game_specific);
    EmulatorSettings.SetVolumeSlider(ui->volumeSlider->value(), is_game_specific);
    EmulatorSettings.SetTrophyPopupDisabled(ui->disableTrophycheckBox->isChecked(),
                                            is_game_specific);
    EmulatorSettings.SetTrophyNotificationDuration(ui->popUpDurationSpinBox->value(),
                                                   is_game_specific);

    std::string trophy_loc = ui->popUpPosComboBox->currentText().toStdString();
    EmulatorSettings.SetTrophyNotificationSide(trophy_loc, is_game_specific);

    // ------------------ Graphics tab --------------------------------------------------------
    bool isFullscreen = ui->fullscreenModeComboBox->currentText() != tr("Windowed");
    EmulatorSettings.SetFullScreen(isFullscreen, is_game_specific);
    EmulatorSettings.SetPresentMode(
        presentModeMap.value(ui->presentModeComboBox->currentText()).toStdString(),
        is_game_specific);
    EmulatorSettings.SetFullScreenMode(ui->fullscreenModeComboBox->currentText().toStdString(),
                                       is_game_specific);

    EmulatorSettings.SetWindowHeight(ui->heightSpinBox->value(), is_game_specific);
    EmulatorSettings.SetWindowWidth(ui->widthSpinBox->value(), is_game_specific);

    EmulatorSettings.SetFsrEnabled(ui->FSRCheckBox->isChecked(), is_game_specific);
    EmulatorSettings.SetRcasEnabled(ui->RCASCheckBox->isChecked(), is_game_specific);
    EmulatorSettings.SetRcasAttenuation(ui->RCASSlider->value(), is_game_specific);

    // First options is auto selection -1, so gpuId on the GUI will always have to subtract 1
    // when setting and add 1 when getting to select the correct gpu in Qt
    EmulatorSettings.SetGpuId(ui->graphicsAdapterBox->currentIndex() - 1, is_game_specific);

    // ------------------ Input tab --------------------------------------------------------
    EmulatorSettings.SetCursorState(cursorStateMap.value(ui->hideCursorComboBox->currentText()),
                                    is_game_specific);
    EmulatorSettings.SetCursorHideTimeout(ui->idleTimeoutSpinBox->value(), is_game_specific);
    EmulatorSettings.SetMotionControlsEnabled(ui->motionControlsCheckBox->isChecked(),
                                              is_game_specific);
    EmulatorSettings.SetBackgroundControllerInput(ui->backgroundControllerCheckBox->isChecked(),
                                                  is_game_specific);

    // ------------------ Log tab --------------------------------------------------------
    EmulatorSettings.SetLogFilter(ui->logFilterLineEdit->text().toStdString(), is_game_specific);
    EmulatorSettings.SetLogType(ui->logTypeComboBox->currentText().toStdString(), is_game_specific);

    // ------------------ Debug tab --------------------------------------------------------
    EmulatorSettings.SetCopyGpuBuffers(ui->GPUBufferCheckBox->isChecked(), is_game_specific);

    if (is_game_specific) {
        EmulatorSettings.SetReadbacksMode(ui->readbacksModeComboBox->currentIndex(),
                                          is_game_specific);
        EmulatorSettings.SetPSNSignedIn(ui->psnSignInCheckBox->isChecked(), is_game_specific);
        EmulatorSettings.SetConnectedToNetwork(ui->networkConnectedCheckBox->isChecked(),
                                               is_game_specific);
        EmulatorSettings.SetPipelineCacheEnabled(ui->pipelineCacheCheckBox->isChecked(),
                                                 is_game_specific);
        EmulatorSettings.SetExtraDmemInMBytes(ui->dmemSpinBox->value(), is_game_specific);
        EmulatorSettings.SetVblankFrequency(ui->vblankSpinBox->value(), is_game_specific);
    } else {
        EmulatorSettings.SetDiscordRPCEnabled(ui->discordRPCCheckbox->isChecked());
        EmulatorSettings.SetHomeDir(ui->HomePathLineEdit->text().toStdString());

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

        QFile file(keysJsonPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // handle
        }
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull()) {
            // handle
        }

        QString key = ui->trophyKeyLineEdit->text();
        Config::TrophyKey = key.toStdString();
        QJsonObject obj = doc.object();
        QJsonValue trophyKeySetObj = obj.value("TrophyKeySet");
        if (trophyKeySetObj.isObject()) {
            QJsonObject releaseObj = trophyKeySetObj.toObject();
            releaseObj["ReleaseTrophyKey"] = key;
            obj["TrophyKeySet"] = releaseObj;
        }

        doc.setObject(obj);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            // handle
        }

        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    if (is_game_specific) {
        if (!EmulatorSettings.Save(Common::game_serial)) {
            QMessageBox::information(this, "Error", "Unable to save settings");
            return;
        }
    } else {
        if (!EmulatorSettings.Save()) {
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
        ui->readbacksModeComboBox->setCurrentIndex(0);
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

ShadSettings::~ShadSettings() {
    // Clean up game-specific settings when dialog closes
    EmulatorSettings.Load();
}

bool ShadSettings::IsSettingOverrideable(const char* setting_key,
                                         const QString& setting_group) const {
    // Check if the setting is in the overrideable list for the given group
    if (setting_group == "General") {
        for (const auto& item : EmulatorSettings.GetGeneralOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    } else if (setting_group == "Debug") {
        for (const auto& item : EmulatorSettings.GetDebugOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    } else if (setting_group == "Input") {
        for (const auto& item : EmulatorSettings.GetInputOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    } else if (setting_group == "Audio") {
        for (const auto& item : EmulatorSettings.GetAudioOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    } else if (setting_group == "GPU") {
        for (const auto& item : EmulatorSettings.GetGPUOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    } else if (setting_group == "Vulkan") {
        for (const auto& item : EmulatorSettings.GetVulkanOverrideableFields()) {
            if (std::string(item.key) == setting_key) {
                return true;
            }
        }
    }

    return false;
}

void ShadSettings::MapUIControls() {
    // General Settings
    m_uiSettingMap[ui->showSplashCheckBox] = {"show_splash", "General"};
    m_uiSettingMap[ui->volumeSlider] = {"volume_slider", "General"};
    m_uiSettingMap[ui->disableTrophycheckBox] = {"trophy_popup_disabled", "General"};
    m_uiSettingMap[ui->popUpDurationSpinBox] = {"trophy_notification_duration", "General"};
    m_uiSettingMap[ui->popUpPosComboBox] = {"trophy_notification_side", "General"};
    m_uiSettingMap[ui->discordRPCCheckbox] = {"discord_rpc_enabled", "General"};

    // GPU Settings
    m_uiSettingMap[ui->graphicsAdapterBox] = {"gpu_id", "Vulkan"}; // Note: This is in Vulkan group
    m_uiSettingMap[ui->heightSpinBox] = {"window_height", "GPU"};
    m_uiSettingMap[ui->widthSpinBox] = {"window_width", "GPU"};
    m_uiSettingMap[ui->FSRCheckBox] = {"fsr_enabled", "GPU"};
    m_uiSettingMap[ui->RCASCheckBox] = {"rcas_enabled", "GPU"};
    m_uiSettingMap[ui->RCASSlider] = {"rcas_attenuation", "GPU"};
    m_uiSettingMap[ui->GPUBufferCheckBox] = {"copy_gpu_buffers", "GPU"};
    m_uiSettingMap[ui->fullscreenModeComboBox] = {"full_screen_mode", "GPU"};
    m_uiSettingMap[ui->presentModeComboBox] = {"present_mode", "GPU"};
    m_uiSettingMap[ui->readbacksModeComboBox] = {"readbacks_mode", "GPU"};

    // Input Settings
    m_uiSettingMap[ui->hideCursorComboBox] = {"cursor_state", "Input"};
    m_uiSettingMap[ui->idleTimeoutSpinBox] = {"cursor_hide_timeout", "Input"};
    m_uiSettingMap[ui->motionControlsCheckBox] = {"motion_controls_enabled", "Input"};
    m_uiSettingMap[ui->backgroundControllerCheckBox] = {"background_controller_input", "Input"};

    // Debug Settings
    m_uiSettingMap[ui->logFilterLineEdit] = {"log_filter", "General"};
    m_uiSettingMap[ui->logTypeComboBox] = {"log_type", "General"};

    // Vulkan Settings
    m_uiSettingMap[ui->pipelineCacheCheckBox] = {"pipeline_cache_enabled", "Vulkan"};

    // Experimental/Other Settings
    m_uiSettingMap[ui->psnSignInCheckBox] = {"psn_signed_in", "General"};
    m_uiSettingMap[ui->networkConnectedCheckBox] = {"connected_to_network", "General"};
    m_uiSettingMap[ui->dmemSpinBox] = {"extra_dmem_in_mbytes", "General"};
    m_uiSettingMap[ui->vblankSpinBox] = {"vblank_frequency", "GPU"};
}

void ShadSettings::CreateKeysJson() {
    QJsonObject releaseObject;
    releaseObject["ReleaseTrophyKey"] = "";

    QJsonObject rootObject;
    rootObject["TrophyKeySet"] = releaseObject;

    QJsonDocument doc(rootObject);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

    QFile file(keysJsonPath);
    if (!file.open(QIODevice::WriteOnly)) {
        // handle
    }

    file.write(jsonData);
    file.close();
}
