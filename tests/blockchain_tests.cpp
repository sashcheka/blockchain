#include "blockchain/block.hpp"
#include "blockchain/blockchain.hpp"
#include "blockchain/hash.hpp"
#include "blockchain/transaction.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

TEST(HashTest, MatchesSha256KnownVector) {
    EXPECT_EQ(blockchain::Hash::sha256("abc"),
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(TransactionTest, RejectsInvalidFields) {
    EXPECT_THROW(blockchain::Transaction("", "bob", 1), std::invalid_argument);
    EXPECT_THROW(blockchain::Transaction("alice", "", 1), std::invalid_argument);
    EXPECT_THROW(blockchain::Transaction("alice", "bob", 0), std::invalid_argument);
    EXPECT_THROW(blockchain::Transaction("alice", "bob", -1), std::invalid_argument);
}

TEST(TransactionTest, EncodesFieldsUnambiguously) {
    const blockchain::Transaction first("ab", "c", 1);
    const blockchain::Transaction second("a", "bc", 1);
    EXPECT_NE(first.serialize(), second.serialize());
}

TEST(BlockTest, EveryTransactionAffectsBlockHash) {
    const blockchain::Transaction first("alice", "bob", 10, "first");
    const blockchain::Transaction second("bob", "carol", 5, "second");
    const std::string parentHash(64, '0');
    blockchain::Block one(0, 123, parentHash, {first}, 0);
    blockchain::Block both(0, 123, parentHash, {first, second}, 0);
    EXPECT_NE(one.calculateHash(0), both.calculateHash(0));
}

TEST(BlockTest, MinesAndValidatesProofOfWorkAndContext) {
    const std::string parentHash(64, '0');
    blockchain::Block block(0, 123, parentHash, {blockchain::Transaction("alice", "bob", 1)}, 1);
    EXPECT_FALSE(block.isValid(0, parentHash, 1));
    ASSERT_TRUE(block.mine());
    EXPECT_EQ(block.hash().front(), '0');
    EXPECT_TRUE(block.isValid(0, parentHash, 1));
    EXPECT_FALSE(block.isValid(1, parentHash, 1));
    EXPECT_FALSE(block.isValid(0, std::string(63, '0') + '1', 1));
}

TEST(BlockchainTest, MinesPendingTransactionsAndLinksBlocks) {
    blockchain::Blockchain chain(1);
    EXPECT_TRUE(chain.isValid());
    EXPECT_FALSE(chain.minePendingTransactions());

    chain.addTransaction(blockchain::Transaction("alice", "bob", 7, "coffee"));
    chain.addTransaction(blockchain::Transaction("bob", "carol", 2));
    EXPECT_EQ(chain.pendingCount(), 2);
    ASSERT_TRUE(chain.minePendingTransactions());
    EXPECT_EQ(chain.pendingCount(), 0);
    ASSERT_EQ(chain.blocks().size(), 1);
    EXPECT_EQ(chain.blocks().front().transactions().size(), 2);
    EXPECT_TRUE(chain.isValid());

    chain.addTransaction(blockchain::Transaction("carol", "dave", 1));
    ASSERT_TRUE(chain.minePendingTransactions());
    ASSERT_EQ(chain.blocks().size(), 2);
    EXPECT_EQ(chain.blocks()[1].previousHash(), chain.blocks()[0].hash());
    EXPECT_TRUE(chain.isValid());
}

TEST(BlockchainTest, BoundsMiningDifficulty) {
    EXPECT_THROW(blockchain::Blockchain(7), std::invalid_argument);
    EXPECT_THROW(blockchain::Block(0, 123, "genesis", {}, 7), std::invalid_argument);
}
