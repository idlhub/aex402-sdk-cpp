#pragma once
/**
 * AeX402 AMM C++ SDK - Type Definitions
 *
 * Account structures matching the on-chain C program.
 * All structs use packed layout for direct memory mapping.
 */

#include <cstdint>
#include <array>
#include <vector>
#include <cstring>
#include "constants.hpp"

namespace aex402 {

// ============================================================================
// Basic Types
// ============================================================================

using Pubkey = std::array<uint8_t, 32>;

/**
 * 128-bit unsigned integer for high-precision calculations.
 * Uses compiler intrinsic __uint128_t when available.
 */
struct U128 {
    uint64_t lo;
    uint64_t hi;

    U128() : lo(0), hi(0) {}
    U128(uint64_t v) : lo(v), hi(0) {}
    U128(uint64_t low, uint64_t high) : lo(low), hi(high) {}

    // Convert from __uint128_t
    U128(__uint128_t v) : lo(static_cast<uint64_t>(v)), hi(static_cast<uint64_t>(v >> 64)) {}

    // Convert to __uint128_t
    __uint128_t to_u128() const {
        return (static_cast<__uint128_t>(hi) << 64) | lo;
    }

    bool operator==(const U128& other) const { return lo == other.lo && hi == other.hi; }
    bool operator!=(const U128& other) const { return !(*this == other); }
    bool is_zero() const { return lo == 0 && hi == 0; }
};

// ============================================================================
// Delta-encoded OHLCV Candle (12 bytes)
// ============================================================================

#pragma pack(push, 1)
struct Candle {
    uint32_t open;      // Base price (scaled 1e6)
    uint16_t high_d;    // High delta: high = open + high_d
    uint16_t low_d;     // Low delta: low = open - low_d
    int16_t  close_d;   // Close delta signed: close = open + close_d
    uint16_t volume;    // Volume in 1e9 units

    // Computed properties
    uint32_t high() const { return open + high_d; }
    uint32_t low() const { return open > low_d ? open - low_d : 0; }
    int32_t close() const { return static_cast<int32_t>(open) + close_d; }

    // Convert volume to actual value (multiply by 1e9)
    uint64_t actual_volume() const { return static_cast<uint64_t>(volume) * 1000000000ULL; }
};
#pragma pack(pop)
static_assert(sizeof(Candle) == 12, "Candle must be 12 bytes");

// ============================================================================
// Decoded Candle (for convenient access)
// ============================================================================

struct CandleDecoded {
    uint32_t open;
    uint32_t high;
    uint32_t low;
    int32_t  close;
    uint64_t volume;

    double open_f64() const { return open / 1e6; }
    double high_f64() const { return high / 1e6; }
    double low_f64() const { return low / 1e6; }
    double close_f64() const { return close / 1e6; }
};

inline CandleDecoded decode_candle(const Candle& c) {
    return {
        .open = c.open,
        .high = c.high(),
        .low = c.low(),
        .close = c.close(),
        .volume = c.actual_volume()
    };
}

// ============================================================================
// Pool (2-token) - 1024 bytes
// Matches C struct in aex402.c
// ============================================================================

#pragma pack(push, 1)
struct Pool {
    uint8_t  disc[8];           // "POOLSWAP"
    Pubkey   authority;         // 32 bytes
    Pubkey   mint0;             // 32 bytes (t0_mint)
    Pubkey   mint1;             // 32 bytes (t1_mint)
    Pubkey   vault0;            // 32 bytes (t0_vault)
    Pubkey   vault1;            // 32 bytes (t1_vault)
    Pubkey   lp_mint;           // 32 bytes
    uint64_t amp;               // Current amplification
    uint64_t init_amp;          // Initial amp for ramping
    uint64_t target_amp;        // Target amp (tgt_amp)
    int64_t  ramp_start;        // Ramp start timestamp
    int64_t  ramp_stop;         // Ramp stop timestamp
    uint64_t fee_bps;           // Swap fee in basis points
    uint64_t admin_fee_pct;     // Admin fee percentage (admin_pct)
    uint64_t bal0;              // Token 0 balance
    uint64_t bal1;              // Token 1 balance
    uint64_t lp_supply;         // Total LP token supply
    uint64_t admin_fee0;        // Accumulated admin fees token 0
    uint64_t admin_fee1;        // Accumulated admin fees token 1
    uint64_t vol0;              // Volume token 0
    uint64_t vol1;              // Volume token 1
    uint8_t  paused;            // Pool paused flag
    uint8_t  bump;              // PDA bump
    uint8_t  v0_bump;           // Vault 0 bump
    uint8_t  v1_bump;           // Vault 1 bump
    uint8_t  lp_bump;           // LP mint bump
    uint8_t  _pad[3];           // Alignment padding
    Pubkey   pending_auth;      // Pending authority for transfer
    int64_t  auth_time;         // Authority transfer timestamp
    uint64_t pending_amp;       // Pending amp change
    int64_t  amp_time;          // Amp commit timestamp
    // Analytics section
    uint64_t trade_count;       // Total swap count
    uint64_t trade_sum;         // Sum of trade sizes
    uint32_t max_price;         // All-time max price (scaled 1e6)
    uint32_t min_price;         // All-time min price (scaled 1e6)
    uint32_t hour_slot;         // Last updated hour (slot / SLOTS_PER_HOUR)
    uint32_t day_slot;          // Last updated day (slot / SLOTS_PER_DAY)
    uint8_t  hour_idx;          // Current hour index 0-23
    uint8_t  day_idx;           // Current day index 0-6
    uint8_t  _pad2[6];          // Alignment
    uint8_t  bloom[BLOOM_SIZE]; // Bloom filter for unique traders (128 bytes)
    Candle   hours[OHLCV_24H];  // 24 hourly candles (288 bytes)
    Candle   days[OHLCV_7D];    // 7 daily candles (84 bytes)

