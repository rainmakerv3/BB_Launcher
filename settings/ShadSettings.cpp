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
#include <QProcess>
#include <QStandardPaths>

#include "LauncherSettings.h"
#include "ShadSettings.h"
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_ShadSettings.h"

ShadSettings::ShadSettings(QWidget* parent) : QDialog(parent), ui(new Ui::ShadSettings) {
    ui->setupUi(this);
    ui->tabWidgetSettings->setUsesScrollButtons(false);
    initialHeight = this->height();

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Close)->setFocus();

    ui->consoleLanguageComboBox->addItems(languageNames);

    ui->fullscreenModeComboBox->addItem("Borderless");
    ui->fullscreenModeComboBox->addItem("True");

    ui->hideCursorComboBox->addItem("Never");
    ui->hideCursorComboBox->addItem("Idle");
    ui->hideCursorComboBox->addItem("Always");

    LoadValuesFromConfig();

    defaultTextEdit = "Point your mouse at an option to display its description.";
    ui->descriptionText->setText(defaultTextEdit);

    if (!std::filesystem::exists(Common::GetShadUserDir() / "imgui.ini")) {
        ui->FSRGroupBox->setVisible(false);
    } else {
        ui->FSRLabel->setVisible(false);
        DevSettingsFile = Common::GetShadUserDir() / "imgui.ini";
        DevSettingsExists = true;
    }
    LoadValuesFromConfig();

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button) {
        if (button == ui->buttonBox->button(QDialogButtonBox::Save)) {
            if (DevSettingsExists)
                SaveFSRValues();
            SaveSettings();
            QWidget::close();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
            if (DevSettingsExists)
                SaveFSRValues();
            SaveSettings();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
            SetDefaults();
        } 
    });

    connect(ui->tabWidgetSettings, &QTabWidget::currentChanged, this,
            [this]() { ui->buttonBox->button(QDialogButtonBox::Close)->setFocus(); });

    ui->updaterGroupBox->setVisible(true);

    connect(ui->hideCursorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) { OnCursorStateChanged(index); });

    connect(ui->updateComboBox, &QComboBox::currentIndexChanged, this,
            [this]() { Config::UpdateChannel = ui->updateComboBox->currentText().toStdString(); });

    connect(ui->checkUpdateButton, &QPushButton::pressed, this, [this]() {
        ui->checkUpdateButton->setEnabled(false);
        ui->buttonBox->setEnabled(false);

        SaveUpdateSettings();

        CheckShadUpdate* shadUpdateWindow = new CheckShadUpdate(false);
        QObject::connect(shadUpdateWindow, &CheckShadUpdate::DownloadProgressed,
                         ui->DownloadProgressBar, &QProgressBar::setValue);
        shadUpdateWindow->exec();

        ui->checkUpdateButton->setEnabled(true);
        ui->buttonBox->setEnabled(true);
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

    // Descriptions
    {
        ui->consoleLanguageGroupBox->installEventFilter(this);
        ui->fullscreenCheckBox->installEventFilter(this);
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

    std::filesystem::path shadConfigFile = Common::GetShadUserDir() / "config.toml";
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
                                toml::find_or<int>(data, "Settings", "consoleLanguage", 6))) %
        languageIndexes.size());
    ui->hideCursorComboBox->setCurrentIndex(toml::find_or<int>(data, "Input", "cursorState", 1));
    OnCursorStateChanged(toml::find_or<int>(data, "Input", "cursorState", 1));
    ui->idleTimeoutSpinBox->setValue(toml::find_or<int>(data, "Input", "cursorHideTimeout", 5));
    ui->widthSpinBox->setValue(toml::find_or<int>(data, "GPU", "screenWidth", 1280));
    ui->heightSpinBox->setValue(toml::find_or<int>(data, "GPU", "screenHeight", 720));
    ui->vblankSpinBox->setValue(toml::find_or<int>(data, "GPU", "vblankDivider", 1));
    ui->ReadbacksCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "readbacks", false));
    ui->GPUBufferCheckBox->setChecked(toml::find_or<bool>(data, "GPU", "copyGPUBuffers", false));
    ui->disableTrophycheckBox->setChecked(
        toml::find_or<bool>(data, "General", "isTrophyPopupDisabled", false));
    ui->discordRPCCheckbox->setChecked(
        toml::find_or<bool>(data, "General", "enableDiscordRPC", true));
    ui->fullscreenCheckBox->setChecked(toml::find_or<bool>(data, "General", "Fullscreen", false));
    ui->DevkitCheckBox->setChecked(toml::find_or<bool>(data, "General", "isDevKit", false));
    ui->logTypeComboBox->setCurrentText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "logType", "async")));
    ui->logFilterLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "logFilter", "")));
    ui->userNameLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "General", "userName", "shadPS4")));
    ui->trophyKeyLineEdit->setText(
        QString::fromStdString(toml::find_or<std::string>(data, "Keys", "TrophyKey", "")));
    ui->trophyKeyLineEdit->setEchoMode(QLineEdit::Password);

    QString save_data_path_string;
    Common::PathToQString(save_data_path_string, Common::SaveDir);
    ui->SavePathLineEdit->setText(save_data_path_string);

    ui->motionControlsCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "isMotionControlsEnabled", true));
    ui->fullscreenModeComboBox->setCurrentText(QString::fromStdString(
        toml::find_or<std::string>(data, "General", "FullscreenMode", "Borderless")));
    ui->backgroundControllerCheckBox->setChecked(
        toml::find_or<bool>(data, "Input", "backgroundControllerInput", false));

    if (DevSettingsExists)
        LoadFSRValues();

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

