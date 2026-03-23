// advancedcrypto_capi.cpp - C API for M15 Advanced Cryptography
#include "advancedcrypto.h"
#include <string>
#include <vector>
#include <cstring>

using namespace Sapphire::AdvancedCrypto;

static std::string g_advcrypto_last_string;

extern "C" {

// ===== Kyber =====

void* advcrypto_kyber_keygen(int security_level) {
    return new KyberKeyPair(kyber_keygen(security_level));
}
void advcrypto_kyber_destroy_keypair(void* kp) { delete static_cast<KyberKeyPair*>(kp); }
const char* advcrypto_kyber_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<KyberKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_kyber_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<KyberKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_kyber_security_level(void* kp) {
    return static_cast<KyberKeyPair*>(kp)->security_level;
}
void* advcrypto_kyber_encapsulate(const char* public_key_hex, int security_level) {
    return new KyberEncapsulation(kyber_encapsulate(public_key_hex ? public_key_hex : "", security_level));
}
void advcrypto_kyber_destroy_encap(void* enc) { delete static_cast<KyberEncapsulation*>(enc); }
const char* advcrypto_kyber_ciphertext(void* enc) {
    g_advcrypto_last_string = static_cast<KyberEncapsulation*>(enc)->ciphertext_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_kyber_shared_secret(void* enc) {
    g_advcrypto_last_string = static_cast<KyberEncapsulation*>(enc)->shared_secret_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_kyber_decapsulate(const char* secret_key_hex, const char* ciphertext_hex, int security_level) {
    g_advcrypto_last_string = kyber_decapsulate(
        secret_key_hex ? secret_key_hex : "",
        ciphertext_hex ? ciphertext_hex : "",
        security_level);
    return g_advcrypto_last_string.c_str();
}

// ===== SPHINCS+ =====

void* advcrypto_sphincs_keygen(const char* variant) {
    return new SphincsKeyPair(sphincs_keygen(variant ? variant : "sha2-128s"));
}
void advcrypto_sphincs_destroy_keypair(void* kp) { delete static_cast<SphincsKeyPair*>(kp); }
const char* advcrypto_sphincs_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<SphincsKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_sphincs_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<SphincsKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_sphincs_sign(const char* secret_key_hex, const char* message, const char* variant) {
    g_advcrypto_last_string = sphincs_sign(
        secret_key_hex ? secret_key_hex : "",
        message ? message : "",
        variant ? variant : "sha2-128s");
    return g_advcrypto_last_string.c_str();
}
int advcrypto_sphincs_verify(const char* public_key_hex, const char* message,
                              const char* signature_hex, const char* variant) {
    return sphincs_verify(
        public_key_hex ? public_key_hex : "",
        message ? message : "",
        signature_hex ? signature_hex : "",
        variant ? variant : "sha2-128s") ? 1 : 0;
}


// ===== McEliece =====

void* advcrypto_mceliece_keygen(int n, int k, int t) {
    return new McElieceKeyPair(mceliece_keygen(n, k, t));
}
void advcrypto_mceliece_destroy_keypair(void* kp) { delete static_cast<McElieceKeyPair*>(kp); }
const char* advcrypto_mceliece_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<McElieceKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_mceliece_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<McElieceKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_mceliece_encrypt(const char* public_key_hex, const char* plaintext_hex) {
    g_advcrypto_last_string = mceliece_encrypt(
        public_key_hex ? public_key_hex : "",
        plaintext_hex ? plaintext_hex : "");
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_mceliece_decrypt(const char* secret_key_hex, const char* ciphertext_hex) {
    g_advcrypto_last_string = mceliece_decrypt(
        secret_key_hex ? secret_key_hex : "",
        ciphertext_hex ? ciphertext_hex : "");
    return g_advcrypto_last_string.c_str();
}

// ===== Rainbow =====

void* advcrypto_rainbow_keygen(const char* level) {
    return new RainbowKeyPair(rainbow_keygen(level ? level : "I"));
}
void advcrypto_rainbow_destroy_keypair(void* kp) { delete static_cast<RainbowKeyPair*>(kp); }
const char* advcrypto_rainbow_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<RainbowKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_rainbow_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<RainbowKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_rainbow_sign(const char* secret_key_hex, const char* message, const char* level) {
    g_advcrypto_last_string = rainbow_sign(
        secret_key_hex ? secret_key_hex : "",
        message ? message : "",
        level ? level : "I");
    return g_advcrypto_last_string.c_str();
}
int advcrypto_rainbow_verify(const char* public_key_hex, const char* message,
                              const char* signature_hex, const char* level) {
    return rainbow_verify(
        public_key_hex ? public_key_hex : "",
        message ? message : "",
        signature_hex ? signature_hex : "",
        level ? level : "I") ? 1 : 0;
}

// ===== zk-SNARK =====

void* advcrypto_zksnark_setup(const char* circuit_description) {
    return new ZkSnarkKeys(zksnark_setup(circuit_description ? circuit_description : ""));
}
void advcrypto_zksnark_destroy_keys(void* keys) { delete static_cast<ZkSnarkKeys*>(keys); }
const char* advcrypto_zksnark_proving_key(void* keys) {
    g_advcrypto_last_string = static_cast<ZkSnarkKeys*>(keys)->proving_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_zksnark_verification_key(void* keys) {
    g_advcrypto_last_string = static_cast<ZkSnarkKeys*>(keys)->verification_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_zksnark_circuit_id(void* keys) {
    g_advcrypto_last_string = static_cast<ZkSnarkKeys*>(keys)->circuit_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_zksnark_prove(const char* proving_key_hex, const char* private_inputs_hex,
                               const char* public_inputs_hex, const char* circuit_id) {
    return new ZkSnarkProof(zksnark_prove(
        proving_key_hex ? proving_key_hex : "",
        private_inputs_hex ? private_inputs_hex : "",
        public_inputs_hex ? public_inputs_hex : "",
        circuit_id ? circuit_id : ""));
}
void advcrypto_zksnark_destroy_proof(void* proof) { delete static_cast<ZkSnarkProof*>(proof); }
const char* advcrypto_zksnark_proof_hex(void* proof) {
    g_advcrypto_last_string = static_cast<ZkSnarkProof*>(proof)->proof_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_zksnark_public_inputs(void* proof) {
    g_advcrypto_last_string = static_cast<ZkSnarkProof*>(proof)->public_inputs_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_zksnark_verify(const char* verification_key_hex, const char* proof_hex,
                              const char* public_inputs_hex, const char* circuit_id) {
    return zksnark_verify(
        verification_key_hex ? verification_key_hex : "",
        proof_hex ? proof_hex : "",
        public_inputs_hex ? public_inputs_hex : "",
        circuit_id ? circuit_id : "") ? 1 : 0;
}

// ===== zk-STARK =====

void* advcrypto_zkstark_prove(const char* computation_trace_hex, int security_bits) {
    return new ZkStarkProof(zkstark_prove(computation_trace_hex ? computation_trace_hex : "", security_bits));
}
void advcrypto_zkstark_destroy_proof(void* proof) { delete static_cast<ZkStarkProof*>(proof); }
const char* advcrypto_zkstark_proof_hex(void* proof) {
    g_advcrypto_last_string = static_cast<ZkStarkProof*>(proof)->proof_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_zkstark_trace_commitment(void* proof) {
    g_advcrypto_last_string = static_cast<ZkStarkProof*>(proof)->trace_commitment_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_zkstark_verify(const char* proof_hex, const char* public_statement_hex, int security_bits) {
    return zkstark_verify(
        proof_hex ? proof_hex : "",
        public_statement_hex ? public_statement_hex : "",
        security_bits) ? 1 : 0;
}

// ===== Bulletproofs =====

void* advcrypto_bulletproof_prove_range(long long value, int bit_length) {
    return new BulletproofRangeProof(bulletproof_prove_range((uint64_t)value, bit_length));
}
void advcrypto_bulletproof_destroy(void* proof) { delete static_cast<BulletproofRangeProof*>(proof); }
const char* advcrypto_bulletproof_proof_hex(void* proof) {
    g_advcrypto_last_string = static_cast<BulletproofRangeProof*>(proof)->proof_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_bulletproof_commitment_hex(void* proof) {
    g_advcrypto_last_string = static_cast<BulletproofRangeProof*>(proof)->commitment_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_bulletproof_verify_range(const char* proof_hex, const char* commitment_hex, int bit_length) {
    return bulletproof_verify_range(
        proof_hex ? proof_hex : "",
        commitment_hex ? commitment_hex : "",
        bit_length) ? 1 : 0;
}

// ===== PLONK =====

void* advcrypto_plonk_setup(int max_degree) {
    return new PlonkSRS(plonk_setup(max_degree));
}
void advcrypto_plonk_destroy_srs(void* srs) { delete static_cast<PlonkSRS*>(srs); }
const char* advcrypto_plonk_srs_hex(void* srs) {
    g_advcrypto_last_string = static_cast<PlonkSRS*>(srs)->srs_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_plonk_srs_id(void* srs) {
    g_advcrypto_last_string = static_cast<PlonkSRS*>(srs)->srs_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_plonk_prove(const char* srs_hex, const char* circuit_hex,
                              const char* witness_hex, const char* srs_id) {
    return new PlonkProof(plonk_prove(
        srs_hex ? srs_hex : "",
        circuit_hex ? circuit_hex : "",
        witness_hex ? witness_hex : "",
        srs_id ? srs_id : ""));
}
void advcrypto_plonk_destroy_proof(void* proof) { delete static_cast<PlonkProof*>(proof); }
const char* advcrypto_plonk_proof_hex(void* proof) {
    g_advcrypto_last_string = static_cast<PlonkProof*>(proof)->proof_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_plonk_verify(const char* srs_hex, const char* proof_hex,
                            const char* public_inputs_hex, const char* srs_id) {
    return plonk_verify(
        srs_hex ? srs_hex : "",
        proof_hex ? proof_hex : "",
        public_inputs_hex ? public_inputs_hex : "",
        srs_id ? srs_id : "") ? 1 : 0;
}


// ===== BGV =====

void* advcrypto_bgv_create_context(int plaintext_modulus, int poly_degree) {
    return new BgvContext(bgv_create_context(plaintext_modulus, poly_degree));
}
void advcrypto_bgv_destroy_context(void* ctx) { delete static_cast<BgvContext*>(ctx); }
const char* advcrypto_bgv_context_id(void* ctx) {
    g_advcrypto_last_string = static_cast<BgvContext*>(ctx)->context_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_bgv_keygen(const char* context_id) {
    return new BgvKeyPair(bgv_keygen(context_id ? context_id : ""));
}
void advcrypto_bgv_destroy_keypair(void* kp) { delete static_cast<BgvKeyPair*>(kp); }
const char* advcrypto_bgv_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<BgvKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_bgv_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<BgvKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_bgv_encrypt(const char* public_key_hex, const char* context_id, long long plaintext) {
    return new BgvCiphertext(bgv_encrypt(
        public_key_hex ? public_key_hex : "",
        context_id ? context_id : "",
        (int64_t)plaintext));
}
void advcrypto_bgv_destroy_ciphertext(void* ct) { delete static_cast<BgvCiphertext*>(ct); }
const char* advcrypto_bgv_ciphertext_hex(void* ct) {
    g_advcrypto_last_string = static_cast<BgvCiphertext*>(ct)->ciphertext_hex;
    return g_advcrypto_last_string.c_str();
}
long long advcrypto_bgv_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex) {
    return (long long)bgv_decrypt(
        secret_key_hex ? secret_key_hex : "",
        context_id ? context_id : "",
        ciphertext_hex ? ciphertext_hex : "");
}
void* advcrypto_bgv_add(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new BgvCiphertext(bgv_add(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}
void* advcrypto_bgv_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new BgvCiphertext(bgv_multiply(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}

// ===== BFV =====

void* advcrypto_bfv_create_context(int plaintext_modulus, int poly_degree) {
    return new BfvContext(bfv_create_context(plaintext_modulus, poly_degree));
}
void advcrypto_bfv_destroy_context(void* ctx) { delete static_cast<BfvContext*>(ctx); }
const char* advcrypto_bfv_context_id(void* ctx) {
    g_advcrypto_last_string = static_cast<BfvContext*>(ctx)->context_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_bfv_keygen(const char* context_id) {
    return new BfvKeyPair(bfv_keygen(context_id ? context_id : ""));
}
void advcrypto_bfv_destroy_keypair(void* kp) { delete static_cast<BfvKeyPair*>(kp); }
const char* advcrypto_bfv_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<BfvKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_bfv_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<BfvKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_bfv_encrypt(const char* public_key_hex, const char* context_id, long long plaintext) {
    return new BfvCiphertext(bfv_encrypt(
        public_key_hex ? public_key_hex : "",
        context_id ? context_id : "",
        (int64_t)plaintext));
}
void advcrypto_bfv_destroy_ciphertext(void* ct) { delete static_cast<BfvCiphertext*>(ct); }
const char* advcrypto_bfv_ciphertext_hex(void* ct) {
    g_advcrypto_last_string = static_cast<BfvCiphertext*>(ct)->ciphertext_hex;
    return g_advcrypto_last_string.c_str();
}
long long advcrypto_bfv_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex) {
    return (long long)bfv_decrypt(
        secret_key_hex ? secret_key_hex : "",
        context_id ? context_id : "",
        ciphertext_hex ? ciphertext_hex : "");
}
void* advcrypto_bfv_add(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new BfvCiphertext(bfv_add(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}
void* advcrypto_bfv_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new BfvCiphertext(bfv_multiply(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}

// ===== CKKS =====

void* advcrypto_ckks_create_context(int poly_degree, int scale_bits) {
    return new CkksContext(ckks_create_context(poly_degree, scale_bits));
}
void advcrypto_ckks_destroy_context(void* ctx) { delete static_cast<CkksContext*>(ctx); }
const char* advcrypto_ckks_context_id(void* ctx) {
    g_advcrypto_last_string = static_cast<CkksContext*>(ctx)->context_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_ckks_keygen(const char* context_id) {
    return new CkksKeyPair(ckks_keygen(context_id ? context_id : ""));
}
void advcrypto_ckks_destroy_keypair(void* kp) { delete static_cast<CkksKeyPair*>(kp); }
const char* advcrypto_ckks_public_key(void* kp) {
    g_advcrypto_last_string = static_cast<CkksKeyPair*>(kp)->public_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_ckks_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<CkksKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_ckks_encrypt(const char* public_key_hex, const char* context_id, double plaintext) {
    return new CkksCiphertext(ckks_encrypt(
        public_key_hex ? public_key_hex : "",
        context_id ? context_id : "",
        plaintext));
}
void advcrypto_ckks_destroy_ciphertext(void* ct) { delete static_cast<CkksCiphertext*>(ct); }
const char* advcrypto_ckks_ciphertext_hex(void* ct) {
    g_advcrypto_last_string = static_cast<CkksCiphertext*>(ct)->ciphertext_hex;
    return g_advcrypto_last_string.c_str();
}
double advcrypto_ckks_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex) {
    return ckks_decrypt(
        secret_key_hex ? secret_key_hex : "",
        context_id ? context_id : "",
        ciphertext_hex ? ciphertext_hex : "");
}
void* advcrypto_ckks_add(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new CkksCiphertext(ckks_add(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}
void* advcrypto_ckks_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id) {
    return new CkksCiphertext(ckks_multiply(
        ct1_hex ? ct1_hex : "",
        ct2_hex ? ct2_hex : "",
        context_id ? context_id : ""));
}

// ===== TFHE =====

void* advcrypto_tfhe_create_context(int security_level) {
    return new TfheContext(tfhe_create_context(security_level));
}
void advcrypto_tfhe_destroy_context(void* ctx) { delete static_cast<TfheContext*>(ctx); }
const char* advcrypto_tfhe_context_id(void* ctx) {
    g_advcrypto_last_string = static_cast<TfheContext*>(ctx)->context_id;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_tfhe_keygen(const char* context_id) {
    return new TfheKeyPair(tfhe_keygen(context_id ? context_id : ""));
}
void advcrypto_tfhe_destroy_keypair(void* kp) { delete static_cast<TfheKeyPair*>(kp); }
const char* advcrypto_tfhe_secret_key(void* kp) {
    g_advcrypto_last_string = static_cast<TfheKeyPair*>(kp)->secret_key_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_tfhe_cloud_key(void* kp) {
    g_advcrypto_last_string = static_cast<TfheKeyPair*>(kp)->cloud_key_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_tfhe_encrypt_bit(const char* secret_key_hex, const char* context_id, int bit) {
    return new TfheCiphertext(tfhe_encrypt_bit(
        secret_key_hex ? secret_key_hex : "",
        context_id ? context_id : "",
        bit != 0));
}
void advcrypto_tfhe_destroy_ciphertext(void* ct) { delete static_cast<TfheCiphertext*>(ct); }
const char* advcrypto_tfhe_ciphertext_hex(void* ct) {
    g_advcrypto_last_string = static_cast<TfheCiphertext*>(ct)->ciphertext_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_tfhe_decrypt_bit(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex) {
    return tfhe_decrypt_bit(
        secret_key_hex ? secret_key_hex : "",
        context_id ? context_id : "",
        ciphertext_hex ? ciphertext_hex : "") ? 1 : 0;
}
void* advcrypto_tfhe_gate_and(const char* ct1_hex, const char* ct2_hex,
                               const char* cloud_key_hex, const char* context_id) {
    return new TfheCiphertext(tfhe_gate_and(
        ct1_hex ? ct1_hex : "", ct2_hex ? ct2_hex : "",
        cloud_key_hex ? cloud_key_hex : "", context_id ? context_id : ""));
}
void* advcrypto_tfhe_gate_or(const char* ct1_hex, const char* ct2_hex,
                              const char* cloud_key_hex, const char* context_id) {
    return new TfheCiphertext(tfhe_gate_or(
        ct1_hex ? ct1_hex : "", ct2_hex ? ct2_hex : "",
        cloud_key_hex ? cloud_key_hex : "", context_id ? context_id : ""));
}
void* advcrypto_tfhe_gate_xor(const char* ct1_hex, const char* ct2_hex,
                               const char* cloud_key_hex, const char* context_id) {
    return new TfheCiphertext(tfhe_gate_xor(
        ct1_hex ? ct1_hex : "", ct2_hex ? ct2_hex : "",
        cloud_key_hex ? cloud_key_hex : "", context_id ? context_id : ""));
}
void* advcrypto_tfhe_gate_not(const char* ct_hex, const char* cloud_key_hex, const char* context_id) {
    return new TfheCiphertext(tfhe_gate_not(
        ct_hex ? ct_hex : "",
        cloud_key_hex ? cloud_key_hex : "",
        context_id ? context_id : ""));
}


// ===== Shamir Secret Sharing =====

struct ShamirSharesWrapper {
    std::vector<ShamirShare> shares;
};

void* advcrypto_shamir_split(long long secret, int n, int k) {
    auto* w = new ShamirSharesWrapper();
    w->shares = shamir_split((int64_t)secret, n, k);
    return w;
}
void advcrypto_shamir_destroy_shares(void* shares) { delete static_cast<ShamirSharesWrapper*>(shares); }
int advcrypto_shamir_share_count(void* shares) {
    return (int)static_cast<ShamirSharesWrapper*>(shares)->shares.size();
}
int advcrypto_shamir_share_x(void* shares, int idx) {
    return static_cast<ShamirSharesWrapper*>(shares)->shares[idx].x;
}
const char* advcrypto_shamir_share_y(void* shares, int idx) {
    g_advcrypto_last_string = static_cast<ShamirSharesWrapper*>(shares)->shares[idx].y_hex;
    return g_advcrypto_last_string.c_str();
}
long long advcrypto_shamir_reconstruct(void* shares, int count) {
    auto* w = static_cast<ShamirSharesWrapper*>(shares);
    std::vector<ShamirShare> subset(w->shares.begin(),
        w->shares.begin() + std::min(count, (int)w->shares.size()));
    return (long long)shamir_reconstruct(subset);
}

// ===== SMPC =====

struct SmpcSharesWrapper {
    std::vector<SmpcShare> shares;
};

void* advcrypto_smpc_share(long long secret, int total_parties, int threshold) {
    auto* w = new SmpcSharesWrapper();
    w->shares = smpc_secret_share((int64_t)secret, total_parties, threshold);
    return w;
}
void advcrypto_smpc_destroy_shares(void* shares) { delete static_cast<SmpcSharesWrapper*>(shares); }
int advcrypto_smpc_share_count(void* shares) {
    return (int)static_cast<SmpcSharesWrapper*>(shares)->shares.size();
}
const char* advcrypto_smpc_share_hex(void* shares, int idx) {
    g_advcrypto_last_string = static_cast<SmpcSharesWrapper*>(shares)->shares[idx].share_hex;
    return g_advcrypto_last_string.c_str();
}
long long advcrypto_smpc_reconstruct(void* shares, int count) {
    auto* w = static_cast<SmpcSharesWrapper*>(shares);
    std::vector<SmpcShare> subset(w->shares.begin(),
        w->shares.begin() + std::min(count, (int)w->shares.size()));
    return (long long)smpc_reconstruct(subset);
}

// ===== Threshold Cryptography =====

struct ThresholdContext {
    ThresholdPublicKey tpk;
    std::vector<ThresholdKeyShare> shares;
};

void* advcrypto_threshold_keygen(int threshold, int total_parties) {
    auto* ctx = new ThresholdContext();
    ctx->tpk = threshold_keygen(threshold, total_parties, ctx->shares);
    return ctx;
}
void advcrypto_threshold_destroy(void* tctx) { delete static_cast<ThresholdContext*>(tctx); }
const char* advcrypto_threshold_public_key(void* tctx) {
    g_advcrypto_last_string = static_cast<ThresholdContext*>(tctx)->tpk.public_key_hex;
    return g_advcrypto_last_string.c_str();
}
int advcrypto_threshold_share_count(void* tctx) {
    return (int)static_cast<ThresholdContext*>(tctx)->shares.size();
}
const char* advcrypto_threshold_share_hex(void* tctx, int idx) {
    g_advcrypto_last_string = static_cast<ThresholdContext*>(tctx)->shares[idx].share_hex;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_threshold_partial_sign(const char* share_hex, const char* message, int party_id) {
    g_advcrypto_last_string = threshold_partial_sign(
        share_hex ? share_hex : "",
        message ? message : "",
        party_id);
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_threshold_combine(const char* sigs_csv, const char* public_key_hex, int threshold) {
    // Parse CSV of partial signatures
    std::vector<std::string> sigs;
    std::string csv = sigs_csv ? sigs_csv : "";
    size_t pos = 0;
    while (pos < csv.size()) {
        size_t comma = csv.find(',', pos);
        if (comma == std::string::npos) { sigs.push_back(csv.substr(pos)); break; }
        sigs.push_back(csv.substr(pos, comma - pos));
        pos = comma + 1;
    }
    g_advcrypto_last_string = threshold_combine_signatures(sigs, public_key_hex ? public_key_hex : "", threshold);
    return g_advcrypto_last_string.c_str();
}
int advcrypto_threshold_verify(const char* public_key_hex, const char* message, const char* combined_sig_hex) {
    return threshold_verify(
        public_key_hex ? public_key_hex : "",
        message ? message : "",
        combined_sig_hex ? combined_sig_hex : "") ? 1 : 0;
}

// ===== Oblivious Transfer =====

struct OTSenderWrapper { OTSender sender; };
struct OTReceiverWrapper { OTReceiver receiver; };
struct OTMessageWrapper {
    std::string enc_m0;
    std::string enc_m1;
};

void* advcrypto_ot_sender_init(const char* m0, const char* m1) {
    auto* w = new OTSenderWrapper();
    w->sender = ot_sender_init(m0 ? m0 : "", m1 ? m1 : "");
    return w;
}
void advcrypto_ot_destroy_sender(void* sender) { delete static_cast<OTSenderWrapper*>(sender); }
const char* advcrypto_ot_sender_state(void* sender) {
    g_advcrypto_last_string = static_cast<OTSenderWrapper*>(sender)->sender.sender_state_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_ot_receiver_init(int choice_bit) {
    auto* w = new OTReceiverWrapper();
    w->receiver = ot_receiver_init(choice_bit);
    return w;
}
void advcrypto_ot_destroy_receiver(void* receiver) { delete static_cast<OTReceiverWrapper*>(receiver); }
const char* advcrypto_ot_receiver_query(void* receiver) {
    g_advcrypto_last_string = static_cast<OTReceiverWrapper*>(receiver)->receiver.receiver_state_hex;
    return g_advcrypto_last_string.c_str();
}
void* advcrypto_ot_sender_respond(const char* sender_state_hex, const char* receiver_query_hex) {
    auto* w = new OTMessageWrapper();
    OTMessage msg = ot_sender_respond(
        sender_state_hex ? sender_state_hex : "",
        receiver_query_hex ? receiver_query_hex : "");
    w->enc_m0 = msg.encrypted_m0_hex;
    w->enc_m1 = msg.encrypted_m1_hex;
    return w;
}
void advcrypto_ot_destroy_message(void* msg) { delete static_cast<OTMessageWrapper*>(msg); }
const char* advcrypto_ot_message_hex(void* msg) {
    auto* w = static_cast<OTMessageWrapper*>(msg);
    g_advcrypto_last_string = w->enc_m0 + "|" + w->enc_m1;
    return g_advcrypto_last_string.c_str();
}
const char* advcrypto_ot_receiver_extract(const char* receiver_state_hex,
                                           const char* ot_message_hex,
                                           int choice_bit) {
    g_advcrypto_last_string = ot_receiver_extract(
        receiver_state_hex ? receiver_state_hex : "",
        ot_message_hex ? ot_message_hex : "",
        choice_bit);
    return g_advcrypto_last_string.c_str();
}

} // extern "C"
