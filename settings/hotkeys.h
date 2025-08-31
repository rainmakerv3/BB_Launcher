// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <filesystem>
#include <QDialog>
#include <QFuture>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>

namespace Ui {
class hotkeys;
}

class hotkeys : public QDialog {
    Q_OBJECT

public:
    explicit hotkeys(QWidget* parent = nullptr);
    ~hotkeys();

signals:
    void PushGamepadEvent();

private Q_SLOTS:
    void SaveHotkeys(bool CloseOnSave);
    void pollSDLEvents();
    void CheckMapping(QPushButton*& button);
    void StartTimer(QPushButton*& button, bool isButton);

private:
    std::unique_ptr<Ui::hotkeys> ui;

    bool eventFilter(QObject* obj, QEvent* event) override;
    void DisableMappingButtons();
    void EnableMappingButtons();
    void LoadHotkeys();
    void CheckGamePad();
    void SetMapping(QString input);
    void Cleanup();
    void createHotkeyFile(std::filesystem::path hotkey_file);

    bool EnableButtonMapping = false;
    bool MappingCompleted = false;
    bool L2Pressed = false;
    bool R2Pressed = false;
    int MappingTimer;
    QString mapping;
    QTimer* timer;
    QPushButton* MappingButton;
    SDL_Gamepad* h_gamepad = nullptr;

    // use QMap instead of QSet to maintain order of inserted strings
    QMap<int, QString> pressedInputs;
    QList<QPushButton*> ButtonsList;
    QFuture<void> Polling;

protected:
    void closeEvent(QCloseEvent* event) override {
        Cleanup();
    }
};
