// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>

#include "Tpf.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

Tpf::Tpf(std::vector<char> data, ModMerger* parent) : BBFormat(parent) {
    bigEndian = false;
    ReadTpf(data);
}

bool Tpf::ReadTpf(std::vector<char> data) {
    if (data.empty()) {
        sendLog("ERROR: empty tpf input data", LogFormat::BoldRed);
        return false;
    }

    istream = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()),
                                                  std::ios_base::in | std::ios_base::binary);

    int intBuffer = 0;
    std::string strBuffer;

    GetStr(strBuffer, 4);
    debugLog("MAGIC: " + strBuffer);

    if (!strBuffer.contains("TPF")) {
        sendLog("ERROR invalid param header: " + strBuffer, LogFormat::BoldRed);
        return false;
    }

    istream->ignore(4); // Data length

    int fileCount = 0;
    GetInt32(fileCount);
    debugLog("fileCount: " + std::to_string(fileCount));

    GetByte(intBuffer);
    if (intBuffer != 4) {
        sendLog("ERROR: tpf data not for PS4 platform", LogFormat::BoldRed);
        return false;
    }

    GetByte(flag2);
    debugLog("flag2: " + std::to_string(flag2)); // assert 0/1/2/3?

    GetByte(encoding);
    debugLog("encoding: " + std::to_string(encoding)); // assert 0/1/2?

    istream->ignore(1); // or assert 0

    for (int i = 0; i < fileCount; i++) {
        Texture tex;
        uint fileOffset;
        GetInt32(fileOffset);
        debugLog("fileOffset: " + std::to_string(fileOffset));

        uint fileSize;
        GetInt32(fileSize);
        debugLog("fileSize: " + std::to_string(fileSize));

        GetByte(tex.format);
        debugLog("tex.format: " + std::to_string(tex.format));

        GetByte(intBuffer);
        debugLog("type: " + std::to_string(intBuffer));
        tex.type = static_cast<TexType>(intBuffer);

        GetByte(tex.mipmaps);
        debugLog("mipmaps: " + std::to_string(tex.mipmaps));

        GetByte(tex.Flags1);
        debugLog("flags1: " + std::to_string(tex.Flags1)); // assert (0/1/2/3/0x80)?

        GetInt16(tex.header.width);
        debugLog("tex.header.width: " + std::to_string(tex.header.width));
        GetInt16(tex.header.height);
        debugLog("tex.header.height: " + std::to_string(tex.header.height));

        // may not be needed?
        try {
            tex.header.dxgiFormat = formatMap.at(tex.format);
        } catch (const std::out_of_range& e) {
            debugLog("Invalid dxgi format type: " + std::to_string(tex.header.dxgiFormat));
            tex.header.dxgiFormat = DxgiFormat::UNKNOWN;
        }

        GetInt32(tex.header.textureCount);
        debugLog("textureCount: " + std::to_string(tex.header.textureCount));
        GetInt32(tex.header.unk2); // assert 0/0x9/0xD?
        debugLog("tex.header.unk2: " + std::to_string(tex.header.unk2));

        uint nameOffset;
        GetInt32(nameOffset);
        debugLog("nameOffset: " + std::to_string(nameOffset));

        int hasFloatStruct;
        GetInt32(hasFloatStruct); // assert 0/1 ==1?
        debugLog("hasFloatStruct: " + std::to_string(hasFloatStruct));

        GetInt32(intBuffer);
        debugLog("dxgiFormat (mapped): " + std::to_string(intBuffer));
        tex.header.dxgiFormat = static_cast<DxgiFormat>(intBuffer);

        if (hasFloatStruct != 0) {
            sendLog("ERROR: unexpected texture fstruct", LogFormat::BoldRed);
            // not sure if needed, but just in case
            GetInt32(tex.fstruct.unk00);
            debugLog("tex.fstruct.unk00: " + std::to_string(tex.fstruct.unk00));

            int length;
            GetInt32(length);
            if (length < 0 || length % 4 != 0) {
                sendLog("ERROR fstruct unexpected length: " + std::to_string(length),
                        LogFormat::BoldRed);
                return false;
            }

            for (int j = 0; j < length / 4; j++) {
                float f;
                GetFloat(f);
                tex.fstruct.values.push_back(f);
            }
        }

        StepIn(fileOffset, *istream);
        GetBytes(tex.data, fileSize);
        StepOut(*istream);

        if (tex.Flags1 == 2 || tex.Flags1 == 3) {
            // decompress, should not be encountered in BB
            sendLog("ERROR unexpected compression flag: " + std::to_string(tex.Flags1),
                    LogFormat::BoldRed);
            return false;
        }

        if (encoding == 1) {
            StepIn(nameOffset, *istream);
            tex.name = ReadUtf16String();
            debugLog("tex.name: " + tex.name);
            StepOut(*istream);
        } else if (encoding == 2 || encoding == 3) {
            // should not encounter this in BB, GetShiftJIS(nameOffset);
            sendLog("ERROR unexpected name encoding: " + std::to_string(encoding),
                    LogFormat::BoldRed);
            return false;
        }

        textures.push_back(tex);
    }

    istream.reset();
    return true;
}

