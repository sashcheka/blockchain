#pragma once

#include "blockchain/transaction.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace blockchain {

class Block {
public:
    Block(std::uint64_t index,
          std::int64_t timestamp,
          std::string previousHash,
          std::vector<Transaction> transactions,
          unsigned difficulty);

    bool mine(std::uint64_t maxNonce = std::numeric_limits<std::uint64_t>::max());
    bool isValid(std::uint64_t expectedIndex,
                 const std::string& expectedPreviousHash,
                 unsigned expectedDifficulty) const;
    std::string calculateHash(std::uint64_t nonce) const;

    std::uint64_t index() const noexcept { return index_; }
    std::int64_t timestamp() const noexcept { return timestamp_; }
    const std::string& previousHash() const noexcept { return previousHash_; }
    const std::string& hash() const noexcept { return hash_; }
    std::uint64_t nonce() const noexcept { return nonce_; }
    unsigned difficulty() const noexcept { return difficulty_; }
    const std::vector<Transaction>& transactions() const noexcept { return transactions_; }

private:
    std::uint64_t index_;
    std::int64_t timestamp_;
    std::string previousHash_;
    std::vector<Transaction> transactions_;
    unsigned difficulty_;
    std::uint64_t nonce_ = 0;
    std::string hash_;
};

} // namespace blockchain
