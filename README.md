# AeX402 AMM C++ SDK

Pure C++17 SDK for the AeX402 AMM on Solana. Header-only with no external dependencies.

**Program ID:** `3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje`

## Features

- **Instruction builders** for all 55+ handlers
- **Account parsing** for Pool, NPool, Farm, Lottery, Registry, CLPool, Orderbook, MLBrain
- **StableSwap math** for off-chain simulation (Newton's method)
- **TWAP decoding** with confidence scores
- **PDA derivation** utilities and base58 encoding
- **All constants and error codes**

## Quick Start

```cpp
#include "aex402.hpp"
using namespace aex402;

// Build swap instruction
auto ix = InstructionBuilder::swapt0t1(1000000, 990000);
std::vector<uint8_t> data = ix.build();

// Simulate swap off-chain
auto out = math::simulate_swap(bal0, bal1, amount_in, amp, fee_bps);

// Parse pool account
auto pool = parse_pool(account_data, account_len);
if (pool && !pool->is_paused()) {
    uint64_t current_amp = pool->get_amp(now);
}

// Decode TWAP result
auto twap = TwapResult::decode(return_value);
double price = twap.price_f64();
double conf = twap.confidence_pct();
```

## Build

### Using CMake (recommended)

```bash
mkdir build && cd build
cmake ..
make
./aex402_example
```

### Direct compilation

```bash
g++ -std=c++17 -O2 example.cpp -o example
./example
```

## File Structure

```
sdk/cpp/
|-- aex402.hpp        # Main header (include this)
|-- constants.hpp     # Program ID, discriminators, error codes
|-- types.hpp         # Account structures (Pool, NPool, Farm, etc.)
|-- accounts.hpp      # Account parsing functions
|-- instructions.hpp  # Instruction builders for all handlers
|-- math.hpp          # StableSwap math (Newton's method)
|-- pda.hpp           # PDA derivation utilities
|-- example.cpp       # Usage examples
|-- CMakeLists.txt    # CMake build configuration
|-- README.md         # This file
```

## Integration

This SDK builds instruction data. Integrate with your Solana RPC client:

```cpp
// 1. Build instruction data
auto ix_data = InstructionBuilder::swapt0t1(amount, min_out).build();

// 2. Create transaction with your RPC library
// Accounts: [pool, vault0, vault1, user_t0, user_t1, user, token_program]
Transaction tx;
tx.add_instruction(PROGRAM_ID, accounts, ix_data);

// 3. Sign and send
tx.sign(wallet);
rpc.send_transaction(tx);
```

## Account Layouts

### Pool (1024 bytes)

```cpp
struct Pool {
    uint8_t  disc[8];           // "POOLSWAP"
    Pubkey   authority;         // 32 bytes
    Pubkey   mint0, mint1;      // Token mints
    Pubkey   vault0, vault1;    // Token vaults
    Pubkey   lp_mint;           // LP token mint
    uint64_t amp;               // Current amplification
    uint64_t init_amp, target_amp;
    int64_t  ramp_start, ramp_stop;
    uint64_t fee_bps;           // Swap fee (basis points)
    uint64_t admin_fee_pct;
    uint64_t bal0, bal1;        // Token balances
    uint64_t lp_supply;         // LP token supply
    // ... analytics fields
    Candle   hours[24];         // Hourly OHLCV candles
    Candle   days[7];           // Daily OHLCV candles
};
```

### Candle (12 bytes, delta-encoded)

```cpp
struct Candle {
    uint32_t open;       // Base price (scaled 1e6)
    uint16_t high_d;     // high = open + high_d
    uint16_t low_d;      // low = open - low_d
    int16_t  close_d;    // close = open + close_d
    uint16_t volume;     // Volume in 1e9 units
};
```

## Discriminators

```cpp
namespace disc {
    // Swaps
    constexpr uint64_t SWAP     = 0x82c69e91e17587c8ULL;
    constexpr uint64_t SWAPT0T1 = 0x642af2b7e0f14e2aULL;
    constexpr uint64_t SWAPT1T0 = 0x3a0e131bac75c4c8ULL;
    constexpr uint64_t SWAPN    = 0xf1a8e3c7b2d9e5f8ULL;

    // Liquidity
    constexpr uint64_t ADDLIQ   = 0xa2e7c4f8b3d1e5a9ULL;
    constexpr uint64_t REMLIQ   = 0x2e54bc2c75c9f902ULL;

    // Admin
    constexpr uint64_t SETPAUSE = 0xe075762b7e0d6ec9ULL;
    constexpr uint64_t UPDFEE   = 0x8f3a2e5b7c9d1f4aULL;

    // ... see constants.hpp for all 55+
}
```

## Error Handling

```cpp
// Parse error from transaction logs
uint32_t code = 6004;
Error err = static_cast<Error>(code);
std::cout << error_message(err);  // "Slippage exceeded"
```

## StableSwap Math

```cpp
// Calculate invariant D (Newton's method)
auto d = math::calc_d(bal0, bal1, amp);

// Calculate output for swap
auto y = math::calc_y(new_x, *d, amp);

// Full swap simulation with fees
auto out = math::simulate_swap(bal0, bal1, amt_in, amp, fee_bps);

// Calculate LP tokens for deposit
auto lp = math::calc_lp_tokens(amt0, amt1, bal0, bal1, supply, amp);

// Calculate current amp during ramping
auto current = math::get_current_amp(amp, target, start, end, now);

// N-token pool math
auto d_n = math::calc_d_n(balances, n_tokens, amp);
auto out_n = math::simulate_swap_n(balances, n_tokens, from, to, amt, amp, fee);
```

## TWAP Oracle

```cpp
// Build gettwap instruction
auto ix = InstructionBuilder::gettwap(TwapWindow::Hour24);

// Decode return value (simulate transaction)
uint64_t result = simulate_tx(ix);
auto twap = TwapResult::decode(result);

std::cout << "Price: " << twap.price_f64() << std::endl;
std::cout << "Confidence: " << twap.confidence_pct() << "%" << std::endl;
```

## PDA Derivation

```cpp
// Build seeds for Pool PDA
auto seeds = pda::pool_seeds(mint0, mint1);

// Add bump for signing
auto seeds_with_bump = pda::pool_seeds_with_bump(mint0, mint1, bump);

// Base58 encode/decode
std::string encoded = pda::base58_encode(pubkey);
Pubkey decoded = pda::base58_decode("3AMM53MsJZy2Jvf7PeHHga3bsGjWV4TSaYz29WUtcdje");
```

## Instruction Categories

| Category | Instructions |
|----------|-------------|
| **Pool Creation** | createpool, createpn, initt0v, initt1v, initlpm |
| **Swaps** | swap, swapt0t1, swapt1t0, swapn, migt0t1, migt1t0 |
| **Liquidity** | addliq, addliq1, addliqn, remliq, remliqn |
| **Admin** | setpause, updfee, wdrawfee, commitamp, rampamp, stopramp, initauth, complauth, cancelauth |
| **Farming** | createfarm, stakelp, unstakelp, claimfarm, locklp, claimulp |
| **Lottery** | createlot, enterlot, drawlot, claimlot |
| **Registry** | initreg, regpool, unregpool, initrega, complrega, cancelrega |
| **Oracle** | gettwap |
| **Circuit Breaker** | setcb, resetcb |
| **Rate Limiting** | setrl |
| **Governance** | govprop, govvote, govexec, govcncl |
| **Orderbook** | initbook, placeord, cancelord, fillord |
| **Concentrated Liquidity** | initclpl, clmint, clburn, clcollect, clswap |
| **Flash Loans** | flashloan, flashrepy |
| **Multi-hop** | multihop |
| **ML Brain** | initml, cfgml, trainml, applyml, logml |
| **Transfer Hook** | th_exec, th_init |

## Requirements

- C++17 or later
- `__uint128_t` support (GCC, Clang)
- For MSVC, a custom 128-bit implementation would be needed

## License

MIT
