#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace Sapphire {
namespace DApp {

// ===== Wallet =====
struct Wallet {
    std::string address;        // Ethereum-style 0x... address
    std::string public_key_hex;
    std::string private_key_hex; // kept in memory only
    double      balance;
    std::string network;        // "mainnet", "testnet", "local"

    std::string to_string() const;
};

Wallet wallet_create(const std::string& network = "local");
Wallet wallet_from_private_key(const std::string& priv_hex,
                                const std::string& network = "local");

// Sign arbitrary data (returns hex signature)
std::string wallet_sign(const Wallet& w, const std::string& data);
bool        wallet_verify(const std::string& address,
                          const std::string& data,
                          const std::string& sig_hex);

// ===== Deployed Contract =====
struct DeployedContract {
    std::string address;
    std::string name;
    std::string abi_json;       // simplified ABI description
    std::string bytecode_hash;  // hash of deployed bytecode
    uint64_t    deploy_block;
    std::string deployer;
    bool        active;

    std::string to_string() const;
};

// ===== Transaction =====
struct Web3Transaction {
    std::string tx_hash;
    std::string from;
    std::string to;
    double      value;
    std::string data;           // encoded call data
    uint64_t    gas_limit;
    uint64_t    gas_used;
    uint64_t    block_number;
    bool        success;
    std::string revert_reason;
    std::string signature;

    std::string to_string() const;
};

// ===== Web3 Provider (simulated) =====
class Web3Provider {
public:
    explicit Web3Provider(const std::string& network = "local",
                          const std::string& rpc_url  = "http://127.0.0.1:8545");
    ~Web3Provider() = default;

    // Network
    std::string network() const { return network_; }
    std::string rpc_url() const { return rpc_url_; }
    uint64_t    block_number() const { return block_number_; }
    std::string chain_id() const;
    void        mine_block();   // advance simulated chain

    // Accounts
    double  get_balance(const std::string& address) const;
    void    set_balance(const std::string& address, double amount);
    void    transfer(const std::string& from, const std::string& to, double amount);

    // Contract deployment
    DeployedContract deploy_contract(const std::string& name,
                                     const std::string& bytecode_hex,
                                     const std::string& abi_json,
                                     const Wallet& deployer,
                                     double value = 0.0);

    // Contract interaction
    Web3Transaction call_contract(const std::string& contract_address,
                                  const std::string& function_name,
                                  const std::vector<std::string>& args,
                                  const Wallet& caller,
                                  double value = 0.0,
                                  uint64_t gas_limit = 1000000);

    // Transaction history
    const std::vector<Web3Transaction>& transactions() const { return txs_; }
    Web3Transaction get_transaction(const std::string& tx_hash) const;
    int transaction_count() const { return static_cast<int>(txs_.size()); }

    // Contract registry
    const DeployedContract* get_contract(const std::string& address) const;
    int contract_count() const { return static_cast<int>(contracts_.size()); }

    std::string status() const;

private:
    std::string network_;
    std::string rpc_url_;
    uint64_t    block_number_;
    uint64_t    tx_counter_;

    std::unordered_map<std::string, double>           balances_;
    std::unordered_map<std::string, DeployedContract> contracts_;
    std::vector<Web3Transaction>                      txs_;

    std::string generate_tx_hash(const std::string& from, const std::string& to,
                                  uint64_t nonce) const;
    std::string generate_contract_address(const std::string& deployer,
                                           uint64_t nonce) const;
};

// ===== ABI Encoder (simplified) =====
// Encodes function call data as "functionName(arg1,arg2,...)"
std::string abi_encode_call(const std::string& function_name,
                             const std::vector<std::string>& args);
std::string abi_function_selector(const std::string& function_sig); // first 4 bytes as hex

// ===== Event Log =====
struct EventLog {
    std::string contract_address;
    std::string event_name;
    std::vector<std::string> topics;
    std::string data;
    uint64_t    block_number;
    std::string tx_hash;
};

} // namespace DApp
} // namespace Sapphire

// ===== C API =====
extern "C" {

// Wallet
void* dapp_wallet_create(const char* network);
void* dapp_wallet_from_privkey(const char* priv_hex, const char* network);
void  dapp_wallet_destroy(void* wallet);
const char* dapp_wallet_address(void* wallet);
const char* dapp_wallet_pubkey_hex(void* wallet);
const char* dapp_wallet_privkey_hex(void* wallet);
double      dapp_wallet_balance(void* wallet);
const char* dapp_wallet_network(void* wallet);
const char* dapp_wallet_sign(void* wallet, const char* data);
int         dapp_wallet_verify(const char* address, const char* data, const char* sig_hex);
const char* dapp_wallet_to_string(void* wallet);

// Web3 Provider
void* dapp_provider_create(const char* network, const char* rpc_url);
void  dapp_provider_destroy(void* provider);
const char* dapp_provider_network(void* provider);
const char* dapp_provider_chain_id(void* provider);
int64_t     dapp_provider_block_number(void* provider);
void        dapp_provider_mine_block(void* provider);
double      dapp_provider_get_balance(void* provider, const char* address);
void        dapp_provider_set_balance(void* provider, const char* address, double amount);
void        dapp_provider_transfer(void* provider, const char* from, const char* to, double amount);
const char* dapp_provider_status(void* provider);

// Contract deployment
void* dapp_deploy_contract(void* provider, const char* name,
                            const char* bytecode_hex, const char* abi_json,
                            void* wallet, double value);
void  dapp_contract_destroy(void* contract);
const char* dapp_contract_address(void* contract);
const char* dapp_contract_name(void* contract);
const char* dapp_contract_deployer(void* contract);
int64_t     dapp_contract_deploy_block(void* contract);
const char* dapp_contract_to_string(void* contract);

// Contract call
void* dapp_call_contract(void* provider, const char* contract_addr,
                          const char* fn_name, const char* args_json,
                          void* wallet, double value, int64_t gas_limit);
void  dapp_tx_destroy(void* tx);
int         dapp_tx_success(void* tx);
const char* dapp_tx_hash(void* tx);
const char* dapp_tx_from(void* tx);
const char* dapp_tx_to(void* tx);
double      dapp_tx_value(void* tx);
int64_t     dapp_tx_gas_used(void* tx);
int64_t     dapp_tx_block(void* tx);
const char* dapp_tx_revert_reason(void* tx);
const char* dapp_tx_to_string(void* tx);

// Transaction history
int         dapp_provider_tx_count(void* provider);
void*       dapp_provider_get_tx(void* provider, int index);
int         dapp_provider_contract_count(void* provider);

// ABI helpers
const char* dapp_abi_encode_call(const char* fn_name, const char* args_json);
const char* dapp_abi_selector(const char* fn_sig);

} // extern "C"
