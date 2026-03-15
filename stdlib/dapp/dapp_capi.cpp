#include "dapp.h"
#include <string>
#include <sstream>

using namespace Sapphire::DApp;

extern "C" {

static std::string g_dapp_last_string;

// ===== Wallet =====

void* dapp_wallet_create(const char* network) {
    return new Wallet(wallet_create(network ? network : "local"));
}

void* dapp_wallet_from_privkey(const char* priv_hex, const char* network) {
    return new Wallet(wallet_from_private_key(
        priv_hex ? priv_hex : "",
        network  ? network  : "local"));
}

void dapp_wallet_destroy(void* wallet) {
    delete static_cast<Wallet*>(wallet);
}

const char* dapp_wallet_address(void* wallet) {
    g_dapp_last_string = static_cast<Wallet*>(wallet)->address;
    return g_dapp_last_string.c_str();
}

const char* dapp_wallet_pubkey_hex(void* wallet) {
    g_dapp_last_string = static_cast<Wallet*>(wallet)->public_key_hex;
    return g_dapp_last_string.c_str();
}

const char* dapp_wallet_privkey_hex(void* wallet) {
    g_dapp_last_string = static_cast<Wallet*>(wallet)->private_key_hex;
    return g_dapp_last_string.c_str();
}

double dapp_wallet_balance(void* wallet) {
    return static_cast<Wallet*>(wallet)->balance;
}

const char* dapp_wallet_network(void* wallet) {
    g_dapp_last_string = static_cast<Wallet*>(wallet)->network;
    return g_dapp_last_string.c_str();
}

const char* dapp_wallet_sign(void* wallet, const char* data) {
    g_dapp_last_string = wallet_sign(*static_cast<Wallet*>(wallet), data ? data : "");
    return g_dapp_last_string.c_str();
}

int dapp_wallet_verify(const char* address, const char* data, const char* sig_hex) {
    return wallet_verify(
        address ? address : "",
        data    ? data    : "",
        sig_hex ? sig_hex : "") ? 1 : 0;
}

const char* dapp_wallet_to_string(void* wallet) {
    g_dapp_last_string = static_cast<Wallet*>(wallet)->to_string();
    return g_dapp_last_string.c_str();
}

// ===== Web3 Provider =====

void* dapp_provider_create(const char* network, const char* rpc_url) {
    return new Web3Provider(
        network ? network : "local",
        rpc_url ? rpc_url : "http://127.0.0.1:8545");
}

void dapp_provider_destroy(void* provider) {
    delete static_cast<Web3Provider*>(provider);
}

const char* dapp_provider_network(void* provider) {
    g_dapp_last_string = static_cast<Web3Provider*>(provider)->network();
    return g_dapp_last_string.c_str();
}

const char* dapp_provider_chain_id(void* provider) {
    g_dapp_last_string = static_cast<Web3Provider*>(provider)->chain_id();
    return g_dapp_last_string.c_str();
}

int64_t dapp_provider_block_number(void* provider) {
    return static_cast<int64_t>(static_cast<Web3Provider*>(provider)->block_number());
}

void dapp_provider_mine_block(void* provider) {
    static_cast<Web3Provider*>(provider)->mine_block();
}

double dapp_provider_get_balance(void* provider, const char* address) {
    return static_cast<Web3Provider*>(provider)->get_balance(address ? address : "");
}

void dapp_provider_set_balance(void* provider, const char* address, double amount) {
    static_cast<Web3Provider*>(provider)->set_balance(address ? address : "", amount);
}

void dapp_provider_transfer(void* provider, const char* from, const char* to, double amount) {
    static_cast<Web3Provider*>(provider)->transfer(
        from ? from : "", to ? to : "", amount);
}

const char* dapp_provider_status(void* provider) {
    g_dapp_last_string = static_cast<Web3Provider*>(provider)->status();
    return g_dapp_last_string.c_str();
}

// ===== Contract Deployment =====

void* dapp_deploy_contract(void* provider, const char* name,
                            const char* bytecode_hex, const char* abi_json,
                            void* wallet, double value) {
    auto* p = static_cast<Web3Provider*>(provider);
    auto* w = static_cast<Wallet*>(wallet);
    auto* dc = new DeployedContract(p->deploy_contract(
        name         ? name         : "",
        bytecode_hex ? bytecode_hex : "",
        abi_json     ? abi_json     : "{}",
        *w, value));
    return dc;
}

void dapp_contract_destroy(void* contract) {
    delete static_cast<DeployedContract*>(contract);
}

const char* dapp_contract_address(void* contract) {
    g_dapp_last_string = static_cast<DeployedContract*>(contract)->address;
    return g_dapp_last_string.c_str();
}

const char* dapp_contract_name(void* contract) {
    g_dapp_last_string = static_cast<DeployedContract*>(contract)->name;
    return g_dapp_last_string.c_str();
}

const char* dapp_contract_deployer(void* contract) {
    g_dapp_last_string = static_cast<DeployedContract*>(contract)->deployer;
    return g_dapp_last_string.c_str();
}

int64_t dapp_contract_deploy_block(void* contract) {
    return static_cast<int64_t>(static_cast<DeployedContract*>(contract)->deploy_block);
}

const char* dapp_contract_to_string(void* contract) {
    g_dapp_last_string = static_cast<DeployedContract*>(contract)->to_string();
    return g_dapp_last_string.c_str();
}

// ===== Contract Call =====

void* dapp_call_contract(void* provider, const char* contract_addr,
                          const char* fn_name, const char* args_json,
                          void* wallet, double value, int64_t gas_limit) {
    auto* p = static_cast<Web3Provider*>(provider);
    auto* w = static_cast<Wallet*>(wallet);

    // Parse simple comma-separated args from args_json
    std::vector<std::string> args;
    if (args_json && *args_json) {
        std::string s(args_json);
        std::string cur;
        for (char c : s) {
            if (c == ',') { if (!cur.empty()) { args.push_back(cur); cur.clear(); } }
            else cur += c;
        }
        if (!cur.empty()) args.push_back(cur);
    }

    auto* tx = new Web3Transaction(p->call_contract(
        contract_addr ? contract_addr : "",
        fn_name       ? fn_name       : "",
        args, *w, value,
        gas_limit > 0 ? static_cast<uint64_t>(gas_limit) : 1000000));
    return tx;
}

void dapp_tx_destroy(void* tx) {
    delete static_cast<Web3Transaction*>(tx);
}

int dapp_tx_success(void* tx) {
    return static_cast<Web3Transaction*>(tx)->success ? 1 : 0;
}

const char* dapp_tx_hash(void* tx) {
    g_dapp_last_string = static_cast<Web3Transaction*>(tx)->tx_hash;
    return g_dapp_last_string.c_str();
}

const char* dapp_tx_from(void* tx) {
    g_dapp_last_string = static_cast<Web3Transaction*>(tx)->from;
    return g_dapp_last_string.c_str();
}

const char* dapp_tx_to(void* tx) {
    g_dapp_last_string = static_cast<Web3Transaction*>(tx)->to;
    return g_dapp_last_string.c_str();
}

double dapp_tx_value(void* tx) {
    return static_cast<Web3Transaction*>(tx)->value;
}

int64_t dapp_tx_gas_used(void* tx) {
    return static_cast<int64_t>(static_cast<Web3Transaction*>(tx)->gas_used);
}

int64_t dapp_tx_block(void* tx) {
    return static_cast<int64_t>(static_cast<Web3Transaction*>(tx)->block_number);
}

const char* dapp_tx_revert_reason(void* tx) {
    g_dapp_last_string = static_cast<Web3Transaction*>(tx)->revert_reason;
    return g_dapp_last_string.c_str();
}

const char* dapp_tx_to_string(void* tx) {
    g_dapp_last_string = static_cast<Web3Transaction*>(tx)->to_string();
    return g_dapp_last_string.c_str();
}

// ===== Transaction History =====

int dapp_provider_tx_count(void* provider) {
    return static_cast<Web3Provider*>(provider)->transaction_count();
}

void* dapp_provider_get_tx(void* provider, int index) {
    auto& txs = static_cast<Web3Provider*>(provider)->transactions();
    if (index < 0 || index >= static_cast<int>(txs.size())) return nullptr;
    return new Web3Transaction(txs[static_cast<size_t>(index)]);
}

int dapp_provider_contract_count(void* provider) {
    return static_cast<Web3Provider*>(provider)->contract_count();
}

// ===== ABI Helpers =====

const char* dapp_abi_encode_call(const char* fn_name, const char* args_json) {
    std::vector<std::string> args;
    if (args_json && *args_json) {
        std::string s(args_json);
        std::string cur;
        for (char c : s) {
            if (c == ',') { if (!cur.empty()) { args.push_back(cur); cur.clear(); } }
            else cur += c;
        }
        if (!cur.empty()) args.push_back(cur);
    }
    g_dapp_last_string = Sapphire::DApp::abi_encode_call(fn_name ? fn_name : "", args);
    return g_dapp_last_string.c_str();
}

const char* dapp_abi_selector(const char* fn_sig) {
    g_dapp_last_string = Sapphire::DApp::abi_function_selector(fn_sig ? fn_sig : "");
    return g_dapp_last_string.c_str();
}

} // extern "C"
