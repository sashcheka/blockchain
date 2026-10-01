#pragma once

#include "blockchain/block.hpp"

#include <cstdint>
#include <vector>

namespace blockchain {

class Blockchain {
public:
    explicit Blockchain(unsigned difficulty = 2);

    void addTransaction(Transaction transaction);
    bool minePendingTransactions();
    bool isValid() const;

    unsigned difficulty() const noexcept { return difficulty_; }
    std::size_t pendingCount() const noexcept { return pendingTransactions_.size(); }
    const std::vector<Transaction>& pendingTransactions() const noexcept { return pendingTransactions_; }
    const std::vector<Block>& blocks() const noexcept { return blocks_; }

private:
    unsigned difficulty_;
    std::vector<Block> blocks_;
    std::vector<Transaction> pendingTransactions_;
};

} // namespace blockchain
