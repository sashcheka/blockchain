#pragma once

#include <cstdint>
#include <string>

namespace blockchain {

class Transaction {
public:
    Transaction(std::string sender, std::string recipient, std::int64_t amount, std::string data = {});

    const std::string& sender() const noexcept { return sender_; }
    const std::string& recipient() const noexcept { return recipient_; }
    std::int64_t amount() const noexcept { return amount_; }
    const std::string& data() const noexcept { return data_; }

    std::string serialize() const;

private:
    std::string sender_;
    std::string recipient_;
    std::int64_t amount_;
    std::string data_;
};

} // namespace blockchain