ShadSettings::~ShadSettings() {}

void ShadSettings::updateNoteTextEdit(const QString& elementName) {
    QString text; // put texts definition somewhere

    // General
    if (elementName == "consoleLanguageGroupBox") {
        text = consoleLanguageGroupBoxtext;
    } else if (elementName == "fullscreenCheckBox") {
        text = fullscreenCheckBoxtext;
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
        text = heightDividertext;
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
    std::filesystem::path shadConfigFile = Common::GetShadUserDir() / "config.toml";

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

    data["General"]["Fullscreen"] = ui->fullscreenCheckBox->isChecked();
    data["General"]["FullscreenMode"] = ui->fullscreenModeComboBox->currentText().toStdString();
    data["General"]["isTrophyPopupDisabled"] = ui->disableTrophycheckBox->isChecked();
    data["General"]["enableDiscordRPC"] = ui->discordRPCCheckbox->isChecked();
    data["General"]["logFilter"] = ui->logFilterLineEdit->text().toStdString();
    data["General"]["logType"] = ui->logTypeComboBox->currentText().toStdString();
    data["General"]["userName"] = ui->userNameLineEdit->text().toStdString();
    data["General"]["isDevKit"] = ui->DevkitCheckBox->isChecked();
    data["Input"]["cursorState"] = ui->hideCursorComboBox->currentIndex();
    data["Input"]["cursorHideTimeout"] = ui->idleTimeoutSpinBox->value();
    data["Input"]["isMotionControlsEnabled"] = ui->motionControlsCheckBox->isChecked();
    data["Input"]["backgroundControllerInput"] = ui->backgroundControllerCheckBox->isChecked();
    data["GPU"]["screenWidth"] = ui->widthSpinBox->value();
    data["GPU"]["screenHeight"] = ui->heightSpinBox->value();
    data["GPU"]["vblankDivider"] = ui->vblankSpinBox->value();
    data["GPU"]["copyGPUBuffers"] = ui->GPUBufferCheckBox->isChecked();
    data["GPU"]["readbacks"] = ui->ReadbacksCheckBox->isChecked();
    data["Keys"]["TrophyKey"] = ui->trophyKeyLineEdit->text().toStdString();
    data["Settings"]["consoleLanguage"] =
        languageIndexes[ui->consoleLanguageComboBox->currentIndex()];

    Config::TrophyKey = ui->trophyKeyLineEdit->text().toStdString();

    std::ofstream file(Common::GetShadUserDir() / "config.toml", std::ios::binary);
    file << data;
    file.close();

    SaveUpdateSettings();
}

void ShadSettings::SetDefaults() {
    ui->hideCursorComboBox->setCurrentIndex(1);
    OnCursorStateChanged(1);
    ui->idleTimeoutSpinBox->setValue(5);
    ui->widthSpinBox->setValue(1280);
    ui->heightSpinBox->setValue(720);
    ui->vblankSpinBox->setValue(1);
    ui->disableTrophycheckBox->setChecked(false);
    ui->discordRPCCheckbox->setChecked(true);
    ui->fullscreenCheckBox->setChecked(false);
    ui->ReadbacksCheckBox->setChecked(false);
    ui->DevkitCheckBox->setChecked(false);
    ui->GPUBufferCheckBox->setChecked(false);
    ui->logTypeComboBox->setCurrentText("async");
    ui->logFilterLineEdit->setText("");
    ui->userNameLineEdit->setText("shadPS4");
    ui->updateComboBox->setCurrentText("Nightly");
    ui->motionControlsCheckBox->setChecked(true);
    ui->backgroundControllerCheckBox->setChecked(false);
    ui->fullscreenModeComboBox->setCurrentText("Borderless");
    ui->FSRCheckBox->setChecked(true);
    ui->RCASCheckBox->setChecked(true);
    ui->RCASSlider->setValue(250);
}

void ShadSettings::LoadFSRValues() {
    std::fstream file(DevSettingsFile);
    std::string line;
    std::vector<std::string> lines;
    int lineCount = 0;
    int FSRVal;
    int RCASVal;
    float RCASAtenVal;

    while (std::getline(file, line)) {
        lineCount++;

        if (line.contains("fsr_enabled")) {
            std::string FSREnabledString(1, line.back());
            FSRVal = std::stoi(FSREnabledString);
        }

        if (line.contains("fsr_rcas_enabled")) {
            std::string RCASEnabledString(1, line.back());
            RCASVal = std::stoi(RCASEnabledString);
        }

        if (line.contains("fsr_rcas_attenuation")) {
            std::size_t equal_pos = line.find('=');
            std::string RCASAtenString(line.substr(equal_pos + 1));
            RCASAtenVal = std::stof(RCASAtenString);
        }
    }
    file.close();

    ui->FSRCheckBox->setChecked((FSRVal == 1 ? true : false));
    ui->RCASCheckBox->setChecked((RCASVal == 1 ? true : false));
    ui->RCASSlider->setValue(round(RCASAtenVal * 1000));

    QString RCASValue = QString::number(RCASAtenVal, 'f', 3);
    ui->RCASValue->setText(RCASValue);
}

void ShadSettings::SaveFSRValues() {
    std::ifstream file(DevSettingsFile);
    std::string line;
    std::vector<std::string> lines;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;

        if (line.contains("fsr_enabled")) {
            std::string FSRVal = ui->FSRCheckBox->isChecked() ? "1" : "0";
            line = "fsr_enabled=" + FSRVal;
        }

        if (line.contains("fsr_rcas_enabled")) {
            std::string RCASVal = ui->RCASCheckBox->isChecked() ? "1" : "0";
            line = "fsr_rcas_enabled=" + RCASVal;
        }

        if (line.contains("fsr_rcas_attenuation")) {
            float RCASAtenVal = ui->RCASSlider->value() / 1000.0;
            line = "fsr_rcas_attenuation=" + std::to_string(RCASAtenVal);
        }

        lines.push_back(line);
    }
    file.close();

    std::ofstream output_file(DevSettingsFile);
    for (auto const& line : lines) {
        output_file << line << '\n';
    }
    output_file.close();
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

CheckShadUpdate::CheckShadUpdate(const bool isAutoupdate, QWidget* parent) : QDialog(parent) {
    setFixedSize(0, 0);
    CheckShadUpdate::UpdateShad(isAutoupdate);
}

void CheckShadUpdate::UpdateShad(bool isAutoupdate) {
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QString updateChannel;
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

    connect(reply, &QNetworkReply::finished, this, [this, isAutoupdate, reply, updateChannel]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Network error:") + "\n" +
                                     reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(response));

        if (jsonDoc.isNull()) {
            QMessageBox::warning(this, "Error", "Failed to parse update information.");
            reply->deleteLater();
            return;
        }

        QString downloadUrl;
        QString latestVersion;
        QString latestRev;
        QString latestDate;
        QString platformString;

#ifdef Q_OS_WIN
        platformString = "win64-qt";
#elif defined(Q_OS_LINUX)
        platformString = "linux-qt";
#elif defined(Q_OS_MAC)
        platformString = "macos-qt";
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
                return;
            }
        } else {
            jsonObj = jsonDoc.object();
            if (jsonObj.contains("tag_name")) {
                latestVersion = jsonObj["tag_name"].toString();
            } else {
                QMessageBox::warning(this, "Error", "Invalid release data.");
                reply->deleteLater();
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
            return;
        }

        std::filesystem::file_time_type shadModifiedDate = std::chrono::floor<std::chrono::days>(
            std::filesystem::last_write_time(Common::shadPs4Executable));
        std::string shadModifiedDateString = std::format("{:%F}", shadModifiedDate);

        std::string latestVerDateString =
            jsonObj["published_at"].toString().toStdString().substr(0, 10);

        if (shadModifiedDateString == latestVerDateString) {
            if (isAutoupdate) {
                close();
                return;
            } else {
                if (QMessageBox::No ==
                    QMessageBox::question(this, "Update confirmation",
                                          "Latest shadPS4 build has the same date as the currently "
                                          "installed build. Proceed with update?",
                                          QMessageBox::Yes | QMessageBox::No)) {
                    close();
                    return;
                }
            }
        } else {
            if (QMessageBox::No ==
                QMessageBox::question(this, "Update confirmation",
                                      "New shadPS4 build detected. Proceed with Update?",
                                      QMessageBox::Yes | QMessageBox::No)) {
                close();
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
            return;
        }

        QString userPath = QString::fromStdString(Common::GetShadUserDir().string());

#ifdef Q_OS_WIN
        QString tempDownloadPath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/Temp/temp_download_update";
#else
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
            QMessageBox::information(
                this, "Download Complete",
                "The update has been downloaded, press OK to install.\n\nBBLauncher will close "
                "to allow copying of shared QT files. The update is finished when BBLauncher "
                "re-opens.");
            InstallUpdate();
        } else {
            QMessageBox::warning(this, "Error",
                                 QString::fromStdString("Failed to save the update file at") +
                                     ":\n" + downloadPath);
            emit DownloadProgressed(0);
        }

        reply->deleteLater();
    });
}

