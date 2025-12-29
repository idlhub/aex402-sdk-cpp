#pragma once
/**
 * AeX402 AMM C++ SDK - StableSwap Math
 *
 * Newton's method implementations for AeX402 curve calculations.
 * Uses __uint128_t for high-precision intermediate calculations.
 *
 * Key formulas:
 * - D: Pool invariant calculated from balances and amp
 * - Y: Output amount calculated from input, D, and amp
 */

#include <cstdint>
#include <optional>
#include <cmath>
#include "constants.hpp"

namespace aex402 {
namespace math {

// ============================================================================
// Constants
// ============================================================================

constexpr uint64_t FEE_DENOMINATOR = 10000;  // For basis points
constexpr uint64_t PRECISION = 1000000000000ULL;  // 1e12 for reward calculations

// ============================================================================
// 128-bit Arithmetic Helpers
// ============================================================================

/**
 * Safe 128-bit multiplication.
 */
inline __uint128_t mul128(uint64_t a, uint64_t b) {
    return static_cast<__uint128_t>(a) * b;
}

/**
 * Safe 128-bit division.
 */
inline uint64_t div128(__uint128_t n, uint64_t d) {
    if (d == 0) return 0;
    return static_cast<uint64_t>(n / d);
}

/**
 * Integer square root using Newton's method.
 */
inline uint64_t isqrt(uint64_t n) {
    if (n == 0) return 0;
    if (n <= 3) return 1;

    uint64_t x = n;
    uint64_t y = (x + 1) / 2;

    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }

    return x;
}

/**
 * Integer square root for 128-bit values.
 */
inline uint64_t isqrt128(__uint128_t n) {
    if (n == 0) return 0;
    if (n <= 3) return 1;

    __uint128_t x = n;
    __uint128_t y = (x + 1) / 2;

    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }

    return static_cast<uint64_t>(x);
}

// ============================================================================
// 2-Token Pool Math (StableSwap)
// ============================================================================

/**
 * Calculate invariant D for 2-token pool using Newton's method.
 *
 * The StableSwap invariant equation:
 * A*n^n * sum(x_i) + D = A*D*n^n + D^(n+1) / (n^n * prod(x_i))
 *
 * For n=2:
 * 4A(x + y) + D = 4AD + D^3 / (4xy)
 *
 * Newton iteration:
 * d = (Ann*S + D_P*n) * D / ((Ann-1)*D + (n+1)*D_P)
 *
 * where:
 * - S = sum of balances (x + y)
 * - D_P = D^(n+1) / (n^n * prod(x_i)) = D^3 / (4xy)
 * - Ann = A * n^n = A * 4
 *
 * @param x Balance of token 0
 * @param y Balance of token 1
 * @param amp Amplification coefficient
 * @return Invariant D, or nullopt if failed to converge
 */
inline std::optional<uint64_t> calc_d(uint64_t x, uint64_t y, uint64_t amp) {
    uint64_t s = x + y;
    if (s == 0) return 0;

    uint64_t d = s;
    uint64_t ann = amp * 4;  // A * n^n where n=2

    for (int i = 0; i < NEWTON_ITERATIONS; i++) {
        // d_p = D^3 / (4 * x * y)
        // Calculate in steps to avoid overflow
        __uint128_t d_p = mul128(d, d) / (2 * x);
        d_p = d_p * d / (2 * y);

        uint64_t d_prev = d;

        // d = (Ann*S + D_P*2) * D / ((Ann-1)*D + 3*D_P)
        __uint128_t num = (mul128(ann, s) + d_p * 2) * d;
        __uint128_t denom = mul128(ann - 1, d) + d_p * 3;

        if (denom == 0) return std::nullopt;

        d = static_cast<uint64_t>(num / denom);

        // Check convergence
        if (d > d_prev) {
            if (d - d_prev <= 1) return d;
        } else {
            if (d_prev - d <= 1) return d;
        }
    }

    return std::nullopt;  // Failed to converge
}

