#ifndef _LVT_MODEL_RAM_POSTGRESQLMODEL_H_
#define _LVT_MODEL_RAM_POSTGRESQLMODEL_H_

#include "interfaces/Model.h"

#include <pqxx/pqxx>
#include <memory>

namespace lvt {
namespace model {
namespace sql {

class PostgresqlModel : public lvt::Model {
public:
    PostgresqlModel(const std::string &host, const std::string &user, const std::string &password, const std::string &dbName);
    
    void insertVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void updateVehicle(const lvt::Vehicle &vehicle, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void deleteVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode)> completionHandler) override;
    
    void getVehicle(const std::string &id, std::function<void(lvt::ModelStatusCode, const lvt::Vehicle &)> completionHandler) override;
    
    void getVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude, std::function<void(lvt::ModelStatusCode, const std::vector<lvt::Vehicle> &)> completionHandler) override;
    
private:
    bool checkConnection();
    
    std::unique_ptr<pqxx::connection> pgConnection;
    std::string opts;
};

} // namespace sql
} // namespace model
} // namespace lvt

#endif // _LVT_MODEL_RAM_POSTGRESQLMODEL_H_
