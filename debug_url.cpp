#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

std::string url_encode(const std::string& str) {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;
    
    for (char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << std::uppercase << static_cast<unsigned char>(c);
        }
    }
    
    return encoded.str();
}

std::string url_decode(const std::string& str) {
    std::ostringstream decoded;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            try {
                char c = static_cast<char>(std::stoi(hex, nullptr, 16));
                decoded << c;
                i += 2;
            } catch (...) {
                decoded << str[i];
            }
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    
    return decoded.str();
}

int main() {
    std::string original = "Hello World! @#$%^&*()";
    std::string encoded = url_encode(original);
    std::string decoded = url_decode(encoded);
    
    std::cout << "Original: '" << original << "'" << std::endl;
    std::cout << "Encoded:  '" << encoded << "'" << std::endl;
    std::cout << "Decoded:  '" << decoded << "'" << std::endl;
    
    return 0;
}
