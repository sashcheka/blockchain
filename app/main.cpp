#include "blockchain/blockchain.hpp"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace {

void printHelp() {
    std::cout << "Commands:\n"
                 "  send <sender> <recipient> <amount> [data]  Add a transaction\n"
                 "  mine                                        Mine pending transactions\n"
                 "  chain                                       Print the chain\n"
                 "  verify                                      Validate the chain\n"
                 "  help                                        Show this help\n"
                 "  exit                                        Quit\n"
                 "\nAmounts are positive integer units. This demo does not implement balances or signatures.\n";
}

bool parseAmount(const std::string& text, std::int64_t& amount) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, amount);
    return result.ec == std::errc{} && result.ptr == end;
}

void printChain(const blockchain::Blockchain& chain) {
    if (chain.blocks().empty()) {
        std::cout << "The chain is empty.\n";
        return;
    }

    for (const auto& block : chain.blocks()) {
        std::cout << "Block #" << block.index() << " | timestamp: " << block.timestamp()
                  << " | nonce: " << block.nonce() << " | transactions: " << block.transactions().size()
                  << "\n  previous: " << block.previousHash() << "\n  hash:     " << block.hash() << '\n';
        for (const auto& transaction : block.transactions()) {
            std::cout << "  " << transaction.sender() << " -> " << transaction.recipient()
                      << " : " << transaction.amount();
            if (!transaction.data().empty()) {
                std::cout << " (" << transaction.data() << ')';
            }
            std::cout << '\n';
        }
    }
}

void handleSend(const std::string& line, blockchain::Blockchain& chain) {
    std::istringstream input(line);
    std::string command;
    std::string sender;
    std::string recipient;
    std::string amountText;
    input >> command >> sender >> recipient >> amountText;

    std::int64_t amount = 0;
    if (sender.empty() || recipient.empty() || amountText.empty() || !parseAmount(amountText, amount)) {
        std::cout << "Usage: send <sender> <recipient> <positive integer amount> [data]\n";
        return;
    }

    std::string data;
    std::getline(input >> std::ws, data);
    try {
        chain.addTransaction(blockchain::Transaction(sender, recipient, amount, data));
        std::cout << "Transaction added. Pending: " << chain.pendingCount() << '\n';
    } catch (const std::invalid_argument& error) {
        std::cout << "Invalid transaction: " << error.what() << '\n';
    }
}

} // namespace

int main() {
    blockchain::Blockchain chain;
    printHelp();

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command.empty()) {
            continue;
        }
        if (command == "exit") {
            break;
        }
        if (command == "help") {
            printHelp();
        } else if (command == "send") {
            handleSend(line, chain);
        } else if (command == "mine") {
            if (!chain.minePendingTransactions()) {
                std::cout << "There are no pending transactions.\n";
            } else {
                std::cout << "Block mined. Chain length: " << chain.blocks().size() << '\n';
            }
        } else if (command == "chain") {
            printChain(chain);
            std::cout << "Pending transactions: " << chain.pendingCount() << '\n';
        } else if (command == "verify") {
            std::cout << (chain.isValid() ? "Chain is valid.\n" : "Chain is INVALID.\n");
        } else {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
    return 0;
}