    // Validation
    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::POOL;
    }

    bool is_paused() const { return paused != 0; }

    // Get current effective amp (handles ramping)
    uint64_t get_amp(int64_t now) const {
        if (now >= ramp_stop || ramp_stop == ramp_start) return target_amp;
        if (now <= ramp_start) return amp;

        int64_t elapsed = now - ramp_start;
        int64_t duration = ramp_stop - ramp_start;

        if (target_amp > amp) {
            return amp + (target_amp - amp) * static_cast<uint64_t>(elapsed) / static_cast<uint64_t>(duration);
        } else {
            return amp - (amp - target_amp) * static_cast<uint64_t>(elapsed) / static_cast<uint64_t>(duration);
        }
    }

    // Get discriminator as u64
    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// NPool (N-token, 2-8 tokens) - 2048 bytes
// ============================================================================

#pragma pack(push, 1)
struct NPool {
    uint8_t  disc[8];               // "NPOOLSWA"
    Pubkey   authority;             // 32 bytes
    uint8_t  n_tokens;              // Number of tokens (2-8)
    uint8_t  paused;                // Paused flag
    uint8_t  bump;                  // PDA bump
    uint8_t  _pad[5];               // Alignment
    uint64_t amp;                   // Amplification
    uint64_t fee_bps;               // Fee in basis points
    uint64_t admin_fee_pct;         // Admin fee percentage
    uint64_t lp_supply;             // LP token supply
    Pubkey   mints[MAX_TOKENS];     // Token mints (256 bytes)
    Pubkey   vaults[MAX_TOKENS];    // Token vaults (256 bytes)
    Pubkey   lp_mint;               // LP mint
    uint64_t balances[MAX_TOKENS];  // Token balances (64 bytes)
    uint64_t admin_fees[MAX_TOKENS]; // Admin fees per token (64 bytes)
    uint64_t total_volume;          // Cumulative volume
    uint64_t trade_count;           // Total number of swaps
    uint64_t last_trade_slot;       // Slot of last trade

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::NPOOL;
    }

    bool is_paused() const { return paused != 0; }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// Farm - Farming state for LP staking rewards
// ============================================================================

#pragma pack(push, 1)
struct Farm {
    uint8_t  disc[8];           // "FARMSWAP"
    Pubkey   pool;              // Associated pool
    Pubkey   reward_mint;       // Reward token mint
    uint64_t reward_rate;       // Rewards per second
    int64_t  start_time;        // Farming start timestamp
    int64_t  end_time;          // Farming end timestamp
    uint64_t total_staked;      // Total LP tokens staked
    uint64_t acc_reward;        // Accumulated reward per share (scaled 1e12)
    int64_t  last_update;       // Last update timestamp

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::FARM;
    }

