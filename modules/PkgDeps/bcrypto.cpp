// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <Windows.h>
#include <bcrypt.h>

#include "bcrypto.h"
#include "picosha2.h"
#include "settings/key_manager.h"

template <typename TKeyset>
static BCRYPT_KEY_HANDLE ImportRsaPrivateKey(const TKeyset& keyset) {
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE key = nullptr;

    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_RSA_ALGORITHM, nullptr, 0) != 0) {
        throw std::runtime_error("BCryptOpenAlgorithmProvider failed");
    }

    const ULONG blobSize = sizeof(BCRYPT_RSAKEY_BLOB) + keyset.PublicExponent.size() +
                           keyset.Modulus.size() + keyset.Prime1.size() + keyset.Prime2.size() +
                           keyset.Exponent1.size() + keyset.Exponent2.size() +
                           keyset.Coefficient.size() + keyset.PrivateExponent.size();

    std::vector<u8> blob(blobSize);
    auto* hdr = reinterpret_cast<BCRYPT_RSAKEY_BLOB*>(blob.data());

    hdr->Magic = BCRYPT_RSAFULLPRIVATE_MAGIC;
    hdr->BitLength = keyset.Modulus.size() * 8;
    hdr->cbPublicExp = keyset.PublicExponent.size();
    hdr->cbModulus = keyset.Modulus.size();
    hdr->cbPrime1 = keyset.Prime1.size();
    hdr->cbPrime2 = keyset.Prime2.size();

    u8* p = blob.data() + sizeof(BCRYPT_RSAKEY_BLOB);

    auto copy = [&](const std::vector<u8>& v) {
        memcpy(p, v.data(), v.size());
        p += v.size();
    };

    copy(keyset.PublicExponent);
    copy(keyset.Modulus);
    copy(keyset.Prime1);
    copy(keyset.Prime2);
    copy(keyset.Exponent1);       // dp
    copy(keyset.Exponent2);       // dq
    copy(keyset.Coefficient);     // qInv
    copy(keyset.PrivateExponent); // d

    if (BCryptImportKeyPair(alg, nullptr, BCRYPT_RSAFULLPRIVATE_BLOB, &key, blob.data(),
                            blob.size(), 0) != 0) {
        BCryptCloseAlgorithmProvider(alg, 0);
        throw std::runtime_error("BCryptImportKeyPair failed");
    }

    BCryptCloseAlgorithmProvider(alg, 0);
    return key;
}

void Crypto::RSA2048Decrypt(std::span<u8, 32> dec_key, std::span<const u8, 256> ciphertext,
                            bool is_dk3) {
    BCRYPT_KEY_HANDLE key = nullptr;

    if (is_dk3) {
        const auto& ks = KeyManager::GetInstance()->GetAllKeys().PkgDerivedKey3Keyset;
        key = ImportRsaPrivateKey(ks);
    } else {
        const auto& ks = KeyManager::GetInstance()->GetAllKeys().FakeKeyset;
        key = ImportRsaPrivateKey(ks);
    }

    std::array<u8, 256> plaintext{};
    DWORD outSize = 0;

    NTSTATUS st =
        BCryptDecrypt(key, const_cast<u8*>(ciphertext.data()), ciphertext.size(), nullptr, nullptr,
                      0, plaintext.data(), plaintext.size(), &outSize, BCRYPT_PAD_PKCS1);

    BCryptDestroyKey(key);

    if (st < 0) {
        throw std::runtime_error("RSA decrypt failed");
    }

    std::copy_n(plaintext.begin(), dec_key.size(), dec_key.begin());
}

inline void xtsMult(__m128i& tweak) {
    // Shift tweak left by 1 bit (128-bit)
    __m128i t_shifted = _mm_slli_epi64(tweak, 1); // shift low and high 64-bit parts
    __m128i t_high = _mm_srli_epi64(tweak, 63);   // extract carries
    t_shifted = _mm_xor_si128(t_shifted, _mm_slli_si128(t_high, 8));

    // Check carry from MSB
    alignas(16) u8 bytes[16];
    _mm_store_si128(reinterpret_cast<__m128i*>(bytes), tweak);
    if (bytes[15] & 0x80)
        t_shifted = _mm_xor_si128(t_shifted, _mm_set_epi32(0, 0, 0, 0x87));

    tweak = t_shifted;
}

