#pragma once
/**
 * AeX402 AMM C++ SDK
 *
 * Pure C++17 SDK for interacting with the AeX402 AMM on Solana.
 * No external dependencies beyond standard library.
 *
 * Program ID: 3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje
 */

#include <cstdint>
#include <cstring>
#include <array>
#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

namespace aex402 {

// ============================================================================
// Constants
// ============================================================================

constexpr const char* PROGRAM_ID = "3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje";

constexpr uint64_t MIN_AMP = 1;
constexpr uint64_t MAX_AMP = 100000;
constexpr uint64_t DEFAULT_FEE_BPS = 30;
constexpr uint64_t ADMIN_FEE_PCT = 50;
constexpr uint64_t MIN_SWAP = 100000;
constexpr uint64_t MIN_DEPOSIT = 100000000;
constexpr uint8_t  NEWTON_ITERATIONS = 255;
constexpr int64_t  RAMP_MIN_DURATION = 86400;  // 1 day
constexpr int64_t  COMMIT_DELAY = 3600;        // 1 hour
constexpr uint64_t MIGRATION_FEE_BPS = 1337;   // 0.1337%
constexpr uint8_t  MAX_TOKENS = 8;
constexpr uint64_t POOL_SIZE = 1024;
constexpr uint64_t NPOOL_SIZE = 2048;
constexpr uint8_t  OHLCV_24H = 24;
constexpr uint8_t  OHLCV_7D = 7;

// ============================================================================
// Discriminators (little-endian u64)
// ============================================================================

namespace disc {
    constexpr uint64_t CREATEPOOL = 0xf2b9e4d1c8a7e3f9ULL;
    constexpr uint64_t CREATEPN   = 0x27c933bce5c77c1bULL;
    constexpr uint64_t INITT0V    = 0x5e8c3b0d0f3e4a9fULL;
    constexpr uint64_t INITT1V    = 0x7a4e9f1c3b2d5e8aULL;
    constexpr uint64_t INITLPM    = 0xf4d1e9a3c5b8e7f2ULL;
    constexpr uint64_t SWAP       = 0x82c69e91e17587c8ULL;
    constexpr uint64_t SWAPT0T1   = 0x642af2b7e0f14e2aULL;
    constexpr uint64_t SWAPT1T0   = 0x3a0e131bac75c4c8ULL;
    constexpr uint64_t SWAPN      = 0xf1a8e3c7b2d9e5f8ULL;
    constexpr uint64_t MIGT0T1    = 0xd2e4f1a8c3b7e9d5ULL;
    constexpr uint64_t MIGT1T0    = 0x1888779426393db8ULL;
    constexpr uint64_t ADDLIQ     = 0xa2e7c4f8b3d1e5a9ULL;
    constexpr uint64_t ADDLIQ1    = 0x51c98b4e3c2e12e6ULL;
    constexpr uint64_t ADDLIQN    = 0xe3f7a2c8d1b9e4f6ULL;
    constexpr uint64_t REMLIQ     = 0x2e54bc2c75c9f902ULL;
    constexpr uint64_t REMLIQN    = 0xb3f8e2a5c7d9e1b4ULL;
    constexpr uint64_t SETPAUSE   = 0xe075762b7e0d6ec9ULL;
    constexpr uint64_t UPDFEE     = 0x8f3a2e5b7c9d1f4aULL;
    constexpr uint64_t WDRAWFEE   = 0xf9e5d3a2c8b1e7f8ULL;
    constexpr uint64_t COMMITAMP  = 0xc1d9e3f7a5b8e2c4ULL;
    constexpr uint64_t RAMPAMP    = 0x9a1c5e3f7b2d8e6aULL;
    constexpr uint64_t STOPRAMP   = 0x3c9427bb15a21053ULL;
    constexpr uint64_t INITAUTH   = 0xf5e2a7c9d3b1e8f4ULL;
    constexpr uint64_t COMPLAUTH  = 0xf6e8d2a4c7b9e1f5ULL;
    constexpr uint64_t CANCELAUTH = 0xf7e3a9c1d5b2e8f6ULL;
    constexpr uint64_t LOCKLP     = 0xfefb83015f028cecULL;
    constexpr uint64_t CLAIMULP   = 0xca8593f45ce88b1eULL;
    constexpr uint64_t CREATEFARM = 0x6d7b0c8e2f1a3d5cULL;
    constexpr uint64_t STAKELP    = 0xf8d4e1a7c3b9e2f7ULL;
    constexpr uint64_t UNSTAKELP  = 0x4166bf654e34f8bcULL;
    constexpr uint64_t CLAIMFARM  = 0x075762b7e0d6ec9bULL;
    constexpr uint64_t ENTERLOT   = 0xe795383a4eef48fcULL;
    constexpr uint64_t DRAWLOT    = 0x1361225a4d7cbc11ULL;
    constexpr uint64_t CLAIMLOT   = 0x7e7b5e3f15f93cf4ULL;
    constexpr uint64_t INITREG    = 0xa1b2c3d4e5f60718ULL;
    constexpr uint64_t REGPOOL    = 0xb2c3d4e5f6071829ULL;
    constexpr uint64_t UNREGPOOL  = 0xc3d4e5f607182930ULL;
    constexpr uint64_t INITREGA   = 0xd4e5f60718293041ULL;
    constexpr uint64_t COMPLREGA  = 0xe5f6071829304152ULL;
    constexpr uint64_t CANCELREGA = 0xf607182930415263ULL;
    constexpr uint64_t GETTWAP    = 0x7477617067657401ULL;
    constexpr uint64_t TH_EXEC    = 0x1a66fb4bc5652569ULL;
    constexpr uint64_t TH_INIT    = 0xebeb58a7310d222bULL;

