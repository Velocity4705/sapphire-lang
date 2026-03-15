#include "crypto.h"

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/bn.h>

#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace Sapphire {
namespace Crypto {

// ===== Helpers =====

static std::string bytes_to_hex(const uint8_t* data, size_t len) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; i++)
        ss << std::setw(2) << static_cast<int>(data[i]);
    return ss.str();
}

std::string to_hex(const std::vector<uint8_t>& bytes) {
    return bytes_to_hex(bytes.data(), bytes.size());
}

std::vector<uint8_t> from_hex(const std::string& hex) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        result.push_back(static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16)));
    }
    return result;
}

std::string to_base64(const std::string& data) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, data.data(), static_cast<int>(data.size()));
    BIO_flush(b64);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    std::string result(bptr->data, bptr->length);
    BIO_free_all(b64);
    return result;
}

std::string from_base64(const std::string& b64) {
    BIO* bio = BIO_new_mem_buf(b64.data(), static_cast<int>(b64.size()));
    BIO* b64f = BIO_new(BIO_f_base64());
    bio = BIO_push(b64f, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    std::string result(b64.size(), '\0');
    int len = BIO_read(bio, result.data(), static_cast<int>(b64.size()));
    BIO_free_all(bio);
    if (len < 0) len = 0;
    result.resize(static_cast<size_t>(len));
    return result;
}

// ===== Hash Functions =====

std::string sha256(const std::string& data) {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
    return bytes_to_hex(hash, SHA256_DIGEST_LENGTH);
}

std::string sha512(const std::string& data) {
    uint8_t hash[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
    return bytes_to_hex(hash, SHA512_DIGEST_LENGTH);
}

std::string sha1(const std::string& data) {
    uint8_t hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
    return bytes_to_hex(hash, SHA_DIGEST_LENGTH);
}

std::string md5(const std::string& data) {
    uint8_t hash[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
    return bytes_to_hex(hash, MD5_DIGEST_LENGTH);
}

std::string ripemd160(const std::string& data) {
    uint8_t hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
    return bytes_to_hex(hash, RIPEMD160_DIGEST_LENGTH);
}

std::string hmac_sha256(const std::string& key, const std::string& data) {
    uint8_t hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(),
         key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const uint8_t*>(data.data()), data.size(),
         hash, &len);
    return bytes_to_hex(hash, len);
}

std::string hmac_sha512(const std::string& key, const std::string& data) {
    uint8_t hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha512(),
         key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const uint8_t*>(data.data()), data.size(),
         hash, &len);
    return bytes_to_hex(hash, len);
}

// ===== ECDSA =====

static int curve_nid(const std::string& curve) {
    if (curve == "secp256k1") return NID_secp256k1;
    if (curve == "prime256v1" || curve == "p256") return NID_X9_62_prime256v1;
    if (curve == "secp384r1" || curve == "p384") return NID_secp384r1;
    return NID_secp256k1; // default
}

static std::string ec_key_to_private_pem(EC_KEY* key) {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);
    std::string pem(bptr->data, bptr->length);
    BIO_free_all(bio);
    return pem;
}

static std::string ec_key_to_public_pem(EC_KEY* key) {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_EC_PUBKEY(bio, key);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);
    std::string pem(bptr->data, bptr->length);
    BIO_free_all(bio);
    return pem;
}

static std::string ec_private_key_to_hex(EC_KEY* key) {
    const BIGNUM* priv = EC_KEY_get0_private_key(key);
    int len = BN_num_bytes(priv);
    std::vector<uint8_t> buf(32, 0);
    BN_bn2binpad(priv, buf.data(), 32);
    return bytes_to_hex(buf.data(), 32);
}

static std::string ec_public_key_to_hex(EC_KEY* key) {
    const EC_GROUP* group = EC_KEY_get0_group(key);
    const EC_POINT* pub   = EC_KEY_get0_public_key(key);
    size_t len = EC_POINT_point2oct(group, pub, POINT_CONVERSION_UNCOMPRESSED,
                                    nullptr, 0, nullptr);
    std::vector<uint8_t> buf(len);
    EC_POINT_point2oct(group, pub, POINT_CONVERSION_UNCOMPRESSED,
                       buf.data(), len, nullptr);
    return bytes_to_hex(buf.data(), len);
}

