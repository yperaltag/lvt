#include "StdMapModel.h"

namespace lvt {
namespace model {
namespace ram {

void StdMapModel::insertVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto result = vehicles.insert(std::make_pair(vehicle.id, vehicle));
    
    auto statusCode = result.second ? lvt::ModelStatusCode::Ok : lvt::ModelStatusCode::Already_Inserted;
    
    completionHandler(statusCode);
}

void StdMapModel::updateVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto result = vehicles.find(vehicle.id);
    
    auto statusCode = lvt::ModelStatusCode::Not_Found;
    
    if (result != vehicles.end()) {
        result->second.latitude = vehicle.latitude;
        result->second.longitude = vehicle.longitude;
        
        statusCode = lvt::ModelStatusCode::Ok;
    }
    
    completionHandler(statusCode);
}

void StdMapModel::deleteVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto result = vehicles.find(id);
    
    auto statusCode = lvt::ModelStatusCode::Not_Found;
    
    if (result != vehicles.end()) {
        vehicles.erase(result);
        
        statusCode = lvt::ModelStatusCode::Ok;
    }
    
    completionHandler(statusCode);
}

void StdMapModel::getVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode, const lvt::Vehicle &)> completionHandler) {
    auto result = vehicles.find(id);
    
    auto statusCode = lvt::ModelStatusCode::Not_Found;
    lvt::Vehicle vehicle;
    
    if (result != vehicles.end()) {
        vehicle = result->second;
        
        statusCode = lvt::ModelStatusCode::Ok;
    }
    
    completionHandler(statusCode, vehicle);
}

void StdMapModel::getVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::ModelStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) {
    std::vector<lvt::Vehicle> result;
    
    for (auto &&elem : vehicles) {
        auto &&vehicle = elem.second;
        if (vehicle.latitude >= fromLatitude && vehicle.latitude <= toLatitude &&
            vehicle.longitude >= fromLongitude && vehicle.longitude <= toLongitude) {
            result.push_back(vehicle);
        }
    }
    
    completionHandler(lvt::ModelStatusCode::Ok, result);
}

} // namespace ram
} // namespace model
} // namespace lvt
