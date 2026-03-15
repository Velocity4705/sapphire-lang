#include "crypto.h"
#include <string>
#include <stdexcept>

extern "C" {

static std::string g_crypto_last_string;

// ===== Hash Functions =====

const char* crypto_sha256(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::sha256(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_sha512(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::sha512(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_sha1(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::sha1(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_md5(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::md5(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_ripemd160(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::ripemd160(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_hmac_sha256(const char* key, const char* data) {
    g_crypto_last_string = Sapphire::Crypto::hmac_sha256(
        key ? key : "", data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_hmac_sha512(const char* key, const char* data) {
    g_crypto_last_string = Sapphire::Crypto::hmac_sha512(
        key ? key : "", data ? data : "");
    return g_crypto_last_string.c_str();
}

// ===== Encoding =====

const char* crypto_to_base64(const char* data) {
    g_crypto_last_string = Sapphire::Crypto::to_base64(data ? data : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_from_base64(const char* b64) {
    g_crypto_last_string = Sapphire::Crypto::from_base64(b64 ? b64 : "");
    return g_crypto_last_string.c_str();
}

// ===== ECDSA Key Pair =====

void* crypto_keypair_generate(const char* curve) {
    auto* kp = new Sapphire::Crypto::ECKeyPair(
        Sapphire::Crypto::generate_keypair(curve ? curve : "secp256k1"));
    return kp;
}

void* crypto_keypair_from_private(const char* priv_hex, const char* curve) {
    auto* kp = new Sapphire::Crypto::ECKeyPair(
        Sapphire::Crypto::keypair_from_private_hex(
            priv_hex ? priv_hex : "",
            curve ? curve : "secp256k1"));
    return kp;
}

void crypto_keypair_destroy(void* kp) {
    delete static_cast<Sapphire::Crypto::ECKeyPair*>(kp);
}

const char* crypto_keypair_private_pem(void* kp) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::ECKeyPair*>(kp)->private_key_pem;
    return g_crypto_last_string.c_str();
}

const char* crypto_keypair_public_pem(void* kp) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::ECKeyPair*>(kp)->public_key_pem;
    return g_crypto_last_string.c_str();
}

const char* crypto_keypair_private_hex(void* kp) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::ECKeyPair*>(kp)->private_key_hex;
    return g_crypto_last_string.c_str();
}

const char* crypto_keypair_public_hex(void* kp) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::ECKeyPair*>(kp)->public_key_hex;
    return g_crypto_last_string.c_str();
}

const char* crypto_keypair_curve(void* kp) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::ECKeyPair*>(kp)->curve;
    return g_crypto_last_string.c_str();
}

// ===== ECDSA Sign / Verify =====

const char* crypto_ecdsa_sign(void* kp, const char* message) {
    auto* k = static_cast<Sapphire::Crypto::ECKeyPair*>(kp);
    g_crypto_last_string = Sapphire::Crypto::ecdsa_sign(
        k->private_key_pem, message ? message : "");
    return g_crypto_last_string.c_str();
}

int crypto_ecdsa_verify(void* kp, const char* message, const char* sig_hex) {
    auto* k = static_cast<Sapphire::Crypto::ECKeyPair*>(kp);
    return Sapphire::Crypto::ecdsa_verify(
        k->public_key_pem,
        message ? message : "",
        sig_hex ? sig_hex : "") ? 1 : 0;
}

int crypto_ecdsa_verify_pubhex(const char* pub_hex, const char* curve,
                                const char* message, const char* sig_hex) {
    return Sapphire::Crypto::ecdsa_verify_hex(
        pub_hex ? pub_hex : "",
        message ? message : "",
        sig_hex ? sig_hex : "",
        curve ? curve : "secp256k1") ? 1 : 0;
}

// ===== Key Management =====

const char* crypto_pubkey_to_eth_address(const char* pub_hex) {
    g_crypto_last_string = Sapphire::Crypto::public_key_to_address(pub_hex ? pub_hex : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_pubkey_to_btc_address(const char* pub_hex) {
    g_crypto_last_string = Sapphire::Crypto::public_key_to_btc_address(pub_hex ? pub_hex : "");
    return g_crypto_last_string.c_str();
}

const char* crypto_privkey_to_wif(const char* priv_hex, int compressed) {
    g_crypto_last_string = Sapphire::Crypto::private_key_to_wif(
        priv_hex ? priv_hex : "", compressed != 0);
    return g_crypto_last_string.c_str();
}

const char* crypto_wif_to_privkey(const char* wif) {
    g_crypto_last_string = Sapphire::Crypto::wif_to_private_key(wif ? wif : "");
    return g_crypto_last_string.c_str();
}

// ===== AES-256-GCM =====

void* crypto_aes_encrypt(const char* key_hex, const char* plaintext) {
    auto* res = new Sapphire::Crypto::AESResult(
        Sapphire::Crypto::aes256_encrypt(
            key_hex ? key_hex : "",
            plaintext ? plaintext : ""));
    return res;
}

void crypto_aes_result_destroy(void* res) {
    delete static_cast<Sapphire::Crypto::AESResult*>(res);
}

const char* crypto_aes_ciphertext(void* res) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::AESResult*>(res)->ciphertext_hex;
    return g_crypto_last_string.c_str();
}

const char* crypto_aes_iv(void* res) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::AESResult*>(res)->iv_hex;
    return g_crypto_last_string.c_str();
}

const char* crypto_aes_tag(void* res) {
    g_crypto_last_string = static_cast<Sapphire::Crypto::AESResult*>(res)->tag_hex;
    return g_crypto_last_string.c_str();
}

const char* crypto_aes_decrypt(const char* key_hex, const char* ct_hex,
                                const char* iv_hex, const char* tag_hex) {
    try {
        g_crypto_last_string = Sapphire::Crypto::aes256_decrypt(
            key_hex  ? key_hex  : "",
            ct_hex   ? ct_hex   : "",
            iv_hex   ? iv_hex   : "",
            tag_hex  ? tag_hex  : "");
    } catch (...) {
        g_crypto_last_string = "ERROR: decryption failed";
    }
    return g_crypto_last_string.c_str();
}

// ===== Key Derivation =====

const char* crypto_pbkdf2(const char* password, const char* salt,
                           int iterations, int key_len) {
    g_crypto_last_string = Sapphire::Crypto::pbkdf2_sha256(
        password ? password : "",
        salt ? salt : "",
        iterations > 0 ? iterations : 100000,
        key_len > 0 ? key_len : 32);
    return g_crypto_last_string.c_str();
}

// ===== Random =====

const char* crypto_random_bytes(int count) {
    g_crypto_last_string = Sapphire::Crypto::random_bytes_hex(count > 0 ? count : 32);
    return g_crypto_last_string.c_str();
}

} // extern "C"
