#include "JsonGenerator.h"

namespace lvt {
namespace utilities {

std::string JsonGenerator::generateForSingleVehicle(const lvt::Vehicle &vehicle, const std::string &initialTabs) {
    std::string json;
    
    json += initialTabs + "{\n";
    json += initialTabs + "\t\"id\" : \"" + vehicle.id + "\",\n";
    json += initialTabs + "\t\"latitude\" : " + std::to_string(vehicle.latitude) + ",\n";
    json += initialTabs + "\t\"longitude\" : " + std::to_string(vehicle.longitude) + "\n";
    json += initialTabs + "}";
    
    return json;
}

std::string JsonGenerator::generateForVehicleArray(const std::vector<lvt::Vehicle> &vehicles) {
    std::string json;
    
    json += "[\n";
    
    for (auto &&elem : vehicles) {
        json += generateForSingleVehicle(elem, "\t");
        json += ",\n";
    }
    
    if (vehicles.empty()) {
        json += "]";
    }
    else {
        auto len = json.size();
        
        json[len - 2] = '\n';
        json[len - 1] = ']';
    }
    
    return json;
}

std::string JsonGenerator::generateForId(const std::string &id) {
    std::string json;
    
    json += "{\n";
    json += "\t\"id\" : \"" + id + "\"\n";
    json += "}";
    
    return json;
}

} // namespace utilities
} // namespace lvt

