#pragma once
/**
 * AeX402 AMM C++ SDK - Account Parsing
 *
 * Functions for parsing raw account data into typed structures.
 * All parsing functions validate discriminators and return std::optional.
 */

#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
#include "types.hpp"
#include "constants.hpp"

namespace aex402 {

// ============================================================================
// Buffer Reading Helpers
// ============================================================================

namespace detail {

inline uint8_t read_u8(const uint8_t* data, size_t& offset) {
    return data[offset++];
}

inline uint16_t read_u16(const uint8_t* data, size_t& offset) {
    uint16_t v;
    std::memcpy(&v, data + offset, sizeof(v));
    offset += 2;
    return v;
}

inline int16_t read_i16(const uint8_t* data, size_t& offset) {
    int16_t v;
    std::memcpy(&v, data + offset, sizeof(v));
    offset += 2;
    return v;
}

inline uint32_t read_u32(const uint8_t* data, size_t& offset) {
    uint32_t v;
    std::memcpy(&v, data + offset, sizeof(v));
    offset += 4;
    return v;
}

inline uint64_t read_u64(const uint8_t* data, size_t& offset) {
    uint64_t v;
    std::memcpy(&v, data + offset, sizeof(v));
    offset += 8;
    return v;
}

inline int64_t read_i64(const uint8_t* data, size_t& offset) {
    int64_t v;
    std::memcpy(&v, data + offset, sizeof(v));
    offset += 8;
    return v;
}

inline Pubkey read_pubkey(const uint8_t* data, size_t& offset) {
    Pubkey pk;
    std::memcpy(pk.data(), data + offset, 32);
    offset += 32;
    return pk;
}

inline Candle read_candle(const uint8_t* data, size_t& offset) {
    Candle c;
    c.open = read_u32(data, offset);
    c.high_d = read_u16(data, offset);
    c.low_d = read_u16(data, offset);
    c.close_d = read_i16(data, offset);
    c.volume = read_u16(data, offset);
    return c;
}

}  // namespace detail

// ============================================================================
// Pool Parsing
// ============================================================================

/**
 * Parse a 2-token Pool from raw account data.
 * Returns std::nullopt if data is invalid or discriminator doesn't match.
 */
inline std::optional<Pool> parse_pool(const uint8_t* data, size_t len) {
    // Direct memory copy approach for packed struct
    if (len < sizeof(Pool)) return std::nullopt;

    Pool pool;
    std::memcpy(&pool, data, sizeof(Pool));

    if (!pool.is_valid()) return std::nullopt;

    return pool;
}

/**
 * Parse a 2-token Pool with field-by-field reading.
 * More portable but slower than direct memory mapping.
 */
inline std::optional<Pool> parse_pool_safe(const uint8_t* data, size_t len) {
    if (len < 900) return std::nullopt;

    size_t offset = 0;

    // Check discriminator
    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::POOL) return std::nullopt;

    Pool pool;
    std::memcpy(pool.disc, &disc, 8);

    // Pubkeys
    pool.authority = detail::read_pubkey(data, offset);
    pool.mint0 = detail::read_pubkey(data, offset);
    pool.mint1 = detail::read_pubkey(data, offset);
    pool.vault0 = detail::read_pubkey(data, offset);
    pool.vault1 = detail::read_pubkey(data, offset);
    pool.lp_mint = detail::read_pubkey(data, offset);

    // Amp fields
    pool.amp = detail::read_u64(data, offset);
    pool.init_amp = detail::read_u64(data, offset);
    pool.target_amp = detail::read_u64(data, offset);
    pool.ramp_start = detail::read_i64(data, offset);
    pool.ramp_stop = detail::read_i64(data, offset);

    // Fee fields
    pool.fee_bps = detail::read_u64(data, offset);
    pool.admin_fee_pct = detail::read_u64(data, offset);

    // Balance fields
    pool.bal0 = detail::read_u64(data, offset);
    pool.bal1 = detail::read_u64(data, offset);
    pool.lp_supply = detail::read_u64(data, offset);
    pool.admin_fee0 = detail::read_u64(data, offset);
    pool.admin_fee1 = detail::read_u64(data, offset);

    // Volume fields
    pool.vol0 = detail::read_u64(data, offset);
    pool.vol1 = detail::read_u64(data, offset);

    // Flags
    pool.paused = detail::read_u8(data, offset);
    pool.bump = detail::read_u8(data, offset);
    pool.v0_bump = detail::read_u8(data, offset);
    pool.v1_bump = detail::read_u8(data, offset);
    pool.lp_bump = detail::read_u8(data, offset);
    offset += 3;  // _pad

    // Pending authority
    pool.pending_auth = detail::read_pubkey(data, offset);
    pool.auth_time = detail::read_i64(data, offset);

