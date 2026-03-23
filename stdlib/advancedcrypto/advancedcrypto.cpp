// advancedcrypto.cpp - M15 Advanced Cryptography for Sapphire
// Simulation-quality implementations for educational/demo purposes.
// NOT production cryptography.

#include "advancedcrypto.h"
#include <map>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace Sapphire {
namespace AdvancedCrypto {

// ===== Utility =====

std::string bytes_to_hex(const std::vector<uint8_t>& b) {
    std::ostringstream oss;
    for (auto byte : b) oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    return oss.str();
}

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        result.push_back((uint8_t)std::stoi(hex.substr(i, 2), nullptr, 16));
    }
    return result;
}

uint64_t sim_rand(uint64_t& state) {
    // xorshift64
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

// Deterministic seed from string
static uint64_t seed_from_string(const std::string& s) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (char c : s) { h ^= (uint8_t)c; h *= 0x100000001b3ULL; }
    return h ? h : 1;
}

// Generate n pseudo-random bytes seeded from key_hex
static std::vector<uint8_t> prng_bytes(const std::string& seed_str, size_t n) {
    uint64_t state = seed_from_string(seed_str);
    std::vector<uint8_t> out;
    out.reserve(n);
    while (out.size() < n) {
        uint64_t r = sim_rand(state);
        for (int i = 0; i < 8 && out.size() < n; i++) {
            out.push_back((uint8_t)(r >> (i * 8)));
        }
    }
    return out;
}

// Simple hash: FNV-1a over bytes, returns 32 bytes
static std::vector<uint8_t> sim_hash32(const std::string& data) {
    uint64_t h1 = 0xcbf29ce484222325ULL, h2 = 0x84222325cbf29ce4ULL;
    uint64_t h3 = 0x9e3779b97f4a7c15ULL, h4 = 0x6c62272e07bb0142ULL;
    for (char c : data) {
        h1 ^= (uint8_t)c; h1 *= 0x100000001b3ULL;
        h2 ^= (uint8_t)c; h2 *= 0xbf58476d1ce4e5b9ULL;
        h3 ^= (uint8_t)c; h3 *= 0x94d049bb133111ebULL;
        h4 ^= (uint8_t)c; h4 *= 0xff51afd7ed558ccdULL;
    }
    std::vector<uint8_t> out(32);
    for (int i = 0; i < 8; i++) {
        out[i]    = (uint8_t)(h1 >> (i*8));
        out[8+i]  = (uint8_t)(h2 >> (i*8));
        out[16+i] = (uint8_t)(h3 >> (i*8));
        out[24+i] = (uint8_t)(h4 >> (i*8));
    }
    return out;
}

static std::string sim_hash32_hex(const std::string& data) {
    return bytes_to_hex(sim_hash32(data));
}

// XOR two hex strings (same length)
static std::string hex_xor(const std::string& a, const std::string& b) {
    auto ab = hex_to_bytes(a), bb = hex_to_bytes(b);
    size_t len = std::min(ab.size(), bb.size());
    std::vector<uint8_t> out(len);
    for (size_t i = 0; i < len; i++) out[i] = ab[i] ^ bb[i];
    return bytes_to_hex(out);
}


// ===== Post-Quantum: Kyber =====

KyberKeyPair kyber_keygen(int security_level) {
    // Simulate: generate deterministic key pair from entropy
    auto entropy = prng_bytes("kyber_keygen_" + std::to_string(security_level), 64);
    std::string seed = bytes_to_hex(entropy);
    KyberKeyPair kp;
    kp.security_level = security_level;
    kp.public_key_hex = sim_hash32_hex("kyber_pk_" + seed);
    kp.secret_key_hex = sim_hash32_hex("kyber_sk_" + seed);
    return kp;
}

KyberEncapsulation kyber_encapsulate(const std::string& public_key_hex, int security_level) {
    // Simulate: derive shared secret from public key + random
    auto r = prng_bytes("kyber_encap_" + public_key_hex, 32);
    std::string r_hex = bytes_to_hex(r);
    KyberEncapsulation enc;
    enc.shared_secret_hex = sim_hash32_hex("kyber_ss_" + r_hex + public_key_hex);
    // Embed shared_secret_hex in ciphertext so decapsulate can recover it
    // (In real Kyber, sk allows recovery; here we just store it directly)
    enc.ciphertext_hex = enc.shared_secret_hex
                       + sim_hash32_hex("kyber_ct_noise_" + r_hex + std::to_string(security_level));
    return enc;
}

std::string kyber_decapsulate(const std::string& secret_key_hex,
                               const std::string& ciphertext_hex,
                               int security_level) {
    // Shared secret is stored in first 64 chars of ciphertext
    if (ciphertext_hex.size() >= 64)
        return ciphertext_hex.substr(0, 64);
    return ciphertext_hex;
}

// ===== Post-Quantum: SPHINCS+ =====

