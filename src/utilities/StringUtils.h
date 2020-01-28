#ifndef _LVT_UTILITIES_STRINGUTILS_H_
#define _LVT_UTILITIES_STRINGUTILS_H_

#include <string>
#include <vector>

namespace lvt {
namespace utilities {

class StringUtils {
public:
    static std::vector<std::string> split(const std::string &str, char delim);

    static bool toDouble(const std::string &str, double &result);
};

} // namespace utilities
} // namespace lvt

#endif // _LVT_UTILITIES_STRINGUTILS_H_
