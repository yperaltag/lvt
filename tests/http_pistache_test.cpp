#include "core/Application.h"
#include "ui/http/pistache/PistacheServer.h"
#include "model/ram/StdMapModel.h"
#include "utilities/StringUtils.h"

#include "gtest/gtest.h"
#include <pistache/client.h>

#include <future>
#include <regex>

bool isAlphaNum(const std::string &str) {
    return std::find_if_not(str.begin(), str.end(), [] (char c) {
        return std::isalnum(c) || c == '-';
    }) == str.end();
}

bool getVehicleId(const std::string &json, std::string &id) {
    std::string initialTabs = "";
    std::string re;
    
    re += "^";
    re += initialTabs + "\\{\n";
    re += initialTabs + "\t\"id\" : \"" + "([^\"]+)" + "\"\n";
    re += initialTabs + "\\}";
    re += "$";
    
    std::smatch m;
    std::regex_search(json, m, std::regex(re));
    
    if (!m.empty() && isAlphaNum(m[1])) {
        id = m[1];
        return true;
    }
    
    return false;
}

bool getVehicle(const std::string &json, std::string &id, double &latitude, double &longitude) {
    std::string initialTabs = "";
    std::string re;
    
    re += "^";
    re += initialTabs + "\\{\n";
    re += initialTabs + "\t\"id\" : \"" + "([^\"]+)" + "\",\n";
    re += initialTabs + "\t\"latitude\" : " + "([^,]+)" + ",\n";
    re += initialTabs + "\t\"longitude\" : " + "(.+)" + "\n";
    re += initialTabs + "\\}";
    re += "$";
    
    std::smatch m;
    std::regex_search(json, m, std::regex(re));
    
    double latitudeTemp;
    double longitudeTemp;
    
    if (!m.empty() && isAlphaNum(m[1])
        && lvt::utilities::StringUtils::toDouble(m[2], latitudeTemp) &&
        lvt::utilities::StringUtils::toDouble(m[3], longitudeTemp)) {
        id = m[1];
        latitude = latitudeTemp;
        longitude = longitudeTemp;
        
        return true;
    }
    
    return false;
}

TEST(http_client_test, create_and_get_vehicle) {
    auto model = std::make_shared<lvt::model::ram::StdMapModel>();
    
    auto app = std::make_shared<lvt::core::Application>(model);
    
    auto pistacheServer = std::make_shared<lvt::ui::http::pistache::PistacheServer>(app, 9080);
    
    auto runningServer = std::async(std::launch::async, [pistacheServer] () {
        pistacheServer->run();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //waiting for the server to run
    
    Pistache::Http::Client client;
    client.init();

    auto rb = client.post("localhost:9080/v1/vehicles").params(Pistache::Http::Uri::Query{
        {"latitude", "1.2"},
        {"longitude", "3.4"}
    });
    
    auto response = rb.send();
    
    bool responseReceived = false;
    std::string id;
    response.then([&responseReceived, &id] (Pistache::Http::Response rsp) {
        responseReceived = true;
        EXPECT_EQ(rsp.code(), Pistache::Http::Code::Created);
        
        EXPECT_TRUE(getVehicleId(rsp.body(), id));
    },
    Pistache::Async::IgnoreException);
    
    Pistache::Async::Barrier<Pistache::Http::Response> barrier(response);
    barrier.wait_for(std::chrono::seconds(5));
    
    if (responseReceived) {
        auto rb2 = client.get("localhost:9080/v1/vehicles/" + id);
        
        auto response2 = rb2.send();
        
        responseReceived = false;
        response2.then([&responseReceived, id] (Pistache::Http::Response rsp) {
            responseReceived = true;
            EXPECT_EQ(rsp.code(), Pistache::Http::Code::Ok);
            
            std::string id2;
            double latitude;
            double longitude;
            EXPECT_TRUE(getVehicle(rsp.body(), id2, latitude, longitude));
            
            EXPECT_EQ(id, id2);
            EXPECT_DOUBLE_EQ(latitude, 1.2);
            EXPECT_DOUBLE_EQ(longitude, 3.4);
        },
        Pistache::Async::IgnoreException);
        
        Pistache::Async::Barrier<Pistache::Http::Response> barrier2(response2);
        barrier2.wait_for(std::chrono::seconds(5));
    }
    
    pistacheServer->stop();
    client.shutdown();
    
    runningServer.wait();
    
    EXPECT_TRUE(responseReceived);
}
