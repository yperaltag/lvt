#ifndef _LVT_UTILITIES_JSONGENERATOR_H_
#define _LVT_UTILITIES_JSONGENERATOR_H_

#include "interfaces/Vehicle.h"

#include <vector>

namespace lvt {
namespace utilities {

class JsonGenerator {
public:
    static std::string generateForSingleVehicle(const lvt::Vehicle &vehicle, const std::string &initialTabs = "");

    static std::string generateForVehicleArray(const std::vector<lvt::Vehicle> &vehicles);

    static std::string generateForId(const std::string &id);
};

} // namespace utilities
} // namespace lvt

#endif // _LVT_UTILITIES_JSONGENERATOR_H_