ECKeyPair generate_keypair(const std::string& curve) {
    int nid = curve_nid(curve);
    EC_KEY* key = EC_KEY_new_by_curve_name(nid);
    if (!key) throw std::runtime_error("Failed to create EC key");
    if (!EC_KEY_generate_key(key)) {
        EC_KEY_free(key);
        throw std::runtime_error("Failed to generate EC key");
    }
    ECKeyPair kp;
    kp.curve           = curve;
    kp.private_key_pem = ec_key_to_private_pem(key);
    kp.public_key_pem  = ec_key_to_public_pem(key);
    kp.private_key_hex = ec_private_key_to_hex(key);
    kp.public_key_hex  = ec_public_key_to_hex(key);
    EC_KEY_free(key);
    return kp;
}

ECKeyPair keypair_from_private_hex(const std::string& priv_hex, const std::string& curve) {
    int nid = curve_nid(curve);
    EC_KEY* key = EC_KEY_new_by_curve_name(nid);
    BIGNUM* priv_bn = nullptr;
    BN_hex2bn(&priv_bn, priv_hex.c_str());
    EC_KEY_set_private_key(key, priv_bn);

    // Derive public key
    const EC_GROUP* group = EC_KEY_get0_group(key);
    EC_POINT* pub = EC_POINT_new(group);
    EC_POINT_mul(group, pub, priv_bn, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(key, pub);

    ECKeyPair kp;
    kp.curve           = curve;
    kp.private_key_pem = ec_key_to_private_pem(key);
    kp.public_key_pem  = ec_key_to_public_pem(key);
    kp.private_key_hex = priv_hex;
    kp.public_key_hex  = ec_public_key_to_hex(key);

    EC_POINT_free(pub);
    BN_free(priv_bn);
    EC_KEY_free(key);
    return kp;
}

std::string ecdsa_sign(const std::string& private_key_pem, const std::string& message) {
    BIO* bio = BIO_new_mem_buf(private_key_pem.data(),
                               static_cast<int>(private_key_pem.size()));
    EC_KEY* key = PEM_read_bio_ECPrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!key) throw std::runtime_error("Failed to load private key PEM");

    // Hash the message
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const uint8_t*>(message.data()), message.size(), hash);

    ECDSA_SIG* sig = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, key);
    EC_KEY_free(key);
    if (!sig) throw std::runtime_error("ECDSA signing failed");

    // DER encode
    int der_len = i2d_ECDSA_SIG(sig, nullptr);
    std::vector<uint8_t> der(static_cast<size_t>(der_len));
    uint8_t* p = der.data();
    i2d_ECDSA_SIG(sig, &p);
    ECDSA_SIG_free(sig);

    return bytes_to_hex(der.data(), der.size());
}

std::string ecdsa_sign_hex(const std::string& priv_hex, const std::string& message,
                           const std::string& curve) {
    auto kp = keypair_from_private_hex(priv_hex, curve);
    return ecdsa_sign(kp.private_key_pem, message);
}

bool ecdsa_verify(const std::string& public_key_pem, const std::string& message,
                  const std::string& signature_hex) {
    BIO* bio = BIO_new_mem_buf(public_key_pem.data(),
                               static_cast<int>(public_key_pem.size()));
    EC_KEY* key = PEM_read_bio_EC_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!key) return false;

    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const uint8_t*>(message.data()), message.size(), hash);

    auto der = from_hex(signature_hex);
    const uint8_t* p = der.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &p, static_cast<long>(der.size()));
    if (!sig) { EC_KEY_free(key); return false; }

    int result = ECDSA_do_verify(hash, SHA256_DIGEST_LENGTH, sig, key);
    ECDSA_SIG_free(sig);
    EC_KEY_free(key);
    return result == 1;
}

bool ecdsa_verify_hex(const std::string& pub_hex, const std::string& message,
                      const std::string& sig_hex, const std::string& curve) {
    int nid = curve_nid(curve);
    EC_KEY* key = EC_KEY_new_by_curve_name(nid);
    const EC_GROUP* group = EC_KEY_get0_group(key);

    auto pub_bytes = from_hex(pub_hex);
    EC_POINT* pub = EC_POINT_new(group);
    EC_POINT_oct2point(group, pub, pub_bytes.data(), pub_bytes.size(), nullptr);
    EC_KEY_set_public_key(key, pub);
    EC_POINT_free(pub);

    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const uint8_t*>(message.data()), message.size(), hash);

    auto der = from_hex(sig_hex);
    const uint8_t* p = der.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &p, static_cast<long>(der.size()));
    if (!sig) { EC_KEY_free(key); return false; }

    int result = ECDSA_do_verify(hash, SHA256_DIGEST_LENGTH, sig, key);
    ECDSA_SIG_free(sig);
    EC_KEY_free(key);
    return result == 1;
}

