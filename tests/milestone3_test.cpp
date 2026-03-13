#include <iostream>
#include <cassert>
#include <fstream>
#include <chrono>

// Include Milestone 3 libraries
#include "stdlib/text/text.h"
#include "stdlib/io/stream.h"
#include "stdlib/error/error.h"

using namespace sapphire::stdlib;

void test_text_processing() {
    std::cout << "Testing Text Processing..." << std::endl;
    
    // Test regex
    auto regex = Text::create_regex(R"(\d+)");
    assert(regex->matches("123"));
    assert(!regex->matches("abc"));
    
    auto matches = regex->find_all("The year 2024 and month 03");
    assert(matches.size() == 2);
    assert(matches[0] == "2024");
    assert(matches[1] == "03");
    
    std::string replaced = regex->replace_all("Price: $25.99", "XX");
    assert(replaced == "Price: $XX.XX");
    
    // Test text processor
    std::string text = "  Hello   World  \n  How are you?  ";
    assert(Text::TextProcessor::word_count(text) == 5);
    assert(Text::TextProcessor::line_count(text) == 2);
    
    std::string normalized = Text::TextProcessor::normalize_whitespace(text);
    assert(normalized == "Hello World How are you?");
    
    std::string capitalized = Text::TextProcessor::capitalize_words("hello world");
    assert(capitalized == "Hello World");
    
    // Test string builder
    auto builder = Text::create_string_builder();
    builder->append("Hello");
    builder->append(" ");
    builder->append("World");
    builder->append_line("!");
    builder->append_format("Number: %d", 42);
    
    std::string result = builder->to_string();
    assert(result.find("Hello World!") != std::string::npos);
    assert(result.find("Number: 42") != std::string::npos);
    
    // Test template engine
    auto template_obj = Text::create_template("Hello {{name}}, you are {{age}} years old!");
    template_obj->set_variable("name", std::string("Alice"));
    template_obj->set_variable("age", 25);
    
    std::string rendered = template_obj->render();
    std::cout << "  Template rendered: '" << rendered << "'" << std::endl;
    assert(rendered == "Hello Alice, you are 25 years old!");
    
    // Test similarity
    double sim = Text::TextProcessor::similarity("hello", "helo");
    std::cout << "  Similarity between 'hello' and 'helo': " << sim << std::endl;
    assert(sim >= 0.8);  // Should be quite similar
    
    size_t distance = Text::TextProcessor::levenshtein_distance("kitten", "sitting");
    assert(distance == 3);  // Known result
    
    std::cout << "✓ Text Processing tests passed" << std::endl;
}

void test_streaming_io() {
    std::cout << "Testing Streaming I/O..." << std::endl;
    
    const std::string test_file = "test_stream.txt";
    const std::string test_content = "Line 1\nLine 2\nLine 3\n";
    
    // Test FileStream writing
    {
        auto stream = IO::create_file_stream(test_file, "w");
        assert(stream->is_open());
        
        stream->write(test_content);
        stream->flush();
        stream->close();
    }
    
    // Test FileStream reading
    {
        auto stream = IO::create_file_stream(test_file, "r");
        assert(stream->is_open());
        
        std::string line1 = stream->read_line();
        assert(line1 == "Line 1");
        
        std::string line2 = stream->read_line();
        assert(line2 == "Line 2");
        
        stream->close();
    }
    
    // Test BufferedWriter
    {
        auto writer = IO::create_buffered_writer("test_buffered.txt");
        
        for (int i = 0; i < 1000; ++i) {
            writer->write_line("Line " + std::to_string(i));
        }
        
        writer->flush();
        writer->close();
    }
    
    // Test BufferedReader
    {
        auto reader = IO::create_buffered_reader("test_buffered.txt");
        
        int count = 0;
        std::string line;
        while (!reader->eof()) {
            line = reader->read_line();
            if (!line.empty()) {
                count++;
            }
        }
        
        assert(count == 1000);
        reader->close();
    }
    
    // Performance test - large file
    const std::string large_file = "test_large.txt";
    auto start = std::chrono::high_resolution_clock::now();
    
    {
        auto writer = IO::create_buffered_writer(large_file);
        for (int i = 0; i < 100000; ++i) {
            writer->write_line("This is line number " + std::to_string(i) + " with some content");
        }
        writer->close();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  100K lines written in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 1000);  // Should be fast
    
    // Read performance test
    start = std::chrono::high_resolution_clock::now();
    
    {
        auto reader = IO::create_buffered_reader(large_file);
        int count = 0;
        while (!reader->eof()) {
            std::string line = reader->read_line();
            if (!line.empty()) count++;
        }
        assert(count == 100000);
        reader->close();
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  100K lines read in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 500);  // Should be fast
    
    // Cleanup
    std::remove(test_file.c_str());
    std::remove("test_buffered.txt");
    std::remove(large_file.c_str());
    
    std::cout << "✓ Streaming I/O tests passed" << std::endl;
}

