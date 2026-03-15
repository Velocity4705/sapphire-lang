#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Sapphire {
namespace Crypto {

// ===== Hash Functions =====
std::string sha256(const std::string& data);
std::string sha512(const std::string& data);
std::string sha1(const std::string& data);   // legacy/compat
std::string md5(const std::string& data);    // legacy/compat
std::string ripemd160(const std::string& data);

// HMAC
std::string hmac_sha256(const std::string& key, const std::string& data);
std::string hmac_sha512(const std::string& key, const std::string& data);

// Encoding helpers
std::string to_hex(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> from_hex(const std::string& hex);
std::string to_base64(const std::string& data);
std::string from_base64(const std::string& b64);

// ===== ECDSA Key Pair =====
struct ECKeyPair {
    std::string private_key_pem;
    std::string public_key_pem;
    std::string private_key_hex;  // raw 32-byte private scalar
    std::string public_key_hex;   // raw 65-byte uncompressed point
    std::string curve;            // "secp256k1" or "prime256v1"
};

ECKeyPair generate_keypair(const std::string& curve = "secp256k1");
ECKeyPair keypair_from_private_hex(const std::string& priv_hex,
                                   const std::string& curve = "secp256k1");

// ===== ECDSA Sign / Verify =====
// Returns DER-encoded signature as hex string
std::string ecdsa_sign(const std::string& private_key_pem,
                       const std::string& message);
std::string ecdsa_sign_hex(const std::string& private_key_hex,
                           const std::string& message,
                           const std::string& curve = "secp256k1");

bool ecdsa_verify(const std::string& public_key_pem,
                  const std::string& message,
                  const std::string& signature_hex);
bool ecdsa_verify_hex(const std::string& public_key_hex,
                      const std::string& message,
                      const std::string& signature_hex,
                      const std::string& curve = "secp256k1");

// ===== Key Management =====
// Derive Ethereum-style address from public key
std::string public_key_to_address(const std::string& public_key_hex);

// Derive Bitcoin-style address (P2PKH, mainnet)
std::string public_key_to_btc_address(const std::string& public_key_hex);

// WIF (Wallet Import Format) encode/decode
std::string private_key_to_wif(const std::string& private_key_hex, bool compressed = true);
std::string wif_to_private_key(const std::string& wif);

// ===== Symmetric Encryption (AES-256-GCM) =====
struct AESResult {
    std::string ciphertext_hex;
    std::string iv_hex;
    std::string tag_hex;
};

AESResult aes256_encrypt(const std::string& key_hex, const std::string& plaintext);
std::string aes256_decrypt(const std::string& key_hex,
                           const std::string& ciphertext_hex,
                           const std::string& iv_hex,
                           const std::string& tag_hex);

// ===== Key Derivation =====
std::string pbkdf2_sha256(const std::string& password,
                          const std::string& salt,
                          int iterations = 100000,
                          int key_len = 32);

// Random bytes (cryptographically secure)
std::string random_bytes_hex(int count);

} // namespace Crypto
} // namespace Sapphire

// ===== C API =====
extern "C" {

// Hash functions
const char* crypto_sha256(const char* data);
const char* crypto_sha512(const char* data);
const char* crypto_sha1(const char* data);
const char* crypto_md5(const char* data);
const char* crypto_ripemd160(const char* data);
const char* crypto_hmac_sha256(const char* key, const char* data);
const char* crypto_hmac_sha512(const char* key, const char* data);

// Encoding
const char* crypto_to_base64(const char* data);
const char* crypto_from_base64(const char* b64);

// ECDSA key pair (returns opaque handle)
void* crypto_keypair_generate(const char* curve);   // curve: "secp256k1" or "prime256v1"
void* crypto_keypair_from_private(const char* priv_hex, const char* curve);
void  crypto_keypair_destroy(void* kp);
const char* crypto_keypair_private_pem(void* kp);
const char* crypto_keypair_public_pem(void* kp);
const char* crypto_keypair_private_hex(void* kp);
const char* crypto_keypair_public_hex(void* kp);
const char* crypto_keypair_curve(void* kp);

// ECDSA sign / verify
const char* crypto_ecdsa_sign(void* kp, const char* message);
int         crypto_ecdsa_verify(void* kp, const char* message, const char* sig_hex);
int         crypto_ecdsa_verify_pubhex(const char* pub_hex, const char* curve,
                                       const char* message, const char* sig_hex);

// Key management
const char* crypto_pubkey_to_eth_address(const char* pub_hex);
const char* crypto_pubkey_to_btc_address(const char* pub_hex);
const char* crypto_privkey_to_wif(const char* priv_hex, int compressed);
const char* crypto_wif_to_privkey(const char* wif);

// AES-256-GCM
void* crypto_aes_encrypt(const char* key_hex, const char* plaintext);
void  crypto_aes_result_destroy(void* res);
const char* crypto_aes_ciphertext(void* res);
const char* crypto_aes_iv(void* res);
const char* crypto_aes_tag(void* res);
const char* crypto_aes_decrypt(const char* key_hex, const char* ct_hex,
                                const char* iv_hex, const char* tag_hex);

// Key derivation
const char* crypto_pbkdf2(const char* password, const char* salt,
                           int iterations, int key_len);

// Random
const char* crypto_random_bytes(int count);

} // extern "C"