/**
 * Calculate output amount y given new input balance x and invariant D.
 *
 * Solving the invariant equation for y:
 * y^2 + (S' + D/Ann - D)*y = D^3/(4*Ann*x)
 *
 * where S' = x (the new balance of token being deposited)
 *
 * Newton iteration:
 * y = (y^2 + c) / (2y + b - D)
 *
 * where:
 * - c = D^3 / (4 * x_new * Ann)
 * - b = x_new + D/Ann
 *
 * @param x_new New balance of input token after deposit
 * @param d Pool invariant D
 * @param amp Amplification coefficient
 * @return New balance of output token, or nullopt if failed
 */
inline std::optional<uint64_t> calc_y(uint64_t x_new, uint64_t d, uint64_t amp) {
    uint64_t ann = amp * 4;

    // c = D^3 / (4 * x_new * Ann)
    __uint128_t c = mul128(d, d) / (2 * x_new);
    c = c * d / (2 * ann);

    // b = x_new + D / Ann
    uint64_t b = x_new + d / ann;

    uint64_t y = d;

    for (int i = 0; i < NEWTON_ITERATIONS; i++) {
        uint64_t y_prev = y;

        // y = (y^2 + c) / (2y + b - D)
        __uint128_t num = mul128(y, y) + c;
        uint64_t denom = 2 * y + b - d;

        if (denom == 0) return std::nullopt;

        y = static_cast<uint64_t>(num / denom);

        // Check convergence
        if (y > y_prev) {
            if (y - y_prev <= 1) return y;
        } else {
            if (y_prev - y <= 1) return y;
        }
    }

    return std::nullopt;
}

/**
 * Simulate a swap and return output amount.
 *
 * @param bal_in Current balance of input token
 * @param bal_out Current balance of output token
 * @param amount_in Amount being swapped in
 * @param amp Amplification coefficient
 * @param fee_bps Fee in basis points
 * @return Output amount after fees, or nullopt if calculation fails
 */
inline std::optional<uint64_t> simulate_swap(
    uint64_t bal_in, uint64_t bal_out,
    uint64_t amount_in, uint64_t amp, uint64_t fee_bps
) {
    // Calculate current invariant
    auto d = calc_d(bal_in, bal_out, amp);
    if (!d) return std::nullopt;

    // Calculate new output balance
    uint64_t new_bal_in = bal_in + amount_in;
    auto new_bal_out = calc_y(new_bal_in, *d, amp);
    if (!new_bal_out) return std::nullopt;

    // Calculate output amount
    if (*new_bal_out >= bal_out) return 0;  // Should never happen with valid inputs
    uint64_t amount_out = bal_out - *new_bal_out;

    // Apply fee
    uint64_t fee = amount_out * fee_bps / FEE_DENOMINATOR;
    amount_out -= fee;

    return amount_out;
}

/**
 * Calculate LP tokens for initial deposit.
 * Uses geometric mean: LP = sqrt(amount0 * amount1)
 */
inline uint64_t calc_initial_lp(uint64_t amount0, uint64_t amount1) {
    __uint128_t product = mul128(amount0, amount1);
    return isqrt128(product);
}

/**
 * Calculate LP tokens for deposit into existing pool.
 *
 * @param amt0 Amount of token 0 to deposit
 * @param amt1 Amount of token 1 to deposit
 * @param bal0 Current balance of token 0
 * @param bal1 Current balance of token 1
 * @param lp_supply Current LP token supply
 * @param amp Amplification coefficient
 * @return LP tokens to mint, or nullopt if calculation fails
 */