void test_error_handling() {
    std::cout << "Testing Error Handling..." << std::endl;
    
    // Test stack trace
    Error::ErrorHandler::push_frame("test_function", "test_file.spp", 42);
    Error::ErrorHandler::push_frame("main", "main.spp", 10);
    
    Error::StackTrace trace = Error::ErrorHandler::capture_stack_trace();
    assert(trace.depth() == 2);
    
    std::string trace_str = trace.to_string();
    assert(trace_str.find("test_function") != std::string::npos);
    assert(trace_str.find("main") != std::string::npos);
    
    Error::ErrorHandler::pop_frame();
    Error::ErrorHandler::pop_frame();
    
    // Test SapphireError
    Error::SapphireError error("Test error message", "TEST_ERROR");
    error.add_suggestion("Try checking your input");
    error.add_suggestion("Make sure the file exists");
    
    std::string formatted = error.format_error();
    assert(formatted.find("Test error message") != std::string::npos);
    assert(formatted.find("TEST_ERROR") != std::string::npos);
    assert(formatted.find("Try checking your input") != std::string::npos);
    
    // Test specific error types
    try {
        throw Error::RuntimeError("Runtime error occurred");
    } catch (const Error::SapphireError& e) {
        assert(e.get_error_code() == "RUNTIME_ERROR");
        assert(e.get_message() == "Runtime error occurred");
    }
    
    try {
        throw Error::TypeError("Type mismatch");
    } catch (const Error::SapphireError& e) {
        assert(e.get_error_code() == "TYPE_ERROR");
    }
    
    // Test Result<T>
    Error::Result<int> ok_result(42);
    assert(ok_result.is_ok());
    assert(ok_result.value() == 42);
    assert(ok_result.value_or(0) == 42);
    
    Error::Result<int> error_result = Error::Result<int>::error("Something went wrong");
    assert(error_result.is_error());
    assert(error_result.get_error() == "Something went wrong");
    assert(error_result.value_or(99) == 99);
    
    // Test Result mapping
    auto mapped = ok_result.map<double>([](int x) { return static_cast<double>(x) * 2.0; });
    assert(mapped.is_ok());
    assert(mapped.value() == 84.0);
    
    auto mapped_error = error_result.map<double>([](int x) { return static_cast<double>(x) * 2.0; });
    assert(mapped_error.is_error());
    
    // Test unwrap (should throw)
    try {
        error_result.unwrap();
        assert(false);  // Should not reach here
    } catch (const Error::RuntimeError& e) {
        assert(std::string(e.what()).find("Result unwrap failed") != std::string::npos);
    }
    
    std::cout << "✓ Error Handling tests passed" << std::endl;
}

