// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentMap>

#include "Common.h"
#include "PkgExtractor.h"
#include "modules/PkgDeps/loader.h"
#include "modules/PkgDeps/pkg.h"
#include "settings/PSF/psf.h"
#include "settings/config.h"
#include "settings/formatting.h"

PkgExtractor::PkgExtractor(QWidget* parent) : QDialog(parent) {
    setupUI();
    resize(600, 60);
    this->setWindowTitle(tr("PKG Extractor"));

    if (!std::filesystem::exists(Common::GetShadUserDir() / "addcont"))
        std::filesystem::create_directories(Common::GetShadUserDir() / "addcont");

    std::filesystem::path dlcPath = Common::GetDlcDir();

    QString qDlcPath;
    Common::PathToQString(qDlcPath, dlcPath);
    dlcLineEdit->setText(qDlcPath);
}

void PkgExtractor::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* selectPkgLabel = new QLabel(tr("<b>%1</b>").arg("Select PKG to extract"));
    QHBoxLayout* selectPkgHLayout = new QHBoxLayout(this);
    pkgLineEdit = new QLineEdit();
    pkgLineEdit->setPlaceholderText(tr("Select PKG File"));
    QPushButton* pkgBrowseButton = new QPushButton(tr("Browse"));

    selectPkgHLayout->addWidget(pkgLineEdit);
    selectPkgHLayout->addWidget(pkgBrowseButton);

    QLabel* selectOutputLabel = new QLabel(tr("<b>%1</b>").arg("Select game output folder"));
    QHBoxLayout* selectOutputHLayout = new QHBoxLayout(this);
    outputLineEdit = new QLineEdit();
    outputLineEdit->setPlaceholderText(tr("Select Game Output Folder"));
    QPushButton* outputBrowseButton = new QPushButton(tr("Browse"));

    selectOutputHLayout->addWidget(outputLineEdit);
    selectOutputHLayout->addWidget(outputBrowseButton);

    QLabel* selectDlcLabel = new QLabel(tr("<b>%1</b>").arg("Select Dlc output folder"));
    QHBoxLayout* selectDlcHLayout = new QHBoxLayout(this);
    dlcLineEdit = new QLineEdit();
    dlcLineEdit->setPlaceholderText(tr("Select DLC Output Folder"));
    QPushButton* dlcBrowseButton = new QPushButton(tr("Browse"));

    selectDlcHLayout->addWidget(dlcLineEdit);
    selectDlcHLayout->addWidget(dlcBrowseButton);

    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Extract"));

    separateUpdateCheckBox = new QCheckBox("Use Separate Update Folder");
    separateUpdateCheckBox->setChecked(true);

    mainLayout->addWidget(selectPkgLabel);
    mainLayout->addLayout(selectPkgHLayout);
    mainLayout->addWidget(selectOutputLabel);
    mainLayout->addLayout(selectOutputHLayout);
    mainLayout->addWidget(selectDlcLabel);
    mainLayout->addLayout(selectDlcHLayout);
    mainLayout->addWidget(separateUpdateCheckBox);
    mainLayout->addWidget(buttonBox);

    connect(pkgBrowseButton, &QPushButton::clicked, this, &PkgExtractor::browsePkg);
    connect(outputBrowseButton, &QPushButton::clicked, this, &PkgExtractor::browseOutput);
    connect(dlcBrowseButton, &QPushButton::clicked, this, &PkgExtractor::browseDlc);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PkgExtractor::ExtractPkg);
}