inline std::optional<uint64_t> calc_lp_tokens(
    uint64_t amt0, uint64_t amt1,
    uint64_t bal0, uint64_t bal1,
    uint64_t lp_supply, uint64_t amp
) {
    if (lp_supply == 0) {
        // Initial deposit
        return calc_initial_lp(amt0, amt1);
    }

    // Calculate D before and after
    auto d0 = calc_d(bal0, bal1, amp);
    auto d1 = calc_d(bal0 + amt0, bal1 + amt1, amp);

    if (!d0 || !d1 || *d0 == 0) return std::nullopt;

    // LP tokens = lp_supply * (D1 - D0) / D0
    __uint128_t lp = mul128(lp_supply, *d1 - *d0) / *d0;
    return static_cast<uint64_t>(lp);
}

/**
 * Calculate tokens received for LP burn.
 *
 * @param lp_amount LP tokens to burn
 * @param bal0 Current balance of token 0
 * @param bal1 Current balance of token 1
 * @param lp_supply Current LP token supply
 * @return Pair of (amount0, amount1), or nullopt if invalid
 */
struct WithdrawResult {
    uint64_t amount0;
    uint64_t amount1;
};

inline std::optional<WithdrawResult> calc_withdraw(
    uint64_t lp_amount,
    uint64_t bal0, uint64_t bal1,
    uint64_t lp_supply
) {
    if (lp_supply == 0) return std::nullopt;

    uint64_t amount0 = div128(mul128(bal0, lp_amount), lp_supply);
    uint64_t amount1 = div128(mul128(bal1, lp_amount), lp_supply);

    return WithdrawResult{amount0, amount1};
}

/**
 * Calculate current amp during ramping.
 *
 * @param amp Initial/current amp
 * @param target_amp Target amp after ramp completes
 * @param ramp_start Ramp start timestamp
 * @param ramp_end Ramp end timestamp
 * @param now Current timestamp
 * @return Current effective amp
 */
inline uint64_t get_current_amp(
    uint64_t amp, uint64_t target_amp,
    int64_t ramp_start, int64_t ramp_end,
    int64_t now
) {
    if (now >= ramp_end || ramp_end == ramp_start) {
        return target_amp;
    }

    if (now <= ramp_start) {
        return amp;
    }

    int64_t elapsed = now - ramp_start;
    int64_t duration = ramp_end - ramp_start;

    if (target_amp > amp) {
        return amp + (target_amp - amp) * static_cast<uint64_t>(elapsed) / static_cast<uint64_t>(duration);
    } else {
        return amp - (amp - target_amp) * static_cast<uint64_t>(elapsed) / static_cast<uint64_t>(duration);
    }
}

/**
 * Calculate price impact for a swap as a percentage.
 *
 * @param bal_in Balance of input token
 * @param bal_out Balance of output token
 * @param amount_in Amount to swap
 * @param amp Amplification coefficient
 * @param fee_bps Fee in basis points
 * @return Price impact as decimal (0.01 = 1%), or nullopt if calculation fails
 */
inline std::optional<double> calc_price_impact(
    uint64_t bal_in, uint64_t bal_out,
    uint64_t amount_in, uint64_t amp, uint64_t fee_bps
) {
    auto amount_out = simulate_swap(bal_in, bal_out, amount_in, amp, fee_bps);
    if (!amount_out || amount_in == 0) return std::nullopt;

    // Ideal rate without slippage would be 1:1 for stable pools
    // Price impact = 1 - (actual_out / expected_out)
    // For stable pools with 1:1 expectation:
    double ratio = static_cast<double>(*amount_out) / static_cast<double>(amount_in);
    return 1.0 - ratio;
}

/**
 * Calculate minimum output with slippage tolerance.
 *
 * @param expected_output Expected output amount
 * @param slippage_bps Slippage tolerance in basis points
 * @return Minimum acceptable output
 */
inline uint64_t calc_min_output(uint64_t expected_output, uint64_t slippage_bps) {
    return expected_output * (FEE_DENOMINATOR - slippage_bps) / FEE_DENOMINATOR;
}

/**
 * Calculate virtual price (LP value relative to underlying).
 * Virtual price = D * 1e18 / LP_supply
 *
 * @param bal0 Balance of token 0
 * @param bal1 Balance of token 1
 * @param lp_supply Total LP supply
 * @param amp Amplification coefficient
 * @return Virtual price scaled by 1e18, or nullopt if calculation fails
 */
