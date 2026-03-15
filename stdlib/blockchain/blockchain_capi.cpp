#include "blockchain.h"
#include <cstring>
#include <string>
#include <chrono>

extern "C" {

static std::string g_bc_last_string;

// ===== SHA-256 =====

const char* bc_sha256(const char* data) {
    g_bc_last_string = Sapphire::Blockchain::sha256(data ? data : "");
    return g_bc_last_string.c_str();
}

const char* bc_sha256_double(const char* data) {
    g_bc_last_string = Sapphire::Blockchain::sha256_double(data ? data : "");
    return g_bc_last_string.c_str();
}

// ===== Merkle Tree =====

void* bc_merkle_create() {
    return new Sapphire::Blockchain::MerkleTree();
}

void bc_merkle_destroy(void* tree) {
    delete static_cast<Sapphire::Blockchain::MerkleTree*>(tree);
}

void bc_merkle_add_leaf(void* tree, const char* data) {
    auto* t = static_cast<Sapphire::Blockchain::MerkleTree*>(tree);
    t->leaves.push_back(Sapphire::Blockchain::sha256(data ? data : ""));
}

const char* bc_merkle_build(void* tree) {
    auto* t = static_cast<Sapphire::Blockchain::MerkleTree*>(tree);
    t->root = t->compute_root(t->leaves);
    g_bc_last_string = t->root;
    return g_bc_last_string.c_str();
}

const char* bc_merkle_root(void* tree) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::MerkleTree*>(tree)->root;
    return g_bc_last_string.c_str();
}

int bc_merkle_leaf_count(void* tree) {
    return static_cast<int>(
        static_cast<Sapphire::Blockchain::MerkleTree*>(tree)->leaves.size());
}

// ===== Transaction =====

void* bc_tx_create(const char* sender, const char* receiver, double amount, const char* data) {
    auto* tx = new Sapphire::Blockchain::Transaction();
    tx->sender    = sender   ? sender   : "";
    tx->receiver  = receiver ? receiver : "";
    tx->amount    = amount;
    tx->data      = data     ? data     : "";
    tx->timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    tx->id = "tx_standalone_" + std::to_string(tx->timestamp);
    return tx;
}

void bc_tx_destroy(void* tx) {
    delete static_cast<Sapphire::Blockchain::Transaction*>(tx);
}

const char* bc_tx_id(void* tx) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Transaction*>(tx)->id;
    return g_bc_last_string.c_str();
}

const char* bc_tx_hash(void* tx) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Transaction*>(tx)->hash();
    return g_bc_last_string.c_str();
}

const char* bc_tx_to_string(void* tx) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Transaction*>(tx)->to_string();
    return g_bc_last_string.c_str();
}

// ===== Block =====

void* bc_block_create(int index, const char* prev_hash, int difficulty) {
    return new Sapphire::Blockchain::Block(
        static_cast<uint64_t>(index),
        prev_hash ? prev_hash : std::string(64, '0'),
        static_cast<uint32_t>(difficulty));
}

void bc_block_destroy(void* block) {
    delete static_cast<Sapphire::Blockchain::Block*>(block);
}

void bc_block_add_tx(void* block, void* tx) {
    auto* b = static_cast<Sapphire::Blockchain::Block*>(block);
    auto* t = static_cast<Sapphire::Blockchain::Transaction*>(tx);
    b->transactions.push_back(*t);
}

void bc_block_mine(void* block) {
    static_cast<Sapphire::Blockchain::Block*>(block)->mine();
}

const char* bc_block_hash(void* block) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Block*>(block)->hash;
    return g_bc_last_string.c_str();
}

const char* bc_block_prev_hash(void* block) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Block*>(block)->previous_hash;
    return g_bc_last_string.c_str();
}

int bc_block_index(void* block) {
    return static_cast<int>(static_cast<Sapphire::Blockchain::Block*>(block)->index);
}

int bc_block_nonce(void* block) {
    return static_cast<int>(static_cast<Sapphire::Blockchain::Block*>(block)->nonce);
}

int bc_block_tx_count(void* block) {
    return static_cast<Sapphire::Blockchain::Block*>(block)->transaction_count();
}

int bc_block_is_valid(void* block) {
    return static_cast<Sapphire::Blockchain::Block*>(block)->is_valid() ? 1 : 0;
}