    // Account discriminators
    constexpr uint64_t POOL_DISC   = 0x504f4f4c53574150ULL;  // "POOLSWAP"
    constexpr uint64_t NPOOL_DISC  = 0x4e504f4f4c535741ULL;  // "NPOOLSWA"
    constexpr uint64_t FARM_DISC   = 0x4641524d53574150ULL;  // "FARMSWAP"
    constexpr uint64_t UFARM_DISC  = 0x554641524d535741ULL;  // "UFARMSWA"
    constexpr uint64_t LOT_DISC    = 0x4c4f545445525921ULL;  // "LOTTERY!"
    constexpr uint64_t LOTE_DISC   = 0x4c4f54454e545259ULL;  // "LOTENTRY"
    constexpr uint64_t REG_DISC    = 0x5245474953545259ULL;  // "REGISTRY"
}

// ============================================================================
// Error Codes
// ============================================================================

enum class Error : uint32_t {
    Paused              = 6000,
    InvalidAmp          = 6001,
    MathOverflow        = 6002,
    ZeroAmount          = 6003,
    SlippageExceeded    = 6004,
    InvalidInvariant    = 6005,
    InsufficientLiquidity = 6006,
    VaultMismatch       = 6007,
    Expired             = 6008,
    AlreadyInitialized  = 6009,
    Unauthorized        = 6010,
    RampConstraint      = 6011,
    Locked              = 6012,
    FarmingError        = 6013,
    InvalidOwner        = 6014,
    InvalidDiscriminator = 6015,
    CpiFailed           = 6016,
};

inline const char* error_message(Error e) {
    switch (e) {
        case Error::Paused: return "Pool is paused";
        case Error::InvalidAmp: return "Invalid amplification coefficient";
        case Error::MathOverflow: return "Math overflow";
        case Error::ZeroAmount: return "Zero amount";
        case Error::SlippageExceeded: return "Slippage exceeded";
        case Error::InvalidInvariant: return "Invalid invariant or PDA mismatch";
        case Error::InsufficientLiquidity: return "Insufficient liquidity";
        case Error::VaultMismatch: return "Vault mismatch";
        case Error::Expired: return "Expired or ended";
        case Error::AlreadyInitialized: return "Already initialized";
        case Error::Unauthorized: return "Unauthorized";
        case Error::RampConstraint: return "Ramp constraint violated";
        case Error::Locked: return "Tokens are locked";
        case Error::FarmingError: return "Farming error";
        case Error::InvalidOwner: return "Invalid account owner";
        case Error::InvalidDiscriminator: return "Invalid account discriminator";
        case Error::CpiFailed: return "CPI call failed";
        default: return "Unknown error";
    }
}

// ============================================================================
// Types
// ============================================================================

using Pubkey = std::array<uint8_t, 32>;

// Delta-encoded OHLCV candle (12 bytes)
struct Candle {
    uint32_t open;      // Base price (scaled 1e6)
    uint16_t high_d;    // High = open + high_d
    uint16_t low_d;     // Low = open - low_d
    int16_t  close_d;   // Close = open + close_d
    uint16_t volume;    // Volume in 1e9 units

