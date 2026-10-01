#include "blockchain/blockchain.hpp"

#include <chrono>
#include <stdexcept>
#include <utility>

namespace blockchain {
namespace {

constexpr char kGenesisPreviousHash[] = "0000000000000000000000000000000000000000000000000000000000000000";

std::int64_t currentTimestamp() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

} // namespace

Blockchain::Blockchain(unsigned difficulty) : difficulty_(difficulty) {
    if (difficulty_ > 6) {
        throw std::invalid_argument("difficulty must be between 0 and 6 hexadecimal zeroes");
    }
}

void Blockchain::addTransaction(Transaction transaction) {
    pendingTransactions_.push_back(std::move(transaction));
}

bool Blockchain::minePendingTransactions() {
    if (pendingTransactions_.empty()) {
        return false;
    }

    const std::uint64_t index = static_cast<std::uint64_t>(blocks_.size());
    const std::string& previousHash = blocks_.empty() ? kGenesisPreviousHash : blocks_.back().hash();
    Block candidate(index, currentTimestamp(), previousHash, pendingTransactions_, difficulty_);
    if (!candidate.mine()) {
        return false;
    }

    blocks_.push_back(std::move(candidate));
    pendingTransactions_.clear();
    return true;
}

bool Blockchain::isValid() const {
    std::string expectedPreviousHash = kGenesisPreviousHash;
    for (std::size_t i = 0; i < blocks_.size(); ++i) {
        const auto expectedIndex = static_cast<std::uint64_t>(i);
        const Block& block = blocks_[i];
        if (!block.isValid(expectedIndex, expectedPreviousHash, difficulty_)) {
            return false;
        }
        expectedPreviousHash = block.hash();
    }
    return true;
}

} // namespace blockchain
