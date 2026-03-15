#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <functional>

namespace Sapphire {
namespace Blockchain {

// ===== SHA-256 =====
std::string sha256(const std::string& data);
std::string sha256_double(const std::string& data);

// ===== Merkle Tree =====
struct MerkleTree {
    std::vector<std::string> leaves;
    std::string root;

    void build(const std::vector<std::string>& data);
    std::string compute_root(std::vector<std::string> hashes) const;
    bool verify(const std::string& leaf, const std::vector<std::string>& proof) const;
};

// ===== Transaction =====
struct Transaction {
    std::string id;
    std::string sender;
    std::string receiver;
    double amount;
    uint64_t timestamp;
    std::string signature;
    std::string data; // arbitrary payload

    std::string to_string() const;
    std::string hash() const;
};

// ===== Block =====
struct Block {
    uint64_t index;
    std::string previous_hash;
    std::string hash;
    std::string merkle_root;
    uint64_t timestamp;
    uint64_t nonce;
    uint32_t difficulty;
    std::vector<Transaction> transactions;
    std::string miner;

    Block();
    Block(uint64_t idx, const std::string& prev_hash, uint32_t diff);

    std::string compute_hash() const;
    void mine();  // PoW
    bool is_valid() const;
    std::string to_string() const;
    int transaction_count() const { return static_cast<int>(transactions.size()); }
};

// ===== Consensus =====
enum class ConsensusType { PoW, PoS };

struct Validator {
    std::string address;
    double stake;
    uint64_t last_block;
};

// ===== Blockchain =====
class Chain {
public:
    Chain(uint32_t difficulty = 2, ConsensusType consensus = ConsensusType::PoW);
    ~Chain();

    // Block operations
    Block create_genesis();
    Block create_block(const std::string& miner = "");
    bool add_block(Block& block);
    bool validate_chain() const;

    // Transactions
    std::string add_transaction(const std::string& sender,
                                const std::string& receiver,
                                double amount,
                                const std::string& data = "");
    int pending_count() const { return static_cast<int>(pending_tx_.size()); }

    // Queries
    const Block* get_block(uint64_t index) const;
    const Block* get_block_by_hash(const std::string& hash) const;
    int block_count() const { return static_cast<int>(chain_.size()); }
    double get_balance(const std::string& address) const;
    std::string last_hash() const;
    uint32_t difficulty() const { return difficulty_; }

    // PoS validators
    void add_validator(const std::string& address, double stake);
    std::string select_validator() const; // PoS leader selection

    // Stats
    std::string chain_stats() const;

private:
    std::vector<Block> chain_;
    std::vector<Transaction> pending_tx_;
    uint32_t difficulty_;
    ConsensusType consensus_;
    std::vector<Validator> validators_;
    uint64_t tx_counter_;

    std::string generate_tx_id();
};

// ===== P2P Node (simulated) =====
struct PeerNode {
    std::string node_id;
    std::string address;
    int port;
    bool connected;
};

class P2PNetwork {
public:
    P2PNetwork();
    ~P2PNetwork();

    std::string add_node(const std::string& address, int port);
    bool connect(const std::string& node_id);
    bool disconnect(const std::string& node_id);
    bool broadcast_block(const std::string& node_id, const std::string& block_hash);
    bool broadcast_tx(const std::string& node_id, const std::string& tx_id);
    int node_count() const { return static_cast<int>(nodes_.size()); }
    int connected_count() const;
    std::string network_status() const;

private:
    std::vector<PeerNode> nodes_;
    int next_port_;
};

} // namespace Blockchain
} // namespace Sapphire

// ===== C API =====
extern "C" {

// SHA-256
const char* bc_sha256(const char* data);
const char* bc_sha256_double(const char* data);

// Merkle Tree
void* bc_merkle_create();
void  bc_merkle_destroy(void* tree);
void  bc_merkle_add_leaf(void* tree, const char* data);
const char* bc_merkle_build(void* tree);
const char* bc_merkle_root(void* tree);
int   bc_merkle_leaf_count(void* tree);

// Transaction
void* bc_tx_create(const char* sender, const char* receiver, double amount, const char* data);
void  bc_tx_destroy(void* tx);
const char* bc_tx_id(void* tx);
const char* bc_tx_hash(void* tx);
const char* bc_tx_to_string(void* tx);

// Block
void* bc_block_create(int index, const char* prev_hash, int difficulty);
void  bc_block_destroy(void* block);
void  bc_block_add_tx(void* block, void* tx);
void  bc_block_mine(void* block);
const char* bc_block_hash(void* block);
const char* bc_block_prev_hash(void* block);
int   bc_block_index(void* block);
int   bc_block_nonce(void* block);
int   bc_block_tx_count(void* block);
int   bc_block_is_valid(void* block);
const char* bc_block_to_string(void* block);
const char* bc_block_merkle_root(void* block);

// Chain
void* bc_chain_create(int difficulty, int consensus); // 0=PoW, 1=PoS
void  bc_chain_destroy(void* chain);
const char* bc_chain_add_tx(void* chain, const char* sender, const char* receiver, double amount, const char* data);
int   bc_chain_mine_block(void* chain, const char* miner);
int   bc_chain_block_count(void* chain);
int   bc_chain_pending_count(void* chain);
int   bc_chain_is_valid(void* chain);
double bc_chain_get_balance(void* chain, const char* address);
const char* bc_chain_last_hash(void* chain);
const char* bc_chain_get_block_hash(void* chain, int index);
const char* bc_chain_stats(void* chain);
void  bc_chain_add_validator(void* chain, const char* address, double stake);
const char* bc_chain_select_validator(void* chain);

// P2P Network
void* bc_p2p_create();
void  bc_p2p_destroy(void* net);
const char* bc_p2p_add_node(void* net, const char* address, int port);
int   bc_p2p_connect(void* net, const char* node_id);
int   bc_p2p_disconnect(void* net, const char* node_id);
int   bc_p2p_broadcast_block(void* net, const char* node_id, const char* block_hash);
int   bc_p2p_broadcast_tx(void* net, const char* node_id, const char* tx_id);
int   bc_p2p_node_count(void* net);
int   bc_p2p_connected_count(void* net);
const char* bc_p2p_status(void* net);

} // extern "C"