    // Pending amp
    pool.pending_amp = detail::read_u64(data, offset);
    pool.amp_time = detail::read_i64(data, offset);

    // Analytics
    pool.trade_count = detail::read_u64(data, offset);
    pool.trade_sum = detail::read_u64(data, offset);
    pool.max_price = detail::read_u32(data, offset);
    pool.min_price = detail::read_u32(data, offset);
    pool.hour_slot = detail::read_u32(data, offset);
    pool.day_slot = detail::read_u32(data, offset);
    pool.hour_idx = detail::read_u8(data, offset);
    pool.day_idx = detail::read_u8(data, offset);
    offset += 6;  // _pad2

    // Bloom filter
    std::memcpy(pool.bloom, data + offset, BLOOM_SIZE);
    offset += BLOOM_SIZE;

    // Hourly candles
    for (int i = 0; i < OHLCV_24H; i++) {
        pool.hours[i] = detail::read_candle(data, offset);
    }

    // Daily candles
    for (int i = 0; i < OHLCV_7D; i++) {
        pool.days[i] = detail::read_candle(data, offset);
    }

    return pool;
}

// ============================================================================
// NPool Parsing
// ============================================================================

/**
 * Parse an N-token Pool from raw account data.
 */
inline std::optional<NPool> parse_npool(const uint8_t* data, size_t len) {
    if (len < sizeof(NPool)) return std::nullopt;

    NPool pool;
    std::memcpy(&pool, data, sizeof(NPool));

    if (!pool.is_valid()) return std::nullopt;

    return pool;
}

/**
 * Parse an N-token Pool with field-by-field reading.
 */
inline std::optional<NPool> parse_npool_safe(const uint8_t* data, size_t len) {
    if (len < 800) return std::nullopt;

    size_t offset = 0;

    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::NPOOL) return std::nullopt;

    NPool pool;
    std::memcpy(pool.disc, &disc, 8);

    pool.authority = detail::read_pubkey(data, offset);
    pool.n_tokens = detail::read_u8(data, offset);
    pool.paused = detail::read_u8(data, offset);
    pool.bump = detail::read_u8(data, offset);
    offset += 5;  // _pad

    pool.amp = detail::read_u64(data, offset);
    pool.fee_bps = detail::read_u64(data, offset);
    pool.admin_fee_pct = detail::read_u64(data, offset);
    pool.lp_supply = detail::read_u64(data, offset);

    for (int i = 0; i < MAX_TOKENS; i++) {
        pool.mints[i] = detail::read_pubkey(data, offset);
    }

    for (int i = 0; i < MAX_TOKENS; i++) {
        pool.vaults[i] = detail::read_pubkey(data, offset);
    }

    pool.lp_mint = detail::read_pubkey(data, offset);

    for (int i = 0; i < MAX_TOKENS; i++) {
        pool.balances[i] = detail::read_u64(data, offset);
    }

    for (int i = 0; i < MAX_TOKENS; i++) {
        pool.admin_fees[i] = detail::read_u64(data, offset);
    }

    pool.total_volume = detail::read_u64(data, offset);
    pool.trade_count = detail::read_u64(data, offset);
    pool.last_trade_slot = detail::read_u64(data, offset);

    return pool;
}

// ============================================================================
// Farm Parsing
// ============================================================================

/**
 * Parse a Farm from raw account data.
 */
inline std::optional<Farm> parse_farm(const uint8_t* data, size_t len) {
    if (len < sizeof(Farm)) return std::nullopt;

    Farm farm;
    std::memcpy(&farm, data, sizeof(Farm));

    if (!farm.is_valid()) return std::nullopt;

    return farm;
}

/**
 * Parse a Farm with field-by-field reading.
 */
inline std::optional<Farm> parse_farm_safe(const uint8_t* data, size_t len) {
    if (len < 120) return std::nullopt;

    size_t offset = 0;

    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::FARM) return std::nullopt;

    Farm farm;
    std::memcpy(farm.disc, &disc, 8);

    farm.pool = detail::read_pubkey(data, offset);
    farm.reward_mint = detail::read_pubkey(data, offset);
    farm.reward_rate = detail::read_u64(data, offset);
    farm.start_time = detail::read_i64(data, offset);
    farm.end_time = detail::read_i64(data, offset);
    farm.total_staked = detail::read_u64(data, offset);
    farm.acc_reward = detail::read_u64(data, offset);
    farm.last_update = detail::read_i64(data, offset);

    return farm;
}

// ============================================================================
// UserFarm Parsing
// ============================================================================

/**
 * Parse a UserFarm from raw account data.
 */
