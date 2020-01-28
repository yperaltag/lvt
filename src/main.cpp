#include "core/Application.h"
#include "ui/http/pistache/PistacheServer.h"
#include "ui/rpc/grpc/GrpcServer.h"
#include "model/sql/postgresql/PostgresqlModel.h"

#include <future>

int main() {
    auto model = std::make_shared<lvt::model::sql::PostgresqlModel>("localhost",
                                                                    "postgres",
                                                                    "postgres",
                                                                    "lvt");
    
    auto app = std::make_shared<lvt::core::Application>(model);
    
    auto pistacheServer = std::make_shared<lvt::ui::http::pistache::PistacheServer>(app, 9080);
    
    auto grpcServer = std::make_shared<lvt::ui::rpc::GrpcServer>(app, 50051);
    
    auto runningServer = std::async(std::launch::async, [grpcServer] () {
        grpcServer->run();
    });
    
    pistacheServer->run();
    
    runningServer.wait();
    
    return 0;
}
