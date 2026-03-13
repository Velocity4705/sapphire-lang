#pragma once

#include <cstdint>
#include <random>
#include <memory>

namespace sapphire {
namespace stdlib {
namespace Random {

// Random Number Generator
class RandomGenerator {
private:
    std::mt19937_64 generator;
    std::uniform_real_distribution<double> uniform_dist;
    std::normal_distribution<double> normal_dist;
    
public:
    RandomGenerator();
    explicit RandomGenerator(uint64_t seed);
    
    // Seed control
    void seed(uint64_t seed_value);
    void seed_random();
    
    // Uniform distribution [0, 1)
    double uniform();
    double uniform(double min, double max);
    
    // Integer uniform distribution
    int64_t uniform_int(int64_t min, int64_t max);
    
    // Normal distribution (mean=0, stddev=1)
    double normal();
    double normal(double mean, double stddev);
    
    // Boolean with probability
    bool boolean(double probability = 0.5);
    
    // Choice from array (returns index)
    size_t choice(size_t count);
    
    // Shuffle array indices
    void shuffle(int64_t* array, size_t count);
};

// Global random generator
RandomGenerator& get_global_generator();

// Convenience functions
void random_seed(uint64_t seed);
void random_seed_random();
double random_uniform();
double random_uniform_range(double min, double max);
int64_t random_int(int64_t min, int64_t max);
double random_normal();
double random_normal_params(double mean, double stddev);
bool random_boolean(double probability = 0.5);

// Cryptographic random (system entropy)
class CryptoRandom {
public:
    static std::vector<uint8_t> bytes(size_t count);
    static uint64_t uint64();
    static double uniform();
    static int64_t uniform_int(int64_t min, int64_t max);
};

} // namespace Random
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Seeding
    void sapphire_random_seed(uint64_t seed);
    void sapphire_random_seed_random();
    
    // Uniform distribution
    double sapphire_random_uniform();
    double sapphire_random_uniform_range(double min, double max);
    int64_t sapphire_random_int(int64_t min, int64_t max);
    
    // Normal distribution
    double sapphire_random_normal();
    double sapphire_random_normal_params(double mean, double stddev);
    
    // Boolean
    bool sapphire_random_boolean();
    bool sapphire_random_boolean_prob(double probability);
    
    // Cryptographic random
    uint64_t sapphire_crypto_random_uint64();
    double sapphire_crypto_random_uniform();
    int64_t sapphire_crypto_random_int(int64_t min, int64_t max);
}