bool Tpf::RepackTpf(std::vector<char>& outputData) {
    ostream = std::make_unique<std::stringstream>(std::ios_base::out | std::ios_base::binary);
    std::string strBuffer;

    strBuffer = "TPF";
    WriteStr(strBuffer, 4);
    ReserveBytes("DataSize", 4);
    WriteInt32(static_cast<int>(textures.size()));
    WriteByte(4);
    WriteByte(flag2);
    WriteByte(encoding);
    WriteByte(0);

    for (int i = 0; i < textures.size(); i++) {
        strBuffer = "FileData" + std::to_string(i);
        ReserveBytes(strBuffer, 4);

        strBuffer = "FileSize" + std::to_string(i);
        ReserveBytes(strBuffer, 4);

        WriteByte(textures[i].format);
        WriteByte(static_cast<int>(textures[i].type));
        WriteByte(textures[i].mipmaps);
        WriteByte(textures[i].Flags1);

        WriteInt16(textures[i].header.width);
        WriteInt16(textures[i].header.height);

        WriteInt32(textures[i].header.textureCount);
        WriteInt32(textures[i].header.unk2);

        strBuffer = "FileName" + std::to_string(i);
        ReserveBytes(strBuffer, 4);

        int fstruct;
        fstruct = textures[i].fstruct.values.size() == 0 ? 0 : 1;
        WriteInt32(fstruct);

        WriteInt32(static_cast<int>(textures[i].header.dxgiFormat));

        if (fstruct == 1) {
            sendLog("ERROR: encountered unexpected texture fstruct", LogFormat::BoldRed);
            // just in case
            WriteInt32(textures[i].fstruct.unk00);
            WriteInt32(static_cast<int>(textures[i].fstruct.values.size()));

            for (int j = 0; j < textures[i].fstruct.values.size(); j++) {
                float f = textures[i].fstruct.values[j];
                ostream->write(reinterpret_cast<const char*>(&f), 4);
            }
        }
    }

    for (int i = 0; i < textures.size(); i++) {
        strBuffer = "FileName" + std::to_string(i);
        FillReservedInt32(strBuffer, static_cast<uint>(ostream->tellp()));

        if (encoding == 1) {
            WriteUtf16String(textures[i].name);
        } else {
            sendLog("ERROR: encountered unexpected nonUTF name encoding", LogFormat::BoldRed);
            return false;
            // if (encoding == 0 || encoding == 2)
            //     bw.WriteShiftJIS(Name, true);
        }
    }

    int texturePaddingSize = 0x4;
    PadStream(0x10);

    std::streamoff dataStart = ostream->tellp();
    uint textureDataSize = 0;

    for (int i = 0; i < textures.size(); i++) {
        if (textures[i].data.size() > 0)
            PadStream(texturePaddingSize);

        strBuffer = "FileData" + std::to_string(i);
        FillReservedInt32(strBuffer, static_cast<uint>(ostream->tellp()));

        if (textures[i].Flags1 == 2 || textures[i].Flags1 == 3) {
            sendLog("ERROR: Encountered unexpected compressed texture format", LogFormat::BoldRed);
            return false;
        }

        strBuffer = "FileSize" + std::to_string(i);
        FillReservedInt32(strBuffer, static_cast<int>(textures[i].data.size()));

        ostream->write(textures[i].data.data(), textures[i].data.size());

        textureDataSize += textures[i].data.size();
    }

    FillReservedInt32("DataSize", static_cast<int>(ostream->tellp() - dataStart));

    auto& stringStream = static_cast<std::stringstream&>(*ostream);
    std::string str = stringStream.str();
    outputData = std::vector<char>(str.begin(), str.end());

    /* tests
    std::ofstream outFile("test.tpf", std::ios::out | std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(data.data(), data.size());
        outFile.close();
    }
    */

    ostream.reset();
    sendLog("TPF Repacking complete\n");
    return true;
}

