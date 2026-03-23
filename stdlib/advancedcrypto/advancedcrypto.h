#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Sapphire {
namespace AdvancedCrypto {

// ===== Utility =====
std::string bytes_to_hex(const std::vector<uint8_t>& b);
std::vector<uint8_t> hex_to_bytes(const std::string& hex);
// Simple deterministic PRNG seeded from a string (for simulation)
uint64_t sim_rand(uint64_t& state);

// ===== Post-Quantum: Kyber (simplified KEM simulation) =====
struct KyberKeyPair {
    std::string public_key_hex;   // 32-byte simulated public key
    std::string secret_key_hex;   // 32-byte simulated secret key
    int security_level;           // 512, 768, or 1024
};

struct KyberEncapsulation {
    std::string ciphertext_hex;   // encapsulated ciphertext
    std::string shared_secret_hex;// derived shared secret
};

KyberKeyPair kyber_keygen(int security_level = 768);
KyberEncapsulation kyber_encapsulate(const std::string& public_key_hex, int security_level = 768);
std::string kyber_decapsulate(const std::string& secret_key_hex,
                               const std::string& ciphertext_hex,
                               int security_level = 768);

// ===== Post-Quantum: SPHINCS+ (hash-based signature simulation) =====
struct SphincsKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    std::string variant; // "sha2-128s", "sha2-256s", etc.
};

SphincsKeyPair sphincs_keygen(const std::string& variant = "sha2-128s");
std::string sphincs_sign(const std::string& secret_key_hex,
                          const std::string& message,
                          const std::string& variant = "sha2-128s");
bool sphincs_verify(const std::string& public_key_hex,
                    const std::string& message,
                    const std::string& signature_hex,
                    const std::string& variant = "sha2-128s");

// ===== Post-Quantum: Code-based (McEliece-style simulation) =====
struct McElieceKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    int n; // code length
    int k; // dimension
    int t; // error correction capability
};

McElieceKeyPair mceliece_keygen(int n = 3488, int k = 2720, int t = 64);
std::string mceliece_encrypt(const std::string& public_key_hex, const std::string& plaintext_hex);
std::string mceliece_decrypt(const std::string& secret_key_hex, const std::string& ciphertext_hex);

// ===== Post-Quantum: Multivariate (Rainbow-style simulation) =====
struct RainbowKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    std::string security_level; // "I", "III", "V"
};

RainbowKeyPair rainbow_keygen(const std::string& level = "I");
std::string rainbow_sign(const std::string& secret_key_hex, const std::string& message, const std::string& level = "I");
bool rainbow_verify(const std::string& public_key_hex, const std::string& message,
                    const std::string& signature_hex, const std::string& level = "I");

// ===== Zero-Knowledge Proofs: zk-SNARK (simplified Groth16-style) =====
struct ZkSnarkProof {
    std::string proof_hex;
    std::string public_inputs_hex;
    std::string circuit_id;
};

struct ZkSnarkKeys {
    std::string proving_key_hex;
    std::string verification_key_hex;
    std::string circuit_id;
};

ZkSnarkKeys zksnark_setup(const std::string& circuit_description);
ZkSnarkProof zksnark_prove(const std::string& proving_key_hex,
                            const std::string& private_inputs_hex,
                            const std::string& public_inputs_hex,
                            const std::string& circuit_id);
bool zksnark_verify(const std::string& verification_key_hex,
                    const std::string& proof_hex,
                    const std::string& public_inputs_hex,
                    const std::string& circuit_id);

// ===== Zero-Knowledge Proofs: zk-STARK (simplified FRI-based) =====
struct ZkStarkProof {
    std::string proof_hex;
    std::string trace_commitment_hex;
    int security_bits;
};

ZkStarkProof zkstark_prove(const std::string& computation_trace_hex, int security_bits = 80);
bool zkstark_verify(const std::string& proof_hex,
                    const std::string& public_statement_hex,
                    int security_bits = 80);

// ===== Zero-Knowledge Proofs: Bulletproofs (range proofs) =====
struct BulletproofRangeProof {
    std::string proof_hex;
    std::string commitment_hex;
    int bit_length; // e.g. 64 for 64-bit range
};

BulletproofRangeProof bulletproof_prove_range(uint64_t value, int bit_length = 64);
bool bulletproof_verify_range(const std::string& proof_hex,
                               const std::string& commitment_hex,
                               int bit_length = 64);

// ===== Zero-Knowledge Proofs: PLONK (simplified) =====
struct PlonkProof {
    std::string proof_hex;
    std::string public_inputs_hex;
    std::string srs_id;
};

