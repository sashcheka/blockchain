#include "blockchain/transaction.hpp"

#include <stdexcept>
#include <utility>

namespace blockchain {
namespace {

void appendU64(std::string& output, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        output.push_back(static_cast<char>((value >> shift) & 0xffU));
    }
}

void appendField(std::string& output, const std::string& value) {
    appendU64(output, static_cast<std::uint64_t>(value.size()));
    output.append(value);
}

} // namespace

Transaction::Transaction(std::string sender, std::string recipient, std::int64_t amount, std::string data)
    : sender_(std::move(sender)),
      recipient_(std::move(recipient)),
      amount_(amount),
      data_(std::move(data)) {
    if (sender_.empty()) {
        throw std::invalid_argument("sender must not be empty");
    }
    if (recipient_.empty()) {
        throw std::invalid_argument("recipient must not be empty");
    }
    if (amount_ <= 0) {
        throw std::invalid_argument("amount must be greater than zero");
    }
}

std::string Transaction::serialize() const {
    std::string result;
    result.reserve(sender_.size() + recipient_.size() + data_.size() + 32);
    result.append("TXv1", 4);
    appendField(result, sender_);
    appendField(result, recipient_);
    appendU64(result, static_cast<std::uint64_t>(amount_));
    appendField(result, data_);
    return result;
}

} // namespace blockchain
