// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

#include "LauncherSettings.h"
#include "config.h"
#include "formatting.h"
#include "modules/Common.h"
#include "settings/ui_LauncherSettings.h"
#include "settings/updater/CheckUpdate.h"

#ifdef Q_OS_WIN
#include <ShlObj.h>
#include <Windows.h>
#include <objbase.h>
#include <shlguid.h>
#include <shobjidl.h>
#include <wrl/client.h>
#endif

LauncherSettings::LauncherSettings(QWidget* parent)
    : QDialog(parent), ui(new Ui::LauncherSettings) {
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    using namespace Config;

    ui->BackupIntervalComboBox->addItems(BackupFreqList);
    ui->BackupNumberComboBox->addItems(BackupNumList);

    std::string fallbackPath;
#ifdef __APPLE__
    fallbackPath = "~/Library/Application Support/shadPS4";
#elif defined(__linux__)
    const char* xdg = getenv("XDG_DATA_HOME");
    fallbackPath = (xdg && strlen(xdg) > 0)
                       ? std::string(xdg) + "/shadPS4"
                       : "~/.local/share/shadPS4";
#elif _WIN32
    TCHAR appdata[MAX_PATH] = {0};
    SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdata);
    QString fallbackQStr;
    Common::PathToQString(fallbackQStr, std::filesystem::path(appdata) / "shadPS4");
    fallbackPath = fallbackQStr.toStdString();
#endif
    QString desc = ui->LauncherSetDescText->toHtml();
    desc.replace("%FALLBACK%", QString::fromStdString(fallbackPath));
    ui->LauncherSetDescText->setHtml(desc);

    if (theme == "Dark") {
        ui->DarkThemeRadioButton->setChecked(true);
    } else {
        ui->LightThemeRadioButton->setChecked(true);
    }

    QString customPath;
    Common::PathToQString(customPath, CustomUserFolder);
    ui->CustomFolderLineEdit->setText(customPath);

    if (UseCustomUserFolder) {
        ui->CustomFolderRadioButton->setChecked(true);
        ui->CustomFolderLineEdit->setEnabled(true);
        ui->BrowseCustomFolderButton->setEnabled(true);
    } else if (PortableFolderinLauncherFolder) {
        ui->PortableLauncherRadioButton->setChecked(true);
    } else {
        ui->PortableBuildRadioButton->setChecked(true);
    }

    ui->UpdateCheckBox->setChecked(AutoUpdateEnabled);
    ui->SoundFixCheckBox->setChecked(SoundFixEnabled);
    ui->BackupSaveCheckBox->setChecked(BackupSaveEnabled);
    ui->BackupIntervalComboBox->setCurrentText(QString::number(BackupInterval));
    ui->BackupNumberComboBox->setCurrentText(QString::number(BackupNumber));
    OnBackupStateChanged();

    connect(ui->UpdateButton, &QPushButton::clicked, this, []() {
        auto checkUpdate = new CheckUpdate(true);
        checkUpdate->exec();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::pressed, this, [this]() {
        SaveSettings();
        QWidget::close();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this,
            &LauncherSettings::SaveSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed, this,
            &LauncherSettings::SetLauncherDefaults);

    connect(ui->BackupSaveCheckBox, &QCheckBox::checkStateChanged, this,
            &LauncherSettings::OnBackupStateChanged);

    connect(ui->shortcutButton, &QPushButton::clicked, this, &LauncherSettings::CreateShortcut);

    connect(ui->BrowseCustomFolderButton, &QPushButton::clicked, this,
            &LauncherSettings::BrowseCustomFolder);

    connect(ui->CustomFolderRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        ui->CustomFolderLineEdit->setEnabled(checked);
        ui->BrowseCustomFolderButton->setEnabled(checked);
    });

    connect(ui->PortableBuildRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->CustomFolderLineEdit->setEnabled(false);
            ui->BrowseCustomFolderButton->setEnabled(false);
        }
    });

    connect(ui->PortableLauncherRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->CustomFolderLineEdit->setEnabled(false);
            ui->BrowseCustomFolderButton->setEnabled(false);
        }
    });
}