bool Tpf::HandleConflict(const std::vector<char>& mod1Data, const std::vector<char>& mod2Data) {
    Tpf mod1Tpf = Tpf(mod1Data, merger);
    const std::vector<Tpf::Texture> mod1Textures = mod1Tpf.textures;

    Tpf mod2Tpf = Tpf(mod2Data, merger);
    const std::vector<Tpf::Texture> mod2Textures = mod2Tpf.textures;

    for (int i = 0; i < textures.size(); i++) {
        bool mod1Modified = false;
        bool mod2Modified = false;

        std::optional<Tpf::Texture> mod1tex = GetSameTexture(textures[i].name, mod1Textures);
        std::optional<Tpf::Texture> mod2tex = GetSameTexture(textures[i].name, mod2Textures);

        mod1Modified = mod1tex && (mod1tex->data != textures[i].data);
        mod2Modified = mod2tex && (mod2tex->data != textures[i].data);

        if (mod1Modified && mod2Modified) {
            if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                merger->SetModPriority();
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    return false;
                }
            }

            if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                textures[i] = mod1tex.value();
                sendLog("Unresolvable conflict, using texture " + mod1tex.value().name +
                            " from prioritized mod: " + merger->Mod1Name(),
                        LogFormat::Yellow);
            } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                textures[i] = mod2tex.value();
                sendLog("Unresolvable conflict, using texture " + mod2tex.value().name +
                            " from prioritized mod: " + merger->Mod2Name(),
                        LogFormat::Yellow);
            }
        }

        if (mod1Modified && !mod2Modified) {
            textures[i] = mod1tex.value();
            sendLog("Texture data merged: " + mod1tex.value().name +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            textures[i] = mod2tex.value();
            sendLog("Texture data merged: " + mod2tex.value().name +
                    " from mod: " + merger->Mod2Name());
        }
    }

    for (const auto& tex : mod1Textures) {
        std::optional<Tpf::Texture> origTex = GetSameTexture(tex.name, textures);

        if (!origTex.has_value()) {
            textures.push_back(tex);
            sendLog("Texture data added: " + tex.name);
        }
    }

    for (const auto& tex : mod2Textures) {
        std::optional<Tpf::Texture> origTex = GetSameTexture(tex.name, textures);

        if (!origTex.has_value()) {
            textures.push_back(tex);
            sendLog("Texture data added: " + tex.name);
        }
    }

    return true;
}

std::optional<Tpf::Texture> Tpf::GetSameTexture(const std::string& name,
                                                const std::vector<Tpf::Texture>& otherTextures) {
    std::optional<Tpf::Texture> tex = std::nullopt;
    for (const auto& texture : otherTextures) {
        if (texture.name == name) {
            return tex.emplace(texture);
        }
    }

    return tex;
}

Tpf::~Tpf() {}

} // namespace FileHelper