    uint32_t high() const { return open + high_d; }
    uint32_t low() const { return open - low_d; }
    int32_t close() const { return static_cast<int32_t>(open) + close_d; }
};
static_assert(sizeof(Candle) == 12, "Candle must be 12 bytes");

// 2-token Pool state (1024 bytes)
struct Pool {
    uint64_t disc;              // "POOLSWAP"
    uint8_t  bump;
    uint8_t  paused;
    uint8_t  padding[6];
    Pubkey   authority;
    Pubkey   pending_auth;
    int64_t  auth_time;
    Pubkey   mint0;
    Pubkey   mint1;
    Pubkey   vault0;
    Pubkey   vault1;
    Pubkey   lp_mint;
    uint64_t bal0;
    uint64_t bal1;
    uint64_t lp_supply;
    uint64_t amp;
    uint64_t target_amp;
    int64_t  ramp_start;
    int64_t  ramp_end;
    uint64_t pending_amp;
    int64_t  commit_time;
    uint64_t fee_bps;
    uint64_t admin_fee0;
    uint64_t admin_fee1;
    uint64_t total_swaps;
    uint64_t total_volume;
    uint64_t last_slot;
    uint8_t  hourly_idx;
    uint8_t  daily_idx;
    uint8_t  padding2[6];
    Candle   hourly_candles[24];
    Candle   daily_candles[7];
    uint32_t trade_count;
    uint64_t trade_sum;
    uint32_t max_price;
    uint32_t min_price;
    uint8_t  bloom[128];

    bool is_valid() const { return disc == disc::POOL_DISC; }
    bool is_paused() const { return paused != 0; }

    // Get current effective amp (handles ramping)
    uint64_t get_amp(int64_t now) const {
        if (now >= ramp_end || ramp_end == ramp_start) return target_amp;
        if (now <= ramp_start) return amp;
        
        int64_t elapsed = now - ramp_start;
        int64_t duration = ramp_end - ramp_start;
        
        if (target_amp > amp) {
            return amp + (target_amp - amp) * elapsed / duration;
        } else {
            return amp - (amp - target_amp) * elapsed / duration;
        }
    }
};

// N-token Pool state (2048 bytes)
struct NPool {
    uint64_t disc;              // "NPOOLSWA"
    uint8_t  bump;
    uint8_t  paused;
    uint8_t  n_tokens;
    uint8_t  padding[5];
    Pubkey   authority;
    Pubkey   pending_auth;
    int64_t  auth_time;
    Pubkey   mints[8];
    Pubkey   vaults[8];
    Pubkey   lp_mint;
    uint64_t balances[8];
    uint64_t lp_supply;
    uint64_t amp;
    uint64_t target_amp;
    int64_t  ramp_start;
    int64_t  ramp_end;
    uint64_t pending_amp;
    int64_t  commit_time;
    uint64_t fee_bps;
    uint64_t admin_fees[8];
    uint64_t total_swaps;
    uint64_t total_volume;

    bool is_valid() const { return disc == disc::NPOOL_DISC; }
    bool is_paused() const { return paused != 0; }
};

// Farm state
struct Farm {
    uint64_t disc;              // "FARMSWAP"
    uint8_t  bump;
    uint8_t  padding[7];
    Pubkey   authority;
    Pubkey   pool;
    Pubkey   reward_mint;
    Pubkey   reward_vault;
    Pubkey   lp_vault;
    uint64_t reward_rate;
    int64_t  start_time;
    int64_t  end_time;
    int64_t  last_update;
    __uint128_t acc_reward;     // Scaled 1e12
    uint64_t total_staked;