struct PlonkSRS {
    std::string srs_hex;
    std::string srs_id;
    int max_degree;
};

PlonkSRS plonk_setup(int max_degree = 1024);
PlonkProof plonk_prove(const std::string& srs_hex,
                        const std::string& circuit_hex,
                        const std::string& witness_hex,
                        const std::string& srs_id);
bool plonk_verify(const std::string& srs_hex,
                  const std::string& proof_hex,
                  const std::string& public_inputs_hex,
                  const std::string& srs_id);

// ===== Homomorphic Encryption: BGV scheme =====
struct BgvContext {
    std::string params_hex;
    std::string context_id;
    int plaintext_modulus;
    int poly_degree;
};

struct BgvKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    std::string context_id;
};

struct BgvCiphertext {
    std::string ciphertext_hex;
    std::string context_id;
};

BgvContext bgv_create_context(int plaintext_modulus = 65537, int poly_degree = 4096);
BgvKeyPair bgv_keygen(const std::string& context_id);
BgvCiphertext bgv_encrypt(const std::string& public_key_hex,
                           const std::string& context_id,
                           int64_t plaintext);
int64_t bgv_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex);
BgvCiphertext bgv_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);
BgvCiphertext bgv_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);

// ===== Homomorphic Encryption: BFV scheme =====
struct BfvContext {
    std::string params_hex;
    std::string context_id;
    int plaintext_modulus;
    int poly_degree;
};

struct BfvKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    std::string context_id;
};

struct BfvCiphertext {
    std::string ciphertext_hex;
    std::string context_id;
};

BfvContext bfv_create_context(int plaintext_modulus = 65537, int poly_degree = 4096);
BfvKeyPair bfv_keygen(const std::string& context_id);
BfvCiphertext bfv_encrypt(const std::string& public_key_hex,
                           const std::string& context_id,
                           int64_t plaintext);
int64_t bfv_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex);
BfvCiphertext bfv_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);
BfvCiphertext bfv_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);

// ===== Homomorphic Encryption: CKKS scheme (approximate arithmetic) =====
struct CkksContext {
    std::string params_hex;
    std::string context_id;
    int poly_degree;
    int scale_bits;
};

struct CkksKeyPair {
    std::string public_key_hex;
    std::string secret_key_hex;
    std::string context_id;
};

struct CkksCiphertext {
    std::string ciphertext_hex;
    std::string context_id;
    double scale;
};

CkksContext ckks_create_context(int poly_degree = 8192, int scale_bits = 40);
CkksKeyPair ckks_keygen(const std::string& context_id);
CkksCiphertext ckks_encrypt(const std::string& public_key_hex,
                             const std::string& context_id,
                             double plaintext);
double ckks_decrypt(const std::string& secret_key_hex,
                    const std::string& context_id,
                    const std::string& ciphertext_hex);
CkksCiphertext ckks_add(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);
CkksCiphertext ckks_multiply(const std::string& ct1_hex, const std::string& ct2_hex, const std::string& context_id);

// ===== Homomorphic Encryption: TFHE (gate bootstrapping) =====
struct TfheContext {
    std::string params_hex;
    std::string context_id;
    int security_level;
};

struct TfheKeyPair {
    std::string secret_key_hex;
    std::string cloud_key_hex;
    std::string context_id;
};

struct TfheCiphertext {
    std::string ciphertext_hex;
    std::string context_id;
};

TfheContext tfhe_create_context(int security_level = 128);
TfheKeyPair tfhe_keygen(const std::string& context_id);
TfheCiphertext tfhe_encrypt_bit(const std::string& secret_key_hex,
                                 const std::string& context_id,
                                 bool bit);
bool tfhe_decrypt_bit(const std::string& secret_key_hex,
                      const std::string& context_id,
                      const std::string& ciphertext_hex);
TfheCiphertext tfhe_gate_and(const std::string& ct1_hex, const std::string& ct2_hex,
                              const std::string& cloud_key_hex, const std::string& context_id);
TfheCiphertext tfhe_gate_or(const std::string& ct1_hex, const std::string& ct2_hex,
                             const std::string& cloud_key_hex, const std::string& context_id);
TfheCiphertext tfhe_gate_xor(const std::string& ct1_hex, const std::string& ct2_hex,
                              const std::string& cloud_key_hex, const std::string& context_id);
TfheCiphertext tfhe_gate_not(const std::string& ct_hex,
                              const std::string& cloud_key_hex, const std::string& context_id);

