// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "BBFormats.h"

namespace FileHelper {

class Tpf : public BBFormat {
    Q_OBJECT

private:
    int platform = 4; // PS4

public:
    explicit Tpf(std::vector<char> data, ModMerger* parent);
    ~Tpf() override;

    enum class TexType : int { Texture = 0, Cubemap = 1, Volume = 2, TextureArray = 3 };

    enum DxgiFormat : int {
        UNKNOWN,
        R32G32B32A32_TYPELESS,
        R32G32B32A32_FLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32_TYPELESS,
        R32G32B32_FLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R16G16B16A16_TYPELESS,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_UINT,
        R16G16B16A16_SNORM,
        R16G16B16A16_SINT,
        R32G32_TYPELESS,
        R32G32_FLOAT,
        R32G32_UINT,
        R32G32_SINT,
        R32G8X24_TYPELESS,
        D32_FLOAT_S8X24_UINT,
        R32_FLOAT_X8X24_TYPELESS,
        X32_TYPELESS_G8X24_UINT,
        R10G10B10A2_TYPELESS,
        R10G10B10A2_UNORM,
        R10G10B10A2_UINT,
        R11G11B10_FLOAT,
        R8G8B8A8_TYPELESS,
        R8G8B8A8_UNORM,
        R8G8B8A8_UNORM_SRGB,
        R8G8B8A8_UINT,
        R8G8B8A8_SNORM,
        R8G8B8A8_SINT,
        R16G16_TYPELESS,
        R16G16_FLOAT,
        R16G16_UNORM,
        R16G16_UINT,
        R16G16_SNORM,
        R16G16_SINT,
        R32_TYPELESS,
        D32_FLOAT,
        R32_FLOAT,
        R32_UINT,
        R32_SINT,
        R24G8_TYPELESS,
        D24_UNORM_S8_UINT,
        R24_UNORM_X8_TYPELESS,
        X24_TYPELESS_G8_UINT,
        R8G8_TYPELESS,
        R8G8_UNORM,
        R8G8_UINT,
        R8G8_SNORM,
        R8G8_SINT,
        R16_TYPELESS,
        R16_FLOAT,
        D16_UNORM,
        R16_UNORM,
        R16_UINT,
        R16_SNORM,
        R16_SINT,
        R8_TYPELESS,
        R8_UNORM,
        R8_UINT,
        R8_SNORM,
        R8_SINT,
        A8_UNORM,
        R1_UNORM,
        R9G9B9E5_SHAREDEXP,
        R8G8_B8G8_UNORM,
        G8R8_G8B8_UNORM,
        BC1_TYPELESS,
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_TYPELESS,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_TYPELESS,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_TYPELESS,
        BC4_UNORM,
        BC4_SNORM,
        BC5_TYPELESS,
        BC5_UNORM,
        BC5_SNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        B8G8R8A8_UNORM,
        B8G8R8X8_UNORM,
        R10G10B10_XR_BIAS_A2_UNORM,
        B8G8R8A8_TYPELESS,
        B8G8R8A8_UNORM_SRGB,
        B8G8R8X8_TYPELESS,
        B8G8R8X8_UNORM_SRGB,
        BC6H_TYPELESS,
        BC6H_UF16,
        BC6H_SF16,
        BC7_TYPELESS,
        BC7_UNORM,
        BC7_UNORM_SRGB,
        AYUV,
        Y410,
        Y416,
        NV12,
        P010,
        P016,
        OPAQUE_420,
        YUY2,
        Y210,
        Y216,
        NV11,
        AI44,
        IA44,
        P8,
        A8P8,
        B4G4R4A4_UNORM,
        P208,
        V208,
        V408,
        FORCE_UINT
    };

    const std::unordered_map<int, DxgiFormat> formatMap = {
        {0, BC1_UNORM},      {1, BC1_UNORM},           {3, BC2_UNORM},      {5, BC3_UNORM},
        {6, B5G5R5A1_UNORM}, {8, R8G8B8A8_UNORM},      {9, B8G8R8A8_UNORM}, {10, R8G8B8A8_UNORM},
        {16, A8_UNORM},      {22, R16G16B16A16_UNORM}, {23, BC3_UNORM},     {24, BC4_UNORM},
        {25, BC1_UNORM},     {26, A8_UNORM},           {29, BC1_UNORM},     {33, BC3_UNORM},
        {36, BC5_UNORM},     {100, BC6H_UF16},         {102, BC7_UNORM},    {103, BC4_UNORM},
        {104, BC5_UNORM},    {105, R8G8B8A8_UNORM},    {106, BC7_UNORM},    {107, BC7_UNORM},
        {108, BC1_UNORM},    {109, BC1_UNORM},         {110, BC3_UNORM},    {112, BC7_UNORM_SRGB},
        {113, BC6H_UF16},    {115, BC6H_UF16}};

    struct TexHeader {
        int width;
        int height;
        int textureCount;
        // int unk1; PS3 only
        int unk2;
        // int remap; PS3 only
        DxgiFormat dxgiFormat;
    };

    struct FloatStruct {
        int unk00;
        std::vector<float> values;
    };

    struct Texture {
        std::string name = "unnamed";
        int platform = 4; // PS4
        int format;
        TexType type;
        int mipmaps;
        int Flags1;
        TexHeader header;
        FloatStruct fstruct;
        std::vector<char> data = {};
    };

    bool ReadTpf(std::vector<char> data);
    bool RepackTpf(std::vector<char>& data);
    bool HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data);
    std::optional<Tpf::Texture> GetSameTexture(const std::string& name,
                                               const std::vector<Tpf::Texture>);

    std::vector<Texture> textures;
    int flag2;
    int encoding;
};

} // namespace FileHelper
