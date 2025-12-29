#pragma once
/**
 * AeX402 AMM C++ SDK
 *
 * Complete C++ SDK for interacting with the AeX402 AMM on Solana.
 * Single header that includes all SDK components.
 *
 * Program ID: 3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje
 *
 * Components:
 * - constants.hpp: Program ID, discriminators, error codes
 * - types.hpp:     Account structures (Pool, NPool, Farm, etc.)
 * - accounts.hpp:  Account parsing functions
 * - instructions.hpp: Instruction builders
 * - math.hpp:      StableSwap math (Newton's method)
 * - pda.hpp:       PDA derivation utilities
 *
 * Example usage:
 *
 *   #include <aex402.hpp>
 *
 *   using namespace aex402;
 *
 *   // Parse pool account data
 *   auto pool = parse_pool(data, len);
 *   if (pool) {
 *       std::cout << "Pool amp: " << pool->amp << std::endl;
 *   }
 *
 *   // Build swap instruction
 *   auto ix = InstructionBuilder::swapt0t1(1000000, 990000);
 *   // Submit ix.data() to Solana...
 *
 *   // Simulate swap off-chain
 *   auto out = math::simulate_swap(bal0, bal1, amount_in, amp, fee_bps);
 *   if (out) {
 *       std::cout << "Expected output: " << *out << std::endl;
 *   }
 *
 * Requirements:
 * - C++17 or later
 * - __uint128_t support (GCC, Clang)
 *
 * For MSVC, a custom 128-bit implementation would be needed.
 */

#include "constants.hpp"
#include "types.hpp"
#include "accounts.hpp"
#include "instructions.hpp"
#include "math.hpp"
#include "pda.hpp"

namespace aex402 {

/**
 * SDK version information.
 */
constexpr int SDK_VERSION_MAJOR = 1;
constexpr int SDK_VERSION_MINOR = 0;
constexpr int SDK_VERSION_PATCH = 0;

inline constexpr const char* sdk_version() {
    return "1.0.0";
}

/**
 * Check if SDK is compatible with a given program version.
 */
inline bool is_compatible(int major, int minor = 0) {
    return major == SDK_VERSION_MAJOR && minor <= SDK_VERSION_MINOR;
}

}  // namespace aex402

// Backwards compatibility alias
namespace stableswap = aex402;
