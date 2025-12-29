/**
 * AeX402 AMM C++ SDK Example
 *
 * Demonstrates building instructions and parsing pool state.
 * This is a standalone example - integrate with your Solana RPC client.
 *
 * Build:
 *   mkdir build && cd build
 *   cmake ..
 *   make
 *   ./aex402_example
 */

#include "aex402.hpp"
#include <iostream>
#include <iomanip>

using namespace aex402;

void print_hex(const std::vector<uint8_t>& data) {
    std::cout << "  Data (" << data.size() << " bytes): ";
    for (auto b : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== AeX402 AMM C++ SDK Example ===" << std::endl;
    std::cout << "SDK Version: " << sdk_version() << std::endl;
    std::cout << "Program ID: " << PROGRAM_ID_STR << std::endl << std::endl;

    // ========== Building Instructions ==========
    std::cout << "--- Building Instructions ---" << std::endl;

    // 1. Create pool instruction
    auto create_ix = InstructionBuilder::createpool(1000, 255);
    std::cout << "createpool(amp=1000, bump=255):" << std::endl;
    print_hex(create_ix.data());

    // 2. Swap instruction
    auto swap_ix = InstructionBuilder::swapt0t1(1000000, 990000);
    std::cout << "\nswapt0t1(amount_in=1000000, min_out=990000):" << std::endl;
    print_hex(swap_ix.data());

    // 3. Add liquidity instruction
    auto addliq_ix = InstructionBuilder::addliq(1000000000, 1000000000, 0);
    std::cout << "\naddliq(amt0=1B, amt1=1B, min_lp=0):" << std::endl;
    print_hex(addliq_ix.data());

    // 4. Generic swap with deadline
    int64_t deadline = 1735084800;  // Future timestamp
    auto generic_swap = InstructionBuilder::swap(0, 1, 500000, 495000, deadline);
    std::cout << "\nswap(from=0, to=1, amt=500000, min=495000, deadline):" << std::endl;
    print_hex(generic_swap.data());

    // 5. Admin operations
    auto pause_ix = InstructionBuilder::setpause(true);
    std::cout << "\nsetpause(true):" << std::endl;
    print_hex(pause_ix.data());

    auto fee_ix = InstructionBuilder::updfee(50);  // 0.5%
    std::cout << "\nupdfee(50 bps):" << std::endl;
    print_hex(fee_ix.data());

    // 6. Amp ramping
    auto commit_ix = InstructionBuilder::commitamp(2000);
    std::cout << "\ncommitamp(target=2000):" << std::endl;
    print_hex(commit_ix.data());

    auto ramp_ix = InstructionBuilder::rampamp(2000, 86400);  // 1 day ramp
    std::cout << "\nrampamp(target=2000, duration=86400):" << std::endl;
    print_hex(ramp_ix.data());

    // 7. TWAP oracle
    auto twap_ix = InstructionBuilder::gettwap(TwapWindow::Hour24);
    std::cout << "\ngettwap(24h window):" << std::endl;
    print_hex(twap_ix.data());

    // ========== Math Simulation ==========
    std::cout << "\n--- StableSwap Math Simulation ---" << std::endl;

    uint64_t bal0 = 1000000000000;  // 1M tokens (6 decimals)
    uint64_t bal1 = 1000000000000;
    uint64_t amp = 1000;
    uint64_t fee_bps = 30;  // 0.3%

    // Simulate swap
    uint64_t swap_in = 10000000000;  // 10K tokens
    auto swap_out = math::simulate_swap(bal0, bal1, swap_in, amp, fee_bps);
    
    if (swap_out) {
        std::cout << "Swap simulation:" << std::endl;
        std::cout << "  Input:  " << swap_in / 1000000.0 << " tokens" << std::endl;
        std::cout << "  Output: " << *swap_out / 1000000.0 << " tokens" << std::endl;
        std::cout << "  Price impact: " << 
            (1.0 - (*swap_out / (double)swap_in)) * 100 << "%" << std::endl;
    }

    // Calculate invariant D
    auto d = math::calc_d(bal0, bal1, amp);
    if (d) {
        std::cout << "\nInvariant D: " << *d << std::endl;
    }

    // LP token calculation
    uint64_t deposit0 = 100000000000;  // 100K
    uint64_t deposit1 = 100000000000;
    uint64_t lp_supply = 2000000000000;  // Existing supply
    
    auto lp_tokens = math::calc_lp_tokens(deposit0, deposit1, bal0, bal1, lp_supply, amp);
    if (lp_tokens) {
        std::cout << "\nLP tokens for deposit: " << *lp_tokens << std::endl;
    }

    // ========== TWAP Decoding ==========
    std::cout << "\n--- TWAP Result Decoding ---" << std::endl;

    // Simulated TWAP return value
    uint64_t twap_encoded = 0x270F001800100000ULL;  // price=1.0, samples=24, conf=99.99%
    auto twap = TwapResult::decode(twap_encoded);
    
    std::cout << "Encoded: 0x" << std::hex << twap_encoded << std::dec << std::endl;
    std::cout << "  Price: " << twap.price_f64() << std::endl;
    std::cout << "  Samples: " << twap.samples << std::endl;
    std::cout << "  Confidence: " << twap.confidence_pct() << "%" << std::endl;

    // ========== Error Handling ==========
    std::cout << "\n--- Error Codes ---" << std::endl;
    std::cout << "Error 6000: " << error_message(Error::Paused) << std::endl;
    std::cout << "Error 6004: " << error_message(Error::SlippageExceeded) << std::endl;
    std::cout << "Error 6010: " << error_message(Error::Unauthorized) << std::endl;

    std::cout << "\n=== Done ===" << std::endl;
    return 0;
}