inline __m128i xtsXorBlock(const __m128i& a, const __m128i& b) {
    return _mm_xor_si128(a, b);
}

// AES-128 decrypt key setup using AES-NI
struct AES128Key {
    __m128i roundKeys[11];
};

// AES-128 key schedule (unrolled, RCON constants)
__attribute__((target("aes"))) static inline void aes128_set_encrypt_key(const u8* key,
                                                                         AES128Key& k) {
    k.roundKeys[0] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key));

    // Unrolled rounds
    k.roundKeys[1] = _mm_aeskeygenassist_si128(k.roundKeys[0], 0x01);
    k.roundKeys[1] = _mm_shuffle_epi32(k.roundKeys[1], _MM_SHUFFLE(3, 3, 3, 3));
    __m128i rk = k.roundKeys[0];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[1] = _mm_xor_si128(rk, k.roundKeys[1]);

    k.roundKeys[2] = _mm_aeskeygenassist_si128(k.roundKeys[1], 0x02);
    k.roundKeys[2] = _mm_shuffle_epi32(k.roundKeys[2], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[1];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[2] = _mm_xor_si128(rk, k.roundKeys[2]);

    k.roundKeys[3] = _mm_aeskeygenassist_si128(k.roundKeys[2], 0x04);
    k.roundKeys[3] = _mm_shuffle_epi32(k.roundKeys[3], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[2];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[3] = _mm_xor_si128(rk, k.roundKeys[3]);

    k.roundKeys[4] = _mm_aeskeygenassist_si128(k.roundKeys[3], 0x08);
    k.roundKeys[4] = _mm_shuffle_epi32(k.roundKeys[4], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[3];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[4] = _mm_xor_si128(rk, k.roundKeys[4]);

    k.roundKeys[5] = _mm_aeskeygenassist_si128(k.roundKeys[4], 0x10);
    k.roundKeys[5] = _mm_shuffle_epi32(k.roundKeys[5], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[4];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[5] = _mm_xor_si128(rk, k.roundKeys[5]);

    k.roundKeys[6] = _mm_aeskeygenassist_si128(k.roundKeys[5], 0x20);
    k.roundKeys[6] = _mm_shuffle_epi32(k.roundKeys[6], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[5];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[6] = _mm_xor_si128(rk, k.roundKeys[6]);

    k.roundKeys[7] = _mm_aeskeygenassist_si128(k.roundKeys[6], 0x40);
    k.roundKeys[7] = _mm_shuffle_epi32(k.roundKeys[7], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[6];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[7] = _mm_xor_si128(rk, k.roundKeys[7]);

    k.roundKeys[8] = _mm_aeskeygenassist_si128(k.roundKeys[7], 0x80);
    k.roundKeys[8] = _mm_shuffle_epi32(k.roundKeys[8], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[7];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[8] = _mm_xor_si128(rk, k.roundKeys[8]);

    k.roundKeys[9] = _mm_aeskeygenassist_si128(k.roundKeys[8], 0x1B);
    k.roundKeys[9] = _mm_shuffle_epi32(k.roundKeys[9], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[8];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[9] = _mm_xor_si128(rk, k.roundKeys[9]);

    k.roundKeys[10] = _mm_aeskeygenassist_si128(k.roundKeys[9], 0x36);
    k.roundKeys[10] = _mm_shuffle_epi32(k.roundKeys[10], _MM_SHUFFLE(3, 3, 3, 3));
    rk = k.roundKeys[9];
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    rk = _mm_xor_si128(rk, _mm_slli_si128(rk, 4));
    k.roundKeys[10] = _mm_xor_si128(rk, k.roundKeys[10]);
}

__attribute__((target("aes"))) inline void aes128_set_decrypt_key(const AES128Key& enc,
                                                                  AES128Key& dec) {
    dec.roundKeys[0] = enc.roundKeys[10];
    for (int i = 1; i < 10; ++i)
        dec.roundKeys[i] = _mm_aesimc_si128(enc.roundKeys[10 - i]);
    dec.roundKeys[10] = enc.roundKeys[0];
}

__attribute__((target("aes"))) inline void aes128_encrypt_block(const u8 in[16], u8 out[16],
                                                                const AES128Key& rk) {
    __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
    block = _mm_xor_si128(block, rk.roundKeys[0]);
    for (int i = 1; i < 10; ++i)
        block = _mm_aesenc_si128(block, rk.roundKeys[i]);
    block = _mm_aesenclast_si128(block, rk.roundKeys[10]);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), block);
}

__attribute__((target("aes"))) inline void aes128_decrypt_block(const u8 in[16], u8 out[16],
                                                                const AES128Key& rk) {
    __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
    block = _mm_xor_si128(block, rk.roundKeys[0]);
    for (int i = 1; i < 10; ++i)
        block = _mm_aesdec_si128(block, rk.roundKeys[i]);
    block = _mm_aesdeclast_si128(block, rk.roundKeys[10]);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), block);
}

inline bool cpu_supports_avx2() {
    int info[4] = {0, 0, 0, 0};

#ifdef _MSC_VER
    __cpuid(info, 0);
    int nIds = info[0];
    if (nIds >= 7) {
        __cpuidex(info, 7, 0);
        return (info[1] & (1 << 5)) != 0; // AVX2 = EBX bit 5
    }
#else
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid_max(0, nullptr) >= 7) {
        __cpuid_count(7, 0, eax, ebx, ecx, edx);
        return (ebx & (1 << 5)) != 0; // AVX2 = EBX bit 5
    }
#endif
    return false;
}
void Crypto::decryptPFS(std::span<const u8, 16> dataKey, std::span<const u8, 16> tweakKey,
                        std::span<const u8> src_image, std::span<u8> dst_image, u64 sector_start) {
    if (cpu_supports_avx2()) {
        decryptPFS_AVX2(dataKey, tweakKey, src_image, dst_image, sector_start);
    } else {
        decryptPFS_AES(dataKey, tweakKey, src_image, dst_image, sector_start);
    }
}
// ----------------- Decrypt PFS -----------------

__attribute__((target("aes"))) void Crypto::decryptPFS_AES(std::span<const u8, 16> dataKey,
                                                           std::span<const u8, 16> tweakKey,
                                                           std::span<const u8> src_image,
                                                           std::span<u8> dst_image,
                                                           u64 sector_start) {

    AES128Key aesTweakEncKey, aesDataEncKey, aesDataDecKey;
    aes128_set_encrypt_key(tweakKey.data(), aesTweakEncKey);
    aes128_set_encrypt_key(dataKey.data(), aesDataEncKey);
    aes128_set_decrypt_key(aesDataEncKey, aesDataDecKey);

    constexpr size_t SECTOR_SIZE = 0x1000;
    constexpr size_t BLOCK_SIZE = 16;

    size_t total_sectors = src_image.size() / SECTOR_SIZE;

    for (size_t s = 0; s < total_sectors; ++s) {
        u64 current_sector = sector_start + s;
        __m128i tweak = _mm_set_epi64x(0, current_sector);
        aes128_encrypt_block(reinterpret_cast<u8*>(&tweak), reinterpret_cast<u8*>(&tweak),
                             aesTweakEncKey);

        __m128i currentTweak = tweak;

        for (size_t block_offset = 0; block_offset < SECTOR_SIZE; block_offset += BLOCK_SIZE) {
            size_t pos = s * SECTOR_SIZE + block_offset;

            __m128i block =
                _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_image.data() + pos));
            block = xtsXorBlock(block, currentTweak);
            aes128_decrypt_block(reinterpret_cast<u8*>(&block), reinterpret_cast<u8*>(&block),
                                 aesDataDecKey);
            block = xtsXorBlock(block, currentTweak);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_image.data() + pos), block);

            xtsMult(currentTweak);
        }
    }
}

