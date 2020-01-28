#include "core/Application.h"
#include "ui/rpc/grpc/GrpcServer.h"
#include "model/ram/StdMapModel.h"

#include "lvt.grpc.pb.h"

#include "gtest/gtest.h"
#include <grpcpp/grpcpp.h>

#include <future>

bool isAlphaNum(const std::string &str) {
    return std::find_if_not(str.begin(), str.end(), [] (char c) {
        return std::isalnum(c) || c == '-';
    }) == str.end();
}

class LvtClient {
public:
    LvtClient(std::shared_ptr<::grpc::Channel> channel)
    : stub(Lvt::NewStub(channel)) {
        
    }

    std::pair<std::string, bool> CreateVehicle(double latitude, double longitude) {
        VehicleLocation request;
        request.set_latitude(latitude);
        request.set_longitude(longitude);

        VehicleId response;

        ::grpc::ClientContext context;

        auto status = stub->CreateVehicle(&context, request, &response);
        
        return std::make_pair(response.id(), status.ok());
    }
    
    std::pair<lvt::Vehicle, bool> GetVehicle(const std::string &id) {
        VehicleId request;
        request.set_id(id);

        VehicleValues response;

        ::grpc::ClientContext context;

        auto status = stub->GetVehicle(&context, request, &response);
        
        return std::make_pair(lvt::Vehicle{response.id(), response.latitude(), response.longitude()}, status.ok());
    }
    
    std::pair<std::vector<lvt::Vehicle>, bool> GetVehiclesInArea(double fromLatitude, double toLatitude, double fromLongitude, double toLongitude) {
        Area request;
        request.set_fromlatitude(fromLatitude);
        request.set_tolatitude(toLatitude);
        request.set_fromlongitude(fromLongitude);
        request.set_tolongitude(toLongitude);

        VehicleValues response;
        std::vector<lvt::Vehicle> vehicles;

        ::grpc::ClientContext context;

        auto reader = stub->GetVehiclesInArea(&context, request);
        
        while (reader->Read(&response)) {
            vehicles.push_back(lvt::Vehicle{response.id(), response.latitude(), response.longitude()});
        }
        
        auto status = reader->Finish();
        
        return std::make_pair(std::move(vehicles), status.ok());
    }

private:
    std::unique_ptr<Lvt::Stub> stub;
};

TEST(rpc_client_test, create_and_get_single_vehicle) {
    auto model = std::make_shared<lvt::model::ram::StdMapModel>();
    
    auto app = std::make_shared<lvt::core::Application>(model);
    
    auto grpcServer = std::make_shared<lvt::ui::rpc::GrpcServer>(app, 50051);
    
    auto runningServer = std::async(std::launch::async, [grpcServer] () {
        grpcServer->run();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //waiting for the server to run
    
    LvtClient client(::grpc::CreateChannel("localhost:50051", ::grpc::InsecureChannelCredentials()));
    
    auto id = client.CreateVehicle(1.2, 3.4);
    
    EXPECT_TRUE(id.second);
    
    EXPECT_TRUE(isAlphaNum(id.first));
    
    auto vehicle = client.GetVehicle(id.first);
    
    EXPECT_TRUE(vehicle.second);
    
    EXPECT_EQ(vehicle.first.id, id.first);
    EXPECT_DOUBLE_EQ(vehicle.first.latitude, 1.2);
    EXPECT_DOUBLE_EQ(vehicle.first.longitude, 3.4);
    
    grpcServer->stop();
    
    runningServer.wait();
}

TEST(rpc_client_test, create_and_get_several_vehicles) {
    auto model = std::make_shared<lvt::model::ram::StdMapModel>();
    
    auto app = std::make_shared<lvt::core::Application>(model);
    
    auto grpcServer = std::make_shared<lvt::ui::rpc::GrpcServer>(app, 50051);
    
    auto runningServer = std::async(std::launch::async, [grpcServer] () {
        grpcServer->run();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //waiting for the server to run
    
    LvtClient client(::grpc::CreateChannel("localhost:50051", ::grpc::InsecureChannelCredentials()));
    
    auto id = client.CreateVehicle(1.2, 3.4);
    
    EXPECT_TRUE(id.second);
    
    EXPECT_TRUE(isAlphaNum(id.first));
    
    auto id2 = client.CreateVehicle(1.3, 3.3);
    
    EXPECT_TRUE(id2.second);
    
    EXPECT_TRUE(isAlphaNum(id2.first));
    
    auto vehicles = client.GetVehiclesInArea(1.2, 1.3, 3.3, 3.4);
    
    EXPECT_TRUE(vehicles.second);
    
    EXPECT_EQ(vehicles.first.size(), 2);
    
    grpcServer->stop();
    
    runningServer.wait();
}