inline std::optional<UserFarm> parse_user_farm(const uint8_t* data, size_t len) {
    if (len < sizeof(UserFarm)) return std::nullopt;

    UserFarm uf;
    std::memcpy(&uf, data, sizeof(UserFarm));

    if (!uf.is_valid()) return std::nullopt;

    return uf;
}

/**
 * Parse a UserFarm with field-by-field reading.
 */
inline std::optional<UserFarm> parse_user_farm_safe(const uint8_t* data, size_t len) {
    if (len < 96) return std::nullopt;

    size_t offset = 0;

    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::UFARM) return std::nullopt;

    UserFarm uf;
    std::memcpy(uf.disc, &disc, 8);

    uf.owner = detail::read_pubkey(data, offset);
    uf.farm = detail::read_pubkey(data, offset);
    uf.staked = detail::read_u64(data, offset);
    uf.reward_debt = detail::read_u64(data, offset);
    uf.lock_end = detail::read_i64(data, offset);

    return uf;
}

// ============================================================================
// Lottery Parsing
// ============================================================================

/**
 * Parse a Lottery from raw account data.
 */
inline std::optional<Lottery> parse_lottery(const uint8_t* data, size_t len) {
    if (len < sizeof(Lottery)) return std::nullopt;

    Lottery lot;
    std::memcpy(&lot, data, sizeof(Lottery));

    if (!lot.is_valid()) return std::nullopt;

    return lot;
}

/**
 * Parse a Lottery with field-by-field reading.
 */
inline std::optional<Lottery> parse_lottery_safe(const uint8_t* data, size_t len) {
    if (len < 152) return std::nullopt;

    size_t offset = 0;

    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::LOTTERY) return std::nullopt;

    Lottery lot;
    std::memcpy(lot.disc, &disc, 8);

    lot.pool = detail::read_pubkey(data, offset);
    lot.authority = detail::read_pubkey(data, offset);
    lot.lottery_vault = detail::read_pubkey(data, offset);
    lot.ticket_price = detail::read_u64(data, offset);
    lot.total_tickets = detail::read_u64(data, offset);
    lot.prize_pool = detail::read_u64(data, offset);
    lot.end_time = detail::read_i64(data, offset);
    lot.winning_ticket = detail::read_u64(data, offset);
    lot.drawn = detail::read_u8(data, offset);
    lot.claimed = detail::read_u8(data, offset);

    return lot;
}

// ============================================================================
// LotteryEntry Parsing
// ============================================================================

/**
 * Parse a LotteryEntry from raw account data.
 */
inline std::optional<LotteryEntry> parse_lottery_entry(const uint8_t* data, size_t len) {
    if (len < sizeof(LotteryEntry)) return std::nullopt;

    LotteryEntry entry;
    std::memcpy(&entry, data, sizeof(LotteryEntry));

    if (!entry.is_valid()) return std::nullopt;

    return entry;
}

/**
 * Parse a LotteryEntry with field-by-field reading.
 */
inline std::optional<LotteryEntry> parse_lottery_entry_safe(const uint8_t* data, size_t len) {
    if (len < 88) return std::nullopt;

    size_t offset = 0;

    uint64_t disc = detail::read_u64(data, offset);
    if (disc != account_disc::LOTENTRY) return std::nullopt;

    LotteryEntry entry;
    std::memcpy(entry.disc, &disc, 8);

    entry.owner = detail::read_pubkey(data, offset);
    entry.lottery = detail::read_pubkey(data, offset);
    entry.ticket_start = detail::read_u64(data, offset);
    entry.ticket_count = detail::read_u64(data, offset);

    return entry;
}

// ============================================================================
// Registry Parsing
// ============================================================================

/**
 * Parse a Registry header from raw account data.
 * Note: Pools array must be parsed separately due to variable length.
 */
inline std::optional<Registry> parse_registry(const uint8_t* data, size_t len) {
    if (len < sizeof(Registry)) return std::nullopt;

    Registry reg;
    std::memcpy(&reg, data, sizeof(Registry));

    if (!reg.is_valid()) return std::nullopt;

    return reg;
}

/**
 * Parse registry pools array.
 * Returns vector of registered pool pubkeys.
 */
inline std::vector<Pubkey> parse_registry_pools(const uint8_t* data, size_t len, uint32_t count) {
    std::vector<Pubkey> pools;

    size_t offset = sizeof(Registry);
    for (uint32_t i = 0; i < count && offset + 32 <= len; i++) {
        pools.push_back(detail::read_pubkey(data, offset));
    }

    return pools;
}

// ============================================================================
// Governance Parsing
// ============================================================================

/**
 * Parse a GovProposal from raw account data.
 */
inline std::optional<GovProposal> parse_gov_proposal(const uint8_t* data, size_t len) {
    if (len < sizeof(GovProposal)) return std::nullopt;

    GovProposal prop;
    std::memcpy(&prop, data, sizeof(GovProposal));

    if (!prop.is_valid()) return std::nullopt;

    return prop;
}