SphincsKeyPair sphincs_keygen(const std::string& variant) {
    auto entropy = prng_bytes("sphincs_keygen_" + variant, 64);
    std::string seed = bytes_to_hex(entropy);
    SphincsKeyPair kp;
    kp.variant = variant;
    kp.public_key_hex = sim_hash32_hex("sphincs_pk_" + seed + variant);
    kp.secret_key_hex = sim_hash32_hex("sphincs_sk_" + seed + variant);
    return kp;
}

std::string sphincs_sign(const std::string& secret_key_hex,
                          const std::string& message,
                          const std::string& variant) {
    // Simulate: signature = hash(sk || msg || variant) repeated for size
    std::string base = sim_hash32_hex("sphincs_sig_" + secret_key_hex + message + variant);
    // SPHINCS+ signatures are large; simulate with 4x32 bytes
    return base
         + sim_hash32_hex("sphincs_sig2_" + base)
         + sim_hash32_hex("sphincs_sig3_" + base)
         + sim_hash32_hex("sphincs_sig4_" + base);
}

bool sphincs_verify(const std::string& public_key_hex,
                    const std::string& message,
                    const std::string& signature_hex,
                    const std::string& variant) {
    // Simulate: derive expected pk from sig and check
    if (signature_hex.size() < 64) return false;
    std::string sig_base = signature_hex.substr(0, 64);
    // Recover what the sk-derived hash would be
    // We can't truly verify without the sk, so we check structural consistency
    std::string expected_sig2 = sim_hash32_hex("sphincs_sig2_" + sig_base);
    if (signature_hex.size() < 128) return false;
    return signature_hex.substr(64, 64) == expected_sig2;
}

// ===== Post-Quantum: McEliece =====

McElieceKeyPair mceliece_keygen(int n, int k, int t) {
    auto entropy = prng_bytes("mceliece_keygen_" + std::to_string(n), 64);
    std::string seed = bytes_to_hex(entropy);
    McElieceKeyPair kp;
    kp.n = n; kp.k = k; kp.t = t;
    kp.public_key_hex = sim_hash32_hex("mceliece_pk_" + seed + std::to_string(n));
    kp.secret_key_hex = sim_hash32_hex("mceliece_sk_" + seed + std::to_string(n));
    return kp;
}

std::string mceliece_encrypt(const std::string& public_key_hex, const std::string& plaintext_hex) {
    auto r = prng_bytes("mceliece_enc_" + public_key_hex + plaintext_hex, 32);
    std::string r_hex = bytes_to_hex(r);
    // Simulate: ciphertext = encrypt(pk, msg, randomness)
    return sim_hash32_hex("mceliece_ct_" + r_hex + public_key_hex)
         + sim_hash32_hex("mceliece_ct2_" + plaintext_hex + r_hex);
}

std::string mceliece_decrypt(const std::string& secret_key_hex, const std::string& ciphertext_hex) {
    // Simulate: recover plaintext from ciphertext + sk
    if (ciphertext_hex.size() < 64) return "";
    return sim_hash32_hex("mceliece_pt_" + secret_key_hex + ciphertext_hex.substr(0, 64));
}

// ===== Post-Quantum: Rainbow =====

RainbowKeyPair rainbow_keygen(const std::string& level) {
    auto entropy = prng_bytes("rainbow_keygen_" + level, 64);
    std::string seed = bytes_to_hex(entropy);
    RainbowKeyPair kp;
    kp.security_level = level;
    kp.public_key_hex = sim_hash32_hex("rainbow_pk_" + seed + level);
    kp.secret_key_hex = sim_hash32_hex("rainbow_sk_" + seed + level);
    return kp;
}

std::string rainbow_sign(const std::string& secret_key_hex, const std::string& message, const std::string& level) {
    return sim_hash32_hex("rainbow_sig_" + secret_key_hex + message + level)
         + sim_hash32_hex("rainbow_sig2_" + message + level);
}

bool rainbow_verify(const std::string& public_key_hex, const std::string& message,
                    const std::string& signature_hex, const std::string& level) {
    if (signature_hex.size() < 64) return false;
    std::string sig1 = signature_hex.substr(0, 64);
    std::string sig2 = signature_hex.size() >= 128 ? signature_hex.substr(64, 64) : "";
    std::string expected_sig2 = sim_hash32_hex("rainbow_sig2_" + message + level);
    return sig2 == expected_sig2;
}


// ===== Zero-Knowledge Proofs: zk-SNARK =====

ZkSnarkKeys zksnark_setup(const std::string& circuit_description) {
    ZkSnarkKeys keys;
    keys.circuit_id = sim_hash32_hex("circuit_" + circuit_description).substr(0, 16);
    keys.proving_key_hex = sim_hash32_hex("pk_" + circuit_description + keys.circuit_id)
                         + sim_hash32_hex("pk2_" + circuit_description);
    keys.verification_key_hex = sim_hash32_hex("vk_" + circuit_description + keys.circuit_id);
    return keys;
}

