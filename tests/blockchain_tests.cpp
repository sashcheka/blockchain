#include "blockchain/block.hpp"
#include "blockchain/blockchain.hpp"
#include "blockchain/hash.hpp"
#include "blockchain/transaction.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
        ++failures;
    }
}

#define CHECK(expression) check((expression), #expression, __FILE__, __LINE__)

template <typename Callable>
void checkThrows(Callable&& callable, const char* expression, const char* file, int line) {
    try {
        std::forward<Callable>(callable)();
        std::cerr << file << ':' << line << ": expected exception: " << expression << '\n';
        ++failures;
    } catch (const std::exception&) {
    }
}

#define CHECK_THROWS(expression) checkThrows([&] { (void)(expression); }, #expression, __FILE__, __LINE__)

void testSha256KnownVector() {
    CHECK(blockchain::Hash::sha256("abc") ==
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

void testTransactionValidation() {
    CHECK_THROWS(blockchain::Transaction("", "bob", 1));
    CHECK_THROWS(blockchain::Transaction("alice", "", 1));
    CHECK_THROWS(blockchain::Transaction("alice", "bob", 0));
    CHECK_THROWS(blockchain::Transaction("alice", "bob", -1));
}

void testTransactionFieldsHaveUnambiguousEncoding() {
    const blockchain::Transaction first("ab", "c", 1);
    const blockchain::Transaction second("a", "bc", 1);
    CHECK(first.serialize() != second.serialize());
}

void testEveryTransactionAffectsBlockHash() {
    const blockchain::Transaction first("alice", "bob", 10, "first");
    const blockchain::Transaction second("bob", "carol", 5, "second");
    const std::string parentHash(64, '0');
    blockchain::Block one(0, 123, parentHash, {first}, 0);
    blockchain::Block both(0, 123, parentHash, {first, second}, 0);
    CHECK(one.calculateHash(0) != both.calculateHash(0));
}

void testProofOfWorkAndContextValidation() {
    const std::string parentHash(64, '0');
    blockchain::Block block(0, 123, parentHash, {blockchain::Transaction("alice", "bob", 1)}, 1);
    CHECK(!block.isValid(0, parentHash, 1));
    CHECK(block.mine());
    CHECK(block.hash().front() == '0');
    CHECK(block.isValid(0, parentHash, 1));
    CHECK(!block.isValid(1, parentHash, 1));
    CHECK(!block.isValid(0, std::string(63, '0') + '1', 1));
}

void testBlockchainLifecycle() {
    blockchain::Blockchain chain(1);
    CHECK(chain.isValid());
    CHECK(!chain.minePendingTransactions());

    chain.addTransaction(blockchain::Transaction("alice", "bob", 7, "coffee"));
    chain.addTransaction(blockchain::Transaction("bob", "carol", 2));
    CHECK(chain.pendingCount() == 2);
    CHECK(chain.minePendingTransactions());
    CHECK(chain.pendingCount() == 0);
    CHECK(chain.blocks().size() == 1);
    CHECK(chain.blocks().front().transactions().size() == 2);
    CHECK(chain.isValid());

    chain.addTransaction(blockchain::Transaction("carol", "dave", 1));
    CHECK(chain.minePendingTransactions());
    CHECK(chain.blocks().size() == 2);
    CHECK(chain.blocks()[1].previousHash() == chain.blocks()[0].hash());
    CHECK(chain.isValid());
}

void testDifficultyIsBounded() {
    CHECK_THROWS(blockchain::Blockchain(7));
    CHECK_THROWS(blockchain::Block(0, 123, "genesis", {}, 7));
}

} // namespace

int main() {
    testSha256KnownVector();
    testTransactionValidation();
    testTransactionFieldsHaveUnambiguousEncoding();
    testEveryTransactionAffectsBlockHash();
    testProofOfWorkAndContextValidation();
    testBlockchainLifecycle();
    testDifficultyIsBounded();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }
    std::cout << "All blockchain tests passed.\n";
    return 0;
}