    bool is_valid() const { return disc == disc::FARM_DISC; }
};

// User farm position
struct UserFarm {
    uint64_t disc;              // "UFARMSWA"
    uint8_t  bump;
    uint8_t  padding[7];
    Pubkey   owner;
    Pubkey   farm;
    uint64_t staked;
    __uint128_t reward_debt;
    uint64_t locked_amount;
    int64_t  unlock_time;

    bool is_valid() const { return disc == disc::UFARM_DISC; }
};

// Lottery state
struct Lottery {
    uint64_t disc;              // "LOTTERY!"
    uint8_t  bump;
    uint8_t  drawn;
    uint8_t  padding[6];
    Pubkey   authority;
    Pubkey   pool;
    Pubkey   lp_vault;
    uint64_t ticket_price;
    uint64_t total_tickets;
    int64_t  start_time;
    int64_t  end_time;
    Pubkey   winner;
    uint64_t winning_ticket;

    bool is_valid() const { return disc == disc::LOT_DISC; }
    bool is_drawn() const { return drawn != 0; }
};

// Lottery entry
struct LotteryEntry {
    uint64_t disc;              // "LOTENTRY"
    uint8_t  bump;
    uint8_t  claimed;
    uint8_t  padding[6];
    Pubkey   owner;
    Pubkey   lottery;
    uint64_t ticket_start;
    uint64_t ticket_count;

    bool is_valid() const { return disc == disc::LOTE_DISC; }
    bool is_claimed() const { return claimed != 0; }
    
    bool is_winner(uint64_t winning_ticket) const {
        return winning_ticket >= ticket_start && 
               winning_ticket < ticket_start + ticket_count;
    }
};

// Pool registry
struct Registry {
    uint64_t disc;              // "REGISTRY"
    uint8_t  bump;
    uint8_t  padding[7];
    Pubkey   authority;
    Pubkey   pending_auth;
    int64_t  auth_time;
    uint32_t count;
    Pubkey   pools[256];

    bool is_valid() const { return disc == disc::REG_DISC; }
};

// TWAP window options
enum class TwapWindow : uint8_t {
    Hour1  = 0,
    Hour4  = 1,
    Hour24 = 2,
    Day7   = 3,
};

// Decoded TWAP result
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
// Instruction Builders
// ============================================================================

class InstructionBuilder {
public:
    InstructionBuilder() { data_.reserve(64); }

    // Get built instruction data
    const std::vector<uint8_t>& data() const { return data_; }
    std::vector<uint8_t> build() { return std::move(data_); }

    // Reset builder
    void clear() { data_.clear(); }

    // ========== Pool Creation ==========