    bool is_active(int64_t now) const {
        return now >= start_time && now <= end_time;
    }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// UserFarm - User's farming position
// ============================================================================

#pragma pack(push, 1)
struct UserFarm {
    uint8_t  disc[8];           // "UFARMSWA"
    Pubkey   owner;             // User wallet
    Pubkey   farm;              // Farm account
    uint64_t staked;            // Amount of LP tokens staked
    uint64_t reward_debt;       // Reward debt for pending calculation
    int64_t  lock_end;          // Lock expiration timestamp

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::UFARM;
    }

    bool is_locked(int64_t now) const {
        return now < lock_end;
    }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// Lottery - Lottery state for LP token prize pool
// ============================================================================

#pragma pack(push, 1)
struct Lottery {
    uint8_t  disc[8];           // "LOTTERY!"
    Pubkey   pool;              // Associated pool
    Pubkey   authority;         // Lottery authority
    Pubkey   lottery_vault;     // Vault holding LP tokens for prizes
    uint64_t ticket_price;      // LP tokens required per ticket
    uint64_t total_tickets;     // Total tickets sold
    uint64_t prize_pool;        // Total prize in LP tokens
    int64_t  end_time;          // Lottery end timestamp
    uint64_t winning_ticket;    // Set after draw
    uint8_t  drawn;             // 0 = not drawn, 1 = drawn
    uint8_t  claimed;           // 0 = not claimed, 1 = claimed
    uint8_t  _pad[6];           // Alignment

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::LOTTERY;
    }

    bool is_drawn() const { return drawn != 0; }
    bool is_claimed() const { return claimed != 0; }
    bool is_ended(int64_t now) const { return now >= end_time; }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// LotteryEntry - User's lottery entry
// ============================================================================

#pragma pack(push, 1)
struct LotteryEntry {
    uint8_t  disc[8];           // "LOTENTRY"
    Pubkey   owner;             // Entry owner
    Pubkey   lottery;           // Lottery account
    uint64_t ticket_start;      // First ticket number
    uint64_t ticket_count;      // Number of tickets

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::LOTENTRY;
    }

    bool is_winner(uint64_t winning_ticket) const {
        return winning_ticket >= ticket_start &&
               winning_ticket < ticket_start + ticket_count;
    }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// Registry - Pool registry for enumeration
// ============================================================================

#pragma pack(push, 1)
struct Registry {
    uint8_t  disc[8];           // "REGISTRY"
    Pubkey   authority;         // Registry authority
    Pubkey   pending_auth;      // Pending authority for transfer
    int64_t  auth_time;         // Authority transfer timestamp
    uint32_t count;             // Number of registered pools
    uint8_t  _pad[4];           // Alignment
    // Pools array follows (variable length)

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::REGISTRY;
    }

    uint64_t discriminator() const {
        return *reinterpret_cast<const uint64_t*>(disc);
    }
};
#pragma pack(pop)

// ============================================================================
// Governance Proposal
// ============================================================================

#pragma pack(push, 1)
struct GovProposal {
    uint8_t  disc[8];           // "GOVPROP!"
    Pubkey   pool;              // Pool this proposal is for
    Pubkey   proposer;          // Who created the proposal
    uint8_t  prop_type;         // ProposalType
    uint8_t  status;            // ProposalStatus
    uint8_t  _pad[6];           // Alignment
    uint64_t value;             // New value (fee_bps, amp, etc.)
    uint64_t votes_for;         // LP tokens voted for
    uint64_t votes_against;     // LP tokens voted against
    uint64_t lp_snapshot;       // Total LP supply at proposal creation
    int64_t  start_slot;        // When voting started
    int64_t  end_slot;          // When voting ends
    int64_t  exec_after;        // Earliest execution slot (after timelock)
    uint8_t  description[64];   // Short description

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::GOVPROP;
    }

    bool can_execute(int64_t now_slot) const {
        return status == static_cast<uint8_t>(ProposalStatus::Passed) &&
               now_slot >= exec_after;
    }

    double approval_rate() const {
        uint64_t total = votes_for + votes_against;
        if (total == 0) return 0.0;
        return static_cast<double>(votes_for) / static_cast<double>(total);
    }

    double quorum_rate() const {
        if (lp_snapshot == 0) return 0.0;
        return static_cast<double>(votes_for + votes_against) / static_cast<double>(lp_snapshot);
    }
};
#pragma pack(pop)

