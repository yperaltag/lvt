#ifndef _LVT_APPLICATION_H_
#define _LVT_APPLICATION_H_

#include "interfaces/UiListener.h"
#include "interfaces/Model.h"

#include <memory>

namespace lvt {
namespace core {

class Application : public lvt::UiListener {
public:
    Application(std::shared_ptr<lvt::Model> model);
    
    void onCreateVehicle(double latitude, double longitude, std::function<void(lvt::UiStatusCode, const std::string &)> completionHandler) override;
    
    void onModifyVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::UiStatusCode)> completionHandler) override;
    
    void onDeleteVehicle(const std::string &id, std::function<void(lvt::UiStatusCode)> completionHandler) override;
    
    void onGetVehicle(const std::string &id, std::function<void(lvt::UiStatusCode, double, double)> completionHandler) override;
    
    void onGetVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::UiStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) override;
    
private:
    std::shared_ptr<lvt::Model> model;
};

} // namespace core
} // namespace lvt

#endif // _LVT_APPLICATION_H_
