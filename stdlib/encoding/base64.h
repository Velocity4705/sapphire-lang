#ifndef SAPPHIRE_STDLIB_BASE64_H
#define SAPPHIRE_STDLIB_BASE64_H

#include <string>

namespace sapphire {
namespace base64 {

std::string encode(const std::string& data);
std::string decode(const std::string& encoded);

} // namespace base64
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_BASE64_H