void LauncherSettings::SetLauncherDefaults() {
    ui->UpdateCheckBox->setChecked(false);
    ui->DarkThemeRadioButton->setChecked(true);
    ui->SoundFixCheckBox->setChecked(true);
    ui->BackupIntervalComboBox->setCurrentText("10");
    ui->BackupNumberComboBox->setCurrentText("2");
    ui->PortableBuildRadioButton->setChecked(true);
    ui->CustomFolderLineEdit->clear();
    ui->CustomFolderLineEdit->setEnabled(false);
    ui->BrowseCustomFolderButton->setEnabled(false);
}

void LauncherSettings::SaveSettings() {
    using namespace Config;
    toml::value data;
    std::error_code error;

    if (std::filesystem::exists(SettingsFile, error)) {
        try {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(SettingsFile, std::ios_base::binary);
            data = toml::parse(ifs, std::string{fmt::UTF(SettingsFile.filename().u8string()).data});
        } catch (const std::exception& ex) {
            QMessageBox::critical(this, "Filesystem error", ex.what());
            return;
        }
    } else {
        if (error) {
            QMessageBox::critical(this, "Filesystem error",
                                  QString::fromStdString(error.message()));
        }
    }

    if (ui->DarkThemeRadioButton->isChecked()) {
        theme = "Dark";
    } else {
        theme = "Light";
    }

    PortableFolderinLauncherFolder = ui->PortableLauncherRadioButton->isChecked();
    UseCustomUserFolder = ui->CustomFolderRadioButton->isChecked();
    CustomUserFolder =
        Common::PathFromQString(ui->CustomFolderLineEdit->text());
    SoundFixEnabled = ui->SoundFixCheckBox->isChecked();
    AutoUpdateEnabled = ui->UpdateCheckBox->isChecked();

    BackupSaveEnabled = ui->BackupSaveCheckBox->isChecked();
    BackupInterval = ui->BackupIntervalComboBox->currentText().toInt();
    BackupNumber = ui->BackupNumberComboBox->currentText().toInt();

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();

    Config::SetTheme(theme);
    Config::SaveLauncherSettings();
}

void LauncherSettings::OnBackupStateChanged() {
    if (ui->BackupSaveCheckBox->isChecked()) {
        ui->BackupIntervalLabel->show();
        ui->BackupIntervalComboBox->show();
        ui->BackupNumberLabel->show();
        ui->BackupNumberComboBox->show();
    } else {
        ui->BackupIntervalLabel->hide();
        ui->BackupIntervalComboBox->hide();
        ui->BackupNumberLabel->hide();
        ui->BackupNumberComboBox->hide();
    }
}

