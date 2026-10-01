#include "blockchain/hash.hpp"

#include <openssl/evp.h>

#include <array>
#include <stdexcept>

namespace blockchain {

std::string Hash::sha256(std::string_view input) {
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
    unsigned int digestLength = 0;

    if (EVP_Digest(input.data(), input.size(), digest.data(), &digestLength, EVP_sha256(), nullptr) != 1) {
        throw std::runtime_error("OpenSSL could not calculate SHA-256");
    }

    static constexpr char hex[] = "0123456789abcdef";
    std::string result;
    result.reserve(static_cast<std::size_t>(digestLength) * 2);
    for (unsigned int i = 0; i < digestLength; ++i) {
        result.push_back(hex[digest[i] >> 4]);
        result.push_back(hex[digest[i] & 0x0f]);
    }
    return result;
}

} // namespace blockchain