// ============================================================================
// Governance Vote Record
// ============================================================================

#pragma pack(push, 1)
struct GovVote {
    uint8_t  disc[8];           // "GOVVOTE!"
    Pubkey   proposal;          // Which proposal
    Pubkey   voter;             // Who voted
    uint64_t amount;            // LP tokens used to vote
    uint8_t  vote_for;          // 1 = for, 0 = against
    uint8_t  _pad[7];           // Alignment

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::GOVVOTE;
    }

    bool voted_for() const { return vote_for != 0; }
};
#pragma pack(pop)

// ============================================================================
// Concentrated Liquidity Pool
// ============================================================================

#pragma pack(push, 1)
struct CLPool {
    uint8_t  disc[8];           // "CLPOOL!!"
    Pubkey   pool;              // Associated base pool
    Pubkey   authority;         // Authority
    int16_t  tick_lower;        // Current active tick range lower
    int16_t  tick_upper;        // Current active tick range upper
    int16_t  current_tick;      // Current price tick
    uint8_t  initialized;       // Initialized flag
    uint8_t  _pad;              // Alignment
    uint64_t sqrt_price;        // sqrt(price) scaled 1e12
    uint64_t liquidity;         // Total active liquidity
    uint64_t fee_growth_0;      // Fee accumulator token0
    uint64_t fee_growth_1;      // Fee accumulator token1
    uint8_t  tick_bitmap[128];  // Bitmap for 1024 ticks
    uint8_t  reserved[256];     // Reserved for future use

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::CLPOOL;
    }

    bool is_initialized() const { return initialized != 0; }
};
#pragma pack(pop)

// ============================================================================
// Concentrated Liquidity Position
// ============================================================================

#pragma pack(push, 1)
struct CLPosition {
    uint8_t  disc[8];           // "CLPOSIT!"
    Pubkey   owner;             // Position owner
    Pubkey   cl_pool;           // CL pool
    int16_t  tick_lower;        // Lower tick
    int16_t  tick_upper;        // Upper tick
    uint8_t  _pad[4];           // Alignment
    uint64_t liquidity;         // Position liquidity
    uint64_t fee_inside_0;      // Fee snapshot token0
    uint64_t fee_inside_1;      // Fee snapshot token1
    uint64_t tokens_owed_0;     // Tokens owed 0
    uint64_t tokens_owed_1;     // Tokens owed 1
    int64_t  created_at;        // Creation timestamp (for JIT protection)

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::CLPOS;
    }

    // Check if position meets minimum duration for fee collection
    bool can_collect_fees(int64_t now) const {
        return (now - created_at) >= static_cast<int64_t>(CL_MIN_DURATION);
    }
};
#pragma pack(pop)

// ============================================================================
// Order (for orderbook)
// ============================================================================

#pragma pack(push, 1)
struct Order {
    Pubkey   owner;             // Order owner
    uint64_t price;             // Price scaled 1e6
    uint64_t amount;            // Token amount
    int64_t  expiry;            // Expiration timestamp
    uint8_t  order_type;        // OrderType (Buy/Sell)
    uint8_t  active;            // 0 = cancelled/filled, 1 = active
    uint8_t  _pad[6];           // Alignment

    bool is_active() const { return active != 0; }
    bool is_expired(int64_t now) const { return now >= expiry; }
    bool is_buy() const { return order_type == static_cast<uint8_t>(OrderType::Buy); }
    bool is_sell() const { return order_type == static_cast<uint8_t>(OrderType::Sell); }
};
#pragma pack(pop)

// ============================================================================
// Orderbook
// ============================================================================

constexpr size_t MAX_ORDERS = 64;

#pragma pack(push, 1)
struct Orderbook {
    uint8_t  disc[8];           // "ORDERBOK"
    Pubkey   pool;              // Associated pool
    Pubkey   authority;         // Authority
    uint8_t  order_count;       // Number of active orders
    uint8_t  _pad[7];           // Alignment
    Order    orders[MAX_ORDERS]; // Order array

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::BOOK;
    }
};
#pragma pack(pop)

// ============================================================================
// ML Observation
// ============================================================================

