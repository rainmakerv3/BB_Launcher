// SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDir>
#include <QMessageBox>
#include <QProcessEnvironment>

#include "ipc_client.h"
#include "settings/config.h"

using u32 = std::uint32_t;
using u64 = std::uint64_t;

IpcClient::IpcClient(QObject* parent) : QObject(parent) {}

void IpcClient::startEmulator(const QFileInfo& exe, const QStringList& args,
                              const QString& workDir) {
    if (process) {
        process->disconnect();
        process->deleteLater();
        process = nullptr;
    }
    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, this, [=, this] { onStdout(); });
    connect(process, &QProcess::readyReadStandardError, this, [this] { onStderr(); });
    connect(process, &QProcess::finished, this, [this] { onProcessClosed(); });

    process->setProcessChannelMode(QProcess::SeparateChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("SHADPS4_ENABLE_IPC", "true");
    process->setProcessEnvironment(env);

    std::filesystem::path userPath = Config::PortableFolderinLauncherFolder
                                         ? Common::GetCurrentPath()
                                         : Common::shadPs4Executable.parent_path();

    QString userDir;
    Common::PathToQString(userDir, userPath);

    process->setWorkingDirectory(userDir);
    process->start(exe.absoluteFilePath(), args, QIODevice::ReadWrite);
}

void IpcClient::startGame() {
    writeLine("START");
}

void IpcClient::pauseGame() {
    writeLine("PAUSE");
}

void IpcClient::resumeGame() {
    writeLine("RESUME");
}

void IpcClient::stopEmulator() {
    if (!Config::GameRunning) {
        QMessageBox::information(nullptr, "BBLauncher", "No runnning game to stop");
        return;
    }

    writeLine("STOP");
}

void IpcClient::restartEmulator() {
    stopEmulator();
    pendingRestart = true;
}

void IpcClient::toggleFullscreen() {
    if (!Config::GameRunning) {
        QMessageBox::information(nullptr, "BBLauncher", "No runnning game to toggle fullscreen");
        return;
    }

    writeLine("TOGGLE_FULLSCREEN");
}

void IpcClient::adjustVol(int volume, bool is_game_specific) {
    writeLine("ADJUST_VOLUME");
    writeLine(QString::number(volume));
    writeLine(is_game_specific ? "1" : "0");
}

void IpcClient::setFsr(bool enable) {
    writeLine("SET_FSR");
    writeLine(enable ? "1" : "0");
}

void IpcClient::setRcas(bool enable) {
    writeLine("SET_RCAS");
    writeLine(enable ? "1" : "0");
}

void IpcClient::setRcasAttenuation(int value) {
    writeLine("SET_RCAS_ATTENUATION");
    writeLine(QString::number(value));
}

void IpcClient::reloadInputs(std::string config) {
    writeLine("RELOAD_INPUTS");
    writeLine(QString::fromStdString(config));
}

void IpcClient::setActiveController(std::string GUID) {
    writeLine("SET_ACTIVE_CONTROLLER");
    writeLine(QString::fromStdString(GUID));
}

void IpcClient::sendMemoryPatches(std::string modNameStr, std::string offsetStr,
                                  std::string valueStr, std::string targetStr, std::string sizeStr,
                                  bool isOffset, bool littleEndian,
                                  MemoryPatcher::PatchMask patchMask, int maskOffset) {
    writeLine("PATCH_MEMORY");
    writeLine(QString::fromStdString(modNameStr));
    writeLine(QString::fromStdString(offsetStr));
    writeLine(QString::fromStdString(valueStr));
    writeLine(QString::fromStdString(targetStr));
    writeLine(QString::fromStdString(sizeStr));
    writeLine(isOffset ? "1" : "0");
    writeLine(littleEndian ? "1" : "0");
    writeLine(QString::number(static_cast<int>(patchMask)));
    writeLine(QString::number(maskOffset));
}

void IpcClient::onStderr() {
    buffer.append(process->readAllStandardError());
    int idx;
    while ((idx = buffer.indexOf('\n')) != -1) {
        QByteArray line = buffer.left(idx);
        buffer.remove(0, idx + 1);

        if (!line.isEmpty() && line.back() == '\r') {
            line.chop(1);
        }

        if (!line.startsWith(";")) {
            // LOG_ERROR(Tty, "{}", line.toStdString());
            continue;
        }

        const QString s = QString::fromUtf8(line.mid(1)).trimmed();
        if (s == "#IPC_ENABLED") {
            // LOG_INFO(IPC, "IPC detected");
        } else if (s == "ENABLE_MEMORY_PATCH") {
            supportedCapabilities["memory_patch"] = true;
            // LOG_INFO(IPC, "Feature detected: 'memory_patch'");
        } else if (s == "ENABLE_EMU_CONTROL") {
            supportedCapabilities["emu_control"] = true;
            // LOG_INFO(IPC, "Feature detected: 'emu_control'");
        } else if (s == "#IPC_END") {
            for (const auto& [capability, supported] : supportedCapabilities) {
                if (not supported) {
                    // LOG_WARNING(IPC,
                    //             "Feature: '{}' is not supported by the choosen emulator version",
                    //             capability);
                }
            }
            // LOG_INFO(IPC, "Start emu");
            writeLine("RUN");
            startGameFunc();
        } else if (s == "RESTART") {
            parsingState = ParsingState::args_counter;
        }

        else if (parsingState == ParsingState::args_counter) {
            argsCounter = s.toInt();
            parsingState = ParsingState::args;
        }

        else if (parsingState == ParsingState::args) {
            parsedArgs.push_back(s.toStdString());
            if (parsedArgs.size() == argsCounter) {
                parsingState = ParsingState::normal;
                pendingRestart = true;
                stopEmulator();
            }
        }
    }
}