void PkgExtractor::ExtractPkg() {
    bool useSeparateUpdate = separateUpdateCheckBox->isChecked();
    QString outputFolder = outputLineEdit->text().replace("\\", "/");
    std::filesystem::path game_install_dir = Common::PathFromQString(outputFolder);

    QString pkgFile = pkgLineEdit->text().replace("\\", "/");
    std::filesystem::path file = Common::PathFromQString(pkgFile);

    QString dlcFolder = dlcLineEdit->text().replace("\\", "/");
    std::filesystem::path dlcPath = Common::PathFromQString(dlcFolder);

    if (!std::filesystem::exists(file)) {
        QMessageBox::information(this, "Error", "Selected pkg file does not exist");
        return;
    }

    if (Loader::DetectFileType(file) == Loader::FileTypes::Pkg) {
        std::string failreason;
        PKG pkg = PKG();
        PSF psf;

        if (!pkg.Open(file, failreason)) {
            QMessageBox::critical(nullptr, tr("PKG ERROR"), QString::fromStdString(failreason));
            return;
        }
        if (!psf.Open(pkg.sfo)) {
            QMessageBox::critical(nullptr, tr("PKG ERROR"),
                                  "Could not read SFO. Check log for details");
            return;
        }

        auto category = psf.GetString("CATEGORY");

        if (!std::filesystem::exists(game_install_dir) && category != "ac") {
            QMessageBox::information(this, "Error", "Selected game output folder does not exist");
            return;
        }

        QString pkgType = QString::fromStdString(pkg.GetPkgFlags());
        bool use_game_update = pkgType.contains("PATCH") && useSeparateUpdate;

        // Default paths
        auto game_folder_path = game_install_dir / pkg.GetTitleID();
        auto game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                      (std::string{pkg.GetTitleID()} + "-patch")
                                                : game_folder_path;
        const int max_depth = 5;

        if (pkgType.contains("PATCH")) {
            // For patches, try to find the game recursively
            auto found_game =
                FindGameByID(game_install_dir, std::string{pkg.GetTitleID()}, max_depth);
            if (found_game.has_value()) {
                game_folder_path = found_game.value().parent_path();
                game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                         (std::string{pkg.GetTitleID()} + "-patch")
                                                   : game_folder_path;
            }
        } else {
            // For base games, we check if the game is already installed
            auto found_game =
                FindGameByID(game_install_dir, std::string{pkg.GetTitleID()}, max_depth);
            if (found_game.has_value()) {
                game_folder_path = found_game.value().parent_path();
            }
            // If the game is not found, we install it in the game install directory
            else {
                game_folder_path = game_install_dir / pkg.GetTitleID();
            }
            game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                     (std::string{pkg.GetTitleID()} + "-patch")
                                               : game_folder_path;
        }

        std::string content_id;
        if (auto value = psf.GetString("CONTENT_ID"); value.has_value()) {
            content_id = std::string{*value};
        } else {
            QMessageBox::critical(this, tr("PKG ERROR"), "PSF file there is no CONTENT_ID");
            return;
        }

        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("PKG Installation"));

        QString addonDirPath;
        QString gameDirPath;
        PathToQString(gameDirPath, game_folder_path);
        QDir game_dir(gameDirPath);

        std::string entitlement_label = SplitString(content_id, '-')[2];
        auto addon_extract_path = dlcPath / pkg.GetTitleID() / entitlement_label;
        Common::PathToQString(addonDirPath, addon_extract_path);
        QDir addon_dir(addonDirPath);

        if (category == "ac") {
            if (!std::filesystem::exists(dlcPath)) {
                QMessageBox::information(this, "Error", "Selected dlc folder does not exist");
                return;
            }

            if (!addon_dir.exists()) {
                QMessageBox addonMsgBox(this);
                addonMsgBox.setWindowTitle(tr("DLC Install"));
                addonMsgBox.setText(QString(tr("Would you like to install DLC: %1?"))
                                        .arg(QString::fromStdString(entitlement_label)));

                addonMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                addonMsgBox.setDefaultButton(QMessageBox::No);
                int result = addonMsgBox.exec();
                if (result == QMessageBox::Yes) {
                    game_update_path = addon_extract_path;
                } else {
                    return;
                }
            } else {
                msgBox.setText(QString(tr("DLC already installed:") + "\n" + addonDirPath + "\n\n" +
                                       tr("Would you like to overwrite?")));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int result = msgBox.exec();
                if (result == QMessageBox::Yes) {
                    game_update_path = addon_extract_path;
                } else {
                    return;
                }
            }
        } else {
            if (game_dir.exists()) {
                if (pkgType.contains("PATCH")) {
                    QString pkg_app_version;
                    if (auto app_ver = psf.GetString("APP_VER"); app_ver.has_value()) {
                        pkg_app_version = QString::fromStdString(std::string{*app_ver});
                    } else {
                        QMessageBox::critical(this, tr("PKG ERROR"),
                                              "PSF file there is no APP_VER");
                        return;
                    }
                    std::filesystem::path sce_folder_path =
                        std::filesystem::exists(game_update_path / "sce_sys" / "param.sfo")
                            ? game_update_path / "sce_sys" / "param.sfo"
                            : game_folder_path / "sce_sys" / "param.sfo";
                    psf.Open(sce_folder_path);
                    QString game_app_version;
                    if (auto app_ver = psf.GetString("APP_VER"); app_ver.has_value()) {
                        game_app_version = QString::fromStdString(std::string{*app_ver});
                    } else {
                        QMessageBox::critical(this, tr("PKG ERROR"),
                                              "PSF file there is no APP_VER");
                        return;
                    }
                    double appD = game_app_version.toDouble();
                    double pkgD = pkg_app_version.toDouble();
                    if (pkgD == appD) {
                        msgBox.setText(QString(
                            tr("Patch detected!") + "\n" + tr("PKG and Game versions match: ") +
                            pkg_app_version + "\n" + tr("Would you like to overwrite?")));
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setDefaultButton(QMessageBox::No);
                    } else if (pkgD < appD) {
                        msgBox.setText(QString(tr("Patch detected!") + "\n" +
                                               tr("PKG Version %1 is older than existing version: ")
                                                   .arg(pkg_app_version) +
                                               game_app_version + "\n" +
                                               tr("Would you like to overwrite?")));
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setDefaultButton(QMessageBox::No);
                    } else {
                        msgBox.setText(QString(
                            tr("Patch detected!") + "\n" + tr("Game exists: ") + game_app_version +
                            "\n" + tr("Would you like to apply Patch: ") + pkg_app_version + " ?"));
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setDefaultButton(QMessageBox::No);
                    }
                    int result = msgBox.exec();
                    if (result == QMessageBox::Yes) {
                        // Do nothing.
                    } else {
                        return;
                    }
                } else {
                    msgBox.setText(QString(tr("Game already installed") + "\n" + gameDirPath +
                                           "\n" + tr("Would you like to overwrite?")));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                    int result = msgBox.exec();
                    if (result == QMessageBox::Yes) {
                        // Do nothing.
                    } else {
                        return;
                    }
                }
            } else {
                if (pkgType.contains("PATCH")) {
                    QMessageBox::information(this, tr("PKG Installation"),
                                             tr("PKG is a patch, please install base game first. "
                                                "Make sure you're extracting to the same output "
                                                "folder where the base game was extracted"));
                    return;
                }
            }
        }

        if (!pkg.Extract(file, game_update_path, failreason)) {
            QMessageBox::critical(this, tr("PKG ERROR"), QString::fromStdString(failreason));
        } else {
            int nfiles = pkg.GetNumberOfFiles();

            if (nfiles > 0) {
                QVector<int> indices;
                for (int i = 0; i < nfiles; i++) {
                    indices.append(i);
                }

                QProgressDialog dialog(this);
                dialog.setWindowTitle(tr("PKG Installation"));
                QString extractmsg = QString(tr("Installing PKG"));
                dialog.setLabelText(extractmsg);
                dialog.setAutoClose(true);
                dialog.setRange(0, nfiles);

                bool isSystemDarkMode;
#if defined(__linux__)
                const QPalette defaultPalette;
                const auto text = defaultPalette.color(QPalette::WindowText);
                const auto window = defaultPalette.color(QPalette::Window);
                if (text.lightness() > window.lightness()) {
                    isSystemDarkMode = true;
                } else {
                    isSystemDarkMode = false;
                }
#else
                if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
                    isSystemDarkMode = true;
                } else {
                    isSystemDarkMode = false;
                }
#endif
                if (isSystemDarkMode) {
                    dialog.setStyleSheet(
                        "QProgressBar::chunk { background-color: #0000A3; border-radius: 5px; }"
                        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: "
                        "center; }");
                } else {
                    dialog.setStyleSheet(
                        "QProgressBar::chunk { background-color: #aaaaaa; border-radius: 5px; }"
                        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: "
                        "center; }");
                }

                QFutureWatcher<void> futureWatcher;
                connect(&futureWatcher, &QFutureWatcher<void>::finished, this, [=, this]() {
                    QString path;

                    // We want to show the parent path instead of the full path
                    PathToQString(path, game_folder_path.parent_path());
                    QIcon windowIcon(
                        Common::PathToU8(game_folder_path / "sce_sys/icon0.png").c_str());

                    QMessageBox extractMsgBox(this);
                    extractMsgBox.setWindowTitle(tr("Installation Finished"));
                    if (!windowIcon.isNull()) {
                        extractMsgBox.setWindowIcon(windowIcon);
                    }

                    if (category == "ac") {
                        path = addonDirPath;
                    }

                    extractMsgBox.setText(QString(tr("Successfully installed at %1")).arg(path));
                    extractMsgBox.addButton(QMessageBox::Ok);
                    extractMsgBox.setDefaultButton(QMessageBox::Ok);
                    connect(&extractMsgBox, &QMessageBox::buttonClicked, this,
                            [&](QAbstractButton* button) {
                                if (extractMsgBox.button(QMessageBox::Ok) == button) {
                                    extractMsgBox.close();
                                }
                            });
                    extractMsgBox.exec();
                });

                connect(&dialog, &QProgressDialog::canceled, [&]() { futureWatcher.cancel(); });

                connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged, &dialog,
                        &QProgressDialog::setValue);

                futureWatcher.setFuture(
                    QtConcurrent::map(indices, [&](int index) { pkg.ExtractFiles(index); }));

                dialog.exec();
            }
        }
    } else {
        QMessageBox::critical(this, tr("PKG ERROR"),
                              tr("File doesn't appear to be a valid PKG file"));
    }
}

