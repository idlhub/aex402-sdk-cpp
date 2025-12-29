#pragma once
/**
 * AeX402 AMM C++ SDK - Constants
 *
 * Program ID, instruction discriminators, account discriminators, and error codes.
 * All discriminators are in little-endian byte order.
 *
 * Program ID: 3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje
 */

#include <cstdint>
#include <array>
#include <string_view>

namespace aex402 {

// ============================================================================
// Program ID
// ============================================================================

constexpr std::string_view PROGRAM_ID_STR = "3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje";

// Program ID as raw bytes (base58 decoded)
constexpr std::array<uint8_t, 32> PROGRAM_ID_BYTES = {
    0x21, 0x2d, 0xa1, 0xc2, 0x2f, 0x51, 0xb8, 0x19,
    0x3e, 0xb9, 0x5a, 0x5c, 0x64, 0x4a, 0x34, 0x9f,
    0x93, 0xf3, 0x4c, 0x8a, 0x33, 0x3f, 0x47, 0xd8,
    0x2a, 0x5e, 0x1f, 0x0b, 0xc1, 0x3b, 0x62, 0xf2
};

// ============================================================================
// Token Programs
// ============================================================================

constexpr std::string_view TOKEN_PROGRAM_ID = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";
constexpr std::string_view TOKEN_2022_PROGRAM_ID = "TokenzQdBNbLqP5VEhdkAS6EPFLC1PHnBqCXEpPxuEb";

// ============================================================================
// Pool Constants
// ============================================================================

constexpr uint64_t MIN_AMP = 1;
constexpr uint64_t MAX_AMP = 100000;
constexpr uint64_t DEFAULT_FEE_BPS = 30;
constexpr uint64_t ADMIN_FEE_PCT = 50;
constexpr uint64_t MIN_SWAP = 100000;
constexpr uint64_t MIN_DEPOSIT = 100000000;
constexpr uint8_t  NEWTON_ITERATIONS = 255;
constexpr int64_t  RAMP_MIN_DURATION = 86400;   // 1 day in seconds
constexpr int64_t  COMMIT_DELAY = 3600;         // 1 hour in seconds
constexpr uint64_t MIGRATION_FEE_BPS = 1337;    // 0.1337%
constexpr uint8_t  MAX_TOKENS = 8;
constexpr uint64_t POOL_SIZE = 1024;
constexpr uint64_t NPOOL_SIZE = 2048;
constexpr uint8_t  BLOOM_SIZE = 128;
constexpr uint8_t  OHLCV_24H = 24;
constexpr uint8_t  OHLCV_7D = 7;
constexpr uint64_t SLOTS_PER_HOUR = 9000;       // ~400ms slots
constexpr uint64_t SLOTS_PER_DAY = 216000;      // 24 hours

// ============================================================================
// Circuit Breaker Constants
// ============================================================================

constexpr uint64_t CB_PRICE_DEV_BPS = 1000;     // 10% price deviation triggers
constexpr uint64_t CB_VOLUME_MULT = 10;         // Volume > 10x avg triggers
constexpr uint64_t CB_COOLDOWN_SLOTS = 9000;    // ~1 hour cooldown
constexpr uint64_t CB_AUTO_RESUME_SLOTS = 54000; // 6 hour auto-resume

// ============================================================================
// Rate Limiting Constants
// ============================================================================

constexpr uint64_t RL_SLOTS_PER_EPOCH = 750;    // ~5 minute epochs

// ============================================================================
// Governance Constants
// ============================================================================

constexpr uint64_t GOV_VOTE_SLOTS = 518400;     // ~3 days voting
constexpr uint64_t GOV_TIMELOCK_SLOTS = 172800; // ~1 day execution delay
constexpr uint64_t GOV_QUORUM_BPS = 1000;       // 10% of LP supply must vote
constexpr uint64_t GOV_THRESHOLD_BPS = 5000;    // 50%+ of votes to pass

// ============================================================================
// ML Brain Constants
// ============================================================================

constexpr uint64_t ML_NUM_STATES = 27;          // 3^3 states
constexpr uint64_t ML_NUM_ACTIONS = 9;          // 9 possible actions
constexpr double   ML_GAMMA = 0.9;              // Discount factor
constexpr double   ML_ALPHA = 0.1;              // Learning rate
constexpr double   ML_EPSILON = 0.1;            // Exploration rate

// ============================================================================
// Concentrated Liquidity Constants
// ============================================================================

constexpr int16_t  CL_TICK_MIN = -500;
constexpr int16_t  CL_TICK_MAX = 500;
constexpr int16_t  CL_MAX_TICKS = 1001;
constexpr uint64_t CL_MIN_DURATION = 300;       // 5 min minimum position

// ============================================================================
// Instruction Discriminators (little-endian u64)
// ============================================================================

namespace disc {
    // Pool creation
    constexpr uint64_t CREATEPOOL = 0xf2b9e4d1c8a7e3f9ULL;
    constexpr uint64_t CREATEPN   = 0x27c933bce5c77c1bULL;
    constexpr uint64_t INITT0V    = 0x5e8c3b0d0f3e4a9fULL;
    constexpr uint64_t INITT1V    = 0x7a4e9f1c3b2d5e8aULL;
    constexpr uint64_t INITLPM    = 0xf4d1e9a3c5b8e7f2ULL;

