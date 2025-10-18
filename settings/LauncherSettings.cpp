// SPDX-FileCopyrightText: Copyright 2024 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFileInfo>
#include <QMessageBox>
#include <QProcessEnvironment>
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
    using namespace Config;

    ui->BackupIntervalComboBox->addItems(BackupFreqList);
    ui->BackupNumberComboBox->addItems(BackupNumList);

    if (theme == "Dark") {
        ui->DarkThemeRadioButton->setChecked(true);
    } else {
        ui->LightThemeRadioButton->setChecked(true);
    }

    ui->UpdateCheckBox->setChecked(AutoUpdateEnabled);
    ui->PortableSettingsCheckBox->setChecked(CheckPortableSettings);
    ui->SoundFixCheckBox->setChecked(SoundFixEnabled);
    ui->BackupSaveCheckBox->setChecked(BackupSaveEnabled);
    ui->BackupIntervalComboBox->setCurrentText(QString::number(BackupInterval));
    ui->BackupNumberComboBox->setCurrentText(QString::number(BackupNumber));
    OnBackupStateChanged();

    connect(ui->UpdateButton, &QPushButton::clicked, this, []() {
        auto checkUpdate = new CheckUpdate(true);
        checkUpdate->exec();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::pressed, this,
            &LauncherSettings::SaveAndCloseLauncherSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::pressed, this,
            &LauncherSettings::SaveLauncherSettings);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed, this,
            &LauncherSettings::SetLauncherDefaults);

#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    connect(ui->BackupSaveCheckBox, &QCheckBox::stateChanged, this,
            &LauncherSettings::OnBackupStateChanged);

#else
    connect(ui->BackupSaveCheckBox, &QCheckBox::checkStateChanged, this,
            &LauncherSettings::OnBackupStateChanged);
#endif

    connect(ui->shortcutButton, &QPushButton::clicked, this, &LauncherSettings::CreateShortcut);
}

void LauncherSettings::SetLauncherDefaults() {
    ui->PortableSettingsCheckBox->setChecked(true);
    ui->UpdateCheckBox->setChecked(false);
    ui->DarkThemeRadioButton->setChecked(true);
    ui->SoundFixCheckBox->setChecked(true);
    ui->BackupIntervalComboBox->setCurrentText("10");
    ui->BackupNumberComboBox->setCurrentText("2");
}

void LauncherSettings::SaveLauncherSettings() {
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

    SoundFixEnabled = ui->SoundFixCheckBox->isChecked();
    BackupSaveEnabled = ui->BackupSaveCheckBox->isChecked();
    BackupInterval = ui->BackupIntervalComboBox->currentText().toInt();
    BackupNumber = ui->BackupNumberComboBox->currentText().toInt();
    AutoUpdateEnabled = ui->UpdateCheckBox->isChecked();
    CheckPortableSettings = ui->PortableSettingsCheckBox->isChecked();

    data["Launcher"]["Theme"] = theme;
    data["Launcher"]["SoundFixEnabled"] = SoundFixEnabled;
    data["Launcher"]["AutoUpdateEnabled"] = AutoUpdateEnabled;
    data["Launcher"]["PortableSettings"] = CheckPortableSettings;

    data["Backups"]["BackupSaveEnabled"] = BackupSaveEnabled;
    data["Backups"]["BackupInterval"] = BackupInterval;
    data["Backups"]["BackupNumber"] = BackupNumber;

    std::ofstream file(SettingsFile, std::ios::binary);
    file << data;
    file.close();

    Config::SetTheme(theme);
}

void LauncherSettings::SaveAndCloseLauncherSettings() {
    SaveLauncherSettings();
    QWidget::close();
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

    char* appdir_env = std::getenv("APPDIR");
    QString appImagePath;

    if (appdir_env != nullptr) {
        appImagePath = QProcessEnvironment::systemEnvironment().value(QStringLiteral("APPIMAGE"));
    } else {
        appImagePath = QCoreApplication::applicationFilePath();
    }

    QTextStream out(&shortcutFile);
    out << "[Desktop Entry]\n";
    out << "Version=1.0\n";
    out << "Name=" << QString::fromStdString(name) << "\n";
    out << "Exec=" << appImagePath << " -n\n";
    out << "Icon=" << iconPath << "\n";
    out << "Terminal=false\n";
    out << "Type=Application\n";
    shortcutFile.close();

    return true;
}
#endif

LauncherSettings::~LauncherSettings() {
    delete ui;
}