// ===== Key Management =====

std::string public_key_to_address(const std::string& pub_hex) {
    // Ethereum: keccak256(pubkey[1:])[12:] — we use SHA3-256 as approximation
    // For a proper impl we'd use keccak, but we'll use SHA256+RIPEMD160 as a demo
    auto pub_bytes = from_hex(pub_hex);
    // Skip the 0x04 prefix (uncompressed point marker)
    std::string pub_body(reinterpret_cast<const char*>(pub_bytes.data() + 1),
                         pub_bytes.size() - 1);
    std::string h = sha256(pub_body);
    // Take last 20 bytes (40 hex chars) as address
    return "0x" + h.substr(h.size() - 40);
}

std::string public_key_to_btc_address(const std::string& pub_hex) {
    // Bitcoin P2PKH: Base58Check(0x00 + RIPEMD160(SHA256(pubkey)))
    auto pub_bytes = from_hex(pub_hex);
    std::string pub_str(reinterpret_cast<const char*>(pub_bytes.data()), pub_bytes.size());

    // SHA256 then RIPEMD160
    uint8_t sha[SHA256_DIGEST_LENGTH];
    SHA256(pub_bytes.data(), pub_bytes.size(), sha);
    uint8_t ripe[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160(sha, SHA256_DIGEST_LENGTH, ripe);

    // Version byte 0x00 + hash
    std::vector<uint8_t> payload(21);
    payload[0] = 0x00;
    std::memcpy(payload.data() + 1, ripe, 20);

    // Checksum: SHA256(SHA256(payload))[0:4]
    uint8_t check1[SHA256_DIGEST_LENGTH], check2[SHA256_DIGEST_LENGTH];
    SHA256(payload.data(), 21, check1);
    SHA256(check1, SHA256_DIGEST_LENGTH, check2);

    std::vector<uint8_t> full(25);
    std::memcpy(full.data(), payload.data(), 21);
    std::memcpy(full.data() + 21, check2, 4);

    // Base58 encode
    static const char* BASE58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    BIGNUM* bn = BN_new();
    BN_bin2bn(full.data(), static_cast<int>(full.size()), bn);
    BIGNUM* base = BN_new(); BN_set_word(base, 58);
    BIGNUM* rem  = BN_new();
    BN_CTX* ctx  = BN_CTX_new();
    std::string result;
    while (!BN_is_zero(bn)) {
        BN_div(bn, rem, bn, base, ctx);
        result += BASE58[BN_get_word(rem)];
    }
    for (auto b : full) { if (b == 0) result += '1'; else break; }
    std::reverse(result.begin(), result.end());
    BN_free(bn); BN_free(base); BN_free(rem); BN_CTX_free(ctx);
    return result;
}

std::string private_key_to_wif(const std::string& priv_hex, bool compressed) {
    auto priv = from_hex(priv_hex);
    std::vector<uint8_t> payload;
    payload.push_back(0x80); // mainnet
    payload.insert(payload.end(), priv.begin(), priv.end());
    if (compressed) payload.push_back(0x01);

    uint8_t c1[SHA256_DIGEST_LENGTH], c2[SHA256_DIGEST_LENGTH];
    SHA256(payload.data(), payload.size(), c1);
    SHA256(c1, SHA256_DIGEST_LENGTH, c2);
    payload.insert(payload.end(), c2, c2 + 4);

    static const char* BASE58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    BIGNUM* bn = BN_new();
    BN_bin2bn(payload.data(), static_cast<int>(payload.size()), bn);
    BIGNUM* base = BN_new(); BN_set_word(base, 58);
    BIGNUM* rem  = BN_new();
    BN_CTX* ctx  = BN_CTX_new();
    std::string result;
    while (!BN_is_zero(bn)) {
        BN_div(bn, rem, bn, base, ctx);
        result += BASE58[BN_get_word(rem)];
    }
    std::reverse(result.begin(), result.end());
    BN_free(bn); BN_free(base); BN_free(rem); BN_CTX_free(ctx);
    return result;
}

std::string wif_to_private_key(const std::string& wif) {
    static const char* BASE58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    BIGNUM* bn = BN_new(); BN_zero(bn);
    BIGNUM* base = BN_new(); BN_set_word(base, 58);
    BIGNUM* tmp  = BN_new();
    BN_CTX* ctx  = BN_CTX_new();
    for (char c : wif) {
        const char* pos = std::strchr(BASE58, c);
        if (!pos) continue;
        BN_mul(bn, bn, base, ctx);
        BN_set_word(tmp, static_cast<unsigned long>(pos - BASE58));
        BN_add(bn, bn, tmp);
    }
    int len = BN_num_bytes(bn);
    std::vector<uint8_t> buf(static_cast<size_t>(len));
    BN_bn2bin(bn, buf.data());
    BN_free(bn); BN_free(base); BN_free(tmp); BN_CTX_free(ctx);
    // Strip version byte (0x80) and checksum (4 bytes), optional compression flag
    size_t start = 1;
    size_t end = buf.size() - 4;
    if (end > start && buf[end - 1] == 0x01) end--;
    return bytes_to_hex(buf.data() + start, end - start);
}

// ===== AES-256-GCM =====

AESResult aes256_encrypt(const std::string& key_hex, const std::string& plaintext) {
    auto key_bytes = from_hex(key_hex);
    if (key_bytes.size() < 32) key_bytes.resize(32, 0);

    // Random 12-byte IV
    uint8_t iv[12];
    RAND_bytes(iv, 12);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, nullptr);
    EVP_EncryptInit_ex(ctx, nullptr, nullptr, key_bytes.data(), iv);

    std::vector<uint8_t> ct(plaintext.size() + 16);
    int len = 0, ct_len = 0;
    EVP_EncryptUpdate(ctx, ct.data(), &len,
                      reinterpret_cast<const uint8_t*>(plaintext.data()),
                      static_cast<int>(plaintext.size()));
    ct_len = len;
    EVP_EncryptFinal_ex(ctx, ct.data() + ct_len, &len);
    ct_len += len;

    uint8_t tag[16];
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    EVP_CIPHER_CTX_free(ctx);

    AESResult res;
    res.ciphertext_hex = bytes_to_hex(ct.data(), static_cast<size_t>(ct_len));
    res.iv_hex         = bytes_to_hex(iv, 12);
    res.tag_hex        = bytes_to_hex(tag, 16);
    return res;
}

