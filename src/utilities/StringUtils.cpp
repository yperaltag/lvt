#include "StringUtils.h"

#include <algorithm>
#include <sstream>

namespace lvt {
namespace utilities {

std::vector<std::string> StringUtils::split(const std::string &str, char delim) {
    auto count = std::count_if(str.begin(), str.end(), [delim] (char c) {
        return c == delim;
    });
    
    std::stringstream stream(str);
    std::string temp;
    std::vector<std::string> result;
    result.reserve(count + 1);

    while(std::getline(stream, temp, delim)) {
       result.push_back(temp);
    }
    
    return result;
}

bool StringUtils::toDouble(const std::string &str, double &result) {
    if (!str.empty() && str[0] != ' ') {
        double temp;
        std::istringstream stream(str);
        stream >> temp;
        
        if (!stream.fail() && stream.eof()) {
            result = temp;
            return true;
        }
    }
    
    return false;
}

} // namespace utilities
} // namespace lvt