#pragma pack(push, 1)
struct MLObservation {
    uint32_t price;             // Price scaled 1e6
    uint32_t volume;            // Volume in 1e9 units
    uint32_t tvl;               // TVL in 1e9 units
    uint32_t slot;              // Solana slot number
    uint16_t fee_bps;           // Current fee setting
    uint16_t amp;               // Current amplification
    uint8_t  is_new;            // Bloom filter indicated new trader
    uint8_t  direction;         // Swap direction (0=t0->t1, 1=t1->t0)
    uint8_t  _pad[2];           // Alignment
};
#pragma pack(pop)
static_assert(sizeof(MLObservation) == 24, "MLObservation must be 24 bytes");

// ============================================================================
// ML Brain
// ============================================================================

constexpr size_t ML_STATES_COUNT = 27;
constexpr size_t ML_ACTIONS_COUNT = 9;
constexpr size_t ML_OBS_MAX = 200;

#pragma pack(push, 1)
struct MLBrain {
    uint8_t  disc[8];           // "MLBRAIN!"
    Pubkey   pool;              // Associated pool
    Pubkey   authority;         // Training authority
    uint8_t  enabled;           // Learning enabled flag
    uint8_t  auto_apply;        // Auto-apply best action
    uint8_t  last_action;       // Last action taken
    uint8_t  last_state;        // Last observed state
    uint8_t  is_stable;         // 1 = stable pool, 0 = volatile
    uint8_t  _pad_type[3];      // Alignment
    uint16_t obs_count;         // Total observations recorded
    uint16_t train_count;       // Training iterations completed
    uint32_t epoch;             // Current training epoch
    uint32_t last_train_slot;   // Last training slot
    uint32_t last_action_slot;  // Last action application slot
    uint16_t cur_alpha;         // Current learning rate (scaled 1000)
    uint16_t cur_epsilon;       // Current exploration rate (scaled 1000)
    // Parameter bounds
    uint16_t min_fee;
    uint16_t max_fee;
    uint16_t min_amp;
    uint16_t max_amp;
    uint16_t fee_step;
    uint16_t amp_step;
    uint16_t _pad1[2];
    uint64_t min_farm_rate;
    uint64_t max_farm_rate;
    uint64_t farm_step;
    uint64_t min_lot_price;
    uint64_t max_lot_price;
    uint64_t lot_step;
    // Q-Table: 27 states x 9 actions
    int32_t q_table[ML_STATES_COUNT][ML_ACTIONS_COUNT];
    // Observation buffer metadata
    uint16_t obs_head;
    uint16_t obs_tail;
    uint8_t  _pad2[4];
    // Observation buffer follows

    bool is_valid() const {
        return *reinterpret_cast<const uint64_t*>(disc) == account_disc::MLBRAIN;
    }

    bool is_enabled() const { return enabled != 0; }
    bool is_auto_apply() const { return auto_apply != 0; }
};
#pragma pack(pop)

// ============================================================================
// TWAP Result
// ============================================================================

struct TwapResult {
    uint32_t price;       // Scaled 1e6
    uint16_t samples;     // Number of candles used
    uint16_t confidence;  // 0-10000 (0-100%)

    double price_f64() const { return price / 1e6; }
    double confidence_pct() const { return confidence / 100.0; }

    static TwapResult decode(uint64_t encoded) {
        return {
            .price = static_cast<uint32_t>(encoded & 0xFFFFFFFF),
            .samples = static_cast<uint16_t>((encoded >> 32) & 0xFFFF),
            .confidence = static_cast<uint16_t>((encoded >> 48) & 0xFFFF),
        };
    }
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Compare two pubkeys for equality
 */
inline bool pubkey_eq(const Pubkey& a, const Pubkey& b) {
    return std::memcmp(a.data(), b.data(), 32) == 0;
}

/**
 * Create a zero pubkey
 */
inline Pubkey pubkey_zero() {
    Pubkey pk{};
    return pk;
}

/**
 * Check if pubkey is zero
 */
inline bool pubkey_is_zero(const Pubkey& pk) {
    for (auto b : pk) {
        if (b != 0) return false;
    }
    return true;
}

/**
 * Copy pubkey
 */
inline void pubkey_copy(Pubkey& dst, const Pubkey& src) {
    std::memcpy(dst.data(), src.data(), 32);
}

}  // namespace aex402