std::string aes256_decrypt(const std::string& key_hex,
                           const std::string& ct_hex,
                           const std::string& iv_hex,
                           const std::string& tag_hex) {
    auto key_bytes = from_hex(key_hex);
    if (key_bytes.size() < 32) key_bytes.resize(32, 0);
    auto ct  = from_hex(ct_hex);
    auto iv  = from_hex(iv_hex);
    auto tag = from_hex(tag_hex);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
                        static_cast<int>(iv.size()), nullptr);
    EVP_DecryptInit_ex(ctx, nullptr, nullptr, key_bytes.data(), iv.data());

    std::vector<uint8_t> pt(ct.size());
    int len = 0, pt_len = 0;
    EVP_DecryptUpdate(ctx, pt.data(), &len, ct.data(), static_cast<int>(ct.size()));
    pt_len = len;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
                        static_cast<int>(tag.size()),
                        const_cast<uint8_t*>(tag.data()));
    int ret = EVP_DecryptFinal_ex(ctx, pt.data() + pt_len, &len);
    EVP_CIPHER_CTX_free(ctx);

    if (ret <= 0) throw std::runtime_error("AES-GCM decryption failed: authentication tag mismatch");
    pt_len += len;
    return std::string(reinterpret_cast<char*>(pt.data()), static_cast<size_t>(pt_len));
}

// ===== Key Derivation =====

std::string pbkdf2_sha256(const std::string& password, const std::string& salt,
                          int iterations, int key_len) {
    std::vector<uint8_t> out(static_cast<size_t>(key_len));
    PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()),
                      reinterpret_cast<const uint8_t*>(salt.data()),
                      static_cast<int>(salt.size()),
                      iterations, EVP_sha256(), key_len, out.data());
    return bytes_to_hex(out.data(), out.size());
}

std::string random_bytes_hex(int count) {
    std::vector<uint8_t> buf(static_cast<size_t>(count));
    RAND_bytes(buf.data(), count);
    return bytes_to_hex(buf.data(), buf.size());
}

} // namespace Crypto
} // namespace Sapphire