// ===== Secure Multi-Party Computation =====
struct SmpcShare {
    std::string share_hex;
    int party_id;
    int total_parties;
};

std::vector<SmpcShare> smpc_secret_share(int64_t secret, int total_parties, int threshold);
int64_t smpc_reconstruct(const std::vector<SmpcShare>& shares);
std::string smpc_compute_sum(const std::vector<std::string>& shares_hex, int total_parties);
std::string smpc_compute_product(const std::vector<std::string>& shares_hex, int total_parties);

// ===== Secret Sharing: Shamir =====
struct ShamirShare {
    int x;           // x-coordinate (party index)
    std::string y_hex; // y-coordinate (share value)
};

std::vector<ShamirShare> shamir_split(int64_t secret, int n, int k);
int64_t shamir_reconstruct(const std::vector<ShamirShare>& shares);

// ===== Threshold Cryptography =====
struct ThresholdKeyShare {
    std::string share_hex;
    int party_id;
    int threshold;
    int total_parties;
};

struct ThresholdPublicKey {
    std::string public_key_hex;
    int threshold;
    int total_parties;
};

ThresholdPublicKey threshold_keygen(int threshold, int total_parties,
                                     std::vector<ThresholdKeyShare>& out_shares);
std::string threshold_partial_sign(const std::string& share_hex,
                                    const std::string& message,
                                    int party_id);
std::string threshold_combine_signatures(const std::vector<std::string>& partial_sigs,
                                          const std::string& public_key_hex,
                                          int threshold);
bool threshold_verify(const std::string& public_key_hex,
                      const std::string& message,
                      const std::string& combined_sig_hex);

// ===== Oblivious Transfer =====
struct OTSender {
    std::string m0_hex; // message 0
    std::string m1_hex; // message 1
    std::string sender_state_hex;
};

struct OTReceiver {
    int choice_bit;
    std::string receiver_state_hex;
};

struct OTMessage {
    std::string encrypted_m0_hex;
    std::string encrypted_m1_hex;
};

OTSender ot_sender_init(const std::string& m0, const std::string& m1);
OTReceiver ot_receiver_init(int choice_bit);
OTMessage ot_sender_respond(const std::string& sender_state_hex,
                              const std::string& receiver_query_hex);
std::string ot_receiver_extract(const std::string& receiver_state_hex,
                                 const std::string& ot_message_hex,
                                 int choice_bit);

} // namespace AdvancedCrypto
} // namespace Sapphire

