#pragma once

#include <string>
#include <string_view>

namespace blockchain {

class Hash {
public:
    static std::string sha256(std::string_view input);
};

} // namespace blockchain
