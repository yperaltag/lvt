#ifndef _LVT_MODEL_RAM_STDMAPMODEL_H_
#define _LVT_MODEL_RAM_STDMAPMODEL_H_

#include "interfaces/Model.h"

#include <map>

namespace lvt {
namespace model {
namespace ram {

class StdMapModel : public lvt::Model {
public:
    void insertVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void updateVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void deleteVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void getVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode, const lvt::Vehicle &)> completionHandler) override;
    
    void getVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::ModelStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) override;
    
private:
    std::map<std::string, lvt::Vehicle> vehicles;
};

} // namespace ram
} // namespace model
} // namespace lvt

#endif // _LVT_MODEL_RAM_STDMAPMODEL_H_