    // Swaps
    constexpr uint64_t SWAP       = 0x82c69e91e17587c8ULL;
    constexpr uint64_t SWAPT0T1   = 0x642af2b7e0f14e2aULL;
    constexpr uint64_t SWAPT1T0   = 0x3a0e131bac75c4c8ULL;
    constexpr uint64_t SWAPN      = 0xf1a8e3c7b2d9e5f8ULL;
    constexpr uint64_t MIGT0T1    = 0xd2e4f1a8c3b7e9d5ULL;
    constexpr uint64_t MIGT1T0    = 0x1888779426393db8ULL;

    // Liquidity
    constexpr uint64_t ADDLIQ     = 0xa2e7c4f8b3d1e5a9ULL;
    constexpr uint64_t ADDLIQ1    = 0x51c98b4e3c2e12e6ULL;
    constexpr uint64_t ADDLIQN    = 0xe3f7a2c8d1b9e4f6ULL;
    constexpr uint64_t REMLIQ     = 0x2e54bc2c75c9f902ULL;
    constexpr uint64_t REMLIQN    = 0xb3f8e2a5c7d9e1b4ULL;

    // Admin
    constexpr uint64_t SETPAUSE   = 0xe075762b7e0d6ec9ULL;
    constexpr uint64_t UPDFEE     = 0x8f3a2e5b7c9d1f4aULL;
    constexpr uint64_t WDRAWFEE   = 0xf9e5d3a2c8b1e7f8ULL;
    constexpr uint64_t COMMITAMP  = 0xc1d9e3f7a5b8e2c4ULL;
    constexpr uint64_t RAMPAMP    = 0x9a1c5e3f7b2d8e6aULL;
    constexpr uint64_t STOPRAMP   = 0x3c9427bb15a21053ULL;
    constexpr uint64_t INITAUTH   = 0xf5e2a7c9d3b1e8f4ULL;
    constexpr uint64_t COMPLAUTH  = 0xf6e8d2a4c7b9e1f5ULL;
    constexpr uint64_t CANCELAUTH = 0xf7e3a9c1d5b2e8f6ULL;

    // Farming
    constexpr uint64_t CREATEFARM = 0x6d7b0c8e2f1a3d5cULL;
    constexpr uint64_t STAKELP    = 0xf8d4e1a7c3b9e2f7ULL;
    constexpr uint64_t UNSTAKELP  = 0x4166bf654e34f8bcULL;
    constexpr uint64_t CLAIMFARM  = 0x075762b7e0d6ec9bULL;
    constexpr uint64_t LOCKLP     = 0xfefb83015f028cecULL;
    constexpr uint64_t CLAIMULP   = 0xca8593f45ce88b1eULL;

    // Lottery
    constexpr uint64_t CREATELOT  = 0x6c6f74746572793cULL;
    constexpr uint64_t ENTERLOT   = 0xe795383a4eef48fcULL;
    constexpr uint64_t DRAWLOT    = 0x1361225a4d7cbc11ULL;
    constexpr uint64_t CLAIMLOT   = 0x7e7b5e3f15f93cf4ULL;