void LauncherSettings::CreateShortcut() {

#ifdef Q_OS_APPLE
    QMessageBox::critical(this, "Not currently supported",
                          "Shortcut creation not currently enabled on MacOS");
    return;
#endif

    if (!std::filesystem::exists(Common::shadPs4Executable) || Common::shadPs4Executable.empty()) {
        QMessageBox::critical(this, "Error", "Set valid shadPS4 path first.");
        return;
    }

    // Path to shortcut/link
    QString linkPath;

    // Eboot path
    QString targetPath;
    Common::PathToQString(targetPath, Common::installPath);
    QString ebootPath = targetPath + "/eboot.bin";

    // Get the full path to the icon
    QString iconPath;
    Common::PathToQString(iconPath, Common::installPath / "sce_sys" / "icon0.png");

    QFileInfo iconFileInfo(iconPath);
    QString icoPath = iconFileInfo.absolutePath() + "/" + iconFileInfo.baseName() + ".ico";

    const QString name = "Bloodborne (skip GUI)";

    QString exePath;
#ifdef Q_OS_WIN
    linkPath =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + name + ".lnk";

    exePath = QCoreApplication::applicationFilePath().replace("\\", "/");
#else
    linkPath =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + name + ".desktop";
#endif

    // Convert the icon to .ico if necessary
    if (iconFileInfo.suffix().toLower() == "png") {
        // Convert icon from PNG to ICO
        if (convertPngToIco(iconPath, icoPath)) {

#ifdef Q_OS_WIN
            if (createShortcutWin(linkPath, icoPath, exePath)) {
#else
            if (createShortcutLinux(linkPath, name.toStdString(), iconPath)) {
#endif
                QMessageBox::information(
                    nullptr, tr("Shortcut creation"),
                    QString(tr("Shortcut created successfully!") + "\n%1").arg(linkPath));
            } else {
                QMessageBox::critical(
                    nullptr, tr("Error"),
                    QString(tr("Error creating shortcut!") + "\n%1").arg(linkPath));
            }
        } else {
            QMessageBox::critical(nullptr, tr("Error"), tr("Failed to convert icon."));
        }

        // If the icon is already in ICO format, we just create the shortcut
    } else {
#ifdef Q_OS_WIN
        if (createShortcutWin(linkPath, iconPath, exePath)) {
#else
        if (createShortcutLinux(linkPath, name.toStdString(), iconPath)) {
#endif
            QMessageBox::information(
                nullptr, tr("Shortcut creation"),
                QString(tr("Shortcut created successfully!") + "\n%1").arg(linkPath));
        } else {
            QMessageBox::critical(nullptr, tr("Error"),
                                  QString(tr("Error creating shortcut!") + "\n%1").arg(linkPath));
        }
    }
}

bool LauncherSettings::convertPngToIco(const QString& pngFilePath, const QString& icoFilePath) {
    // Load the PNG image
    QImage image(pngFilePath);
    if (image.isNull()) {
        return false;
    }

    // Scale the image to the default icon size (256x256 pixels)
    QImage scaledImage =
        image.scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Convert the image to QPixmap
    QPixmap pixmap = QPixmap::fromImage(scaledImage);

    // Save the pixmap as an ICO file
    if (pixmap.save(icoFilePath, "ICO")) {
        return true;
    } else {
        return false;
    }
}

#ifdef Q_OS_WIN
bool LauncherSettings::createShortcutWin(const QString& linkPath, const QString& iconPath,
                                         const QString& exePath) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Create the ShellLink object
    Microsoft::WRL::ComPtr<IShellLink> pShellLink;
    HRESULT hres =
        CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pShellLink));
    if (SUCCEEDED(hres)) {
        // Defines the path to the program executable
        pShellLink->SetPath((LPCWSTR)exePath.utf16());

        // Sets the home directory ("Start in")
        pShellLink->SetWorkingDirectory((LPCWSTR)QFileInfo(exePath).absolutePath().utf16());

        // Set arguments, eboot.bin file location
        QString arguments;
        arguments = QString("-n");
        pShellLink->SetArguments((LPCWSTR)arguments.utf16());

        // Set the icon for the shortcut
        pShellLink->SetIconLocation((LPCWSTR)iconPath.utf16(), 0);

        // Save the shortcut
        Microsoft::WRL::ComPtr<IPersistFile> pPersistFile;
        hres = pShellLink.As(&pPersistFile);
        if (SUCCEEDED(hres)) {
            hres = pPersistFile->Save((LPCWSTR)linkPath.utf16(), TRUE);
        }
    }

    CoUninitialize();

    return SUCCEEDED(hres);
}
#else
bool LauncherSettings::createShortcutLinux(const QString& linkPath, const std::string& name,
                                           const QString& iconPath) {
    QFile shortcutFile(linkPath);
    if (!shortcutFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error",
                              QString("Error creating shortcut!\n %1").arg(linkPath));
        return false;
    }

    QString appPath;
    Common::PathToQString(appPath, Common::GetCurrentPath(true));

    QTextStream out(&shortcutFile);
    out << "[Desktop Entry]\n";
    out << "Version=1.0\n";
    out << "Name=" << QString::fromStdString(name) << "\n";
    out << "Exec=" << appPath << " -n\n";
    out << "Icon=" << iconPath << "\n";
    out << "Terminal=false\n";
    out << "Type=Application\n";
    shortcutFile.close();

    return true;
}
#endif

void LauncherSettings::BrowseCustomFolder() {
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select shadPS4 User Folder"),
        ui->CustomFolderLineEdit->text().isEmpty() ? QString()
                                                   : ui->CustomFolderLineEdit->text());
    if (!dir.isEmpty()) {
        std::filesystem::path selectedPath = Common::PathFromQString(dir);
        if (!std::filesystem::exists(selectedPath / "config.json")) {
            QMessageBox::warning(
                this, tr("Invalid User Folder"),
                tr("The selected folder does not contain a config.json file.\n"
                   "Make sure this is the shadPS4 user folder with config.json, "
                   "cache/, patches/, etc."));
        }
        ui->CustomFolderLineEdit->setText(dir);
    }
}

LauncherSettings::~LauncherSettings() {
    delete ui;
}
