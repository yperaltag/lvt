#include "PostgresqlModel.h"

namespace lvt {
namespace model {
namespace sql {

PostgresqlModel::PostgresqlModel(const std::string &host, const std::string &user, const std::string &password, const std::string &dbName) {
    opts += "host=" + host;
    opts += " user=" + user;
    opts += " password=" + password;
    opts += " dbname=" + dbName;
    
    checkConnection();
}

bool PostgresqlModel::checkConnection() {
    if (!pgConnection) {
        try {
            pgConnection = std::make_unique<pqxx::connection>(opts);
        }
        catch (...) {
            
        }
    }
    
    return pgConnection.get() != nullptr;
}

void PostgresqlModel::insertVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto status = lvt::ModelStatusCode::Internal_Error;
    
    if (checkConnection()) {
        try {
            pqxx::work pgWork(*pgConnection);
            
            auto result = pgWork.exec(
                "INSERT INTO vehicle(id, latitude, longitude) "
                "VALUES(" + pgWork.quote(vehicle.id) + ", " + std::to_string(vehicle.latitude) + ", " + std::to_string(vehicle.longitude) + ")"
            );
            
            pgWork.commit();
            
            if (result.affected_rows() == 1) {
                status = lvt::ModelStatusCode::Ok;
            }
        }
        catch (...) {
            
        }
    }
    
    completionHandler(status);
}

void PostgresqlModel::updateVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto status = lvt::ModelStatusCode::Not_Found;
    
    if (checkConnection()) {
        try {
            pqxx::work pgWork(*pgConnection);
            
            auto result = pgWork.exec(
                "UPDATE vehicle "
                "SET id = " + pgWork.quote(vehicle.id) + ", latitude = " + std::to_string(vehicle.latitude) + ", longitude = " + std::to_string(vehicle.longitude) + " "
                "WHERE id = " + pgWork.quote(vehicle.id)
            );
            
            pgWork.commit();
            
            if (result.affected_rows() == 1) {
                status = lvt::ModelStatusCode::Ok;
            }
        }
        catch (...) {
            status = lvt::ModelStatusCode::Internal_Error;
        }
    }
    
    completionHandler(status);
}

void PostgresqlModel::deleteVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode)> completionHandler) {
    auto status = lvt::ModelStatusCode::Not_Found;
    
    if (checkConnection()) {
        try {
            pqxx::work pgWork(*pgConnection);
            
            auto result = pgWork.exec(
                "DELETE FROM vehicle WHERE id = " + pgWork.quote(id)
            );
            
            pgWork.commit();
            
            if (result.affected_rows() == 1) {
                status = lvt::ModelStatusCode::Ok;
            }
        }
        catch (...) {
            status = lvt::ModelStatusCode::Internal_Error;
        }
    }
    
    completionHandler(status);
}

void PostgresqlModel::getVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode, const lvt::Vehicle &)> completionHandler) {
    auto status = lvt::ModelStatusCode::Not_Found;
    lvt::Vehicle vehicle;
    
    if (checkConnection()) {
        try {
            pqxx::work pgWork(*pgConnection);
            
            auto result = pgWork.exec(
                "SELECT * FROM vehicle "
                "WHERE id = " + pgWork.quote(id)
            );
            
            pgWork.commit();
            
            if (result.affected_rows() == 1) {
                vehicle.id = result[0][0].as<std::string>();
                vehicle.latitude = result[0][1].as<double>();
                vehicle.longitude = result[0][2].as<double>();
                
                status = lvt::ModelStatusCode::Ok;
            }
        }
        catch (...) {
            status = lvt::ModelStatusCode::Internal_Error;
        }
    }
    
    completionHandler(status, vehicle);
}

void PostgresqlModel::getVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::ModelStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) {
    auto status = lvt::ModelStatusCode::Ok;
    std::vector<lvt::Vehicle> vehicles;
    
    if (checkConnection()) {
        try {
            pqxx::work pgWork(*pgConnection);
            
            auto result = pgWork.exec(
                "SELECT * FROM vehicle "
                "WHERE latitude BETWEEN " + std::to_string(fromLatitude) + " AND " + std::to_string(toLatitude) + " "
                "AND longitude BETWEEN " + std::to_string(fromLongitude) + " AND " + std::to_string(toLongitude)
            );
            
            pgWork.commit();
            
            for (auto &&row : result) {
                lvt::Vehicle tempVehicle;
                tempVehicle.id = row[0].as<std::string>();
                tempVehicle.latitude = row[1].as<double>();
                tempVehicle.longitude = row[2].as<double>();
                
                vehicles.push_back(std::move(tempVehicle));
            }
        }
        catch (...) {
            status = lvt::ModelStatusCode::Internal_Error;
        }
    }
    
    completionHandler(status, vehicles);
}

} // namespace sql
} // namespace model
} // namespace lvt