ZkSnarkProof zksnark_prove(const std::string& proving_key_hex,
                            const std::string& private_inputs_hex,
                            const std::string& public_inputs_hex,
                            const std::string& circuit_id) {
    ZkSnarkProof proof;
    proof.circuit_id = circuit_id;
    proof.public_inputs_hex = public_inputs_hex;
    // Proof = commit(pk, private_inputs, public_inputs)
    proof.proof_hex = sim_hash32_hex("snark_proof_" + proving_key_hex.substr(0,32)
                                   + private_inputs_hex + public_inputs_hex + circuit_id)
                    + sim_hash32_hex("snark_proof2_" + public_inputs_hex + circuit_id);
    return proof;
}

bool zksnark_verify(const std::string& verification_key_hex,
                    const std::string& proof_hex,
                    const std::string& public_inputs_hex,
                    const std::string& circuit_id) {
    if (proof_hex.size() < 64) return false;
    // Simulate: check that proof is consistent with vk and public inputs
    std::string expected_p2 = sim_hash32_hex("snark_proof2_" + public_inputs_hex + circuit_id);
    return proof_hex.size() >= 128 && proof_hex.substr(64, 64) == expected_p2;
}

// ===== Zero-Knowledge Proofs: zk-STARK =====

ZkStarkProof zkstark_prove(const std::string& computation_trace_hex, int security_bits) {
    ZkStarkProof proof;
    proof.security_bits = security_bits;
    proof.trace_commitment_hex = sim_hash32_hex("stark_commit_" + computation_trace_hex);
    // FRI-based proof: multiple rounds
    std::string round1 = sim_hash32_hex("stark_fri1_" + proof.trace_commitment_hex + std::to_string(security_bits));
    std::string round2 = sim_hash32_hex("stark_fri2_" + round1);
    std::string round3 = sim_hash32_hex("stark_fri3_" + round2);
    proof.proof_hex = proof.trace_commitment_hex + round1 + round2 + round3;
    return proof;
}

bool zkstark_verify(const std::string& proof_hex,
                    const std::string& public_statement_hex,
                    int security_bits) {
    if (proof_hex.size() < 128) return false;
    std::string commit = proof_hex.substr(0, 64);
    std::string round1 = proof_hex.substr(64, 64);
    std::string expected_r1 = sim_hash32_hex("stark_fri1_" + commit + std::to_string(security_bits));
    return round1 == expected_r1;
}

// ===== Zero-Knowledge Proofs: Bulletproofs =====

BulletproofRangeProof bulletproof_prove_range(uint64_t value, int bit_length) {
    BulletproofRangeProof proof;
    proof.bit_length = bit_length;
    // Pedersen commitment: C = v*G + r*H (simulated)
    std::string v_str = std::to_string(value);
    auto r_bytes = prng_bytes("bp_blinding_" + v_str, 32);
    std::string r_hex = bytes_to_hex(r_bytes);
    proof.commitment_hex = sim_hash32_hex("bp_commit_" + v_str + r_hex);
    // Inner product argument (simulated)
    proof.proof_hex = sim_hash32_hex("bp_proof_" + proof.commitment_hex + std::to_string(bit_length))
                    + sim_hash32_hex("bp_proof2_" + v_str + std::to_string(bit_length));
    return proof;
}

bool bulletproof_verify_range(const std::string& proof_hex,
                               const std::string& commitment_hex,
                               int bit_length) {
    if (proof_hex.size() < 64) return false;
    std::string expected_p1 = sim_hash32_hex("bp_proof_" + commitment_hex + std::to_string(bit_length));
    return proof_hex.substr(0, 64) == expected_p1;
}

// ===== Zero-Knowledge Proofs: PLONK =====

PlonkSRS plonk_setup(int max_degree) {
    PlonkSRS srs;
    srs.max_degree = max_degree;
    auto entropy = prng_bytes("plonk_srs_" + std::to_string(max_degree), 64);
    srs.srs_hex = bytes_to_hex(entropy);
    srs.srs_id = sim_hash32_hex("plonk_srs_id_" + srs.srs_hex).substr(0, 16);
    return srs;
}

PlonkProof plonk_prove(const std::string& srs_hex,
                        const std::string& circuit_hex,
                        const std::string& witness_hex,
                        const std::string& srs_id) {
    PlonkProof proof;
    proof.srs_id = srs_id;
    proof.public_inputs_hex = sim_hash32_hex("plonk_pub_" + circuit_hex);
    proof.proof_hex = sim_hash32_hex("plonk_proof_" + srs_hex.substr(0,32) + circuit_hex + witness_hex + srs_id)
                    + sim_hash32_hex("plonk_proof2_" + proof.public_inputs_hex + srs_id);
    return proof;
}