inline std::optional<__uint128_t> calc_virtual_price(
    uint64_t bal0, uint64_t bal1,
    uint64_t lp_supply, uint64_t amp
) {
    if (lp_supply == 0) return std::nullopt;

    auto d = calc_d(bal0, bal1, amp);
    if (!d) return std::nullopt;

    constexpr __uint128_t PRECISION_18 = 1000000000000000000ULL;  // 1e18
    return static_cast<__uint128_t>(*d) * PRECISION_18 / lp_supply;
}

// ============================================================================
// N-Token Pool Math
// ============================================================================

/**
 * Calculate invariant D for N-token pool.
 *
 * Generalized StableSwap for N tokens:
 * A*n^n * sum(x_i) + D = A*D*n^n + D^(n+1) / (n^n * prod(x_i))
 *
 * @param balances Token balances
 * @param n_tokens Number of tokens (2-8)
 * @param amp Amplification coefficient
 * @return Invariant D, or nullopt if failed to converge
 */
inline std::optional<uint64_t> calc_d_n(
    const uint64_t* balances, uint8_t n_tokens, uint64_t amp
) {
    // Calculate sum and product
    uint64_t s = 0;
    __uint128_t nn = 1;

    for (uint8_t i = 0; i < n_tokens; i++) {
        s += balances[i];
        nn *= n_tokens;  // n^n
    }

    if (s == 0) return 0;

    uint64_t d = s;
    uint64_t ann = amp * static_cast<uint64_t>(nn);

    for (int iter = 0; iter < NEWTON_ITERATIONS; iter++) {
        // Calculate D_P = D^(n+1) / (n^n * prod(balances))
        __uint128_t d_p = d;
        for (uint8_t i = 0; i < n_tokens; i++) {
            if (balances[i] == 0) return std::nullopt;
            d_p = d_p * d / (n_tokens * balances[i]);
        }

        uint64_t d_prev = d;

        // d = (Ann*S + D_P*n) * D / ((Ann-1)*D + (n+1)*D_P)
        __uint128_t num = (mul128(ann, s) + d_p * n_tokens) * d;
        __uint128_t denom = mul128(ann - 1, d) + d_p * (n_tokens + 1);

        if (denom == 0) return std::nullopt;

        d = static_cast<uint64_t>(num / denom);

        // Check convergence
        if (d > d_prev) {
            if (d - d_prev <= 1) return d;
        } else {
            if (d_prev - d <= 1) return d;
        }
    }

    return std::nullopt;
}

/**
 * Calculate output amount for N-token pool swap.
 *
 * @param balances Current token balances
 * @param n_tokens Number of tokens
 * @param from_idx Index of input token
 * @param to_idx Index of output token
 * @param amount_in Amount to swap
 * @param amp Amplification coefficient
 * @return New balance of output token, or nullopt if failed
 */
