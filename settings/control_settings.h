// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDialog>
#include <QFuture>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

#include "modules/ipc/ipc_client.h"

namespace Ui {
class ControlSettings;
}

class ControlSettings : public QDialog {
    Q_OBJECT
public:
    explicit ControlSettings(std::shared_ptr<IpcClient> ipc_client, QWidget* parent = nullptr);
    ~ControlSettings();

signals:
    void PushGamepadEvent();
    void AxisChanged();

private Q_SLOTS:
    void SaveControllerConfig(bool CloseOnSave);
    void SetDefault();
    void OnProfileChanged();
    void UpdateLightbarColor();
    void CheckMapping(QPushButton*& button);
    void StartTimer(QPushButton*& button, bool isButton);
    void ConnectAxisInputs(QPushButton*& button);
    void ActiveControllerChanged(int value);

private:
    std::unique_ptr<Ui::ControlSettings> ui;
    std::shared_ptr<IpcClient> m_ipc_client;

    bool eventFilter(QObject* obj, QEvent* event) override;
    void AddBoxItems();
    void SetUIValuestoMappings();
    void GetGameTitle();

    void CheckGamePad();
    void processSDLEvents(int Type, int Input, int Value);
    void pollSDLEvents();
    void SetMapping(QString input);
    void DisableMappingButtons();
    void EnableMappingButtons();
    void Cleanup();

    QList<QPushButton*> ButtonsList;
    QList<QPushButton*> AxisList;
    QMap<int, QString> pressedButtons;

    bool L2Pressed = false;
    bool R2Pressed = false;
    bool EnableButtonMapping = false;
    bool EnableAxisMapping = false;
    bool MappingCompleted = false;
    QString mapping;
    int MappingTimer;
    QTimer* timer;
    QPushButton* MappingButton;
    SDL_Gamepad* gamepad = nullptr;
    QFuture<void> Polling;
    SDL_JoystickID* gamepads;

    const std::vector<std::string> ControllerInputs = {
        "cross",        "circle",    "square",      "triangle",    "l1",
        "r1",           "l2",        "r2",          "l3",

        "r3",           "options",   "pad_up",

        "pad_down",

        "pad_left",     "pad_right", "axis_left_x", "axis_left_y", "axis_right_x",
        "axis_right_y", "back"};

protected:
    void closeEvent(QCloseEvent* event) override {
        Cleanup();
    }
};
