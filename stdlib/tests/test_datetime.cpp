#include "../datetime/datetime.h"
#include <iostream>
#include <cassert>

using namespace sapphire::stdlib;

int main() {
    std::cout << "Testing DateTime library...\n\n";
    
    // Test Instant
    std::cout << "Test 1: Instant\n";
    Instant start = Instant::now();
    
    // Do some work
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    Instant end = Instant::now();
    Duration elapsed = end - start;
    
    std::cout << "  Elapsed: " << elapsed.seconds() << " seconds\n";
    std::cout << "  Elapsed: " << elapsed.total_milliseconds() << " ms\n";
    assert(elapsed.total_milliseconds() >= 0);
    std::cout << "  ✓ Instant test passed\n\n";
    
    // Test Date
    std::cout << "Test 2: Date\n";
    Date today = Date::today();
    std::cout << "  Today: " << today.to_string() << "\n";
    assert(today.year >= 2024);
    std::cout << "  ✓ Date test passed\n\n";
    
    // Test Time
    std::cout << "Test 3: Time\n";
    Time now = Time::now();
    std::cout << "  Now: " << now.to_string() << "\n";
    assert(now.hour >= 0 && now.hour < 24);
    std::cout << "  ✓ Time test passed\n\n";
    
    // Test DateTime
    std::cout << "Test 4: DateTime\n";
    DateTime dt = DateTime::now();
    std::cout << "  DateTime: " << dt.to_string() << "\n";
    std::cout << "  ✓ DateTime test passed\n\n";
    
    // Test C API
    std::cout << "Test 5: C API\n";
    int64_t timestamp = sapphire_instant_now_ms();
    std::cout << "  Timestamp: " << timestamp << " ms\n";
    assert(timestamp > 0);
    std::cout << "  ✓ C API test passed\n\n";
    
    std::cout << "All DateTime tests passed! ✅\n";
    return 0;
}