/**
 * Parse a GovVote from raw account data.
 */
inline std::optional<GovVote> parse_gov_vote(const uint8_t* data, size_t len) {
    if (len < sizeof(GovVote)) return std::nullopt;

    GovVote vote;
    std::memcpy(&vote, data, sizeof(GovVote));

    if (!vote.is_valid()) return std::nullopt;

    return vote;
}

// ============================================================================
// Concentrated Liquidity Parsing
// ============================================================================

/**
 * Parse a CLPool from raw account data.
 */
inline std::optional<CLPool> parse_cl_pool(const uint8_t* data, size_t len) {
    if (len < sizeof(CLPool)) return std::nullopt;

    CLPool pool;
    std::memcpy(&pool, data, sizeof(CLPool));

    if (!pool.is_valid()) return std::nullopt;

    return pool;
}

/**
 * Parse a CLPosition from raw account data.
 */
inline std::optional<CLPosition> parse_cl_position(const uint8_t* data, size_t len) {
    if (len < sizeof(CLPosition)) return std::nullopt;

    CLPosition pos;
    std::memcpy(&pos, data, sizeof(CLPosition));

    if (!pos.is_valid()) return std::nullopt;

    return pos;
}

// ============================================================================
// ML Brain Parsing
// ============================================================================

/**
 * Parse an MLBrain header from raw account data.
 * Note: Observation buffer must be parsed separately.
 */
inline std::optional<MLBrain> parse_ml_brain(const uint8_t* data, size_t len) {
    if (len < sizeof(MLBrain)) return std::nullopt;

    MLBrain brain;
    std::memcpy(&brain, data, sizeof(MLBrain));

    if (!brain.is_valid()) return std::nullopt;

    return brain;
}

// ============================================================================
// Orderbook Parsing
// ============================================================================

/**
 * Parse an Orderbook from raw account data.
 */
inline std::optional<Orderbook> parse_orderbook(const uint8_t* data, size_t len) {
    if (len < sizeof(Orderbook)) return std::nullopt;

    Orderbook book;
    std::memcpy(&book, data, sizeof(Orderbook));

    if (!book.is_valid()) return std::nullopt;

    return book;
}

// ============================================================================
// Generic Account Type Detection
// ============================================================================

/**
 * Account type enumeration for generic parsing.
 */
enum class AccountType {
    Unknown,
    Pool,
    NPool,
    Farm,
    UserFarm,
    Lottery,
    LotteryEntry,
    Registry,
    MLBrain,
    CLPool,
    CLPosition,
    Orderbook,
    GovProposal,
    GovVote,
};

/**
 * Detect account type from raw data.
 */
inline AccountType detect_account_type(const uint8_t* data, size_t len) {
    if (len < 8) return AccountType::Unknown;

    uint64_t disc;
    std::memcpy(&disc, data, 8);

    switch (disc) {
        case account_disc::POOL:     return AccountType::Pool;
        case account_disc::NPOOL:    return AccountType::NPool;
        case account_disc::FARM:     return AccountType::Farm;
        case account_disc::UFARM:    return AccountType::UserFarm;
        case account_disc::LOTTERY:  return AccountType::Lottery;
        case account_disc::LOTENTRY: return AccountType::LotteryEntry;
        case account_disc::REGISTRY: return AccountType::Registry;
        case account_disc::MLBRAIN:  return AccountType::MLBrain;
        case account_disc::CLPOOL:   return AccountType::CLPool;
        case account_disc::CLPOS:    return AccountType::CLPosition;
        case account_disc::BOOK:     return AccountType::Orderbook;
        case account_disc::GOVPROP:  return AccountType::GovProposal;
        case account_disc::GOVVOTE:  return AccountType::GovVote;
        default:                     return AccountType::Unknown;
    }
}

/**
 * Get account type name as string.
 */
inline const char* account_type_name(AccountType type) {
    switch (type) {
        case AccountType::Pool:         return "Pool";
        case AccountType::NPool:        return "NPool";
        case AccountType::Farm:         return "Farm";
        case AccountType::UserFarm:     return "UserFarm";
        case AccountType::Lottery:      return "Lottery";
        case AccountType::LotteryEntry: return "LotteryEntry";
        case AccountType::Registry:     return "Registry";
        case AccountType::MLBrain:      return "MLBrain";
        case AccountType::CLPool:       return "CLPool";
        case AccountType::CLPosition:   return "CLPosition";
        case AccountType::Orderbook:    return "Orderbook";
        case AccountType::GovProposal:  return "GovProposal";
        case AccountType::GovVote:      return "GovVote";
        default:                        return "Unknown";
    }
}

}  // namespace aex402