const char* bc_block_to_string(void* block) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Block*>(block)->to_string();
    return g_bc_last_string.c_str();
}

const char* bc_block_merkle_root(void* block) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Block*>(block)->merkle_root;
    return g_bc_last_string.c_str();
}

// ===== Chain =====

void* bc_chain_create(int difficulty, int consensus) {
    auto ct = consensus == 1
        ? Sapphire::Blockchain::ConsensusType::PoS
        : Sapphire::Blockchain::ConsensusType::PoW;
    return new Sapphire::Blockchain::Chain(
        static_cast<uint32_t>(difficulty), ct);
}

void bc_chain_destroy(void* chain) {
    delete static_cast<Sapphire::Blockchain::Chain*>(chain);
}

const char* bc_chain_add_tx(void* chain, const char* sender, const char* receiver,
                             double amount, const char* data) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Chain*>(chain)->add_transaction(
        sender ? sender : "", receiver ? receiver : "", amount, data ? data : "");
    return g_bc_last_string.c_str();
}

int bc_chain_mine_block(void* chain, const char* miner) {
    auto* c = static_cast<Sapphire::Blockchain::Chain*>(chain);
    Sapphire::Blockchain::Block b = c->create_block(miner ? miner : "");
    return c->add_block(b) ? 1 : 0;
}

int bc_chain_block_count(void* chain) {
    return static_cast<Sapphire::Blockchain::Chain*>(chain)->block_count();
}

int bc_chain_pending_count(void* chain) {
    return static_cast<Sapphire::Blockchain::Chain*>(chain)->pending_count();
}

int bc_chain_is_valid(void* chain) {
    return static_cast<Sapphire::Blockchain::Chain*>(chain)->validate_chain() ? 1 : 0;
}

double bc_chain_get_balance(void* chain, const char* address) {
    return static_cast<Sapphire::Blockchain::Chain*>(chain)->get_balance(address ? address : "");
}

const char* bc_chain_last_hash(void* chain) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Chain*>(chain)->last_hash();
    return g_bc_last_string.c_str();
}

const char* bc_chain_get_block_hash(void* chain, int index) {
    auto* c = static_cast<Sapphire::Blockchain::Chain*>(chain);
    const auto* b = c->get_block(static_cast<uint64_t>(index));
    g_bc_last_string = b ? b->hash : "";
    return g_bc_last_string.c_str();
}

const char* bc_chain_stats(void* chain) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Chain*>(chain)->chain_stats();
    return g_bc_last_string.c_str();
}

void bc_chain_add_validator(void* chain, const char* address, double stake) {
    static_cast<Sapphire::Blockchain::Chain*>(chain)->add_validator(
        address ? address : "", stake);
}

const char* bc_chain_select_validator(void* chain) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::Chain*>(chain)->select_validator();
    return g_bc_last_string.c_str();
}

// ===== P2P Network =====

void* bc_p2p_create() {
    return new Sapphire::Blockchain::P2PNetwork();
}

void bc_p2p_destroy(void* net) {
    delete static_cast<Sapphire::Blockchain::P2PNetwork*>(net);
}

const char* bc_p2p_add_node(void* net, const char* address, int port) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->add_node(
        address ? address : "127.0.0.1", port);
    return g_bc_last_string.c_str();
}

int bc_p2p_connect(void* net, const char* node_id) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->connect(
        node_id ? node_id : "") ? 1 : 0;
}

int bc_p2p_disconnect(void* net, const char* node_id) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->disconnect(
        node_id ? node_id : "") ? 1 : 0;
}

int bc_p2p_broadcast_block(void* net, const char* node_id, const char* block_hash) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->broadcast_block(
        node_id ? node_id : "", block_hash ? block_hash : "") ? 1 : 0;
}

int bc_p2p_broadcast_tx(void* net, const char* node_id, const char* tx_id) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->broadcast_tx(
        node_id ? node_id : "", tx_id ? tx_id : "") ? 1 : 0;
}

int bc_p2p_node_count(void* net) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->node_count();
}

int bc_p2p_connected_count(void* net) {
    return static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->connected_count();
}

const char* bc_p2p_status(void* net) {
    g_bc_last_string = static_cast<Sapphire::Blockchain::P2PNetwork*>(net)->network_status();
    return g_bc_last_string.c_str();
}

} // extern "C"