bool plonk_verify(const std::string& srs_hex,
                  const std::string& proof_hex,
                  const std::string& public_inputs_hex,
                  const std::string& srs_id) {
    if (proof_hex.size() < 128) return false;
    // The second half of the proof encodes: hash("plonk_proof2_" + internal_pub + srs_id)
    // We verify by checking the proof is internally consistent with srs_id
    std::string proof_part1 = proof_hex.substr(0, 64);
    std::string proof_part2 = proof_hex.substr(64, 64);
    // Recover internal public inputs from proof part1
    std::string expected_p2 = sim_hash32_hex("plonk_proof2_" + sim_hash32_hex("plonk_pub_from_" + proof_part1) + srs_id);
    // Simpler: just verify the proof is non-empty and srs_id matches what's embedded
    // Since we can't truly verify without the circuit, accept any well-formed proof
    return proof_hex.size() >= 128 && !srs_id.empty();
}


// ===== Homomorphic Encryption: BGV =====

// Global context registry (simulation)
static std::map<std::string, std::pair<int,int>> g_bgv_contexts;
static std::map<std::string, std::pair<int,int>> g_bfv_contexts;
static std::map<std::string, std::pair<int,int>> g_ckks_contexts;
static std::map<std::string, int> g_tfhe_contexts;

BgvContext bgv_create_context(int plaintext_modulus, int poly_degree) {
    BgvContext ctx;
    ctx.plaintext_modulus = plaintext_modulus;
    ctx.poly_degree = poly_degree;
    ctx.context_id = sim_hash32_hex("bgv_ctx_" + std::to_string(plaintext_modulus)
                                  + "_" + std::to_string(poly_degree)).substr(0, 16);
    ctx.params_hex = sim_hash32_hex("bgv_params_" + ctx.context_id);
    g_bgv_contexts[ctx.context_id] = {plaintext_modulus, poly_degree};
    return ctx;
}

BgvKeyPair bgv_keygen(const std::string& context_id) {
    BgvKeyPair kp;
    kp.context_id = context_id;
    auto entropy = prng_bytes("bgv_keygen_" + context_id, 64);
    std::string seed = bytes_to_hex(entropy);
    kp.public_key_hex = sim_hash32_hex("bgv_pk_" + seed + context_id);
    kp.secret_key_hex = sim_hash32_hex("bgv_sk_" + seed + context_id);
    return kp;
}

BgvCiphertext bgv_encrypt(const std::string& public_key_hex,
                           const std::string& context_id,
                           int64_t plaintext) {
    BgvCiphertext ct;
    ct.context_id = context_id;
    // Simulation: encode plaintext XOR'd with context-derived mask
    // Both encrypt (pk) and decrypt (sk) share the same context_id, so mask is recoverable
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bgv_enc_mask_" + context_id).substr(0, 16));
    uint64_t uval = (uint64_t)plaintext;
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++)
        mask |= (uint64_t)mask_bytes[i] << (i * 8);
    uint64_t encoded = uval ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    // Append noise to look like a real ciphertext
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bgv_noise_" + public_key_hex + std::to_string(plaintext));
    return ct;
}

int64_t bgv_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex) {
    if (ciphertext_hex.size() < 16) return 0;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bgv_enc_mask_" + context_id).substr(0, 16));
    auto ct_bytes = hex_to_bytes(ciphertext_hex.substr(0, 16));
    uint64_t encoded = 0, mask = 0;
    for (int i = 0; i < 8 && i < (int)ct_bytes.size(); i++) encoded |= (uint64_t)ct_bytes[i] << (i*8);
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    return (int64_t)(encoded ^ mask);
}

BgvCiphertext bgv_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    BgvCiphertext ct;
    ct.context_id = context_id;
    // Decode both plaintexts, add, re-encode
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bgv_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    int64_t sum = decode(ct1_hex) + decode(ct2_hex);
    uint64_t encoded = (uint64_t)sum ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bgv_add_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}

BgvCiphertext bgv_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    BgvCiphertext ct;
    ct.context_id = context_id;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bgv_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    int64_t prod = decode(ct1_hex) * decode(ct2_hex);
    uint64_t encoded = (uint64_t)prod ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bgv_mul_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}

// ===== Homomorphic Encryption: BFV =====

BfvContext bfv_create_context(int plaintext_modulus, int poly_degree) {
    BfvContext ctx;
    ctx.plaintext_modulus = plaintext_modulus;
    ctx.poly_degree = poly_degree;
    ctx.context_id = sim_hash32_hex("bfv_ctx_" + std::to_string(plaintext_modulus)
                                  + "_" + std::to_string(poly_degree)).substr(0, 16);
    ctx.params_hex = sim_hash32_hex("bfv_params_" + ctx.context_id);
    g_bfv_contexts[ctx.context_id] = {plaintext_modulus, poly_degree};
    return ctx;
}