__attribute__((target("aes"))) void Crypto::decryptPFS_AVX2(std::span<const u8, 16> dataKey,
                                                            std::span<const u8, 16> tweakKey,
                                                            std::span<const u8> src_image,
                                                            std::span<u8> dst_image,
                                                            u64 sector_start) {
    AES128Key aesTweakEncKey, aesDataEncKey, aesDataDecKey;
    aes128_set_encrypt_key(tweakKey.data(), aesTweakEncKey);
    aes128_set_encrypt_key(dataKey.data(), aesDataEncKey);
    aes128_set_decrypt_key(aesDataEncKey, aesDataDecKey);

    constexpr size_t SECTOR_SIZE = 0x1000;
    constexpr size_t BLOCK_SIZE = 16;

    size_t total_sectors = src_image.size() / SECTOR_SIZE;

    for (size_t s = 0; s < total_sectors; ++s) {
        u64 current_sector = sector_start + s;
        __m128i tweak = _mm_set_epi64x(0, current_sector);
        aes128_encrypt_block(reinterpret_cast<u8*>(&tweak), reinterpret_cast<u8*>(&tweak),
                             aesTweakEncKey);

        __m128i currentTweak = tweak;

        for (size_t block_offset = 0; block_offset < SECTOR_SIZE; block_offset += 32) {
            size_t pos = s * SECTOR_SIZE + block_offset;

            // Load two blocks
            __m128i block1 =
                _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_image.data() + pos));
            __m128i block2 = _mm_loadu_si128(
                reinterpret_cast<const __m128i*>(src_image.data() + pos + BLOCK_SIZE));

            // XOR with tweaks
            block1 = xtsXorBlock(block1, currentTweak);
            __m128i nextTweak = currentTweak;
            xtsMult(nextTweak);
            block2 = xtsXorBlock(block2, nextTweak);

            // AES decrypt
            aes128_decrypt_block(reinterpret_cast<u8*>(&block1), reinterpret_cast<u8*>(&block1),
                                 aesDataDecKey);
            aes128_decrypt_block(reinterpret_cast<u8*>(&block2), reinterpret_cast<u8*>(&block2),
                                 aesDataDecKey);

            // XOR with tweaks again
            block1 = xtsXorBlock(block1, currentTweak);
            block2 = xtsXorBlock(block2, nextTweak);

            // Store results
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_image.data() + pos), block1);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_image.data() + pos + BLOCK_SIZE),
                             block2);

            // Prepare tweak for next iteration
            xtsMult(nextTweak);
            currentTweak = nextTweak;
        }
    }
}

