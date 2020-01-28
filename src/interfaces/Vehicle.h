#ifndef _LVT_VEHICLE_H_
#define _LVT_VEHICLE_H_

#include <string>

namespace lvt {

struct Vehicle {
    std::string id;
    double latitude;
    double longitude;
};

} // namespace lvt

#endif // _LVT_VEHICLE_H_
