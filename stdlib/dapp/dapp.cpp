#include "dapp.h"
#include "../crypto/crypto.h"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <cstring>

namespace Sapphire {
namespace DApp {

// ===== Helpers =====
static std::string hex_bytes(const uint8_t* d, size_t n) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < n; i++) ss << std::setw(2) << (int)d[i];
    return ss.str();
}

static uint64_t now_sec() {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

// ===== Wallet =====

std::string Wallet::to_string() const {
    std::ostringstream ss;
    ss << "{\"address\":\"" << address
       << "\",\"network\":\"" << network
       << "\",\"balance\":" << balance
       << ",\"public_key\":\"" << public_key_hex.substr(0, 16) << "...\"}";
    return ss.str();
}

Wallet wallet_create(const std::string& network) {
    Crypto::ECKeyPair kp = Crypto::generate_keypair("secp256k1");
    Wallet w;
    w.private_key_hex = kp.private_key_hex;
    w.public_key_hex  = kp.public_key_hex;
    w.address         = Crypto::public_key_to_address(kp.public_key_hex);
    w.balance         = 0.0;
    w.network         = network;
    return w;
}

Wallet wallet_from_private_key(const std::string& priv_hex, const std::string& network) {
    Crypto::ECKeyPair kp = Crypto::keypair_from_private_hex(priv_hex, "secp256k1");
    Wallet w;
    w.private_key_hex = kp.private_key_hex;
    w.public_key_hex  = kp.public_key_hex;
    w.address         = Crypto::public_key_to_address(kp.public_key_hex);
    w.balance         = 0.0;
    w.network         = network;
    return w;
}

std::string wallet_sign(const Wallet& w, const std::string& data) {
    return Crypto::ecdsa_sign_hex(w.private_key_hex, data, "secp256k1");
}

bool wallet_verify(const std::string& address, const std::string& data,
                   const std::string& sig_hex) {
    (void)address; // in a full impl we'd recover pubkey from sig
    // For simulation: just check sig is non-empty and valid hex
    return !sig_hex.empty() && sig_hex.size() > 8;
}

// ===== DeployedContract =====

std::string DeployedContract::to_string() const {
    std::ostringstream ss;
    ss << "{\"name\":\"" << name
       << "\",\"address\":\"" << address
       << "\",\"deployer\":\"" << deployer
       << "\",\"deploy_block\":" << deploy_block
       << ",\"active\":" << (active ? "true" : "false") << "}";
    return ss.str();
}

// ===== Web3Transaction =====

std::string Web3Transaction::to_string() const {
    std::ostringstream ss;
    ss << "{\"tx_hash\":\"" << tx_hash
       << "\",\"from\":\"" << from
       << "\",\"to\":\"" << to
       << "\",\"value\":" << value
       << ",\"gas_used\":" << gas_used
       << ",\"block\":" << block_number
       << ",\"success\":" << (success ? "true" : "false");
    if (!revert_reason.empty())
        ss << ",\"revert_reason\":\"" << revert_reason << "\"";
    ss << "}";
    return ss.str();
}

// ===== Web3Provider =====

Web3Provider::Web3Provider(const std::string& network, const std::string& rpc_url)
    : network_(network), rpc_url_(rpc_url), block_number_(1), tx_counter_(0) {}

std::string Web3Provider::chain_id() const {
    if (network_ == "mainnet")  return "1";
    if (network_ == "testnet")  return "11155111"; // Sepolia
    if (network_ == "polygon")  return "137";
    return "1337"; // local/hardhat
}

void Web3Provider::mine_block() {
    block_number_++;
}

double Web3Provider::get_balance(const std::string& address) const {
    auto it = balances_.find(address);
    return it != balances_.end() ? it->second : 0.0;
}

void Web3Provider::set_balance(const std::string& address, double amount) {
    balances_[address] = amount;
}

void Web3Provider::transfer(const std::string& from, const std::string& to, double amount) {
    double bal = get_balance(from);
    if (bal < amount) throw std::runtime_error("insufficient balance");
    balances_[from] = bal - amount;
    balances_[to]   = get_balance(to) + amount;
}

std::string Web3Provider::generate_tx_hash(const std::string& from,
                                            const std::string& to,
                                            uint64_t nonce) const {
    std::string seed = from + to + std::to_string(nonce) + std::to_string(now_sec());
    return "0x" + Crypto::sha256(seed);
}

std::string Web3Provider::generate_contract_address(const std::string& deployer,
                                                      uint64_t nonce) const {
    std::string seed = deployer + std::to_string(nonce);
    std::string h = Crypto::sha256(seed);
    return "0x" + h.substr(h.size() - 40); // last 20 bytes
}

DeployedContract Web3Provider::deploy_contract(const std::string& name,
                                                const std::string& bytecode_hex,
                                                const std::string& abi_json,
                                                const Wallet& deployer,
                                                double value) {
    tx_counter_++;
    std::string addr = generate_contract_address(deployer.address, tx_counter_);

    DeployedContract dc;
    dc.name          = name;
    dc.address       = addr;
    dc.abi_json      = abi_json;
    dc.bytecode_hash = Crypto::sha256(bytecode_hex);
    dc.deploy_block  = block_number_;
    dc.deployer      = deployer.address;
    dc.active        = true;

    contracts_[addr] = dc;

    // Record deployment tx
    Web3Transaction tx;
    tx.tx_hash     = generate_tx_hash(deployer.address, addr, tx_counter_);
    tx.from        = deployer.address;
    tx.to          = addr;
    tx.value       = value;
    tx.data        = "deploy:" + name;
    tx.gas_limit   = 3000000;
    tx.gas_used    = 1500000;
    tx.block_number= block_number_;
    tx.success     = true;
    tx.signature   = wallet_sign(deployer, tx.tx_hash);
    txs_.push_back(tx);

    if (value > 0) transfer(deployer.address, addr, value);
    mine_block();
    return dc;
}

Web3Transaction Web3Provider::call_contract(const std::string& contract_address,
                                             const std::string& function_name,
                                             const std::vector<std::string>& args,
                                             const Wallet& caller,
                                             double value,
                                             uint64_t gas_limit) {
    tx_counter_++;
    Web3Transaction tx;
    tx.tx_hash      = generate_tx_hash(caller.address, contract_address, tx_counter_);
    tx.from         = caller.address;
    tx.to           = contract_address;
    tx.value        = value;
    tx.gas_limit    = gas_limit;
    tx.block_number = block_number_;

    // Encode call data
    tx.data = abi_encode_call(function_name, args);

    auto it = contracts_.find(contract_address);
    if (it == contracts_.end() || !it->second.active) {
        tx.success       = false;
        tx.revert_reason = "contract not found or inactive";
        tx.gas_used      = 21000;
    } else {
        tx.success  = true;
        tx.gas_used = 21000 + static_cast<uint64_t>(tx.data.size()) * 68;
        if (tx.gas_used > gas_limit) {
            tx.success       = false;
            tx.revert_reason = "out of gas";
            tx.gas_used      = gas_limit;
        }
    }

    tx.signature = wallet_sign(caller, tx.tx_hash);
    txs_.push_back(tx);

    if (tx.success && value > 0)
        transfer(caller.address, contract_address, value);

    mine_block();
    return tx;
}

Web3Transaction Web3Provider::get_transaction(const std::string& tx_hash) const {
    for (auto& tx : txs_)
        if (tx.tx_hash == tx_hash) return tx;
    throw std::runtime_error("transaction not found: " + tx_hash);
}

const DeployedContract* Web3Provider::get_contract(const std::string& address) const {
    auto it = contracts_.find(address);
    return it != contracts_.end() ? &it->second : nullptr;
}

std::string Web3Provider::status() const {
    std::ostringstream ss;
    ss << "{\"network\":\"" << network_
       << "\",\"chain_id\":\"" << chain_id()
       << "\",\"block_number\":" << block_number_
       << ",\"contracts\":" << contracts_.size()
       << ",\"transactions\":" << txs_.size() << "}";
    return ss.str();
}

// ===== ABI Helpers =====

std::string abi_encode_call(const std::string& function_name,
                             const std::vector<std::string>& args) {
    std::ostringstream ss;
    ss << function_name << "(";
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) ss << ",";
        ss << args[i];
    }
    ss << ")";
    return ss.str();
}

std::string abi_function_selector(const std::string& function_sig) {
    std::string h = Crypto::sha256(function_sig);
    return h.substr(0, 8); // first 4 bytes = 8 hex chars
}

} // namespace DApp
} // namespace Sapphire
