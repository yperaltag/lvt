#ifndef _LVT_UILISTENER_H_
#define _LVT_UILISTENER_H_

#include "UiStatusCode.h"
#include "Vehicle.h"

#include <functional>
#include <vector>

namespace lvt {

class UiListener {
public:
    virtual ~UiListener() {}
    
    virtual void onCreateVehicle(double latitude, double longitude, std::function<void(lvt::UiStatusCode, const std::string &)> completionHandler) = 0;
    
    virtual void onModifyVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::UiStatusCode)> completionHandler) = 0;
    
    virtual void onDeleteVehicle(const std::string &id, std::function<void(lvt::UiStatusCode)> completionHandler) = 0;
    
    virtual void onGetVehicle(const std::string &id, std::function<void(lvt::UiStatusCode, double, double)> completionHandler) = 0;
    
    virtual void onGetVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::UiStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) = 0;
};

} // namespace lvt

#endif // _LVT_UILISTENER_H_