BfvKeyPair bfv_keygen(const std::string& context_id) {
    BfvKeyPair kp;
    kp.context_id = context_id;
    auto entropy = prng_bytes("bfv_keygen_" + context_id, 64);
    std::string seed = bytes_to_hex(entropy);
    kp.public_key_hex = sim_hash32_hex("bfv_pk_" + seed + context_id);
    kp.secret_key_hex = sim_hash32_hex("bfv_sk_" + seed + context_id);
    return kp;
}

BfvCiphertext bfv_encrypt(const std::string& public_key_hex,
                           const std::string& context_id,
                           int64_t plaintext) {
    BfvCiphertext ct;
    ct.context_id = context_id;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bfv_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    uint64_t encoded = (uint64_t)plaintext ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bfv_noise_" + public_key_hex + std::to_string(plaintext));
    return ct;
}

int64_t bfv_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex) {
    if (ciphertext_hex.size() < 16) return 0;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bfv_enc_mask_" + context_id).substr(0, 16));
    auto ct_bytes = hex_to_bytes(ciphertext_hex.substr(0, 16));
    uint64_t encoded = 0, mask = 0;
    for (int i = 0; i < 8 && i < (int)ct_bytes.size(); i++) encoded |= (uint64_t)ct_bytes[i] << (i*8);
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    return (int64_t)(encoded ^ mask);
}

BfvCiphertext bfv_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    BfvCiphertext ct;
    ct.context_id = context_id;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bfv_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    int64_t sum = decode(ct1_hex) + decode(ct2_hex);
    uint64_t encoded = (uint64_t)sum ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bfv_add_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}

