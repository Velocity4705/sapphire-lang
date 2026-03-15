#include "blockchain.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <openssl/sha.h>

namespace Sapphire {
namespace Blockchain {

// ===== SHA-256 =====

std::string sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    std::ostringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    return ss.str();
}

std::string sha256_double(const std::string& data) {
    return sha256(sha256(data));
}

// ===== Merkle Tree =====

void MerkleTree::build(const std::vector<std::string>& data) {
    leaves.clear();
    for (const auto& d : data)
        leaves.push_back(sha256(d));
    root = compute_root(leaves);
}

std::string MerkleTree::compute_root(std::vector<std::string> hashes) const {
    if (hashes.empty()) return sha256("");
    while (hashes.size() > 1) {
        if (hashes.size() % 2 != 0)
            hashes.push_back(hashes.back()); // duplicate last
        std::vector<std::string> next;
        for (size_t i = 0; i < hashes.size(); i += 2)
            next.push_back(sha256(hashes[i] + hashes[i+1]));
        hashes = next;
    }
    return hashes[0];
}

bool MerkleTree::verify(const std::string& leaf, const std::vector<std::string>& proof) const {
    std::string current = sha256(leaf);
    for (const auto& sibling : proof)
        current = sha256(current + sibling);
    return current == root;
}

// ===== Transaction =====

static uint64_t current_timestamp() {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

std::string Transaction::hash() const {
    return sha256(id + sender + receiver + std::to_string(amount) +
                  std::to_string(timestamp) + data);
}

std::string Transaction::to_string() const {
    std::ostringstream ss;
    ss << "{\"id\":\"" << id << "\""
       << ",\"sender\":\"" << sender << "\""
       << ",\"receiver\":\"" << receiver << "\""
       << ",\"amount\":" << std::fixed << std::setprecision(8) << amount
       << ",\"timestamp\":" << timestamp
       << ",\"data\":\"" << data << "\""
       << ",\"hash\":\"" << hash() << "\"}";
    return ss.str();
}

// ===== Block =====

Block::Block() : index(0), timestamp(0), nonce(0), difficulty(2) {}

Block::Block(uint64_t idx, const std::string& prev_hash, uint32_t diff)
    : index(idx), previous_hash(prev_hash), timestamp(current_timestamp()),
      nonce(0), difficulty(diff) {}

std::string Block::compute_hash() const {
    // Build merkle root from transactions
    std::vector<std::string> tx_hashes;
    for (const auto& tx : transactions)
        tx_hashes.push_back(tx.hash());
    MerkleTree mt;
    std::string mr = tx_hashes.empty() ? sha256("") : mt.compute_root(tx_hashes);

    std::ostringstream ss;
    ss << index << previous_hash << mr << timestamp << nonce << difficulty;
    return sha256(ss.str());
}

void Block::mine() {
    // Build merkle root
    std::vector<std::string> tx_hashes;
    for (const auto& tx : transactions)
        tx_hashes.push_back(tx.hash());
    MerkleTree mt;
    merkle_root = tx_hashes.empty() ? sha256("") : mt.compute_root(tx_hashes);

    std::string target(difficulty, '0');
    nonce = 0;
    do {
        nonce++;
        hash = compute_hash();
    } while (hash.substr(0, difficulty) != target);
}

bool Block::is_valid() const {
    if (hash != compute_hash()) return false;
    std::string target(difficulty, '0');
    return hash.substr(0, difficulty) == target;
}

std::string Block::to_string() const {
    std::ostringstream ss;
    ss << "{\"index\":" << index
       << ",\"hash\":\"" << hash << "\""
       << ",\"previous_hash\":\"" << previous_hash << "\""
       << ",\"merkle_root\":\"" << merkle_root << "\""
       << ",\"timestamp\":" << timestamp
       << ",\"nonce\":" << nonce
       << ",\"difficulty\":" << difficulty
       << ",\"miner\":\"" << miner << "\""
       << ",\"tx_count\":" << transactions.size()
       << "}";
    return ss.str();
}

// ===== Chain =====

Chain::Chain(uint32_t difficulty, ConsensusType consensus)
    : difficulty_(difficulty), consensus_(consensus), tx_counter_(0) {
    // Create and add genesis block
    Block genesis = create_genesis();
    chain_.push_back(genesis);
}

Chain::~Chain() {}

std::string Chain::generate_tx_id() {
    return "tx_" + std::to_string(++tx_counter_) + "_" +
           std::to_string(current_timestamp());
}

Block Chain::create_genesis() {
    Block b(0, "0000000000000000000000000000000000000000000000000000000000000000", difficulty_);
    b.miner = "genesis";
    b.mine();
    return b;
}

Block Chain::create_block(const std::string& miner) {
    Block b(chain_.size(), last_hash(), difficulty_);
    b.miner = miner;
    b.transactions = pending_tx_;
    pending_tx_.clear();
    return b;
}

bool Chain::add_block(Block& block) {
    if (chain_.empty()) return false;
    if (block.previous_hash != last_hash()) return false;
    block.mine();
    chain_.push_back(block);
    return true;
}

bool Chain::validate_chain() const {
    for (size_t i = 1; i < chain_.size(); i++) {
        const Block& cur  = chain_[i];
        const Block& prev = chain_[i-1];
        if (cur.hash != cur.compute_hash()) return false;
        if (cur.previous_hash != prev.hash) return false;
    }
    return true;
}

std::string Chain::add_transaction(const std::string& sender,
                                    const std::string& receiver,
                                    double amount,
                                    const std::string& data) {
    Transaction tx;
    tx.id        = generate_tx_id();
    tx.sender    = sender;
    tx.receiver  = receiver;
    tx.amount    = amount;
    tx.timestamp = current_timestamp();
    tx.data      = data;
    pending_tx_.push_back(tx);
    return tx.id;
}

const Block* Chain::get_block(uint64_t index) const {
    if (index >= chain_.size()) return nullptr;
    return &chain_[index];
}

const Block* Chain::get_block_by_hash(const std::string& hash) const {
    for (const auto& b : chain_)
        if (b.hash == hash) return &b;
    return nullptr;
}

double Chain::get_balance(const std::string& address) const {
    double balance = 0.0;
    for (const auto& block : chain_) {
        for (const auto& tx : block.transactions) {
            if (tx.receiver == address) balance += tx.amount;
            if (tx.sender   == address) balance -= tx.amount;
        }
    }
    return balance;
}

std::string Chain::last_hash() const {
    if (chain_.empty()) return std::string(64, '0');
    return chain_.back().hash;
}

void Chain::add_validator(const std::string& address, double stake) {
    for (auto& v : validators_) {
        if (v.address == address) { v.stake += stake; return; }
    }
    validators_.push_back({address, stake, 0});
}

std::string Chain::select_validator() const {
    if (validators_.empty()) return "";
    // Weighted random selection by stake
    double total = 0;
    for (const auto& v : validators_) total += v.stake;
    if (total <= 0) return validators_[0].address;
    // Deterministic selection based on chain length for reproducibility
    double target = static_cast<double>(chain_.size() % 1000) / 1000.0 * total;
    double cumulative = 0;
    for (const auto& v : validators_) {
        cumulative += v.stake;
        if (cumulative >= target) return v.address;
    }
    return validators_.back().address;
}

std::string Chain::chain_stats() const {
    double total_tx = 0;
    for (const auto& b : chain_) total_tx += b.transactions.size();
    std::ostringstream ss;
    ss << "{\"blocks\":" << chain_.size()
       << ",\"total_transactions\":" << static_cast<int>(total_tx)
       << ",\"pending_transactions\":" << pending_tx_.size()
       << ",\"difficulty\":" << difficulty_
       << ",\"consensus\":\"" << (consensus_ == ConsensusType::PoW ? "PoW" : "PoS") << "\""
       << ",\"last_hash\":\"" << last_hash() << "\""
       << "}";
    return ss.str();
}

// ===== P2P Network =====

P2PNetwork::P2PNetwork() : next_port_(30303) {}
P2PNetwork::~P2PNetwork() {}

std::string P2PNetwork::add_node(const std::string& address, int port) {
    PeerNode node;
    node.node_id  = "node_" + std::to_string(nodes_.size() + 1);
    node.address  = address;
    node.port     = port > 0 ? port : next_port_++;
    node.connected = false;
    nodes_.push_back(node);
    return node.node_id;
}

bool P2PNetwork::connect(const std::string& node_id) {
    for (auto& n : nodes_) {
        if (n.node_id == node_id) { n.connected = true; return true; }
    }
    return false;
}

bool P2PNetwork::disconnect(const std::string& node_id) {
    for (auto& n : nodes_) {
        if (n.node_id == node_id) { n.connected = false; return true; }
    }
    return false;
}

bool P2PNetwork::broadcast_block(const std::string& node_id, const std::string& /*block_hash*/) {
    for (const auto& n : nodes_)
        if (n.node_id == node_id) return n.connected;
    return false;
}

bool P2PNetwork::broadcast_tx(const std::string& node_id, const std::string& /*tx_id*/) {
    for (const auto& n : nodes_)
        if (n.node_id == node_id) return n.connected;
    return false;
}

int P2PNetwork::connected_count() const {
    int count = 0;
    for (const auto& n : nodes_) if (n.connected) count++;
    return count;
}

std::string P2PNetwork::network_status() const {
    std::ostringstream ss;
    ss << "{\"total_nodes\":" << nodes_.size()
       << ",\"connected_nodes\":" << connected_count()
       << ",\"nodes\":[";
    for (size_t i = 0; i < nodes_.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"id\":\"" << nodes_[i].node_id << "\""
           << ",\"address\":\"" << nodes_[i].address << "\""
           << ",\"port\":" << nodes_[i].port
           << ",\"connected\":" << (nodes_[i].connected ? "true" : "false")
           << "}";
    }
    ss << "]}";
    return ss.str();
}

} // namespace Blockchain
} // namespace Sapphire
