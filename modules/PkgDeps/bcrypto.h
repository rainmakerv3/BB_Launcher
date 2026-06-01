// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <span>

#include "modules/PkgDeps/types.h"

class Crypto {
public:
    void RSA2048Decrypt(std::span<u8, 32> dk3, std::span<const u8, 256> ciphertext,
                        bool is_dk3); // RSAES_PKCS1v15_
    void ivKeyHASH256(std::span<const u8, 64> cipher_input, std::span<u8, 32> ivkey_result);
    void aesCbcCfb128Decrypt(std::span<const u8, 32> ivkey, std::span<const u8, 256> ciphertext,
                             std::span<u8, 256> decrypted);
    void aesCbcCfb128DecryptEntry(std::span<const u8, 32> ivkey, std::span<u8> ciphertext,
                                  std::span<u8> decrypted);
    void decryptEFSM(std::span<u8, 16> trophyKey, std::span<u8, 16> NPcommID,
                     std::span<u8, 16> efsmIv, std::span<u8> ciphertext, std::span<u8> decrypted);
    void PfsGenCryptoKey(std::span<const u8, 32> ekpfs, std::span<const u8, 16> seed,
                         std::span<u8, 16> dataKey, std::span<u8, 16> tweakKey);
    void decryptPFS(std::span<const u8, 16> dataKey, std::span<const u8, 16> tweakKey,
                    std::span<const u8> src_image, std::span<u8> dst_image, u64 sector);
    void decryptPFS_AES(std::span<const u8, 16> dataKey, std::span<const u8, 16> tweakKey,
                        std::span<const u8> src_image, std::span<u8> dst_image, u64 sector);
    void decryptPFS_AVX2(std::span<const u8, 16> dataKey, std::span<const u8, 16> tweakKey,
                         std::span<const u8> src_image, std::span<u8> dst_image, u64 sector);
};