__attribute__((target("aes"))) void Crypto::aesCbcCfb128DecryptEntry(std::span<const u8, 32> ivkey,
                                                                     std::span<u8> ciphertext,
                                                                     std::span<u8> decrypted) {
    constexpr size_t BLOCK_SIZE = 16;
    constexpr size_t KEY_SIZE = 16;

    /*printf("ivkey.size() = %zu\n", ivkey.size());
    printf("ivkey.data() = %p\n", (void*)ivkey.data());
    printf("ciphertext.size() = %zu\n", ciphertext.size());
    printf("decrypted.size() = %zu\n", decrypted.size());*/

    if (!ivkey.data()) {
        throw std::runtime_error("ivkey.data() is NULL!");
    }

    if (ivkey.size() < 32) {
        throw std::runtime_error("ivkey too small: " + std::to_string(ivkey.size()));
    }

    std::array<u8, 16> key;
    std::array<u8, 16> iv;

    std::memcpy(key.data(), ivkey.data() + 16, 16);
    std::memcpy(iv.data(), ivkey.data(), 16);

    // Setup AES
    AES128Key aesEncKey, aesDecKey;
    aes128_set_encrypt_key(key.data(), aesEncKey);
    aes128_set_decrypt_key(aesEncKey, aesDecKey);

    // CBC decryption
    __m128i prevCipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(iv.data()));

    for (size_t offset = 0; offset < ciphertext.size(); offset += BLOCK_SIZE) {
        const u8* cipher_ptr = ciphertext.data() + offset;
        u8* decrypted_ptr = decrypted.data() + offset;

        // Load ciphertext
        __m128i cipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(cipher_ptr));

        // Decrypt
        alignas(16) u8 decryptedBlock[BLOCK_SIZE];
        aes128_decrypt_block(cipher_ptr, decryptedBlock, aesDecKey);

        // XOR with previous block
        __m128i plainBlock = _mm_xor_si128(
            _mm_load_si128(reinterpret_cast<const __m128i*>(decryptedBlock)), prevCipherBlock);

        // Store result
        _mm_storeu_si128(reinterpret_cast<__m128i*>(decrypted_ptr), plainBlock);

        // Update previous block
        prevCipherBlock = cipherBlock;
    }
}