void CheckShadUpdate::InstallUpdate() {
    QString userPath;
    Common::PathToQString(userPath, Common::GetShadUserDir());

    QString rootPath;
#if defined(__APPLE__)
    Common::PathToQString(rootPath, Common::GetBundleParentDirectory());
#else
    Common::PathToQString(rootPath, std::filesystem::current_path());
#endif

    QString shadPath;
    Common::PathToQString(shadPath, Common::shadPs4Executable.parent_path());

    QString tempDirPath = userPath + "/temp_download_update";
    QString startingUpdate = "Starting Update...";

    QString binaryStartingUpdate;
    for (QChar c : startingUpdate) {
        binaryStartingUpdate.append(QString::number(c.unicode(), 2).rightJustified(16, '0'));
    }

    QString scriptContent;
    QString scriptFileName;
    QStringList arguments;
    QString processCommand;

#ifdef Q_OS_WIN
    tempDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
                  "/Temp/temp_download_update";
    scriptFileName = tempDirPath + "/update.ps1";
    scriptContent = QStringLiteral(
        "Set-ExecutionPolicy Bypass -Scope Process -Force\n"
        "$binaryStartingUpdate = '%1'\n"
        "$chars = @()\n"
        "for ($i = 0; $i -lt $binaryStartingUpdate.Length; $i += 16) {\n"
        "    $chars += [char]([convert]::ToInt32($binaryStartingUpdate.Substring($i, 16), 2))\n"
        "}\n"
        "$startingUpdate = -join $chars\n"
        "Write-Output $startingUpdate\n"
        "Expand-Archive -Path '%2\\temp_download_update.zip' -DestinationPath '%2' -Force\n"
        "Start-Sleep -Seconds 3\n"
        "Copy-Item -Recurse -Force '%2\\*' '%4\\'\n"
        "Start-Sleep -Seconds 2\n"
        "Remove-Item -Force -LiteralPath '%4\\update.ps1'\n"
        "Remove-Item -Force -LiteralPath '%4\\temp_download_update.zip'\n"
        "Remove-Item -Recurse -Force '%2'\n"
        "Start-Process -FilePath '%3\\BB_Launcher.exe' "
        "-WorkingDirectory ([WildcardPattern]::Escape('%3'))\n");
    arguments << "-ExecutionPolicy"
              << "Bypass"
              << "-File" << scriptFileName;
    processCommand = "powershell.exe";

#elif defined(Q_OS_LINUX)
    // Linux Shell Script
    scriptFileName = tempDirPath + "/update.sh";
    scriptContent = QStringLiteral(
        "#!/bin/bash\n"
        "check_unzip() {\n"
        "    if ! command -v unzip &> /dev/null && ! command -v 7z &> /dev/null; then\n"
        "        echo \"Neither 'unzip' nor '7z' is installed.\"\n"
        "        read -p \"Would you like to install 'unzip'? (y/n): \" response\n"
        "        if [[ \"$response\" == \"y\" || \"$response\" == \"Y\" ]]; then\n"
        "            if [[ -f /etc/os-release ]]; then\n"
        "                . /etc/os-release\n"
        "                case \"$ID\" in\n"
        "                    ubuntu|debian)\n"
        "                        sudo apt-get install unzip -y\n"
        "                        ;;\n"
        "                    fedora|redhat)\n"
        "                        sudo dnf install unzip -y\n"
        "                        ;;\n"
        "                    *)\n"
        "                        echo \"Unsupported distribution for automatic installation.\"\n"
        "                        exit 1\n"
        "                        ;;\n"
        "                esac\n"
        "            else\n"
        "                echo \"Could not identify the distribution.\"\n"
        "                exit 1\n"
        "            fi\n"
        "        else\n"
        "            echo \"At least one of 'unzip' or '7z' is required to continue. The process "
        "will be terminated.\"\n"
        "            exit 1\n"
        "        fi\n"
        "    fi\n"
        "}\n"
        "extract_file() {\n"
        "    if command -v unzip &> /dev/null; then\n"
        "        unzip -o \"%2/temp_download_update.zip\" -d \"%2/\"\n"
        "    elif command -v 7z &> /dev/null; then\n"
        "        7z x \"%2/temp_download_update.zip\" -o\"%2/\" -y\n"
        "    else\n"
        "        echo \"No suitable extraction tool found.\"\n"
        "        exit 1\n"
        "    fi\n"
        "}\n"
        "main() {\n"
        "    check_unzip\n"
        "    echo \"%1\"\n"
        "    sleep 2\n"
        "    extract_file\n"
        "    sleep 2\n"
        "    if pgrep -f \"Shadps4-qt.AppImage\" > /dev/null; then\n"
        "        pkill -f \"Shadps4-qt.AppImage\"\n"
        "        sleep 2\n"
        "    fi\n"
        "    cp -r \"%2/\"* \"%4/\"\n"
        "    sleep 2\n"
        "    rm \"%4/update.sh\"\n"
        "    rm \"%4/temp_download_update.zip\"\n"
        "    chmod +x \"%4/Shadps4-qt.AppImage\"\n"
        "    rm -r \"%2\"\n"
        "    cd \"%3\" && ./BB_Launcher-qt.AppImage\n"
        "}\n"
        "main\n");
    arguments << scriptFileName;
    processCommand = "bash";

#elif defined(Q_OS_MAC)
    // macOS Shell Script
    scriptFileName = tempDirPath + "/update.sh";
    scriptContent = QStringLiteral(
        "#!/bin/bash\n"
        "check_tools() {\n"
        "    if ! command -v unzip &> /dev/null && ! command -v tar &> /dev/null; then\n"
        "        echo \"Neither 'unzip' nor 'tar' is installed.\"\n"
        "        read -p \"Would you like to install 'unzip'? (y/n): \" response\n"
        "        if [[ \"$response\" == \"y\" || \"$response\" == \"Y\" ]]; then\n"
        "            echo \"Please install 'unzip' using Homebrew or another package manager.\"\n"
        "            exit 1\n"
        "        else\n"
        "            echo \"At least one of 'unzip' or 'tar' is required to continue. The process "
        "will be terminated.\"\n"
        "            exit 1\n"
        "        fi\n"
        "    fi\n"
        "}\n"
        "check_tools\n"
        "echo \"%1\"\n"
        "sleep 2\n"
        "unzip -o \"%2/temp_download_update.zip\" -d \"%2/\"\n"
        "sleep 2\n"
        "tar -xzf \"%2/shadps4-macos-qt.tar.gz\" -C \"%4\"\n"
        "sleep 2\n"
        "rm \"%4/update.sh\"\n"
        "chmod +x \"%4/shadps4.app/Contents/MacOS/shadps4\"\n"
        "open \"%3/BB_Launcher.app\"\n"
        "rm -r \"%2\"\n");

    arguments << scriptFileName;
    processCommand = "bash";

#else
    QMessageBox::warning(this, "Error", "Unsupported operating system.");
    return;
#endif

    QFile scriptFile(scriptFileName);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        scriptFile.write("\xEF\xBB\xBF");
#ifdef Q_OS_WIN
        out << scriptContent.arg(binaryStartingUpdate).arg(tempDirPath).arg(rootPath).arg(shadPath);
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
        out << scriptContent.arg(startingUpdate).arg(tempDirPath).arg(rootPath).arg(shadPath);
#endif
        scriptFile.close();

// Make the script executable on Unix-like systems
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
        scriptFile.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner |
                                  QFileDevice::WriteOwner);
#endif
        QProcess::startDetached(processCommand, arguments);
        exit(EXIT_SUCCESS);

    } else {
        QMessageBox::warning(this, "Error",
                             QString::fromStdString("Failed to create the update script file") +
                                 ":\n" + scriptFileName);
    }
}

CheckShadUpdate::~CheckShadUpdate() {}