// ===== C API =====
extern "C" {

// ---- Kyber ----
void* advcrypto_kyber_keygen(int security_level);
void  advcrypto_kyber_destroy_keypair(void* kp);
const char* advcrypto_kyber_public_key(void* kp);
const char* advcrypto_kyber_secret_key(void* kp);
int   advcrypto_kyber_security_level(void* kp);

void* advcrypto_kyber_encapsulate(const char* public_key_hex, int security_level);
void  advcrypto_kyber_destroy_encap(void* enc);
const char* advcrypto_kyber_ciphertext(void* enc);
const char* advcrypto_kyber_shared_secret(void* enc);

const char* advcrypto_kyber_decapsulate(const char* secret_key_hex,
                                         const char* ciphertext_hex,
                                         int security_level);

// ---- SPHINCS+ ----
void* advcrypto_sphincs_keygen(const char* variant);
void  advcrypto_sphincs_destroy_keypair(void* kp);
const char* advcrypto_sphincs_public_key(void* kp);
const char* advcrypto_sphincs_secret_key(void* kp);
const char* advcrypto_sphincs_sign(const char* secret_key_hex, const char* message, const char* variant);
int   advcrypto_sphincs_verify(const char* public_key_hex, const char* message,
                                const char* signature_hex, const char* variant);

// ---- McEliece ----
void* advcrypto_mceliece_keygen(int n, int k, int t);
void  advcrypto_mceliece_destroy_keypair(void* kp);
const char* advcrypto_mceliece_public_key(void* kp);
const char* advcrypto_mceliece_secret_key(void* kp);
const char* advcrypto_mceliece_encrypt(const char* public_key_hex, const char* plaintext_hex);
const char* advcrypto_mceliece_decrypt(const char* secret_key_hex, const char* ciphertext_hex);

// ---- Rainbow ----
void* advcrypto_rainbow_keygen(const char* level);
void  advcrypto_rainbow_destroy_keypair(void* kp);
const char* advcrypto_rainbow_public_key(void* kp);
const char* advcrypto_rainbow_secret_key(void* kp);
const char* advcrypto_rainbow_sign(const char* secret_key_hex, const char* message, const char* level);
int   advcrypto_rainbow_verify(const char* public_key_hex, const char* message,
                                const char* signature_hex, const char* level);

// ---- zk-SNARK ----
void* advcrypto_zksnark_setup(const char* circuit_description);
void  advcrypto_zksnark_destroy_keys(void* keys);
const char* advcrypto_zksnark_proving_key(void* keys);
const char* advcrypto_zksnark_verification_key(void* keys);
const char* advcrypto_zksnark_circuit_id(void* keys);

void* advcrypto_zksnark_prove(const char* proving_key_hex, const char* private_inputs_hex,
                               const char* public_inputs_hex, const char* circuit_id);
void  advcrypto_zksnark_destroy_proof(void* proof);
const char* advcrypto_zksnark_proof_hex(void* proof);
const char* advcrypto_zksnark_public_inputs(void* proof);
int   advcrypto_zksnark_verify(const char* verification_key_hex, const char* proof_hex,
                                const char* public_inputs_hex, const char* circuit_id);

// ---- zk-STARK ----
void* advcrypto_zkstark_prove(const char* computation_trace_hex, int security_bits);
void  advcrypto_zkstark_destroy_proof(void* proof);
const char* advcrypto_zkstark_proof_hex(void* proof);
const char* advcrypto_zkstark_trace_commitment(void* proof);
int   advcrypto_zkstark_verify(const char* proof_hex, const char* public_statement_hex, int security_bits);

// ---- Bulletproofs ----
void* advcrypto_bulletproof_prove_range(long long value, int bit_length);
void  advcrypto_bulletproof_destroy(void* proof);
const char* advcrypto_bulletproof_proof_hex(void* proof);
const char* advcrypto_bulletproof_commitment_hex(void* proof);
int   advcrypto_bulletproof_verify_range(const char* proof_hex, const char* commitment_hex, int bit_length);

// ---- PLONK ----
void* advcrypto_plonk_setup(int max_degree);
void  advcrypto_plonk_destroy_srs(void* srs);
const char* advcrypto_plonk_srs_hex(void* srs);
const char* advcrypto_plonk_srs_id(void* srs);

void* advcrypto_plonk_prove(const char* srs_hex, const char* circuit_hex,
                              const char* witness_hex, const char* srs_id);
void  advcrypto_plonk_destroy_proof(void* proof);
const char* advcrypto_plonk_proof_hex(void* proof);
int   advcrypto_plonk_verify(const char* srs_hex, const char* proof_hex,
                               const char* public_inputs_hex, const char* srs_id);

// ---- BGV ----
void* advcrypto_bgv_create_context(int plaintext_modulus, int poly_degree);
void  advcrypto_bgv_destroy_context(void* ctx);
const char* advcrypto_bgv_context_id(void* ctx);

void* advcrypto_bgv_keygen(const char* context_id);
void  advcrypto_bgv_destroy_keypair(void* kp);
const char* advcrypto_bgv_public_key(void* kp);
const char* advcrypto_bgv_secret_key(void* kp);

void* advcrypto_bgv_encrypt(const char* public_key_hex, const char* context_id, long long plaintext);
void  advcrypto_bgv_destroy_ciphertext(void* ct);
const char* advcrypto_bgv_ciphertext_hex(void* ct);
long long advcrypto_bgv_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex);

void* advcrypto_bgv_add(const char* ct1_hex, const char* ct2_hex, const char* context_id);
void* advcrypto_bgv_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id);

// ---- BFV ----
void* advcrypto_bfv_create_context(int plaintext_modulus, int poly_degree);
void  advcrypto_bfv_destroy_context(void* ctx);
const char* advcrypto_bfv_context_id(void* ctx);

void* advcrypto_bfv_keygen(const char* context_id);
void  advcrypto_bfv_destroy_keypair(void* kp);
const char* advcrypto_bfv_public_key(void* kp);
const char* advcrypto_bfv_secret_key(void* kp);

void* advcrypto_bfv_encrypt(const char* public_key_hex, const char* context_id, long long plaintext);
void  advcrypto_bfv_destroy_ciphertext(void* ct);
const char* advcrypto_bfv_ciphertext_hex(void* ct);
long long advcrypto_bfv_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex);

void* advcrypto_bfv_add(const char* ct1_hex, const char* ct2_hex, const char* context_id);
void* advcrypto_bfv_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id);

// ---- CKKS ----
void* advcrypto_ckks_create_context(int poly_degree, int scale_bits);
void  advcrypto_ckks_destroy_context(void* ctx);
const char* advcrypto_ckks_context_id(void* ctx);