__attribute__((target("aes"))) void Crypto::aesCbcCfb128Decrypt(std::span<const u8, 32> ivkey,
                                                                std::span<const u8, 256> ciphertext,
                                                                std::span<u8, 256> decrypted) {
    constexpr size_t BLOCK_SIZE = 16;
    constexpr size_t NUM_BLOCKS = 256 / BLOCK_SIZE;

    // Extract key and IV
    alignas(16) u8 key[BLOCK_SIZE];
    alignas(16) u8 iv[BLOCK_SIZE];

    std::memcpy(key, ivkey.data() + BLOCK_SIZE, BLOCK_SIZE);
    std::memcpy(iv, ivkey.data(), BLOCK_SIZE);

    // Setup AES keys
    AES128Key aesEncKey, aesDecKey;
    aes128_set_encrypt_key(key, aesEncKey);
    aes128_set_decrypt_key(aesEncKey, aesDecKey);

    // CBC chaining value
    __m128i prevCipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(iv));

    for (size_t i = 0; i < NUM_BLOCKS; ++i) {
        const u8* cptr = ciphertext.data() + i * BLOCK_SIZE;
        u8* dptr = decrypted.data() + i * BLOCK_SIZE;

        // Load ciphertext block
        __m128i cipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(cptr));

        // AES-128 decrypt
        alignas(16) u8 decryptedBlock[BLOCK_SIZE];
        aes128_decrypt_block(cptr, decryptedBlock, aesDecKey);

        // CBC XOR
        __m128i plainBlock = _mm_xor_si128(
            _mm_load_si128(reinterpret_cast<const __m128i*>(decryptedBlock)), prevCipherBlock);

        // Store plaintext
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dptr), plainBlock);

        // Update chaining value
        prevCipherBlock = cipherBlock;
    }
}

inline void ivKeyHASH256_software(std::span<const u8, 64> cipher_input,
                                  std::span<u8, 32> ivkey_result) {
    picosha2::hash256(cipher_input.begin(), cipher_input.end(), ivkey_result);
}

void Crypto::ivKeyHASH256(std::span<const u8, 64> cipher_input, std::span<u8, 32> ivkey_result) {
    // if (cpu_has_sha_ni()) {
    // ivKeyHASH256_ni(in, out);
    //} else {
    ivKeyHASH256_software(cipher_input, ivkey_result);
    //}
}

