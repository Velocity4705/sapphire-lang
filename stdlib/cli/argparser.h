#ifndef SAPPHIRE_STDLIB_ARGPARSER_H
#define SAPPHIRE_STDLIB_ARGPARSER_H

#include <string>
#include <vector>
#include <map>

namespace sapphire {
namespace cli {

class ArgParser {
private:
    std::map<std::string, std::string> args_;
    std::vector<std::string> positional_;
    std::map<std::string, std::string> descriptions_;
    std::string program_name_;
    
public:
    ArgParser(int argc, char** argv);
    
    bool has(const std::string& name) const;
    std::string get(const std::string& name, const std::string& default_val = "") const;
    int get_int(const std::string& name, int default_val = 0) const;
    bool get_bool(const std::string& name, bool default_val = false) const;
    
    const std::vector<std::string>& positional() const;
    
    void add_option(const std::string& name, const std::string& description);
    void print_help() const;
};

} // namespace cli
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_ARGPARSER_H
