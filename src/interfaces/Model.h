#ifndef _LVT_MODEL_H_
#define _LVT_MODEL_H_

#include "ModelStatusCode.h"
#include "Vehicle.h"

#include <functional>
#include <vector>

namespace lvt {

class Model {
public:
    virtual ~Model() {}
    
    virtual void insertVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) = 0;
    
    virtual void updateVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) = 0;
    
    virtual void deleteVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode)> completionHandler) = 0;
    
    virtual void getVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode, const lvt::Vehicle &)> completionHandler) = 0;
    
    virtual void getVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::ModelStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) = 0;
};

} // namespace lvt

#endif // _LVT_MODEL_H_