inline void hmac_sha256(const u8* key, size_t key_len, const u8* message, size_t msg_len,
                        u8* digest) {
    constexpr size_t BLOCK_SIZE = 64;
    std::array<u8, BLOCK_SIZE> ipad{}, opad{}, key_block{};

    // Prepare key
    if (key_len > BLOCK_SIZE) {
        // Hash key if it's too long
        picosha2::hash256(key, key + key_len, key_block.data(), key_block.data() + 32);
        key_len = 32;
    } else {
        std::memcpy(key_block.data(), key, key_len);
    }

    // Pad key with zeros if necessary
    if (key_len < BLOCK_SIZE) {
        std::fill(key_block.begin() + key_len, key_block.end(), 0);
    }

    // Create ipad and opad
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        ipad[i] = key_block[i] ^ 0x36;
        opad[i] = key_block[i] ^ 0x5C;
    }

    // Inner hash
    std::vector<u8> inner_data;
    inner_data.reserve(BLOCK_SIZE + msg_len);
    inner_data.insert(inner_data.end(), ipad.begin(), ipad.end());
    inner_data.insert(inner_data.end(), message, message + msg_len);

    std::array<u8, 32> inner_hash{};
    picosha2::hash256(inner_data.begin(), inner_data.end(), inner_hash.begin(), inner_hash.end());

    // Outer hash
    std::vector<u8> outer_data;
    outer_data.reserve(BLOCK_SIZE + 32);
    outer_data.insert(outer_data.end(), opad.begin(), opad.end());
    outer_data.insert(outer_data.end(), inner_hash.begin(), inner_hash.end());

    picosha2::hash256(outer_data.begin(), outer_data.end(), digest, digest + 32);
}

void Crypto::PfsGenCryptoKey(std::span<const u8, 32> ekpfs, std::span<const u8, 16> seed,
                             std::span<u8, 16> dataKey, std::span<u8, 16> tweakKey) {
    std::array<u8, 20> d{};
    u32 index = 1;
    std::memcpy(d.data(), &index, sizeof(u32));
    std::memcpy(d.data() + sizeof(u32), seed.data(), seed.size());

    std::array<u8, 32> hmac_result{};
    hmac_sha256(ekpfs.data(), ekpfs.size(), d.data(), d.size(), hmac_result.data());

    std::copy(hmac_result.begin(), hmac_result.begin() + 16, tweakKey.begin());
    std::copy(hmac_result.begin() + 16, hmac_result.end(), dataKey.begin());
}
__attribute__((target("aes"))) void Crypto::decryptEFSM(std::span<u8, 16> trophyKey,
                                                        std::span<u8, 16> NPcommID,
                                                        std::span<u8, 16> efsmIv,
                                                        std::span<u8> ciphertext,
                                                        std::span<u8> decrypted) {
    constexpr size_t BLOCK_SIZE = 16;

    if (ciphertext.size() != decrypted.size()) {
        throw std::runtime_error("Invalid ciphertext/decrypted sizes");
    }

    // Step 1: Encrypt NPcommID with trophyKey (ECB since IV is zero)
    AES128Key trophyEncKey;
    aes128_set_encrypt_key(trophyKey.data(), trophyEncKey);

    alignas(16) std::array<u8, BLOCK_SIZE> trpKey;
    aes128_encrypt_block(NPcommID.data(), trpKey.data(), trophyEncKey);

    // Step 2: Setup decryption for trpKey
    AES128Key trpEncKey, trpDecKey;
    aes128_set_encrypt_key(trpKey.data(), trpEncKey);
    aes128_set_decrypt_key(trpEncKey, trpDecKey);

    // CBC decryption
    __m128i prevCipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(efsmIv.data()));

    size_t num_blocks = ciphertext.size() / BLOCK_SIZE;

    for (size_t i = 0; i < num_blocks; ++i) {
        size_t offset = i * BLOCK_SIZE;
        const u8* cipher_ptr = ciphertext.data() + offset;
        u8* decrypted_ptr = decrypted.data() + offset;

        // Load ciphertext
        __m128i cipherBlock = _mm_loadu_si128(reinterpret_cast<const __m128i*>(cipher_ptr));

        // Use your aes128_decrypt_block function
        alignas(16) u8 decryptedBlockBytes[BLOCK_SIZE];
        aes128_decrypt_block(cipher_ptr, decryptedBlockBytes, trpDecKey);

        // XOR with previous block
        __m128i decryptedBlock =
            _mm_load_si128(reinterpret_cast<const __m128i*>(decryptedBlockBytes));
        __m128i plainBlock = _mm_xor_si128(decryptedBlock, prevCipherBlock);

        // Store result
        _mm_storeu_si128(reinterpret_cast<__m128i*>(decrypted_ptr), plainBlock);

        // Update previous block
        prevCipherBlock = cipherBlock;
    }
}
