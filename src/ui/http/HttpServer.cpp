#include "HttpServer.h"

#include "utilities/JsonGenerator.h"
#include "utilities/StringUtils.h"

#include <algorithm>

namespace lvt {
namespace ui {
namespace http {

HttpServer::HttpServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port)
: lvt::Server(listener, port) {
    
}

void HttpServer::processRequest(lvt::ui::http::Method method, const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> completionHandler) {
    bool isRequestProcessed;
    
    switch (method) {
        case lvt::ui::http::Method::Get: isRequestProcessed = processGetRequest(path, urlParameters, completionHandler); break;
        case lvt::ui::http::Method::Post: isRequestProcessed = processPostRequest(path, urlParameters, completionHandler); break;
        case lvt::ui::http::Method::Put: isRequestProcessed = processPutRequest(path, urlParameters, completionHandler); break;
        case lvt::ui::http::Method::Delete: isRequestProcessed = processDeleteRequest(path, urlParameters, completionHandler); break;
        default: isRequestProcessed = false;
    }
    
    if (!isRequestProcessed) {
        completionHandler(lvt::UiStatusCode::Bad_Request, "");
    }
}

static bool checkPathRequest(const std::vector<std::string> &path) {
    return !path.empty() && path[0] == "v1";
}

static bool checkAndGetValue(const std::vector<lvt::ui::http::UrlParameter> &urlParameters, const std::string &name, double &value) {
    double valueTemp;
    
    auto paramItr = std::find_if(urlParameters.begin(), urlParameters.end(), [name] (auto &&param) {
        return param.name == name;
    });
    
    if (paramItr != urlParameters.end() && utilities::StringUtils::toDouble(paramItr->value, valueTemp)) {
        value = valueTemp;
        return true;
    }
    
    return false;
}

bool HttpServer::processGetRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    auto pathSize = path.size();
    
    if (checkPathRequest(path)) {
        if (pathSize == 2) {
            if (path[1] == "vehiclesInArea" && urlParameters.size() == 4) {
                return processGetVehiclesInArea(path, urlParameters, completionHandler);
            }
        }
        else if (pathSize >= 3 && path[1] == "vehicles" && !path[2].empty() && urlParameters.empty()) {
            return processGetVehicle(path, urlParameters, completionHandler);
        }
    }
    
    return false;
}

bool HttpServer::processPostRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    if (checkPathRequest(path) && path.size() == 2 && path[1] == "vehicles" && urlParameters.size() == 2) {
        return processCreateVehicle(path, urlParameters, completionHandler);
    }
    
    return false;
}

bool HttpServer::processPutRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    if (checkPathRequest(path) && path.size() == 3 && path[1] == "vehicles" && !path[2].empty() && urlParameters.size() == 2) {
        return processModifyVehicle(path, urlParameters, completionHandler);
    }
    
    return false;
}

bool HttpServer::processDeleteRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    if (checkPathRequest(path) && path.size() == 3 && path[1] == "vehicles" && !path[2].empty() && urlParameters.empty()) {
        return processDeleteVehicle(path, urlParameters, completionHandler);
    }
    
    return false;
}

bool HttpServer::processCreateVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    double latitude;
    double longitude;
    
    if (checkAndGetValue(urlParameters, "latitude", latitude) &&
        checkAndGetValue(urlParameters, "longitude", longitude)) {
        listener->onCreateVehicle(latitude, longitude, [completionHandler = std::move(completionHandler)] (lvt::UiStatusCode statusCode, const std::string &id) mutable {
            std::string json;
            
            if (statusCode == lvt::UiStatusCode::Created) {
                json = lvt::utilities::JsonGenerator::generateForId(id);
            }
            
            completionHandler(statusCode, json);
        });
        
        return true;
    }
    
    return false;
}

bool HttpServer::processModifyVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    double latitude;
    double longitude;
    
    if (checkAndGetValue(urlParameters, "latitude", latitude) &&
        checkAndGetValue(urlParameters, "longitude", longitude)) {
        auto &id = path[2];
        
        listener->onModifyVehicle(lvt::Vehicle{id, latitude, longitude}, [completionHandler = std::move(completionHandler)] (lvt::UiStatusCode statusCode) mutable {
            completionHandler(statusCode, "");
        });
        
        return true;
    }
    
    return false;
}

bool HttpServer::processDeleteVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    auto &id = path[2];
    
    listener->onDeleteVehicle(id, [completionHandler = std::move(completionHandler)] (lvt::UiStatusCode statusCode) mutable {
        completionHandler(statusCode, "");
    });
    
    return true;
}

bool HttpServer::processGetVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    auto &id = path[2];
    
    listener->onGetVehicle(id, [id = id, completionHandler = std::move(completionHandler)] (lvt::UiStatusCode statusCode, double latitude, double longitude) mutable {
        std::string json;
        
        if (statusCode == lvt::UiStatusCode::Ok) {
            json = lvt::utilities::JsonGenerator::generateForSingleVehicle(lvt::Vehicle{id, latitude, longitude});
        }
        
        completionHandler(statusCode, json);
    });
    
    return true;
}

bool HttpServer::processGetVehiclesInArea(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler) {
    double fromLatitude;
    double toLatitude;
    double fromLongitude;
    double toLongitude;
    
    if (checkAndGetValue(urlParameters, "fromLatitude", fromLatitude) &&
        checkAndGetValue(urlParameters, "toLatitude", toLatitude) &&
        checkAndGetValue(urlParameters, "fromLongitude", fromLongitude) &&
        checkAndGetValue(urlParameters, "toLongitude", toLongitude)) {
        listener->onGetVehiclesInArea(fromLatitude, toLatitude, fromLongitude, toLongitude, [completionHandler = std::move(completionHandler)] (lvt::UiStatusCode statusCode, const std::vector<lvt::Vehicle> &vehicles) mutable {
            std::string json;
            
            if (statusCode == lvt::UiStatusCode::Ok) {
                json = lvt::utilities::JsonGenerator::generateForVehicleArray(vehicles);
            }
            
            completionHandler(statusCode, json);
        });
        
        return true;
    }
    
    return false;
}

} // namespace http
} // namespace ui
} // namespace lvt