void* advcrypto_ckks_keygen(const char* context_id);
void  advcrypto_ckks_destroy_keypair(void* kp);
const char* advcrypto_ckks_public_key(void* kp);
const char* advcrypto_ckks_secret_key(void* kp);

void* advcrypto_ckks_encrypt(const char* public_key_hex, const char* context_id, double plaintext);
void  advcrypto_ckks_destroy_ciphertext(void* ct);
const char* advcrypto_ckks_ciphertext_hex(void* ct);
double advcrypto_ckks_decrypt(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex);

void* advcrypto_ckks_add(const char* ct1_hex, const char* ct2_hex, const char* context_id);
void* advcrypto_ckks_multiply(const char* ct1_hex, const char* ct2_hex, const char* context_id);

// ---- TFHE ----
void* advcrypto_tfhe_create_context(int security_level);
void  advcrypto_tfhe_destroy_context(void* ctx);
const char* advcrypto_tfhe_context_id(void* ctx);

void* advcrypto_tfhe_keygen(const char* context_id);
void  advcrypto_tfhe_destroy_keypair(void* kp);
const char* advcrypto_tfhe_secret_key(void* kp);
const char* advcrypto_tfhe_cloud_key(void* kp);

void* advcrypto_tfhe_encrypt_bit(const char* secret_key_hex, const char* context_id, int bit);
void  advcrypto_tfhe_destroy_ciphertext(void* ct);
const char* advcrypto_tfhe_ciphertext_hex(void* ct);
int   advcrypto_tfhe_decrypt_bit(const char* secret_key_hex, const char* context_id, const char* ciphertext_hex);

void* advcrypto_tfhe_gate_and(const char* ct1_hex, const char* ct2_hex,
                               const char* cloud_key_hex, const char* context_id);
void* advcrypto_tfhe_gate_or(const char* ct1_hex, const char* ct2_hex,
                              const char* cloud_key_hex, const char* context_id);
void* advcrypto_tfhe_gate_xor(const char* ct1_hex, const char* ct2_hex,
                               const char* cloud_key_hex, const char* context_id);
void* advcrypto_tfhe_gate_not(const char* ct_hex, const char* cloud_key_hex, const char* context_id);

// ---- Shamir Secret Sharing ----
void* advcrypto_shamir_split(long long secret, int n, int k);
void  advcrypto_shamir_destroy_shares(void* shares);
int   advcrypto_shamir_share_count(void* shares);
int   advcrypto_shamir_share_x(void* shares, int idx);
const char* advcrypto_shamir_share_y(void* shares, int idx);
long long advcrypto_shamir_reconstruct(void* shares, int count);

// ---- SMPC ----
void* advcrypto_smpc_share(long long secret, int total_parties, int threshold);
void  advcrypto_smpc_destroy_shares(void* shares);
int   advcrypto_smpc_share_count(void* shares);
const char* advcrypto_smpc_share_hex(void* shares, int idx);
long long advcrypto_smpc_reconstruct(void* shares, int count);

// ---- Threshold Cryptography ----
void* advcrypto_threshold_keygen(int threshold, int total_parties);
void  advcrypto_threshold_destroy(void* tctx);
const char* advcrypto_threshold_public_key(void* tctx);
int   advcrypto_threshold_share_count(void* tctx);
const char* advcrypto_threshold_share_hex(void* tctx, int idx);
const char* advcrypto_threshold_partial_sign(const char* share_hex, const char* message, int party_id);
const char* advcrypto_threshold_combine(const char* sigs_csv, const char* public_key_hex, int threshold);
int   advcrypto_threshold_verify(const char* public_key_hex, const char* message, const char* combined_sig_hex);

// ---- Oblivious Transfer ----
void* advcrypto_ot_sender_init(const char* m0, const char* m1);
void  advcrypto_ot_destroy_sender(void* sender);
const char* advcrypto_ot_sender_state(void* sender);
void* advcrypto_ot_receiver_init(int choice_bit);
void  advcrypto_ot_destroy_receiver(void* receiver);
const char* advcrypto_ot_receiver_query(void* receiver);
void* advcrypto_ot_sender_respond(const char* sender_state_hex, const char* receiver_query_hex);
void  advcrypto_ot_destroy_message(void* msg);
const char* advcrypto_ot_message_hex(void* msg);
const char* advcrypto_ot_receiver_extract(const char* receiver_state_hex,
                                           const char* ot_message_hex,
                                           int choice_bit);

} // extern "C"