void IpcClient::onStdout() {
    QByteArray data = process->readAllStandardOutput();
    QString dataString = QString::fromUtf8(data);
    QStringList lines = dataString.split('\n');
#define ESC "\x1b"
    for (QString& entry : lines) {

#ifdef Q_OS_WIN
        const char* color = "";

        if (entry.contains("<Warning>")) {
            color = ESC "[1;33m";
        } else if (entry.contains("<Critical>")) {
            color = ESC "[1;35m";
        } else if (entry.contains("<Error>")) {
            color = ESC "[1;31m";
        } else {
            color = ESC "[0;37m";
        }

        entry = color + entry;
#endif
        emit LogEntrySent(entry.trimmed());
    }

#undef ESC
}

void IpcClient::onProcessClosed() {
    gameClosedFunc();
    if (process) {
        process->disconnect();
        process->deleteLater();
        process = nullptr;
    }
    if (pendingRestart) {
        pendingRestart = false;
        restartEmulatorFunc();
    }
}

void IpcClient::writeLine(const QString& text) {
    if (process == nullptr) {
        QMessageBox::critical(
            nullptr, tr("ShadPS4"),
            QString(tr("shadPS4 is not found!\nPlease change shadPS4 path in settings.")));
        return;
    }

    QByteArray data = text.toUtf8();
    data.append('\n');
    process->write(data);
    process->waitForBytesWritten(1000);
}

std::string toHex(u64 value, size_t byteSize) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(byteSize * 2) << value;
    return ss.str();
}

std::string MemoryPatcher::convertValueToHex(const std::string type, const std::string valueStr) {
    std::string result;

    if (type == "byte") {
        const u32 value = std::stoul(valueStr, nullptr, 16);
        result = toHex(value, 1);
    } else if (type == "bytes16") {
        const u32 value = std::stoul(valueStr, nullptr, 16);
        result = toHex(value, 2);
    } else if (type == "bytes32") {
        const u32 value = std::stoul(valueStr, nullptr, 16);
        result = toHex(value, 4);
    } else if (type == "bytes64") {
        const u64 value = std::stoull(valueStr, nullptr, 16);
        result = toHex(value, 8);
    } else if (type == "float32") {
        union {
            float f;
            uint32_t i;
        } floatUnion;
        floatUnion.f = std::stof(valueStr);
        result = toHex(floatUnion.i, sizeof(floatUnion.i));
    } else if (type == "float64") {
        union {
            double d;
            uint64_t i;
        } doubleUnion;
        doubleUnion.d = std::stod(valueStr);
        result = toHex(doubleUnion.i, sizeof(doubleUnion.i));
    } else if (type == "utf8") {
        std::vector<unsigned char> byteArray =
            std::vector<unsigned char>(valueStr.begin(), valueStr.end());
        byteArray.push_back('\0');
        std::stringstream ss;
        for (unsigned char c : byteArray) {
            ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c);
        }
        result = ss.str();
    } else if (type == "utf16") {
        std::wstring wide_str(valueStr.size(), L'\0');
        std::mbstowcs(&wide_str[0], valueStr.c_str(), valueStr.size());
        wide_str.resize(std::wcslen(wide_str.c_str()));

        std::u16string valueStringU16;

        for (wchar_t wc : wide_str) {
            if (wc <= 0xFFFF) {
                valueStringU16.push_back(static_cast<char16_t>(wc));
            } else {
                wc -= 0x10000;
                valueStringU16.push_back(static_cast<char16_t>(0xD800 | (wc >> 10)));
                valueStringU16.push_back(static_cast<char16_t>(0xDC00 | (wc & 0x3FF)));
            }
        }

        std::vector<unsigned char> byteArray;
        // convert to little endian
        for (char16_t ch : valueStringU16) {
            unsigned char low_byte = static_cast<unsigned char>(ch & 0x00FF);
            unsigned char high_byte = static_cast<unsigned char>((ch >> 8) & 0x00FF);

            byteArray.push_back(low_byte);
            byteArray.push_back(high_byte);
        }
        byteArray.push_back('\0');
        byteArray.push_back('\0');
        std::stringstream ss;

        for (unsigned char ch : byteArray) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
        }
        result = ss.str();
    } else if (type == "bytes") {
        result = valueStr;
    } else if (type == "mask" || type == "mask_jump32") {
        result = valueStr;
    } else {
        // LOG_INFO(Loader, "Error applying Patch, unknown type: {}", type);
    }
    return result;
}