    static InstructionBuilder createpool(uint64_t amp, uint8_t bump) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEPOOL);
        b.write_u64(amp);
        b.write_u8(bump);
        return b;
    }

    static InstructionBuilder createpn(uint64_t amp, uint8_t n_tokens, uint8_t bump) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEPN);
        b.write_u64(amp);
        b.write_u8(n_tokens);
        b.write_u8(bump);
        return b;
    }

    static InstructionBuilder initt0v() {
        InstructionBuilder b;
        b.write_u64(disc::INITT0V);
        return b;
    }

    static InstructionBuilder initt1v() {
        InstructionBuilder b;
        b.write_u64(disc::INITT1V);
        return b;
    }

    static InstructionBuilder initlpm() {
        InstructionBuilder b;
        b.write_u64(disc::INITLPM);
        return b;
    }

    // ========== Swaps ==========

    static InstructionBuilder swap(uint8_t from, uint8_t to, uint64_t amount_in, 
                                   uint64_t min_out, int64_t deadline) {
        InstructionBuilder b;
        b.write_u64(disc::SWAP);
        b.write_u8(from);
        b.write_u8(to);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        b.write_i64(deadline);
        return b;
    }

    static InstructionBuilder swapt0t1(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::SWAPT0T1);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    static InstructionBuilder swapt1t0(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::SWAPT1T0);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    static InstructionBuilder swapn(uint8_t from_idx, uint8_t to_idx, 
                                    uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::SWAPN);
        b.write_u8(from_idx);
        b.write_u8(to_idx);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    static InstructionBuilder migt0t1(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::MIGT0T1);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    static InstructionBuilder migt1t0(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::MIGT1T0);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    // ========== Liquidity ==========

    static InstructionBuilder addliq(uint64_t amount0, uint64_t amount1, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQ);
        b.write_u64(amount0);
        b.write_u64(amount1);
        b.write_u64(min_lp);
        return b;
    }

    static InstructionBuilder addliq1(uint64_t amount_in, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQ1);
        b.write_u64(amount_in);
        b.write_u64(min_lp);
        return b;
    }

    static InstructionBuilder addliqn(const std::vector<uint64_t>& amounts, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQN);
        for (auto amt : amounts) b.write_u64(amt);
        b.write_u64(min_lp);
        return b;
    }

    static InstructionBuilder remliq(uint64_t lp_amount, uint64_t min0, uint64_t min1) {
        InstructionBuilder b;
        b.write_u64(disc::REMLIQ);
        b.write_u64(lp_amount);
        b.write_u64(min0);
        b.write_u64(min1);
        return b;
    }

    static InstructionBuilder remliqn(uint64_t lp_amount, const std::vector<uint64_t>& mins) {
        InstructionBuilder b;
        b.write_u64(disc::REMLIQN);
        b.write_u64(lp_amount);
        for (auto m : mins) b.write_u64(m);
        return b;
    }

    // ========== Admin ==========

    static InstructionBuilder setpause(bool paused) {
        InstructionBuilder b;
        b.write_u64(disc::SETPAUSE);
        b.write_u8(paused ? 1 : 0);
        return b;
    }

    static InstructionBuilder updfee(uint64_t fee_bps) {
        InstructionBuilder b;
        b.write_u64(disc::UPDFEE);
        b.write_u64(fee_bps);
        return b;
    }

    static InstructionBuilder wdrawfee() {
        InstructionBuilder b;
        b.write_u64(disc::WDRAWFEE);
        return b;
    }

    static InstructionBuilder commitamp(uint64_t target_amp) {
        InstructionBuilder b;
        b.write_u64(disc::COMMITAMP);
        b.write_u64(target_amp);
        return b;
    }

    static InstructionBuilder rampamp(uint64_t target_amp, int64_t duration) {
        InstructionBuilder b;
        b.write_u64(disc::RAMPAMP);
        b.write_u64(target_amp);
        b.write_i64(duration);
        return b;
    }

    static InstructionBuilder stopramp() {
        InstructionBuilder b;
        b.write_u64(disc::STOPRAMP);
        return b;
    }

    static InstructionBuilder initauth() {
        InstructionBuilder b;
        b.write_u64(disc::INITAUTH);
        return b;
    }

    static InstructionBuilder complauth() {
        InstructionBuilder b;
        b.write_u64(disc::COMPLAUTH);
        return b;
    }

    static InstructionBuilder cancelauth() {
        InstructionBuilder b;
        b.write_u64(disc::CANCELAUTH);
        return b;
    }

    // ========== Farming ==========

    static InstructionBuilder createfarm(uint64_t reward_rate, int64_t start_time, int64_t end_time) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEFARM);
        b.write_u64(reward_rate);
        b.write_i64(start_time);
        b.write_i64(end_time);
        return b;
    }

    static InstructionBuilder stakelp(uint64_t amount) {
        InstructionBuilder b;
        b.write_u64(disc::STAKELP);
        b.write_u64(amount);
        return b;
    }

    static InstructionBuilder unstakelp(uint64_t amount) {
        InstructionBuilder b;
        b.write_u64(disc::UNSTAKELP);
        b.write_u64(amount);
        return b;
    }

    static InstructionBuilder claimfarm() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMFARM);
        return b;
    }

    static InstructionBuilder locklp(uint64_t amount, int64_t duration) {
        InstructionBuilder b;
        b.write_u64(disc::LOCKLP);
        b.write_u64(amount);
        b.write_i64(duration);
        return b;
    }

    static InstructionBuilder claimulp() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMULP);
        return b;
    }

    // ========== Lottery ==========

    static InstructionBuilder enterlot(uint64_t ticket_count) {
        InstructionBuilder b;
        b.write_u64(disc::ENTERLOT);
        b.write_u64(ticket_count);
        return b;
    }

    static InstructionBuilder drawlot(uint64_t random_seed) {
        InstructionBuilder b;
        b.write_u64(disc::DRAWLOT);
        b.write_u64(random_seed);
        return b;
    }

    static InstructionBuilder claimlot() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMLOT);
        return b;
    }

    // ========== Registry ==========

    static InstructionBuilder initreg() {
        InstructionBuilder b;
        b.write_u64(disc::INITREG);
        return b;
    }

    static InstructionBuilder regpool() {
        InstructionBuilder b;
        b.write_u64(disc::REGPOOL);
        return b;
    }

    static InstructionBuilder unregpool() {
        InstructionBuilder b;
        b.write_u64(disc::UNREGPOOL);
        return b;
    }

    static InstructionBuilder initrega() {
        InstructionBuilder b;
        b.write_u64(disc::INITREGA);
        return b;
    }

    static InstructionBuilder complrega() {
        InstructionBuilder b;
        b.write_u64(disc::COMPLREGA);
        return b;
    }

    static InstructionBuilder cancelrega() {
        InstructionBuilder b;
        b.write_u64(disc::CANCELREGA);
        return b;
    }

    // ========== Oracle ==========

    static InstructionBuilder gettwap(TwapWindow window) {
        InstructionBuilder b;
        b.write_u64(disc::GETTWAP);
        b.write_u8(static_cast<uint8_t>(window));
        return b;
    }

