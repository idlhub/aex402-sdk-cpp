#pragma once
/**
 * AeX402 AMM C++ SDK - PDA Derivation
 *
 * Utilities for deriving Program Derived Addresses (PDAs).
 * Note: These are helper functions that require an external SHA256 implementation
 * for actual PDA derivation on Ed25519.
 *
 * For full PDA derivation, integrate with libsodium or similar.
 */

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include "types.hpp"
#include "constants.hpp"

namespace aex402 {
namespace pda {

// ============================================================================
// Seed Building Helpers
// ============================================================================

/**
 * Seed bytes container for PDA derivation.
 */
struct Seeds {
    std::vector<std::vector<uint8_t>> seeds;

    Seeds& add(const std::string& s) {
        seeds.push_back(std::vector<uint8_t>(s.begin(), s.end()));
        return *this;
    }

    Seeds& add(const uint8_t* data, size_t len) {
        seeds.push_back(std::vector<uint8_t>(data, data + len));
        return *this;
    }

    Seeds& add(const Pubkey& pk) {
        seeds.push_back(std::vector<uint8_t>(pk.begin(), pk.end()));
        return *this;
    }

    Seeds& add(uint8_t bump) {
        seeds.push_back({bump});
        return *this;
    }

    Seeds& add(uint32_t value) {
        std::vector<uint8_t> bytes(4);
        std::memcpy(bytes.data(), &value, 4);
        seeds.push_back(bytes);
        return *this;
    }

    // Get total seed data for hashing
    std::vector<uint8_t> flatten() const {
        std::vector<uint8_t> result;
        for (const auto& seed : seeds) {
            result.insert(result.end(), seed.begin(), seed.end());
        }
        return result;
    }
};

// ============================================================================
// PDA Seed Constants
// ============================================================================

constexpr const char* POOL_SEED = "pool";
constexpr const char* FARM_SEED = "farm";
constexpr const char* USER_FARM_SEED = "user_farm";
constexpr const char* LOTTERY_SEED = "lottery";
constexpr const char* LOTTERY_ENTRY_SEED = "lottery_entry";
constexpr const char* REGISTRY_SEED = "registry";
constexpr const char* VAULT_SEED = "vault";
constexpr const char* LP_MINT_SEED = "lp_mint";
constexpr const char* VPCLAIM_SEED = "vpclaim";
constexpr const char* GLOBAL_VPOOL_SEED = "global_vpool";

// ============================================================================
// Seed Builders for Each PDA Type
// ============================================================================

/**
 * Build seeds for Pool PDA derivation.
 * Seeds: ["pool", mint0(32), mint1(32)]
 */
inline Seeds pool_seeds(const Pubkey& mint0, const Pubkey& mint1) {
    Seeds s;
    s.add(POOL_SEED);
    s.add(mint0);
    s.add(mint1);
    return s;
}

/**
 * Build seeds for Pool PDA with bump.
 * Seeds: ["pool", mint0(32), mint1(32), bump(1)]
 */
inline Seeds pool_seeds_with_bump(const Pubkey& mint0, const Pubkey& mint1, uint8_t bump) {
    Seeds s = pool_seeds(mint0, mint1);
    s.add(bump);
    return s;
}

/**
 * Build seeds for Farm PDA derivation.
 * Seeds: ["farm", pool(32)]
 */
inline Seeds farm_seeds(const Pubkey& pool) {
    Seeds s;
    s.add(FARM_SEED);
    s.add(pool);
    return s;
}

/**
 * Build seeds for UserFarm PDA derivation.
 * Seeds: ["user_farm", farm(32), user(32)]
 */
inline Seeds user_farm_seeds(const Pubkey& farm, const Pubkey& user) {
    Seeds s;
    s.add(USER_FARM_SEED);
    s.add(farm);
    s.add(user);
    return s;
}

/**
 * Build seeds for Lottery PDA derivation.
 * Seeds: ["lottery", pool(32)]
 */
inline Seeds lottery_seeds(const Pubkey& pool) {
    Seeds s;
    s.add(LOTTERY_SEED);
    s.add(pool);
    return s;
}

/**
 * Build seeds for LotteryEntry PDA derivation.
 * Seeds: ["lottery_entry", lottery(32), user(32)]
 */
inline Seeds lottery_entry_seeds(const Pubkey& lottery, const Pubkey& user) {
    Seeds s;
    s.add(LOTTERY_ENTRY_SEED);
    s.add(lottery);
    s.add(user);
    return s;
}

/**
 * Build seeds for Registry PDA derivation.
 * Seeds: ["registry"]
 */
inline Seeds registry_seeds() {
    Seeds s;
    s.add(REGISTRY_SEED);
    return s;
}

/**
 * Build seeds for Vault PDA derivation.
 * Seeds: ["vault", pool(32), mint(32)]
 */
inline Seeds vault_seeds(const Pubkey& pool, const Pubkey& mint) {
    Seeds s;
    s.add(VAULT_SEED);
    s.add(pool);
    s.add(mint);
    return s;
}

/**
 * Build seeds for LP Mint PDA derivation.
 * Seeds: ["lp_mint", pool(32)]
 */
inline Seeds lp_mint_seeds(const Pubkey& pool) {
    Seeds s;
    s.add(LP_MINT_SEED);
    s.add(pool);
    return s;
}

/**
 * Build seeds for VPoolClaimPDA derivation.
 * Seeds: ["vpclaim", pool_id(4), wallet(32)]
 */
inline Seeds vpclaim_seeds(uint32_t pool_id, const Pubkey& wallet) {
    Seeds s;
    s.add(VPCLAIM_SEED);
    s.add(pool_id);
    s.add(wallet);
    return s;
}

/**
 * Build seeds for Global VPool PDA derivation.
 * Seeds: ["global_vpool"]
 */
inline Seeds global_vpool_seeds() {
    Seeds s;
    s.add(GLOBAL_VPOOL_SEED);
    return s;
}

// ============================================================================
// PDA Result Structure
// ============================================================================

struct PdaResult {
    Pubkey address;
    uint8_t bump;
    bool valid;

