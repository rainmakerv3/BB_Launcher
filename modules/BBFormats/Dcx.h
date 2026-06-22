// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>

#include "BBFormats.h"

namespace FileHelper {

class Dcx : public BBFormat {
    Q_OBJECT

private:
    struct CompressionInfo {
        int unk04;
        int unk10;
        int unk14;
        int unk30;
        int unk38;
    };

    std::filesystem::path origPath;
    std::filesystem::path extractedPath;

public:
    explicit Dcx(std::filesystem::path file, std::vector<char>& output, ModMerger* parent);
    ~Dcx() override;

    bool UnpackDcx(std::filesystem::path file, std::vector<char>& output);
    bool RepackDcx(std::vector<char> input);

    uint32_t Adler32(const std::vector<char>& data);

    CompressionInfo compInfo;
};

} // namespace FileHelper