std::optional<std::filesystem::path> PkgExtractor::FindGameByID(const std::filesystem::path& dir,
                                                                const std::string& game_id,
                                                                int max_depth) {
    if (max_depth < 0) {
        return std::nullopt;
    }

    // Check if this is the game we're looking for
    if (dir.filename() == game_id && std::filesystem::exists(dir / "sce_sys" / "param.sfo")) {
        auto eboot_path = dir / "eboot.bin";
        if (std::filesystem::exists(eboot_path)) {
            return eboot_path;
        }
    }

    // Recursively search subdirectories
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        if (!entry.is_directory()) {
            continue;
        }
        if (auto found = FindGameByID(entry.path(), game_id, max_depth - 1)) {
            return found;
        }
    }

    return std::nullopt;
}

std::vector<std::string> PkgExtractor::SplitString(const std::string& str, char delimiter) {
    std::istringstream iss(str);
    std::vector<std::string> output(1);

    while (std::getline(iss, *output.rbegin(), delimiter)) {
        output.emplace_back();
    }

    output.pop_back();
    return output;
}

void PkgExtractor::browsePkg() {
    QString pkgFile = QFileDialog::getOpenFileName(this, tr("Select Pkg to extract"),
                                                   QDir::homePath(), "Pkg files (*.pkg)");

    if (!pkgFile.isEmpty())
        pkgLineEdit->setText(pkgFile);
}

void PkgExtractor::browseOutput() {
    QString outputFolder =
        QFileDialog::getExistingDirectory(this, tr("Select output folder when extracted"),
                                          QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!outputFolder.isEmpty())
        outputLineEdit->setText(outputFolder);
}

void PkgExtractor::browseDlc() {
    QString dlcFolder =
        QFileDialog::getExistingDirectory(this, tr("Select output folder when extracted"),
                                          QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!dlcFolder.isEmpty()) {
        dlcLineEdit->setText(dlcFolder);
        auto file_path = Common::PathFromQString(dlcFolder);
        Config::dlcDir = file_path;

        Config::ShadSettings settings;
        settings.dlcPath = Common::PathFromQString(dlcFolder);
        Config::SaveShadSettings(settings);
    }
}

PkgExtractor::~PkgExtractor() {}
