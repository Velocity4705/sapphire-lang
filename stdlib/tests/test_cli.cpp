#include "../cli/argparser.h"
#include <iostream>
#include <cassert>

using namespace sapphire::cli;

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " #name "..." << std::endl; \
        try { \
            test_##name(); \
            tests_passed++; \
            std::cout << "  ✓ PASSED" << std::endl; \
        } catch (const std::exception& e) { \
            tests_failed++; \
            std::cout << "  ✗ FAILED: " << e.what() << std::endl; \
        } \
    } \
    void test_##name()

TEST(cli_long_option) {
    char* argv[] = {(char*)"prog", (char*)"--name", (char*)"Alice"};
    ArgParser parser(3, argv);
    
    assert(parser.has("name"));
    assert(parser.get("name") == "Alice");
}

TEST(cli_short_option) {
    char* argv[] = {(char*)"prog", (char*)"-n", (char*)"Bob"};
    ArgParser parser(3, argv);
    
    assert(parser.has("n"));
    assert(parser.get("n") == "Bob");
}

TEST(cli_flag) {
    char* argv[] = {(char*)"prog", (char*)"--verbose"};
    ArgParser parser(2, argv);
    
    assert(parser.has("verbose"));
    assert(parser.get_bool("verbose") == true);
}

TEST(cli_int_option) {
    char* argv[] = {(char*)"prog", (char*)"--port", (char*)"8080"};
    ArgParser parser(3, argv);
    
    assert(parser.get_int("port") == 8080);
}

TEST(cli_positional) {
    char* argv[] = {(char*)"prog", (char*)"file1.txt", (char*)"file2.txt"};
    ArgParser parser(3, argv);
    
    assert(parser.positional().size() == 2);
    assert(parser.positional()[0] == "file1.txt");
    assert(parser.positional()[1] == "file2.txt");
}

TEST(cli_mixed) {
    char* argv[] = {(char*)"prog", (char*)"--name", (char*)"Charlie", (char*)"file.txt", (char*)"--verbose"};
    ArgParser parser(5, argv);
    
    assert(parser.get("name") == "Charlie");
    assert(parser.get_bool("verbose") == true);
    assert(parser.positional().size() == 1);
    assert(parser.positional()[0] == "file.txt");
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "CLI ArgParser Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_cli_long_option();
    run_test_cli_short_option();
    run_test_cli_flag();
    run_test_cli_int_option();
    run_test_cli_positional();
    run_test_cli_mixed();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! CLI ArgParser is working! 🖥️\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