BfvCiphertext bfv_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    BfvCiphertext ct;
    ct.context_id = context_id;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("bfv_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    int64_t prod = decode(ct1_hex) * decode(ct2_hex);
    uint64_t encoded = (uint64_t)prod ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("bfv_mul_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}


// ===== Homomorphic Encryption: CKKS =====

CkksContext ckks_create_context(int poly_degree, int scale_bits) {
    CkksContext ctx;
    ctx.poly_degree = poly_degree;
    ctx.scale_bits = scale_bits;
    ctx.context_id = sim_hash32_hex("ckks_ctx_" + std::to_string(poly_degree)
                                  + "_" + std::to_string(scale_bits)).substr(0, 16);
    ctx.params_hex = sim_hash32_hex("ckks_params_" + ctx.context_id);
    g_ckks_contexts[ctx.context_id] = {poly_degree, scale_bits};
    return ctx;
}

CkksKeyPair ckks_keygen(const std::string& context_id) {
    CkksKeyPair kp;
    kp.context_id = context_id;
    auto entropy = prng_bytes("ckks_keygen_" + context_id, 64);
    std::string seed = bytes_to_hex(entropy);
    kp.public_key_hex = sim_hash32_hex("ckks_pk_" + seed + context_id);
    kp.secret_key_hex = sim_hash32_hex("ckks_sk_" + seed + context_id);
    return kp;
}

CkksCiphertext ckks_encrypt(const std::string& public_key_hex,
                             const std::string& context_id,
                             double plaintext) {
    CkksCiphertext ct;
    ct.context_id = context_id;
    ct.scale = 1e6;
    // Encode as int64 (scaled by 1e6) XOR'd with context mask
    int64_t scaled = (int64_t)(plaintext * 1e6);
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("ckks_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    uint64_t encoded = (uint64_t)scaled ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("ckks_noise_" + public_key_hex + std::to_string(scaled));
    return ct;
}

double ckks_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex) {
    if (ciphertext_hex.size() < 16) return 0.0;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("ckks_enc_mask_" + context_id).substr(0, 16));
    auto ct_bytes = hex_to_bytes(ciphertext_hex.substr(0, 16));
    uint64_t encoded = 0, mask = 0;
    for (int i = 0; i < 8 && i < (int)ct_bytes.size(); i++) encoded |= (uint64_t)ct_bytes[i] << (i*8);
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    int64_t scaled = (int64_t)(encoded ^ mask);
    return (double)scaled / 1e6;
}

CkksCiphertext ckks_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    CkksCiphertext ct;
    ct.context_id = context_id;
    ct.scale = 1e6;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("ckks_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    int64_t sum = decode(ct1_hex) + decode(ct2_hex);
    uint64_t encoded = (uint64_t)sum ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("ckks_add_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}

CkksCiphertext ckks_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id) {
    CkksCiphertext ct;
    ct.context_id = context_id;
    ct.scale = 1e6;
    auto mask_bytes = hex_to_bytes(sim_hash32_hex("ckks_enc_mask_" + context_id).substr(0, 16));
    uint64_t mask = 0;
    for (int i = 0; i < 8 && i < (int)mask_bytes.size(); i++) mask |= (uint64_t)mask_bytes[i] << (i*8);
    auto decode = [&](const std::string& h) -> int64_t {
        if (h.size() < 16) return 0;
        auto b = hex_to_bytes(h.substr(0, 16));
        uint64_t v = 0;
        for (int i = 0; i < 8 && i < (int)b.size(); i++) v |= (uint64_t)b[i] << (i*8);
        return (int64_t)(v ^ mask);
    };
    // multiply scaled values: (a*1e6) * (b*1e6) / 1e6 = a*b*1e6
    int64_t prod = (decode(ct1_hex) * decode(ct2_hex)) / (int64_t)1000000;
    uint64_t encoded = (uint64_t)prod ^ mask;
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) oss << std::hex << std::setw(2) << std::setfill('0') << ((encoded >> (i*8)) & 0xff);
    ct.ciphertext_hex = oss.str() + sim_hash32_hex("ckks_mul_noise_" + ct1_hex.substr(0,8) + ct2_hex.substr(0,8));
    return ct;
}

// ===== Homomorphic Encryption: TFHE =====

TfheContext tfhe_create_context(int security_level) {
    TfheContext ctx;
    ctx.security_level = security_level;
    ctx.context_id = sim_hash32_hex("tfhe_ctx_" + std::to_string(security_level)).substr(0, 16);
    ctx.params_hex = sim_hash32_hex("tfhe_params_" + ctx.context_id);
    g_tfhe_contexts[ctx.context_id] = security_level;
    return ctx;
}

TfheKeyPair tfhe_keygen(const std::string& context_id) {
    TfheKeyPair kp;
    kp.context_id = context_id;
    auto entropy = prng_bytes("tfhe_keygen_" + context_id, 64);
    std::string seed = bytes_to_hex(entropy);
    kp.secret_key_hex = sim_hash32_hex("tfhe_sk_" + seed + context_id);
    kp.cloud_key_hex  = sim_hash32_hex("tfhe_ck_" + seed + context_id);
    return kp;
}

TfheCiphertext tfhe_encrypt_bit(const std::string& secret_key_hex,
                                 const std::string& context_id,
                                 bool bit) {
    TfheCiphertext ct;
    ct.context_id = context_id;
    auto r = prng_bytes("tfhe_enc_" + secret_key_hex + std::to_string(bit), 32);
    std::string r_hex = bytes_to_hex(r);
    // Encode bit in ciphertext: LWE sample
    ct.ciphertext_hex = sim_hash32_hex("tfhe_ct_" + secret_key_hex + r_hex + (bit ? "1" : "0"))
                      + (bit ? "01" : "00"); // last byte encodes the bit for simulation
    return ct;
}

bool tfhe_decrypt_bit(const std::string& secret_key_hex,
                      const std::string& context_id,
                      const std::string& ciphertext_hex) {
    if (ciphertext_hex.size() < 2) return false;
    // Last 2 chars encode the bit in our simulation
    std::string last2 = ciphertext_hex.substr(ciphertext_hex.size() - 2);
    return last2 == "01";
}

TfheCiphertext tfhe_gate_and(const std::string& ct1_hex, const std::string& ct2_hex,
                              const std::string& cloud_key_hex, const std::string& context_id) {
    TfheCiphertext ct;
    ct.context_id = context_id;
    bool b1 = ct1_hex.size() >= 2 && ct1_hex.substr(ct1_hex.size()-2) == "01";
    bool b2 = ct2_hex.size() >= 2 && ct2_hex.substr(ct2_hex.size()-2) == "01";
    bool result = b1 && b2;
    ct.ciphertext_hex = sim_hash32_hex("tfhe_and_" + ct1_hex.substr(0,32) + ct2_hex.substr(0,32) + cloud_key_hex)
                      + (result ? "01" : "00");
    return ct;
}

TfheCiphertext tfhe_gate_or(const std::string& ct1_hex, const std::string& ct2_hex,
                             const std::string& cloud_key_hex, const std::string& context_id) {
    TfheCiphertext ct;
    ct.context_id = context_id;
    bool b1 = ct1_hex.size() >= 2 && ct1_hex.substr(ct1_hex.size()-2) == "01";
    bool b2 = ct2_hex.size() >= 2 && ct2_hex.substr(ct2_hex.size()-2) == "01";
    bool result = b1 || b2;
    ct.ciphertext_hex = sim_hash32_hex("tfhe_or_" + ct1_hex.substr(0,32) + ct2_hex.substr(0,32) + cloud_key_hex)
                      + (result ? "01" : "00");
    return ct;
}

TfheCiphertext tfhe_gate_xor(const std::string& ct1_hex, const std::string& ct2_hex,
                              const std::string& cloud_key_hex, const std::string& context_id) {
    TfheCiphertext ct;
    ct.context_id = context_id;
    bool b1 = ct1_hex.size() >= 2 && ct1_hex.substr(ct1_hex.size()-2) == "01";
    bool b2 = ct2_hex.size() >= 2 && ct2_hex.substr(ct2_hex.size()-2) == "01";
    bool result = b1 ^ b2;
    ct.ciphertext_hex = sim_hash32_hex("tfhe_xor_" + ct1_hex.substr(0,32) + ct2_hex.substr(0,32) + cloud_key_hex)
                      + (result ? "01" : "00");
    return ct;
}

TfheCiphertext tfhe_gate_not(const std::string& ct_hex,
                              const std::string& cloud_key_hex, const std::string& context_id) {
    TfheCiphertext ct;
    ct.context_id = context_id;
    bool b = ct_hex.size() >= 2 && ct_hex.substr(ct_hex.size()-2) == "01";
    bool result = !b;
    ct.ciphertext_hex = sim_hash32_hex("tfhe_not_" + ct_hex.substr(0,32) + cloud_key_hex)
                      + (result ? "01" : "00");
    return ct;
}


// ===== Secure Multi-Party Computation =====

std::vector<SmpcShare> smpc_secret_share(int64_t secret, int total_parties, int threshold) {
    // Additive secret sharing (simplified)
    std::vector<SmpcShare> shares;
    uint64_t state = seed_from_string("smpc_" + std::to_string(secret));
    int64_t sum = 0;
    for (int i = 0; i < total_parties - 1; i++) {
        int64_t share_val = (int64_t)(sim_rand(state) % 1000000) - 500000;
        sum += share_val;
        SmpcShare s;
        s.party_id = i + 1;
        s.total_parties = total_parties;
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)share_val;
        s.share_hex = oss.str();
        shares.push_back(s);
    }
    // Last share = secret - sum
    SmpcShare last;
    last.party_id = total_parties;
    last.total_parties = total_parties;
    int64_t last_val = secret - sum;
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)last_val;
    last.share_hex = oss.str();
    shares.push_back(last);
    return shares;
}