inline std::optional<uint64_t> calc_y_n(
    const uint64_t* balances, uint8_t n_tokens,
    uint8_t from_idx, uint8_t to_idx,
    uint64_t amount_in, uint64_t amp
) {
    // Create new balances array with input added
    uint64_t new_balances[MAX_TOKENS];
    for (uint8_t i = 0; i < n_tokens; i++) {
        new_balances[i] = balances[i];
    }
    new_balances[from_idx] += amount_in;

    // Calculate D with original balances
    auto d = calc_d_n(balances, n_tokens, amp);
    if (!d) return std::nullopt;

    // Calculate n^n
    __uint128_t nn = 1;
    for (uint8_t i = 0; i < n_tokens; i++) {
        nn *= n_tokens;
    }
    uint64_t ann = amp * static_cast<uint64_t>(nn);

    // Calculate S' and P' (excluding output token)
    uint64_t s_prime = 0;
    __uint128_t c = *d;

    for (uint8_t i = 0; i < n_tokens; i++) {
        if (i == to_idx) continue;

        uint64_t x = (i == from_idx) ? new_balances[i] : balances[i];
        s_prime += x;

        if (x == 0) return std::nullopt;
        c = c * (*d) / (n_tokens * x);
    }

    c = c * (*d) / (ann * n_tokens);
    uint64_t b = s_prime + (*d) / ann;

    // Newton iteration to find y
    uint64_t y = *d;

    for (int iter = 0; iter < NEWTON_ITERATIONS; iter++) {
        uint64_t y_prev = y;

        __uint128_t num = mul128(y, y) + static_cast<uint64_t>(c);
        uint64_t denom = 2 * y + b - (*d);

        if (denom == 0) return std::nullopt;

        y = static_cast<uint64_t>(num / denom);

        if (y > y_prev) {
            if (y - y_prev <= 1) return y;
        } else {
            if (y_prev - y <= 1) return y;
        }
    }

    return std::nullopt;
}

/**
 * Simulate N-token pool swap.
 */
inline std::optional<uint64_t> simulate_swap_n(
    const uint64_t* balances, uint8_t n_tokens,
    uint8_t from_idx, uint8_t to_idx,
    uint64_t amount_in, uint64_t amp, uint64_t fee_bps
) {
    auto new_y = calc_y_n(balances, n_tokens, from_idx, to_idx, amount_in, amp);
    if (!new_y) return std::nullopt;

    if (*new_y >= balances[to_idx]) return 0;
    uint64_t amount_out = balances[to_idx] - *new_y;

    // Apply fee
    uint64_t fee = amount_out * fee_bps / FEE_DENOMINATOR;
    amount_out -= fee;

    return amount_out;
}

// ============================================================================
// Farming Math
// ============================================================================

/**
 * Calculate pending farming rewards.
 *
 * Reward calculation:
 * pending = (staked * acc_reward / 1e12) - reward_debt
 *
 * @param staked User's staked LP amount
 * @param acc_reward Farm's accumulated reward per share (scaled 1e12)
 * @param reward_debt User's reward debt
 * @return Pending rewards
 */
inline uint64_t calc_pending_reward(
    uint64_t staked, uint64_t acc_reward, uint64_t reward_debt
) {
    __uint128_t earned = mul128(staked, acc_reward) / PRECISION;
    if (earned <= reward_debt) return 0;
    return static_cast<uint64_t>(earned - reward_debt);
}

/**
 * Calculate new accumulated reward per share.
 *
 * @param current_acc Current accumulated reward
 * @param reward Amount of new rewards
 * @param total_staked Total staked LP tokens
 * @return New accumulated reward per share
 */
inline uint64_t calc_new_acc_reward(
    uint64_t current_acc, uint64_t reward, uint64_t total_staked
) {
    if (total_staked == 0) return current_acc;
    __uint128_t increase = mul128(reward, PRECISION) / total_staked;
    return current_acc + static_cast<uint64_t>(increase);
}

// ============================================================================
// Check Functions
// ============================================================================

/**
 * Check if pool balances are within acceptable imbalance ratio.
 * Returns false if one token is more than maxRatio times the other.
 */
inline bool check_imbalance(uint64_t bal0, uint64_t bal1, uint64_t max_ratio = 10) {
    if (bal0 == 0 || bal1 == 0) return false;

    if (bal0 > bal1) {
        return bal0 <= bal1 * max_ratio;
    } else {
        return bal1 <= bal0 * max_ratio;
    }
}

/**
 * Check if amount meets minimum requirement.
 */
inline bool check_min_amount(uint64_t amount, uint64_t min = MIN_SWAP) {
    return amount >= min;
}

/**
 * Check if amp is within valid range.
 */
inline bool check_amp(uint64_t amp) {
    return amp >= MIN_AMP && amp <= MAX_AMP;
}

}  // namespace math
}  // namespace aex402
