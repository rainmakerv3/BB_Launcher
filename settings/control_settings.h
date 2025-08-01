// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDialog>
#include <QFuture>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

namespace Ui {
class ControlSettings;
}

class ControlSettings : public QDialog {
    Q_OBJECT
public:
    explicit ControlSettings(QWidget* parent = nullptr);
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

private:
    std::unique_ptr<Ui::ControlSettings> ui;

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
    QSet<QString> pressedButtons;

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
