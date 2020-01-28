#ifndef _LVT_UI_HTTP_HTTPSERVER_H_
#define _LVT_UI_HTTP_HTTPSERVER_H_

#include "interfaces/Server.h"
#include "ui/http/Method.h"
#include "ui/http/UrlParameter.h"
#include "interfaces/UiStatusCode.h"

#include <functional>
#include <vector>

namespace lvt {
namespace ui {
namespace http {

class HttpServer : public lvt::Server {
public:
    HttpServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port);
    
protected:
    void processRequest(lvt::ui::http::Method method, const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> completionHandler);
    
private:
    bool processGetRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processPostRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processPutRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processDeleteRequest(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processCreateVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processModifyVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processDeleteVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processGetVehicle(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
    
    bool processGetVehiclesInArea(const std::vector<std::string> &path, const std::vector<lvt::ui::http::UrlParameter> &urlParameters, std::function<void(lvt::UiStatusCode, const std::string &)> &completionHandler);
};

} // namespace http
} // namespace ui
} // namespace lvt

#endif // _LVT_UI_HTTP_HTTPSERVER_H_
