#ifndef _LVT_UI_HTTP_PISTACHE_PISTACHESERVER_H_
#define _LVT_UI_HTTP_PISTACHE_PISTACHESERVER_H_

#include "ui/http/HttpServer.h"

#include "pistache/endpoint.h"

#include <memory>

namespace lvt {
namespace ui {
namespace http {
namespace pistache {

class PistacheServer : public lvt::ui::http::HttpServer {
public:
    PistacheServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port);
    
    void run() override;
    
    void stop() override;
    
private:
    class PistacheServerHandler;
    
    std::shared_ptr<Pistache::Http::Endpoint> endpoint;
};

} // namespace pistache
} // namespace http
} // namespace ui
} // namespace lvt

#endif // _LVT_UI_HTTP_PISTACHE_PISTACHESERVER_H_