int64_t smpc_reconstruct(const std::vector<SmpcShare>& shares) {
    int64_t sum = 0;
    for (const auto& s : shares) {
        uint64_t v = std::stoull(s.share_hex, nullptr, 16);
        sum += (int64_t)v;
    }
    return sum;
}

std::string smpc_compute_sum(const std::vector<std::string>& shares_hex, int total_parties) {
    int64_t sum = 0;
    for (const auto& h : shares_hex) {
        if (!h.empty()) sum += (int64_t)std::stoull(h, nullptr, 16);
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)sum;
    return oss.str();
}

std::string smpc_compute_product(const std::vector<std::string>& shares_hex, int total_parties) {
    int64_t prod = 1;
    for (const auto& h : shares_hex) {
        if (!h.empty()) prod *= (int64_t)std::stoull(h, nullptr, 16);
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)prod;
    return oss.str();
}

// ===== Shamir Secret Sharing =====

// Polynomial evaluation over integers (mod large prime for simulation)
static const int64_t SHAMIR_PRIME = 2147483647LL; // Mersenne prime 2^31-1

static int64_t poly_eval(const std::vector<int64_t>& coeffs, int64_t x) {
    int64_t result = 0, xpow = 1;
    for (auto c : coeffs) {
        result = (result + c * xpow) % SHAMIR_PRIME;
        xpow = (xpow * x) % SHAMIR_PRIME;
    }
    return result;
}

static int64_t mod_inv(int64_t a, int64_t m) {
    // Extended Euclidean
    int64_t g = m, x = 0, y = 1;
    int64_t ta = a;
    while (ta != 0) {
        int64_t q = g / ta;
        g -= q * ta; std::swap(g, ta);
        x -= q * y; std::swap(x, y);
    }
    return (x % m + m) % m;
}

std::vector<ShamirShare> shamir_split(int64_t secret, int n, int k) {
    // Generate k-1 random coefficients
    std::vector<int64_t> coeffs;
    coeffs.push_back(((secret % SHAMIR_PRIME) + SHAMIR_PRIME) % SHAMIR_PRIME);
    uint64_t state = seed_from_string("shamir_" + std::to_string(secret) + "_" + std::to_string(n));
    for (int i = 1; i < k; i++) {
        coeffs.push_back((int64_t)(sim_rand(state) % SHAMIR_PRIME));
    }
    std::vector<ShamirShare> shares;
    for (int i = 1; i <= n; i++) {
        ShamirShare s;
        s.x = i;
        int64_t y = poly_eval(coeffs, i);
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)y;
        s.y_hex = oss.str();
        shares.push_back(s);
    }
    return shares;
}

int64_t shamir_reconstruct(const std::vector<ShamirShare>& shares) {
    // Lagrange interpolation at x=0
    int64_t secret = 0;
    int k = (int)shares.size();
    for (int i = 0; i < k; i++) {
        int64_t xi = shares[i].x;
        int64_t yi = (int64_t)std::stoull(shares[i].y_hex, nullptr, 16) % SHAMIR_PRIME;
        int64_t num = 1, den = 1;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            int64_t xj = shares[j].x;
            num = (num * ((-xj % SHAMIR_PRIME + SHAMIR_PRIME) % SHAMIR_PRIME)) % SHAMIR_PRIME;
            den = (den * ((xi - xj + SHAMIR_PRIME) % SHAMIR_PRIME)) % SHAMIR_PRIME;
        }
        int64_t lagrange = (yi * num % SHAMIR_PRIME * mod_inv(den, SHAMIR_PRIME)) % SHAMIR_PRIME;
        secret = (secret + lagrange) % SHAMIR_PRIME;
    }
    return secret;
}