    // Registry
    constexpr uint64_t INITREG    = 0xa1b2c3d4e5f60718ULL;
    constexpr uint64_t REGPOOL    = 0xb2c3d4e5f6071829ULL;
    constexpr uint64_t UNREGPOOL  = 0xc3d4e5f607182930ULL;
    constexpr uint64_t INITREGA   = 0xd4e5f60718293041ULL;
    constexpr uint64_t COMPLREGA  = 0xe5f6071829304152ULL;
    constexpr uint64_t CANCELREGA = 0xf607182930415263ULL;

    // Oracle
    constexpr uint64_t GETTWAP    = 0x7477617067657401ULL;

    // Circuit Breaker
    constexpr uint64_t SETCB      = 0xcb01cb01cb01cb01ULL;
    constexpr uint64_t RESETCB    = 0xcb02cb02cb02cb02ULL;

    // Rate Limiting
    constexpr uint64_t SETRL      = 0x726c01726c01726cULL;

    // Oracle Config
    constexpr uint64_t SETORACLE  = 0x6f72636c01020304ULL;

    // Governance
    constexpr uint64_t GOVPROP    = 0x676f7670726f7000ULL;
    constexpr uint64_t GOVVOTE    = 0x676f76766f746500ULL;
    constexpr uint64_t GOVEXEC    = 0x676f7665786563ULL;
    constexpr uint64_t GOVCNCL    = 0x676f76636e636cULL;

    // Orderbook
    constexpr uint64_t INITBOOK   = 0x696e6974626f6f6bULL;
    constexpr uint64_t PLACEORD   = 0x706c6163656f7264ULL;
    constexpr uint64_t CANCELORD  = 0x63616e63656c6f72ULL;
    constexpr uint64_t FILLORD    = 0x66696c6c6f726465ULL;

    // Concentrated Liquidity
    constexpr uint64_t INITCLPL   = 0x636c706f6f6c0101ULL;
    constexpr uint64_t CLMINT     = 0x636c6d696e740101ULL;
    constexpr uint64_t CLBURN     = 0x636c6275726e0101ULL;
    constexpr uint64_t CLCOLLECT  = 0x636c636f6c6c6563ULL;
    constexpr uint64_t CLSWAP     = 0x636c737761700101ULL;

    // Flash Loans
    constexpr uint64_t FLASHLOAN  = 0x666c6173686c6f61ULL;
    constexpr uint64_t FLASHREPY  = 0x666c617368726570ULL;

    // Multi-hop
    constexpr uint64_t MULTIHOP   = 0x6d756c7469686f70ULL;

    // ML Brain
    constexpr uint64_t INITML     = 0x696e69746d6c6272ULL;
    constexpr uint64_t CFGML      = 0x6366676d6c627261ULL;
    constexpr uint64_t TRAINML    = 0x747261696e6d6c00ULL;
    constexpr uint64_t APPLYML    = 0x6170706c796d6c00ULL;
    constexpr uint64_t LOGML      = 0x6c6f676d6c737461ULL;