    operator bool() const { return valid; }
};

// ============================================================================
// Base58 Utilities
// ============================================================================

/**
 * Base58 alphabet (Bitcoin style).
 */
constexpr const char* BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/**
 * Encode pubkey as base58 string.
 */
inline std::string base58_encode(const Pubkey& key) {
    std::vector<uint8_t> digits;
    digits.push_back(0);

    for (uint8_t byte : key) {
        uint32_t carry = byte;
        for (auto& d : digits) {
            carry += static_cast<uint32_t>(d) << 8;
            d = carry % 58;
            carry /= 58;
        }
        while (carry) {
            digits.push_back(carry % 58);
            carry /= 58;
        }
    }

    // Count leading zeros
    size_t leading_zeros = 0;
    for (auto b : key) {
        if (b != 0) break;
        leading_zeros++;
    }

    // Add leading '1's for zero bytes
    for (size_t i = 0; i < leading_zeros; i++) {
        digits.push_back(0);
    }

    std::string result;
    result.reserve(digits.size());
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        result += BASE58_ALPHABET[*it];
    }

    return result;
}

/**
 * Decode base58 string to pubkey.
 * Returns empty pubkey if decoding fails.
 */
inline Pubkey base58_decode(const std::string& str) {
    Pubkey result{};

    // Create reverse alphabet lookup
    int8_t alphabet_map[256];
    std::memset(alphabet_map, -1, sizeof(alphabet_map));
    for (int i = 0; i < 58; i++) {
        alphabet_map[static_cast<uint8_t>(BASE58_ALPHABET[i])] = i;
    }

    std::vector<uint8_t> bytes;
    bytes.push_back(0);

    for (char c : str) {
        int8_t value = alphabet_map[static_cast<uint8_t>(c)];
        if (value < 0) {
            return result;  // Invalid character
        }

        uint32_t carry = static_cast<uint32_t>(value);
        for (auto& b : bytes) {
            carry += b * 58;
            b = carry & 0xFF;
            carry >>= 8;
        }
        while (carry) {
            bytes.push_back(carry & 0xFF);
            carry >>= 8;
        }
    }

    // Count leading '1's
    size_t leading_ones = 0;
    for (char c : str) {
        if (c != '1') break;
        leading_ones++;
    }

    // Reverse and pad
    size_t pad = leading_ones;
    size_t data_len = bytes.size();

    if (pad + data_len > 32) {
        return result;  // Too long
    }

    // Fill result (reverse order)
    size_t start = 32 - pad - data_len;
    for (size_t i = 0; i < data_len; i++) {
        result[start + i] = bytes[data_len - 1 - i];
    }

    return result;
}

// ============================================================================
// Program ID Helpers
// ============================================================================

/**
 * Get program ID as Pubkey.
 */
inline Pubkey get_program_id() {
    return base58_decode(std::string(PROGRAM_ID_STR));
}

/**
 * Check if a pubkey matches the program ID.
 */
inline bool is_program_id(const Pubkey& pk) {
    return pubkey_eq(pk, get_program_id());
}

// ============================================================================
// Note on PDA Derivation
// ============================================================================

/**
 * Actual PDA derivation requires:
 * 1. SHA256 hashing
 * 2. Ed25519 point decompression check
 *
 * The algorithm:
 * 1. Concatenate: seeds + program_id + "ProgramDerivedAddress"
 * 2. SHA256 hash the result
 * 3. Check if the hash is a valid Ed25519 point (should NOT be on curve)
 * 4. If valid, try next bump (255 down to 0)
 *
 * For production use, integrate with:
 * - libsodium: crypto_core_ed25519_is_valid_point()
 * - OpenSSL: SHA256_* functions
 *
 * Example pseudocode:
 *
 * PdaResult find_pda(const Seeds& seeds, const Pubkey& program_id) {
 *     for (uint8_t bump = 255; bump >= 0; bump--) {
 *         Seeds full_seeds = seeds;
 *         full_seeds.add(bump);
 *
 *         std::vector<uint8_t> data = full_seeds.flatten();
 *         data.insert(data.end(), program_id.begin(), program_id.end());
 *
 *         const char* suffix = "ProgramDerivedAddress";
 *         data.insert(data.end(), suffix, suffix + 21);
 *
 *         uint8_t hash[32];
 *         sha256(data.data(), data.size(), hash);
 *
 *         if (!is_on_curve(hash)) {
 *             Pubkey pk;
 *             std::memcpy(pk.data(), hash, 32);
 *             return {pk, bump, true};
 *         }
 *     }
 *     return {{}, 0, false};
 * }
 */

}  // namespace pda
}  // namespace aex402
