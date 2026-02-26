#include "argparser.h"
#include <iostream>

namespace sapphire {
namespace cli {

ArgParser::ArgParser(int argc, char** argv) {
    if (argc > 0) {
        program_name_ = argv[0];
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Long option
            std::string name = arg.substr(2);
            size_t eq_pos = name.find('=');
            
            if (eq_pos != std::string::npos) {
                // --name=value
                std::string key = name.substr(0, eq_pos);
                std::string value = name.substr(eq_pos + 1);
                args_[key] = value;
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                // --name value
                args_[name] = argv[++i];
            } else {
                // --flag
                args_[name] = "true";
            }
        } else if (arg[0] == '-' && arg.length() > 1) {
            // Short option
            std::string name = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args_[name] = argv[++i];
            } else {
                args_[name] = "true";
            }
        } else {
            // Positional argument
            positional_.push_back(arg);
        }
    }
}

bool ArgParser::has(const std::string& name) const {
    return args_.find(name) != args_.end();
}

std::string ArgParser::get(const std::string& name, const std::string& default_val) const {
    auto it = args_.find(name);
    return it != args_.end() ? it->second : default_val;
}

int ArgParser::get_int(const std::string& name, int default_val) const {
    auto it = args_.find(name);
    if (it == args_.end()) return default_val;
    try {
        return std::stoi(it->second);
    } catch (...) {
        return default_val;
    }
}

bool ArgParser::get_bool(const std::string& name, bool default_val) const {
    auto it = args_.find(name);
    if (it == args_.end()) return default_val;
    std::string val = it->second;
    return val == "true" || val == "1" || val == "yes";
}

const std::vector<std::string>& ArgParser::positional() const {
    return positional_;
}

void ArgParser::add_option(const std::string& name, const std::string& description) {
    descriptions_[name] = description;
}

void ArgParser::print_help() const {
    std::cout << "Usage: " << program_name_ << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    for (const auto& [name, desc] : descriptions_) {
        std::cout << "  --" << name << "\t" << desc << "\n";
    }
}

} // namespace cli
} // namespace sapphire