void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    // Test text processing C API
    void* regex = sapphire_regex_create(R"(\d+)");
    assert(regex != nullptr);
    
    assert(sapphire_regex_matches(regex, "123"));
    assert(!sapphire_regex_matches(regex, "abc"));
    
    const char* replaced = sapphire_regex_replace(regex, "Price: $25", "XX");
    assert(replaced != nullptr);
    assert(std::string(replaced) == "Price: $XX");
    
    sapphire_regex_destroy(regex);
    
    // Test string builder C API
    void* builder = sapphire_string_builder_create();
    assert(builder != nullptr);
    
    sapphire_string_builder_append(builder, "Hello ");
    sapphire_string_builder_append(builder, "World");
    
    const char* result = sapphire_string_builder_to_string(builder);
    assert(result != nullptr);
    assert(std::string(result) == "Hello World");
    
    sapphire_string_builder_destroy(builder);
    
    // Test template C API
    void* template_obj = sapphire_template_create("Hello {{name}}!");
    assert(template_obj != nullptr);
    
    sapphire_template_set_variable(template_obj, "name", "World");
    
    const char* rendered = sapphire_template_render(template_obj);
    assert(rendered != nullptr);
    std::cout << "  C API Template rendered: '" << rendered << "'" << std::endl;
    assert(std::string(rendered) == "Hello World!");
    
    sapphire_template_destroy(template_obj);
    
    // Test streaming I/O C API
    void* stream = sapphire_file_stream_create("test_c_api.txt", "w");
    assert(stream != nullptr);
    
    sapphire_file_stream_write(stream, "Hello from C API");
    sapphire_file_stream_close(stream);
    sapphire_file_stream_destroy(stream);
    
    stream = sapphire_file_stream_create("test_c_api.txt", "r");
    assert(stream != nullptr);
    
    const char* content = sapphire_file_stream_read_line(stream);
    assert(content != nullptr);
    assert(std::string(content) == "Hello from C API");
    
    sapphire_file_stream_close(stream);
    sapphire_file_stream_destroy(stream);
    
    // Test error handling C API
    sapphire_error_enable_stack_traces(true);
    sapphire_stack_push_frame("test_function", "test.spp", 42);
    
    const char* trace = sapphire_stack_get_trace();
    assert(trace != nullptr);
    assert(std::string(trace).find("test_function") != std::string::npos);
    
    sapphire_stack_pop_frame();
    
    // Cleanup
    std::remove("test_c_api.txt");
    
    std::cout << "✓ C API tests passed" << std::endl;
}

void test_performance() {
    std::cout << "Testing Performance..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Text processing performance
    auto regex = Text::create_regex(R"(\b\w+@\w+\.\w+\b)");  // Email regex
    std::string large_text;
    for (int i = 0; i < 10000; ++i) {
        large_text += "Contact user" + std::to_string(i) + "@example.com for more info. ";
    }
    
    auto matches = regex->find_all(large_text);
    assert(matches.size() == 10000);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  10K regex matches found in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 100);  // Should be fast
    
    // String builder performance
    start = std::chrono::high_resolution_clock::now();
    
    auto builder = Text::create_string_builder();
    for (int i = 0; i < 100000; ++i) {
        builder->append("Item ").append(std::to_string(i)).append("\n");
    }
    
    std::string result = builder->to_string();
    assert(result.length() > 1000000);  // Should be large
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  100K string operations completed in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 200);  // Should be fast
    
    // Template performance
    start = std::chrono::high_resolution_clock::now();
    
    auto template_obj = Text::create_template("User: {{name}}, Age: {{age}}, Score: {{score}}");
    
    for (int i = 0; i < 10000; ++i) {
        template_obj->set_variable("name", "User" + std::to_string(i));
        template_obj->set_variable("age", 20 + (i % 50));
        template_obj->set_variable("score", 85.5 + (i % 15));
        
        std::string rendered = template_obj->render();
        assert(!rendered.empty());
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  10K template renders completed in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 100);  // Should be fast
    
    std::cout << "✓ Performance tests passed" << std::endl;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              MILESTONE 3: LIBRARY TESTS                     ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    try {
        test_text_processing();
        test_streaming_io();
        test_error_handling();
        test_c_api();
        test_performance();
        
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ALL TESTS PASSED! ✅                     ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "🎯 Milestone 3 Libraries Ready for Integration!" << std::endl;
        std::cout << "✅ Text Processing - Regex, templates, analysis" << std::endl;
        std::cout << "✅ Streaming I/O - Buffered reading/writing" << std::endl;
        std::cout << "✅ Error Handling - Stack traces, diagnostics" << std::endl;
        std::cout << "✅ C API - Complete compiler integration" << std::endl;
        std::cout << "✅ Performance - 100K+ operations per second" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}