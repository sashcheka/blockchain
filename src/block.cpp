#include "blockchain/block.hpp"

#include "blockchain/hash.hpp"

#include <stdexcept>
#include <utility>

namespace blockchain {
namespace {

constexpr unsigned kMaxDifficulty = 6;

void appendU64(std::string& output, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        output.push_back(static_cast<char>((value >> shift) & 0xffU));
    }
}

void appendField(std::string& output, const std::string& value) {
    appendU64(output, static_cast<std::uint64_t>(value.size()));
    output.append(value);
}

std::string transactionDigest(const std::vector<Transaction>& transactions) {
    std::string serialized;
    serialized.append("TXLISTv1", 8);
    appendU64(serialized, static_cast<std::uint64_t>(transactions.size()));
    for (const auto& transaction : transactions) {
        appendField(serialized, transaction.serialize());
    }
    return Hash::sha256(serialized);
}

} // namespace

Block::Block(std::uint64_t index,
             std::int64_t timestamp,
             std::string previousHash,
             std::vector<Transaction> transactions,
             unsigned difficulty)
    : index_(index),
      timestamp_(timestamp),
      previousHash_(std::move(previousHash)),
      transactions_(std::move(transactions)),
      difficulty_(difficulty) {
    if (difficulty_ > kMaxDifficulty) {
        throw std::invalid_argument("difficulty must be between 0 and 6 hexadecimal zeroes");
    }
    if (previousHash_.empty()) {
        throw std::invalid_argument("previous hash must not be empty");
    }
    if (timestamp_ < 0) {
        throw std::invalid_argument("timestamp must not be negative");
    }
}

std::string Block::calculateHash(std::uint64_t nonce) const {
    std::string header;
    header.reserve(previousHash_.size() + 128);
    header.append("BLOCKv1", 7);
    appendU64(header, index_);
    appendU64(header, static_cast<std::uint64_t>(timestamp_));
    appendField(header, previousHash_);
    appendField(header, transactionDigest(transactions_));
    appendU64(header, difficulty_);
    appendU64(header, nonce);
    return Hash::sha256(header);
}

bool Block::mine(std::uint64_t maxNonce) {
    const std::string target(difficulty_, '0');
    for (std::uint64_t candidate = 0;; ++candidate) {
        const std::string candidateHash = calculateHash(candidate);
        if (candidateHash.compare(0, target.size(), target) == 0) {
            nonce_ = candidate;
            hash_ = candidateHash;
            return true;
        }
        if (candidate == maxNonce) {
            return false;
        }
    }
}

bool Block::isValid(std::uint64_t expectedIndex,
                    const std::string& expectedPreviousHash,
                    unsigned expectedDifficulty) const {
    if (index_ != expectedIndex || previousHash_ != expectedPreviousHash || difficulty_ != expectedDifficulty) {
        return false;
    }
    if (hash_.empty() || hash_ != calculateHash(nonce_)) {
        return false;
    }
    return hash_.compare(0, difficulty_, std::string(difficulty_, '0')) == 0;
}

} // namespace blockchain