private:
    std::vector<uint8_t> data_;

    void write_u8(uint8_t v) { data_.push_back(v); }
    
    void write_u64(uint64_t v) {
        for (int i = 0; i < 8; i++) {
            data_.push_back(static_cast<uint8_t>(v & 0xFF));
            v >>= 8;
        }
    }
    
    void write_i64(int64_t v) { write_u64(static_cast<uint64_t>(v)); }
};

// ============================================================================
// StableSwap Math (for off-chain simulation)
// ============================================================================

namespace math {

// Calculate invariant D for 2-token pool
inline std::optional<uint64_t> calc_d(uint64_t x, uint64_t y, uint64_t amp) {
    uint64_t s = x + y;
    if (s == 0) return 0;

    uint64_t d = s;
    uint64_t ann = amp * 4;  // A * n^n where n=2

    for (int i = 0; i < NEWTON_ITERATIONS; i++) {
        // d_p = d^3 / (4 * x * y)
        __uint128_t d_p = static_cast<__uint128_t>(d) * d / (2 * x);
        d_p = d_p * d / (2 * y);

        uint64_t d_prev = d;
        
        // d = (ann * s + d_p * 2) * d / ((ann - 1) * d + 3 * d_p)
        __uint128_t num = (static_cast<__uint128_t>(ann) * s + d_p * 2) * d;
        __uint128_t denom = (ann - 1) * d + d_p * 3;
        d = static_cast<uint64_t>(num / denom);

        if (d > d_prev) {
            if (d - d_prev <= 1) return d;
        } else {
            if (d_prev - d <= 1) return d;
        }
    }
    return std::nullopt;  // Failed to converge
}

// Calculate output amount y given input x for swap
inline std::optional<uint64_t> calc_y(uint64_t x_new, uint64_t d, uint64_t amp) {
    uint64_t ann = amp * 4;
    
    // c = d^3 / (4 * x_new * ann)
    __uint128_t c = static_cast<__uint128_t>(d) * d / (2 * x_new);
    c = c * d / (2 * ann);

    // b = x_new + d / ann
    uint64_t b = x_new + d / ann;

    uint64_t y = d;
    for (int i = 0; i < NEWTON_ITERATIONS; i++) {
        uint64_t y_prev = y;
        // y = (y^2 + c) / (2y + b - d)
        __uint128_t num = static_cast<__uint128_t>(y) * y + c;
        uint64_t denom = 2 * y + b - d;
        y = static_cast<uint64_t>(num / denom);

        if (y > y_prev) {
            if (y - y_prev <= 1) return y;
        } else {
            if (y_prev - y <= 1) return y;
        }
    }
    return std::nullopt;
}

// Simulate swap and return output amount
inline std::optional<uint64_t> simulate_swap(
    uint64_t bal_in, uint64_t bal_out, 
    uint64_t amount_in, uint64_t amp, uint64_t fee_bps
) {
    auto d = calc_d(bal_in, bal_out, amp);
    if (!d) return std::nullopt;

    uint64_t new_bal_in = bal_in + amount_in;
    auto new_bal_out = calc_y(new_bal_in, *d, amp);
    if (!new_bal_out) return std::nullopt;

    uint64_t amount_out = bal_out - *new_bal_out;
    
    // Apply fee
    amount_out = amount_out - (amount_out * fee_bps / 10000);
    
    return amount_out;
}

// Calculate LP tokens for deposit
inline std::optional<uint64_t> calc_lp_tokens(
    uint64_t amt0, uint64_t amt1,
    uint64_t bal0, uint64_t bal1,
    uint64_t lp_supply, uint64_t amp
) {
    if (lp_supply == 0) {
        // Initial deposit: LP = sqrt(amt0 * amt1)
        __uint128_t product = static_cast<__uint128_t>(amt0) * amt1;
        uint64_t lp = 1;
        for (int i = 0; i < 64; i++) {
            uint64_t next = (lp + product / lp) / 2;
            if (next >= lp) break;
            lp = next;
        }
        return lp;
    }

    auto d0 = calc_d(bal0, bal1, amp);
    auto d1 = calc_d(bal0 + amt0, bal1 + amt1, amp);
    if (!d0 || !d1 || *d0 == 0) return std::nullopt;

    // LP tokens = lp_supply * (d1 - d0) / d0
    __uint128_t lp = static_cast<__uint128_t>(lp_supply) * (*d1 - *d0) / *d0;
    return static_cast<uint64_t>(lp);
}

}  // namespace math

