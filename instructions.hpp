#pragma once
/**
 * AeX402 AMM C++ SDK - Instruction Builders
 *
 * Functions for building instruction data for all program handlers.
 * Each builder returns a vector of bytes ready to be submitted as instruction data.
 */

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include "constants.hpp"
#include "types.hpp"

namespace aex402 {

/**
 * Instruction builder class for constructing instruction data.
 * Uses fluent interface pattern for chaining.
 */
class InstructionBuilder {
public:
    InstructionBuilder() { data_.reserve(64); }

    // Get the built instruction data
    const std::vector<uint8_t>& data() const { return data_; }

    // Move out the built data
    std::vector<uint8_t> build() { return std::move(data_); }

    // Get data as raw pointer
    const uint8_t* raw() const { return data_.data(); }

    // Get data size
    size_t size() const { return data_.size(); }

    // Reset builder
    void clear() { data_.clear(); }

    // ========================================================================
    // Pool Creation Instructions
    // ========================================================================

    /**
     * Create a 2-token pool.
     * Accounts: [pool, mint0, mint1, authority(signer), system_program]
     */
    static InstructionBuilder createpool(uint64_t amp, uint8_t bump) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEPOOL);
        b.write_u64(amp);
        b.write_u8(bump);
        return b;
    }

    /**
     * Create an N-token pool (2-8 tokens).
     * Accounts: [pool, mint0, mint1, ..., mintN, authority(signer), system_program]
     */
    static InstructionBuilder createpn(uint64_t amp, uint8_t n_tokens, uint8_t bump) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEPN);
        b.write_u64(amp);
        b.write_u8(n_tokens);
        b.write_u8(bump);
        return b;
    }

    /**
     * Initialize token 0 vault.
     * Accounts: [pool, vault, authority(signer), system_program]
     */
    static InstructionBuilder initt0v() {
        InstructionBuilder b;
        b.write_u64(disc::INITT0V);
        return b;
    }

    /**
     * Initialize token 1 vault.
     * Accounts: [pool, vault, authority(signer), system_program]
     */
    static InstructionBuilder initt1v() {
        InstructionBuilder b;
        b.write_u64(disc::INITT1V);
        return b;
    }

    /**
     * Initialize LP mint.
     * Accounts: [pool, lp_mint, authority(signer), system_program]
     */
    static InstructionBuilder initlpm() {
        InstructionBuilder b;
        b.write_u64(disc::INITLPM);
        return b;
    }

    // ========================================================================
    // Swap Instructions
    // ========================================================================

    /**
     * Generic swap with from/to indices.
     * Accounts: [pool, vault0, vault1, user_t0, user_t1, user(signer), token_program]
     */
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

    /**
     * Swap token 0 to token 1.
     * Accounts: [pool, vault0, vault1, user_t0, user_t1, user(signer), token_program]
     */
    static InstructionBuilder swapt0t1(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::SWAPT0T1);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    /**
     * Swap token 1 to token 0.
     * Accounts: [pool, vault0, vault1, user_t0, user_t1, user(signer), token_program]
     */
    static InstructionBuilder swapt1t0(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::SWAPT1T0);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    /**
     * Swap in N-token pool.
     * Accounts: [pool, vault_in, vault_out, user_in, user_out, user(signer), token_program]
     */
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

    /**
     * Migration swap t0 to t1 (1:1 with 0.1337% fee).
     * Accounts: same as swapt0t1
     */
    static InstructionBuilder migt0t1(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::MIGT0T1);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    /**
     * Migration swap t1 to t0 (1:1 with 0.1337% fee).
     * Accounts: same as swapt1t0
     */
    static InstructionBuilder migt1t0(uint64_t amount_in, uint64_t min_out) {
        InstructionBuilder b;
        b.write_u64(disc::MIGT1T0);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        return b;
    }

    // ========================================================================
    // Liquidity Instructions
    // ========================================================================

    /**
     * Add liquidity to 2-token pool.
     * Accounts: [pool, vault0, vault1, lp_mint, user_t0, user_t1, user_lp, user(signer), token_program]
     */
    static InstructionBuilder addliq(uint64_t amount0, uint64_t amount1, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQ);
        b.write_u64(amount0);
        b.write_u64(amount1);
        b.write_u64(min_lp);
        return b;
    }

    /**
     * Single-sided add liquidity.
     * Accounts: [pool, vault_in, lp_mint, user_in, user_lp, user(signer), token_program]
     */
    static InstructionBuilder addliq1(uint64_t amount_in, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQ1);
        b.write_u64(amount_in);
        b.write_u64(min_lp);
        return b;
    }

    /**
     * Add liquidity to N-token pool.
     * Accounts: [pool, vault0..vaultN, lp_mint, user_t0..user_tN, user_lp, user(signer), token_program]
     */
    static InstructionBuilder addliqn(const std::vector<uint64_t>& amounts, uint64_t min_lp) {
        InstructionBuilder b;
        b.write_u64(disc::ADDLIQN);
        for (auto amt : amounts) {
            b.write_u64(amt);
        }
        b.write_u64(min_lp);
        return b;
    }

    /**
     * Remove liquidity from 2-token pool.
     * Accounts: [pool, vault0, vault1, lp_mint, user_t0, user_t1, user_lp, user(signer), token_program]
     */
    static InstructionBuilder remliq(uint64_t lp_amount, uint64_t min0, uint64_t min1) {
        InstructionBuilder b;
        b.write_u64(disc::REMLIQ);
        b.write_u64(lp_amount);
        b.write_u64(min0);
        b.write_u64(min1);
        return b;
    }

    /**
     * Remove liquidity from N-token pool.
     * Accounts: [pool, vault0..vaultN, lp_mint, user_t0..user_tN, user_lp, user(signer), token_program]
     */
    static InstructionBuilder remliqn(uint64_t lp_amount, const std::vector<uint64_t>& mins) {
        InstructionBuilder b;
        b.write_u64(disc::REMLIQN);
        b.write_u64(lp_amount);
        for (auto m : mins) {
            b.write_u64(m);
        }
        return b;
    }

    // ========================================================================
    // Admin Instructions
    // ========================================================================

    /**
     * Pause/unpause pool.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder setpause(bool paused) {
        InstructionBuilder b;
        b.write_u64(disc::SETPAUSE);
        b.write_u8(paused ? 1 : 0);
        return b;
    }

    /**
     * Update swap fee.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder updfee(uint64_t fee_bps) {
        InstructionBuilder b;
        b.write_u64(disc::UPDFEE);
        b.write_u64(fee_bps);
        return b;
    }

    /**
     * Withdraw admin fees.
     * Accounts: [pool, vault0, vault1, dest0, dest1, authority(signer), token_program]
     */
    static InstructionBuilder wdrawfee() {
        InstructionBuilder b;
        b.write_u64(disc::WDRAWFEE);
        return b;
    }

    /**
     * Commit amp change (starts timelock).
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder commitamp(uint64_t target_amp) {
        InstructionBuilder b;
        b.write_u64(disc::COMMITAMP);
        b.write_u64(target_amp);
        return b;
    }

    /**
     * Start amp ramping (after timelock).
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder rampamp(uint64_t target_amp, int64_t duration) {
        InstructionBuilder b;
        b.write_u64(disc::RAMPAMP);
        b.write_u64(target_amp);
        b.write_i64(duration);
        return b;
    }

    /**
     * Stop amp ramping.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder stopramp() {
        InstructionBuilder b;
        b.write_u64(disc::STOPRAMP);
        return b;
    }

    /**
     * Initiate authority transfer.
     * Accounts: [pool, authority(signer), new_authority]
     */
    static InstructionBuilder initauth() {
        InstructionBuilder b;
        b.write_u64(disc::INITAUTH);
        return b;
    }

    /**
     * Complete authority transfer.
     * Accounts: [pool, new_authority(signer)]
     */
    static InstructionBuilder complauth() {
        InstructionBuilder b;
        b.write_u64(disc::COMPLAUTH);
        return b;
    }

    /**
     * Cancel authority transfer.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder cancelauth() {
        InstructionBuilder b;
        b.write_u64(disc::CANCELAUTH);
        return b;
    }

    // ========================================================================
    // Farming Instructions
    // ========================================================================

    /**
     * Create farming period.
     * Accounts: [farm, pool, reward_mint, authority(signer), system_program]
     */
    static InstructionBuilder createfarm(uint64_t reward_rate, int64_t start_time, int64_t end_time) {
        InstructionBuilder b;
        b.write_u64(disc::CREATEFARM);
        b.write_u64(reward_rate);
        b.write_i64(start_time);
        b.write_i64(end_time);
        return b;
    }

    /**
     * Stake LP tokens.
     * Accounts: [user_position, farm, user_lp, lp_vault, user(signer), token_program]
     */
    static InstructionBuilder stakelp(uint64_t amount) {
        InstructionBuilder b;
        b.write_u64(disc::STAKELP);
        b.write_u64(amount);
        return b;
    }

    /**
     * Unstake LP tokens.
     * Accounts: [user_position, farm, user_lp, lp_vault, user(signer), token_program]
     */
    static InstructionBuilder unstakelp(uint64_t amount) {
        InstructionBuilder b;
        b.write_u64(disc::UNSTAKELP);
        b.write_u64(amount);
        return b;
    }

    /**
     * Claim farming rewards.
     * Accounts: [user_position, farm, pool, reward_vault, user_reward, user(signer), token_program]
     */
    static InstructionBuilder claimfarm() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMFARM);
        return b;
    }

    /**
     * Lock LP tokens.
     * Accounts: [user_position, farm, user(signer), system_program]
     */
    static InstructionBuilder locklp(uint64_t amount, int64_t duration) {
        InstructionBuilder b;
        b.write_u64(disc::LOCKLP);
        b.write_u64(amount);
        b.write_i64(duration);
        return b;
    }

    /**
     * Claim unlocked LP.
     * Accounts: [user_position, farm, user(signer), system_program]
     */
    static InstructionBuilder claimulp() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMULP);
        return b;
    }

    // ========================================================================
    // Lottery Instructions
    // ========================================================================

    /**
     * Create lottery for pool.
     * Accounts: [lottery(writable), pool, lottery_vault, authority(signer), system_program]
     */
    static InstructionBuilder createlot(uint64_t ticket_price, int64_t end_time) {
        InstructionBuilder b;
        b.write_u64(disc::CREATELOT);
        b.write_u64(ticket_price);
        b.write_i64(end_time);
        return b;
    }

    /**
     * Enter lottery.
     * Accounts: [lottery, user_entry, user(signer), user_lp, lottery_vault, token_program]
     */
    static InstructionBuilder enterlot(uint64_t ticket_count) {
        InstructionBuilder b;
        b.write_u64(disc::ENTERLOT);
        b.write_u64(ticket_count);
        return b;
    }

    /**
     * Draw lottery winner.
     * Accounts: [lottery, authority(signer), recent_slothashes]
     */
    static InstructionBuilder drawlot(uint64_t random_seed) {
        InstructionBuilder b;
        b.write_u64(disc::DRAWLOT);
        b.write_u64(random_seed);
        return b;
    }

    /**
     * Claim lottery prize.
     * Accounts: [lottery, user_entry, user(signer), user_lp, lottery_vault, pool, token_program]
     */
    static InstructionBuilder claimlot() {
        InstructionBuilder b;
        b.write_u64(disc::CLAIMLOT);
        return b;
    }

    // ========================================================================
    // Registry Instructions
    // ========================================================================

    /**
     * Initialize pool registry.
     */
    static InstructionBuilder initreg() {
        InstructionBuilder b;
        b.write_u64(disc::INITREG);
        return b;
    }

    /**
     * Register pool in registry.
     */
    static InstructionBuilder regpool() {
        InstructionBuilder b;
        b.write_u64(disc::REGPOOL);
        return b;
    }

    /**
     * Unregister pool from registry.
     */
    static InstructionBuilder unregpool() {
        InstructionBuilder b;
        b.write_u64(disc::UNREGPOOL);
        return b;
    }

    /**
     * Initiate registry authority transfer.
     */
    static InstructionBuilder initrega() {
        InstructionBuilder b;
        b.write_u64(disc::INITREGA);
        return b;
    }

    /**
     * Complete registry authority transfer.
     */
    static InstructionBuilder complrega() {
        InstructionBuilder b;
        b.write_u64(disc::COMPLREGA);
        return b;
    }

    /**
     * Cancel registry authority transfer.
     */
    static InstructionBuilder cancelrega() {
        InstructionBuilder b;
        b.write_u64(disc::CANCELREGA);
        return b;
    }

    // ========================================================================
    // Oracle Instructions
    // ========================================================================

    /**
     * Get TWAP price.
     * Accounts: [pool]
     */
    static InstructionBuilder gettwap(TwapWindow window) {
        InstructionBuilder b;
        b.write_u64(disc::GETTWAP);
        b.write_u8(static_cast<uint8_t>(window));
        return b;
    }

    // ========================================================================
    // Circuit Breaker Instructions
    // ========================================================================

    /**
     * Configure circuit breaker parameters.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder setcb(uint64_t price_dev_bps, uint64_t volume_mult,
                                     uint64_t cooldown_slots, uint64_t auto_resume_slots) {
        InstructionBuilder b;
        b.write_u64(disc::SETCB);
        b.write_u64(price_dev_bps);
        b.write_u64(volume_mult);
        b.write_u64(cooldown_slots);
        b.write_u64(auto_resume_slots);
        return b;
    }

    /**
     * Reset triggered circuit breaker.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder resetcb() {
        InstructionBuilder b;
        b.write_u64(disc::RESETCB);
        return b;
    }

    // ========================================================================
    // Rate Limiting Instructions
    // ========================================================================

    /**
     * Configure rate limiting.
     * Accounts: [pool, authority(signer)]
     */
    static InstructionBuilder setrl(uint64_t max_vol, uint32_t max_swaps) {
        InstructionBuilder b;
        b.write_u64(disc::SETRL);
        b.write_u64(max_vol);
        b.write_u32(max_swaps);
        return b;
    }

    // ========================================================================
    // Governance Instructions
    // ========================================================================

    /**
     * Create governance proposal.
     */
    static InstructionBuilder govprop(ProposalType prop_type, uint64_t value,
                                       const std::string& description) {
        InstructionBuilder b;
        b.write_u64(disc::GOVPROP);
        b.write_u8(static_cast<uint8_t>(prop_type));
        b.write_u64(value);
        // Write description (max 64 bytes, null-padded)
        for (size_t i = 0; i < 64; i++) {
            if (i < description.size()) {
                b.write_u8(static_cast<uint8_t>(description[i]));
            } else {
                b.write_u8(0);
            }
        }
        return b;
    }

    /**
     * Vote on proposal.
     */
    static InstructionBuilder govvote(bool vote_for) {
        InstructionBuilder b;
        b.write_u64(disc::GOVVOTE);
        b.write_u8(vote_for ? 1 : 0);
        return b;
    }

    /**
     * Execute passed proposal.
     */
    static InstructionBuilder govexec() {
        InstructionBuilder b;
        b.write_u64(disc::GOVEXEC);
        return b;
    }

    /**
     * Cancel proposal.
     */
    static InstructionBuilder govcncl() {
        InstructionBuilder b;
        b.write_u64(disc::GOVCNCL);
        return b;
    }

    // ========================================================================
    // Orderbook Instructions
    // ========================================================================

    /**
     * Initialize orderbook for pool.
     */
    static InstructionBuilder initbook() {
        InstructionBuilder b;
        b.write_u64(disc::INITBOOK);
        return b;
    }

    /**
     * Place limit order.
     */
    static InstructionBuilder placeord(OrderType order_type, uint64_t price,
                                        uint64_t amount, int64_t expiry) {
        InstructionBuilder b;
        b.write_u64(disc::PLACEORD);
        b.write_u8(static_cast<uint8_t>(order_type));
        b.write_u64(price);
        b.write_u64(amount);
        b.write_i64(expiry);
        return b;
    }

    /**
     * Cancel limit order.
     */
    static InstructionBuilder cancelord(uint8_t order_index) {
        InstructionBuilder b;
        b.write_u64(disc::CANCELORD);
        b.write_u8(order_index);
        return b;
    }

    /**
     * Fill limit order (keeper).
     */
    static InstructionBuilder fillord(uint8_t order_index) {
        InstructionBuilder b;
        b.write_u64(disc::FILLORD);
        b.write_u8(order_index);
        return b;
    }

    // ========================================================================
    // Concentrated Liquidity Instructions
    // ========================================================================

    /**
     * Initialize CL pool extension.
     */
    static InstructionBuilder initclpl() {
        InstructionBuilder b;
        b.write_u64(disc::INITCLPL);
        return b;
    }

    /**
     * Mint CL position (add liquidity to range).
     */
    static InstructionBuilder clmint(int16_t tick_lower, int16_t tick_upper,
                                      uint64_t amount0, uint64_t amount1) {
        InstructionBuilder b;
        b.write_u64(disc::CLMINT);
        b.write_i16(tick_lower);
        b.write_i16(tick_upper);
        b.write_u64(amount0);
        b.write_u64(amount1);
        return b;
    }

    /**
     * Burn CL position (remove liquidity).
     */
    static InstructionBuilder clburn(uint64_t liquidity) {
        InstructionBuilder b;
        b.write_u64(disc::CLBURN);
        b.write_u64(liquidity);
        return b;
    }

    /**
     * Collect accumulated CL fees.
     */
    static InstructionBuilder clcollect() {
        InstructionBuilder b;
        b.write_u64(disc::CLCOLLECT);
        return b;
    }

    /**
     * Swap through concentrated liquidity.
     */
    static InstructionBuilder clswap(uint64_t amount_in, uint64_t min_out, bool zero_for_one) {
        InstructionBuilder b;
        b.write_u64(disc::CLSWAP);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        b.write_u8(zero_for_one ? 1 : 0);
        return b;
    }

    // ========================================================================
    // Flash Loan Instructions
    // ========================================================================

    /**
     * Initiate flash loan.
     */
    static InstructionBuilder flashloan(uint64_t amount0, uint64_t amount1) {
        InstructionBuilder b;
        b.write_u64(disc::FLASHLOAN);
        b.write_u64(amount0);
        b.write_u64(amount1);
        return b;
    }

    /**
     * Flash loan repay callback.
     */
    static InstructionBuilder flashrepy() {
        InstructionBuilder b;
        b.write_u64(disc::FLASHREPY);
        return b;
    }

    // ========================================================================
    // Multi-hop Instructions
    // ========================================================================

    /**
     * Multi-pool swap route (2-4 hops).
     */
    static InstructionBuilder multihop(uint64_t amount_in, uint64_t min_out, int64_t deadline,
                                        const std::vector<uint8_t>& directions) {
        InstructionBuilder b;
        b.write_u64(disc::MULTIHOP);
        b.write_u64(amount_in);
        b.write_u64(min_out);
        b.write_i64(deadline);
        b.write_u8(static_cast<uint8_t>(directions.size()));
        for (auto d : directions) {
            b.write_u8(d);
        }
        return b;
    }

    // ========================================================================
    // ML Brain Instructions
    // ========================================================================

    /**
     * Initialize ML brain for pool.
     */
    static InstructionBuilder initml(bool is_stable, uint16_t min_fee, uint16_t max_fee,
                                      uint16_t min_amp, uint16_t max_amp,
                                      uint16_t fee_step, uint16_t amp_step) {
        InstructionBuilder b;
        b.write_u64(disc::INITML);
        b.write_u8(is_stable ? 1 : 0);
        b.write_u16(min_fee);
        b.write_u16(max_fee);
        b.write_u16(min_amp);
        b.write_u16(max_amp);
        b.write_u16(fee_step);
        b.write_u16(amp_step);
        return b;
    }

    /**
     * Configure ML brain parameters.
     */
    static InstructionBuilder cfgml(bool enabled, bool auto_apply) {
        InstructionBuilder b;
        b.write_u64(disc::CFGML);
        b.write_u8(enabled ? 1 : 0);
        b.write_u8(auto_apply ? 1 : 0);
        return b;
    }

    /**
     * Batch Q-learning training (bot-triggered).
     */
    static InstructionBuilder trainml() {
        InstructionBuilder b;
        b.write_u64(disc::TRAINML);
        return b;
    }

    /**
     * Apply ML-suggested action manually.
     */
    static InstructionBuilder applyml(MLAction action) {
        InstructionBuilder b;
        b.write_u64(disc::APPLYML);
        b.write_u8(static_cast<uint8_t>(action));
        return b;
    }

    /**
     * Log ML state for monitoring.
     */
    static InstructionBuilder logml() {
        InstructionBuilder b;
        b.write_u64(disc::LOGML);
        return b;
    }

    // ========================================================================
    // Transfer Hook Instructions
    // ========================================================================

    /**
     * Transfer hook execute (called on every LP transfer).
     */
    static InstructionBuilder th_exec() {
        InstructionBuilder b;
        b.write_u64(disc::TH_EXEC);
        return b;
    }

    /**
     * Transfer hook init (initialize ExtraAccountMetaList).
     */
    static InstructionBuilder th_init() {
        InstructionBuilder b;
        b.write_u64(disc::TH_INIT);
        return b;
    }

private:
    std::vector<uint8_t> data_;

    void write_u8(uint8_t v) {
        data_.push_back(v);
    }

    void write_u16(uint16_t v) {
        data_.push_back(static_cast<uint8_t>(v & 0xFF));
        data_.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    }

    void write_i16(int16_t v) {
        write_u16(static_cast<uint16_t>(v));
    }

    void write_u32(uint32_t v) {
        for (int i = 0; i < 4; i++) {
            data_.push_back(static_cast<uint8_t>(v & 0xFF));
            v >>= 8;
        }
    }

    void write_u64(uint64_t v) {
        for (int i = 0; i < 8; i++) {
            data_.push_back(static_cast<uint8_t>(v & 0xFF));
            v >>= 8;
        }
    }

    void write_i64(int64_t v) {
        write_u64(static_cast<uint64_t>(v));
    }

    void write_pubkey(const Pubkey& pk) {
        for (auto b : pk) {
            data_.push_back(b);
        }
    }
};

}  // namespace aex402