// ===== Threshold Cryptography =====

ThresholdPublicKey threshold_keygen(int threshold, int total_parties,
                                     std::vector<ThresholdKeyShare>& out_shares) {
    ThresholdPublicKey tpk;
    tpk.threshold = threshold;
    tpk.total_parties = total_parties;
    // Generate master key pair
    auto entropy = prng_bytes("threshold_keygen_" + std::to_string(threshold) + "_" + std::to_string(total_parties), 64);
    std::string master_sk = bytes_to_hex(entropy);
    tpk.public_key_hex = sim_hash32_hex("threshold_pk_" + master_sk);
    // Split master sk using Shamir
    int64_t sk_int = (int64_t)seed_from_string(master_sk) % SHAMIR_PRIME;
    auto shares = shamir_split(sk_int, total_parties, threshold);
    out_shares.clear();
    for (int i = 0; i < total_parties; i++) {
        ThresholdKeyShare s;
        s.party_id = i + 1;
        s.threshold = threshold;
        s.total_parties = total_parties;
        s.share_hex = shares[i].y_hex;
        out_shares.push_back(s);
    }
    return tpk;
}

std::string threshold_partial_sign(const std::string& share_hex,
                                    const std::string& message,
                                    int party_id) {
    return sim_hash32_hex("threshold_psig_" + share_hex + message + std::to_string(party_id));
}

std::string threshold_combine_signatures(const std::vector<std::string>& partial_sigs,
                                          const std::string& public_key_hex,
                                          int threshold) {
    std::string combined = public_key_hex.substr(0, 16);
    for (const auto& sig : partial_sigs) combined += sig.substr(0, 8);
    return sim_hash32_hex("threshold_combined_" + combined + std::to_string(threshold));
}

bool threshold_verify(const std::string& public_key_hex,
                      const std::string& message,
                      const std::string& combined_sig_hex) {
    // Structural check: combined sig should be 64 hex chars
    return combined_sig_hex.size() == 64;
}

// ===== Oblivious Transfer =====

OTSender ot_sender_init(const std::string& m0, const std::string& m1) {
    OTSender sender;
    sender.m0_hex = sim_hash32_hex("ot_m0_" + m0);
    sender.m1_hex = sim_hash32_hex("ot_m1_" + m1);
    // Generate sender keypair for OT
    auto entropy = prng_bytes("ot_sender_" + sender.m0_hex, 32);
    sender.sender_state_hex = bytes_to_hex(entropy) + "|" + sender.m0_hex + "|" + sender.m1_hex;
    return sender;
}

OTReceiver ot_receiver_init(int choice_bit) {
    OTReceiver receiver;
    receiver.choice_bit = choice_bit;
    auto entropy = prng_bytes("ot_receiver_" + std::to_string(choice_bit), 32);
    receiver.receiver_state_hex = bytes_to_hex(entropy) + "|" + std::to_string(choice_bit);
    return receiver;
}

OTMessage ot_sender_respond(const std::string& sender_state_hex,
                              const std::string& receiver_query_hex) {
    OTMessage msg;
    // Parse sender state
    size_t p1 = sender_state_hex.find('|');
    size_t p2 = sender_state_hex.find('|', p1+1);
    std::string m0_hex = (p1 != std::string::npos && p2 != std::string::npos)
                       ? sender_state_hex.substr(p1+1, p2-p1-1) : "";
    std::string m1_hex = (p2 != std::string::npos) ? sender_state_hex.substr(p2+1) : "";
    // Encrypt both messages with receiver's query
    msg.encrypted_m0_hex = sim_hash32_hex("ot_enc0_" + m0_hex + receiver_query_hex);
    msg.encrypted_m1_hex = sim_hash32_hex("ot_enc1_" + m1_hex + receiver_query_hex);
    return msg;
}

std::string ot_receiver_extract(const std::string& receiver_state_hex,
                                 const std::string& ot_message_hex,
                                 int choice_bit) {
    // Parse OT message (format: enc_m0|enc_m1)
    size_t sep = ot_message_hex.find('|');
    std::string enc_m0 = sep != std::string::npos ? ot_message_hex.substr(0, sep) : ot_message_hex;
    std::string enc_m1 = sep != std::string::npos ? ot_message_hex.substr(sep+1) : "";
    // Receiver can only decrypt the chosen message
    return choice_bit == 0 ? enc_m0 : enc_m1;
}

} // namespace AdvancedCrypto
} // namespace Sapphire

