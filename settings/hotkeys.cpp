// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <fstream>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>

#include "LauncherSettings.h"
#include "hotkeys.h"
#include "modules/Common.h"
#include "ui_hotkeys.h"

hotkeys::hotkeys(QWidget* parent) : QDialog(parent), ui(new Ui::hotkeys) {

    ui->setupUi(this);

    SDL_InitSubSystem(SDL_INIT_GAMEPAD);
    SDL_InitSubSystem(SDL_INIT_EVENTS);

    LoadHotkeys();
    CheckGamePad();
    installEventFilter(this);

    ButtonsList = {
        ui->fpsButtonPad,
        ui->quitButtonPad,
        ui->fullscreenButtonPad,
        ui->pauseButtonPad,
    };

    ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button) {
        if (button == ui->buttonBox->button(QDialogButtonBox::Save)) {
            SaveHotkeys(true);
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
            SaveHotkeys(false);
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
            QWidget::close();
        }
    });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);

    for (auto& button : ButtonsList) {
        connect(button, &QPushButton::clicked, this,
                [this, &button]() { StartTimer(button, true); });
    }

    connect(this, &hotkeys::PushGamepadEvent, this, [this]() { CheckMapping(MappingButton); });

    Polling = QtConcurrent::run(&hotkeys::pollSDLEvents, this);
}

void hotkeys::DisableMappingButtons() {
    for (const auto& i : ButtonsList) {
        i->setEnabled(false);
    }

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void hotkeys::EnableMappingButtons() {
    for (const auto& i : ButtonsList) {
        i->setEnabled(true);
    }

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void hotkeys::SaveHotkeys(bool CloseOnSave) {
    const auto hotkey_file = Common::GetShadUserDir() / "hotkeys.ini";
    if (!std::filesystem::exists(hotkey_file)) {
        createHotkeyFile(hotkey_file);
    }

    QString controllerFullscreenString, controllerPauseString, controllerFpsString,
        controllerQuitString = "";
    std::ifstream file(hotkey_file);
    int lineCount = 0;
    std::string line = "";
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lineCount++;

        std::size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            lines.push_back(line);
            continue;
        }

        if (line.contains("controllerFullscreen")) {
            line = "controllerFullscreen = " + ui->fullscreenButtonPad->text().toStdString();
        } else if (line.contains("controllerStop")) {
            line = "controllerStop = " + ui->quitButtonPad->text().toStdString();
        } else if (line.contains("controllerFps")) {
            line = "controllerFps = " + ui->fpsButtonPad->text().toStdString();
        } else if (line.contains("controllerPause")) {
            line = "controllerPause = " + ui->pauseButtonPad->text().toStdString();
        }

        lines.push_back(line);
    }

    file.close();

    std::ofstream output_file(hotkey_file);
    for (auto const& line : lines) {
        output_file << line << '\n';
    }
    output_file.close();

    if (CloseOnSave)
        QWidget::close();
}

void hotkeys::LoadHotkeys() {
    const auto hotkey_file = Common::GetShadUserDir() / "hotkeys.ini";
    if (!std::filesystem::exists(hotkey_file)) {
        createHotkeyFile(hotkey_file);
    }

    QString controllerFullscreenString, controllerPauseString, controllerFpsString,
        controllerQuitString = "";
    std::ifstream file(hotkey_file);
    int lineCount = 0;
    std::string line = "";

    while (std::getline(file, line)) {
        lineCount++;

        std::size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos)
            continue;

        if (line.contains("controllerFullscreen")) {
            controllerFullscreenString = QString::fromStdString(line.substr(equal_pos + 2));
        } else if (line.contains("controllerStop")) {
            controllerQuitString = QString::fromStdString(line.substr(equal_pos + 2));
        } else if (line.contains("controllerFps")) {
            controllerFpsString = QString::fromStdString(line.substr(equal_pos + 2));
        } else if (line.contains("controllerPause")) {
            controllerPauseString = QString::fromStdString(line.substr(equal_pos + 2));
        }
    }

    file.close();

    ui->fpsButtonPad->setText(controllerFpsString);
    ui->quitButtonPad->setText(controllerQuitString);
    ui->fullscreenButtonPad->setText(controllerFullscreenString);
    ui->pauseButtonPad->setText(controllerPauseString);
}

void hotkeys::CheckGamePad() {
    if (h_gamepad) {
        SDL_CloseGamepad(h_gamepad);
        h_gamepad = nullptr;
    }

    int gamepad_count;
    h_gamepads = SDL_GetGamepads(&gamepad_count);

    if (!h_gamepads) {
        // handle
        return;
    }

    if (gamepad_count == 0) {
        // LOG_INFO(Input, "No gamepad found!");
        SDL_free(h_gamepads);
        return;
    }

    int defaultIndex =
        GamepadSelect::GetIndexfromGUID(h_gamepads, gamepad_count, Config::DefaultControllerID);
    int activeIndex = GamepadSelect::GetIndexfromGUID(h_gamepads, gamepad_count,
                                                      GamepadSelect::GetSelectedGamepad());

    if (activeIndex != -1) {
        h_gamepad = SDL_OpenGamepad(h_gamepads[activeIndex]);
    } else if (defaultIndex != -1) {
        h_gamepad = SDL_OpenGamepad(h_gamepads[defaultIndex]);
    } else {
        // LOG_INFO(Input, "Got {} gamepads. Opening the first one.", gamepad_count);
        h_gamepad = SDL_OpenGamepad(h_gamepads[0]);
    };

    if (!h_gamepad) {
        // LOG_ERROR(Input, "Failed to open gamepad: {}", SDL_GetError());
    }
}

