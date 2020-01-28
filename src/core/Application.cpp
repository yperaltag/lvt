#include "Application.h"

#include "utilities/UniqueIdGenerator.h"

namespace lvt {
namespace core {

Application::Application(std::shared_ptr<lvt::Model> model)
: model(model) {
    
}

void Application::onCreateVehicle(double latitude, double longitude, std::function<void(lvt::UiStatusCode, const std::string &)> completionHandler) {
    auto id = lvt::utilities::UniqueIdGenerator::generateUniqueId();
    lvt::Vehicle vehicle{std::move(id), latitude, longitude};
    
    model->insertVehicle(vehicle, [completionHandler = std::move(completionHandler), id = vehicle.id] (lvt::ModelStatusCode statusCode) mutable {
        auto uiStatusCode = statusCode == lvt::ModelStatusCode::Ok ? lvt::UiStatusCode::Created : lvt::UiStatusCode::Internal_Server_Error;
        
        completionHandler(uiStatusCode, id);
    });
}

void Application::onModifyVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::UiStatusCode)> completionHandler) {
    model->updateVehicle(vehicle, [completionHandler = std::move(completionHandler)] (lvt::ModelStatusCode statusCode) mutable {
        auto uiStatusCode = lvt::UiStatusCode::Internal_Server_Error;
        
        if (statusCode == lvt::ModelStatusCode::Ok) {
            uiStatusCode = lvt::UiStatusCode::Ok;
        }
        else if (statusCode == lvt::ModelStatusCode::Not_Found) {
            uiStatusCode = lvt::UiStatusCode::Not_Found;
        }
        
        completionHandler(uiStatusCode);
    });
}

void Application::onDeleteVehicle(const std::string &id, std::function<void(lvt::UiStatusCode)> completionHandler) {
    model->deleteVehicle(id, [completionHandler = std::move(completionHandler)] (lvt::ModelStatusCode statusCode) mutable {
        auto uiStatusCode = lvt::UiStatusCode::Internal_Server_Error;
        
        if (statusCode == lvt::ModelStatusCode::Ok) {
            uiStatusCode = lvt::UiStatusCode::Ok;
        }
        else if (statusCode == lvt::ModelStatusCode::Not_Found) {
            uiStatusCode = lvt::UiStatusCode::Not_Found;
        }
        
        completionHandler(uiStatusCode);
    });
}

void Application::onGetVehicle(const std::string &id, std::function<void(lvt::UiStatusCode, double, double)> completionHandler) {
    model->getVehicle(id, [id = id, completionHandler = std::move(completionHandler)] (lvt::ModelStatusCode statusCode, const lvt::Vehicle &vehicle) mutable {
        auto uiStatusCode = lvt::UiStatusCode::Internal_Server_Error;
        
        if (statusCode == lvt::ModelStatusCode::Not_Found) {
            uiStatusCode = lvt::UiStatusCode::Not_Found;
        }
        else if (statusCode == lvt::ModelStatusCode::Ok && id == vehicle.id) {
            uiStatusCode = lvt::UiStatusCode::Ok;
        }
        
        completionHandler(uiStatusCode, vehicle.latitude, vehicle.longitude);
    });
}

void Application::onGetVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::UiStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) {
    model->getVehiclesInArea(fromLatitude, toLatitude, fromLongitude, toLongitude, [completionHandler = std::move(completionHandler)] (lvt::ModelStatusCode statusCode, const std::vector<lvt::Vehicle> &vehicles) mutable {
        auto uiStatusCode = statusCode == lvt::ModelStatusCode::Ok ? lvt::UiStatusCode::Ok : lvt::UiStatusCode::Internal_Server_Error;
        
        completionHandler(uiStatusCode, vehicles);
    });
}

} // namespace core
} // namespace lvt