// ============================================================================
// Utility Functions
// ============================================================================

// Parse pool from account data
inline std::optional<Pool> parse_pool(const uint8_t* data, size_t len) {
    if (len < sizeof(Pool)) return std::nullopt;
    Pool pool;
    std::memcpy(&pool, data, sizeof(Pool));
    if (!pool.is_valid()) return std::nullopt;
    return pool;
}

// Parse NPool from account data
inline std::optional<NPool> parse_npool(const uint8_t* data, size_t len) {
    if (len < sizeof(NPool)) return std::nullopt;
    NPool pool;
    std::memcpy(&pool, data, sizeof(NPool));
    if (!pool.is_valid()) return std::nullopt;
    return pool;
}

// Base58 encode (simplified)
inline std::string base58_encode(const Pubkey& key) {
    static const char* ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    
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
    
    // Leading zeros
    for (auto b : key) {
        if (b != 0) break;
        digits.push_back(0);
    }
    
    std::string result;
    result.reserve(digits.size());
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        result += ALPHABET[*it];
    }
    return result;
}

// Compare pubkeys
inline bool pubkey_eq(const Pubkey& a, const Pubkey& b) {
    return std::memcmp(a.data(), b.data(), 32) == 0;
}

// Zero pubkey
inline Pubkey pubkey_zero() {
    Pubkey pk{};
    return pk;
}

inline bool pubkey_is_zero(const Pubkey& pk) {
    for (auto b : pk) if (b != 0) return false;
    return true;
}

}  // namespace aex402

// Backwards compatibility
namespace stableswap = aex402;