void hotkeys::StartTimer(QPushButton*& button, bool isButton) {
    MappingTimer = 3;
    EnableButtonMapping = true;
    MappingCompleted = false;
    L2Pressed = false;
    R2Pressed = false;
    mapping = button->text();
    DisableMappingButtons();

    button->setText(tr("Press a button") + " [" + QString::number(MappingTimer) + "]");

    timer = new QTimer(this);
    MappingButton = button;
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, [this]() { CheckMapping(MappingButton); });
}

void hotkeys::CheckMapping(QPushButton*& button) {
    MappingTimer -= 1;
    button->setText(tr("Press a button") + " [" + QString::number(MappingTimer) + "]");

    if (pressedInputs.size() > 0) {
        QStringList keyStrings;

        for (const QString& buttonAction : pressedInputs) {
            keyStrings << buttonAction;
        }

        QString combo = keyStrings.join(",");
        SetMapping(combo);
        MappingButton->setText(combo);
        pressedInputs.clear();
    }

    if (MappingCompleted || MappingTimer <= 0) {
        button->setText(mapping);
        EnableButtonMapping = false;
        EnableMappingButtons();
        timer->stop();
    }
}

void hotkeys::SetMapping(QString input) {
    mapping = input;
    MappingCompleted = true;
}

// use QT events instead of SDL to override default event closing the window with escape
bool hotkeys::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress && EnableButtonMapping) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            SetMapping("unmapped");
            emit PushGamepadEvent();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void hotkeys::pollSDLEvents() {
    SDL_Event event;
    while (true) {

        if (!SDL_WaitEvent(&event)) {
            return;
        }

        if (event.type == SDL_EVENT_QUIT) {
            return;
        }

        if (event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            CheckGamePad();
        }

        if (EnableButtonMapping) {

            if (pressedInputs.size() >= 3) {
                continue;
            }

            if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
                switch (event.gbutton.button) {
                case SDL_GAMEPAD_BUTTON_SOUTH:
                    pressedInputs.insert(5, "cross");
                    break;
                case SDL_GAMEPAD_BUTTON_EAST:
                    pressedInputs.insert(6, "circle");
                    break;
                case SDL_GAMEPAD_BUTTON_NORTH:
                    pressedInputs.insert(7, "triangle");
                    break;
                case SDL_GAMEPAD_BUTTON_WEST:
                    pressedInputs.insert(8, "square");
                    break;
                case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                    pressedInputs.insert(3, "l1");
                    break;
                case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                    pressedInputs.insert(4, "r1");
                    break;
                case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                    pressedInputs.insert(9, "l3");
                    break;
                case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                    pressedInputs.insert(10, "r3");
                    break;
                case SDL_GAMEPAD_BUTTON_DPAD_UP:
                    pressedInputs.insert(13, "pad_up");
                    break;
                case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                    pressedInputs.insert(14, "pad_down");
                    break;
                case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                    pressedInputs.insert(15, "pad_left");
                    break;
                case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                    pressedInputs.insert(16, "pad_right");
                    break;
                case SDL_GAMEPAD_BUTTON_BACK:
                    pressedInputs.insert(11, "back");
                    break;
                case SDL_GAMEPAD_BUTTON_START:
                    pressedInputs.insert(12, "options");
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
                // SDL trigger axis values range from 0 to 32000, set mapping on half movement
                // Set zone for trigger release signal arbitrarily at 5000
                switch (event.gaxis.axis) {
                case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                    if (event.gaxis.value > 16000) {
                        pressedInputs.insert(1, "l2");
                        L2Pressed = true;
                    } else if (event.gaxis.value < 5000) {
                        if (L2Pressed && !R2Pressed)
                            emit PushGamepadEvent();
                    }
                    break;
                case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                    if (event.gaxis.value > 16000) {
                        pressedInputs.insert(2, "r2");
                        R2Pressed = true;
                    } else if (event.gaxis.value < 5000) {
                        if (R2Pressed && !L2Pressed)
                            emit PushGamepadEvent();
                    }
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
                emit PushGamepadEvent();
        }
    }
}

void hotkeys::createHotkeyFile(std::filesystem::path hotkey_file) {
    std::string_view default_hotkeys = R"(controllerStop = l2,r2,back
controllerFps = l2,r2,r3
controllerPause = l2,r2,options
controllerFullscreen = l2,r2,l3

keyboardStop = placeholder
keyboardFps = placeholder
keyboardPause = placeholder
keyboardFullscreen = placeholder
)";

    std::ofstream default_hotkeys_stream(hotkey_file);
    if (default_hotkeys_stream) {
        default_hotkeys_stream << default_hotkeys;
    }
}

void hotkeys::Cleanup() {
    if (h_gamepad) {
        SDL_CloseGamepad(h_gamepad);
        h_gamepad = nullptr;
    }

    SDL_free(h_gamepads);

    SDL_Event quitLoop{};
    quitLoop.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&quitLoop);
    Polling.waitForFinished();

    SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();
}

hotkeys::~hotkeys() {}