    // Transfer Hook
    constexpr uint64_t TH_EXEC    = 0x1a66fb4bc5652569ULL;
    constexpr uint64_t TH_INIT    = 0xebeb58a7310d222bULL;
}

// ============================================================================
// Account Discriminators (8-byte ASCII strings as u64)
// ============================================================================

namespace account_disc {
    constexpr uint64_t POOL      = 0x504f4f4c53574150ULL;  // "POOLSWAP"
    constexpr uint64_t NPOOL     = 0x4e504f4f4c535741ULL;  // "NPOOLSWA"
    constexpr uint64_t FARM      = 0x4641524d53574150ULL;  // "FARMSWAP"
    constexpr uint64_t UFARM     = 0x554641524d535741ULL;  // "UFARMSWA"
    constexpr uint64_t LOTTERY   = 0x4c4f545445525921ULL;  // "LOTTERY!"
    constexpr uint64_t LOTENTRY  = 0x4c4f54454e545259ULL;  // "LOTENTRY"
    constexpr uint64_t REGISTRY  = 0x5245474953545259ULL;  // "REGISTRY"
    constexpr uint64_t MLBRAIN   = 0x4d4c425241494e21ULL;  // "MLBRAIN!"
    constexpr uint64_t CLPOOL    = 0x434c504f4f4c2121ULL;  // "CLPOOL!!"
    constexpr uint64_t CLPOS     = 0x434c504f53495421ULL;  // "CLPOSIT!"
    constexpr uint64_t BOOK      = 0x4f5244455242454bULL;  // "ORDERBOK"
    constexpr uint64_t AIFEE     = 0x4149464545212121ULL;  // "AIFEE!!!"
    constexpr uint64_t THMETA    = 0x54484d4554412121ULL;  // "THMETA!!"
    constexpr uint64_t GOVPROP   = 0x474f5650524f5021ULL;  // "GOVPROP!"
    constexpr uint64_t GOVVOTE   = 0x474f56564f544521ULL;  // "GOVVOTE!"
    constexpr uint64_t GPOOLS    = 0x4750564f4f4c5321ULL;  // "GPVOOLS!" (Virtual Pools)
    constexpr uint64_t VPCLAIM   = 0x5650434c41494d21ULL;  // "VPCLAIM!"
    constexpr uint64_t FARMSTATE = 0x4641524d53544154ULL;  // "FARMSTAT"
    constexpr uint64_t PCONFIG   = 0x50434f4e46494721ULL;  // "PCONFIG!"
}

// ============================================================================
// Error Codes
// ============================================================================

enum class Error : uint32_t {
    // System errors
    Keys              = 3,
    Signature         = 4,
    Data              = 5,
    Immutable         = 6,

    // Custom errors (6000+)
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
    Full                = 6017,
    CircuitBreaker      = 6018,
    OracleError         = 6019,
    RateLimit           = 6020,
    GovernanceError     = 6021,
    OrderError          = 6022,
    TickError           = 6023,
    RangeError          = 6024,
    FlashError          = 6025,
    Cooldown            = 6026,
    MevProtection       = 6027,
    StaleData           = 6028,
    BiasError           = 6029,
    DurationError       = 6030,
};

/**
 * Get human-readable error message
 */
inline constexpr const char* error_message(Error e) {
    switch (e) {
        case Error::Keys: return "Wrong number of accounts";
        case Error::Signature: return "Missing required signature";
        case Error::Data: return "Invalid instruction data";
        case Error::Immutable: return "Account not writable";
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
        case Error::Full: return "Orderbook/registry is full";
        case Error::CircuitBreaker: return "Circuit breaker triggered";
        case Error::OracleError: return "Oracle price validation failed";
        case Error::RateLimit: return "Rate limit exceeded";
        case Error::GovernanceError: return "Governance error";
        case Error::OrderError: return "Orderbook error";
        case Error::TickError: return "Invalid tick";
        case Error::RangeError: return "Invalid price range";
        case Error::FlashError: return "Flash loan error";
        case Error::Cooldown: return "Cooldown period not elapsed";
        case Error::MevProtection: return "MEV protection triggered";
        case Error::StaleData: return "Stale data";
        case Error::BiasError: return "ML bias error";
        case Error::DurationError: return "Invalid duration";
        default: return "Unknown error";
    }
}

// ============================================================================
// TWAP Window Enum
// ============================================================================

enum class TwapWindow : uint8_t {
    Hour1  = 0,
    Hour4  = 1,
    Hour24 = 2,
    Day7   = 3,
};

// ============================================================================
// Governance Proposal Types
// ============================================================================

enum class ProposalType : uint8_t {
    FeeChange    = 1,
    AmpChange    = 2,
    AdminFee     = 3,
    Pause        = 4,
    Authority    = 5,
};

enum class ProposalStatus : uint8_t {
    Voting    = 0,
    Passed    = 1,
    Rejected  = 2,
    Executed  = 3,
    Cancelled = 4,
};

// ============================================================================
// ML Action Types
// ============================================================================

enum class MLAction : uint8_t {
    Hold    = 0,
    FeeUp   = 1,
    FeeDown = 2,
    AmpUp   = 3,
    AmpDown = 4,
    FarmUp  = 5,
    FarmDown = 6,
    LotUp   = 7,
    LotDown = 8,
};

// ============================================================================
// Order Types
// ============================================================================

enum class OrderType : uint8_t {
    Buy  = 0,
    Sell = 1,
};

}  // namespace aex402
