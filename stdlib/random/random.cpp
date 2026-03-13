#include "random.h"
#include <chrono>
#include <algorithm>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace Random {

// RandomGenerator Implementation
RandomGenerator::RandomGenerator() : uniform_dist(0.0, 1.0), normal_dist(0.0, 1.0) {
    seed_random();
}

RandomGenerator::RandomGenerator(uint64_t seed_value) : uniform_dist(0.0, 1.0), normal_dist(0.0, 1.0) {
    seed(seed_value);
}

void RandomGenerator::seed(uint64_t seed_value) {
    generator.seed(seed_value);
}

void RandomGenerator::seed_random() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    seed(static_cast<uint64_t>(nanoseconds.count()));
}

double RandomGenerator::uniform() {
    return uniform_dist(generator);
}

double RandomGenerator::uniform(double min, double max) {
    return min + (max - min) * uniform();
}

int64_t RandomGenerator::uniform_int(int64_t min, int64_t max) {
    if (min >= max) return min;
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(generator);
}

double RandomGenerator::normal() {
    return normal_dist(generator);
}

double RandomGenerator::normal(double mean, double stddev) {
    return mean + stddev * normal();
}

bool RandomGenerator::boolean(double probability) {
    return uniform() < probability;
}

size_t RandomGenerator::choice(size_t count) {
    if (count == 0) return 0;
    return static_cast<size_t>(uniform_int(0, static_cast<int64_t>(count - 1)));
}

void RandomGenerator::shuffle(int64_t* array, size_t count) {
    for (size_t i = count - 1; i > 0; i--) {
        size_t j = choice(i + 1);
        std::swap(array[i], array[j]);
    }
}

// Global generator
RandomGenerator& get_global_generator() {
    static RandomGenerator global_gen;
    return global_gen;
}

// Convenience functions
void random_seed(uint64_t seed) {
    get_global_generator().seed(seed);
}

void random_seed_random() {
    get_global_generator().seed_random();
}

double random_uniform() {
    return get_global_generator().uniform();
}

double random_uniform_range(double min, double max) {
    return get_global_generator().uniform(min, max);
}

int64_t random_int(int64_t min, int64_t max) {
    return get_global_generator().uniform_int(min, max);
}

double random_normal() {
    return get_global_generator().normal();
}

double random_normal_params(double mean, double stddev) {
    return get_global_generator().normal(mean, stddev);
}

bool random_boolean(double probability) {
    return get_global_generator().boolean(probability);
}

// CryptoRandom Implementation
std::vector<uint8_t> CryptoRandom::bytes(size_t count) {
    std::vector<uint8_t> result(count);
    std::random_device rd;
    
    // Fill with random bytes
    for (size_t i = 0; i < count; i += sizeof(uint32_t)) {
        uint32_t random_value = rd();
        size_t bytes_to_copy = std::min(sizeof(uint32_t), count - i);
        memcpy(&result[i], &random_value, bytes_to_copy);
    }
    
    return result;
}

uint64_t CryptoRandom::uint64() {
    std::random_device rd;
    uint64_t result = 0;
    
    // Combine two 32-bit values
    result = static_cast<uint64_t>(rd()) << 32;
    result |= static_cast<uint64_t>(rd());
    
    return result;
}

double CryptoRandom::uniform() {
    // Generate a random double in [0, 1) using crypto random
    uint64_t random_bits = uint64();
    
    // Use the upper 53 bits for double precision
    random_bits >>= 11;
    
    return static_cast<double>(random_bits) / (1ULL << 53);
}

int64_t CryptoRandom::uniform_int(int64_t min, int64_t max) {
    if (min >= max) return min;
    
    uint64_t range = static_cast<uint64_t>(max - min + 1);
    uint64_t random_value = uint64();
    
    return min + static_cast<int64_t>(random_value % range);
}

} // namespace Random
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    void sapphire_random_seed(uint64_t seed) {
        sapphire::stdlib::Random::random_seed(seed);
    }
    
    void sapphire_random_seed_random() {
        sapphire::stdlib::Random::random_seed_random();
    }
    
    double sapphire_random_uniform() {
        return sapphire::stdlib::Random::random_uniform();
    }
    
    double sapphire_random_uniform_range(double min, double max) {
        return sapphire::stdlib::Random::random_uniform_range(min, max);
    }
    
    int64_t sapphire_random_int(int64_t min, int64_t max) {
        return sapphire::stdlib::Random::random_int(min, max);
    }
    
    double sapphire_random_normal() {
        return sapphire::stdlib::Random::random_normal();
    }
    
    double sapphire_random_normal_params(double mean, double stddev) {
        return sapphire::stdlib::Random::random_normal_params(mean, stddev);
    }
    
    bool sapphire_random_boolean() {
        return sapphire::stdlib::Random::random_boolean();
    }
    
    bool sapphire_random_boolean_prob(double probability) {
        return sapphire::stdlib::Random::random_boolean(probability);
    }
    
    uint64_t sapphire_crypto_random_uint64() {
        return sapphire::stdlib::Random::CryptoRandom::uint64();
    }
    
    double sapphire_crypto_random_uniform() {
        return sapphire::stdlib::Random::CryptoRandom::uniform();
    }
    
    int64_t sapphire_crypto_random_int(int64_t min, int64_t max) {
        return sapphire::stdlib::Random::CryptoRandom::uniform_int(min, max);
    }
}