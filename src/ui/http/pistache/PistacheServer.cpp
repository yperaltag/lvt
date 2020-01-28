#include "PistacheServer.h"

#include "utilities/StringUtils.h"

namespace lvt {
namespace ui {
namespace http {
namespace pistache {

class PistacheServer::PistacheServerHandler : public Pistache::Http::Handler {
public:
    HTTP_PROTOTYPE(PistacheServer::PistacheServerHandler)
    
    PistacheServerHandler(PistacheServer &pistacheServer)
    : pistacheServer(pistacheServer) {
        
    }

    void onRequest(const Pistache::Http::Request &request, Pistache::Http::ResponseWriter response) override {
        lvt::ui::http::Method method;
        
        switch (request.method()) {
            case Pistache::Http::Method::Get: method = lvt::ui::http::Method::Get; break;
            case Pistache::Http::Method::Post: method = lvt::ui::http::Method::Post; break;
            case Pistache::Http::Method::Put: method = lvt::ui::http::Method::Put; break;
            case Pistache::Http::Method::Delete: method = lvt::ui::http::Method::Delete; break;
            default: method = lvt::ui::http::Method::Other;
        }
        
        auto path = lvt::utilities::StringUtils::split(request.resource(), '/');
        
        if (!request.resource().empty() && request.resource()[0] == '/') {
            path.erase(path.begin());
        }
        
        std::vector<lvt::ui::http::UrlParameter> urlParameters;
        
        for (auto itr = request.query().parameters_begin(); itr != request.query().parameters_end(); itr ++) {
            urlParameters.push_back(lvt::ui::http::UrlParameter{itr->first, itr->second});
        }
        
        auto copiableResponse = std::make_shared<decltype(response)>(std::move(response));
        
        pistacheServer.processRequest(method, path, urlParameters, [copiableResponse] (lvt::UiStatusCode statusCode, const std::string &json) {
            Pistache::Http::Code pistacheStatusCode;
            
            switch (statusCode) {
                case lvt::UiStatusCode::Ok: pistacheStatusCode = Pistache::Http::Code::Ok; break;
                case lvt::UiStatusCode::Created: pistacheStatusCode = Pistache::Http::Code::Created; break;
                case lvt::UiStatusCode::Bad_Request: pistacheStatusCode = Pistache::Http::Code::Bad_Request; break;
                case lvt::UiStatusCode::Not_Found: pistacheStatusCode = Pistache::Http::Code::Not_Found; break;
                default: pistacheStatusCode = Pistache::Http::Code::Internal_Server_Error;
            }
            
            if (json.empty()) {
                copiableResponse->send(pistacheStatusCode);
            }
            else {
                copiableResponse->send(pistacheStatusCode, json);
            }
        });
    }
    
private:
    PistacheServer &pistacheServer;
};

PistacheServer::PistacheServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port)
: lvt::ui::http::HttpServer(listener, port) {
    
}

void PistacheServer::run() {
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(port));
    auto opts = Pistache::Http::Endpoint::options().threads(1);

    endpoint = std::make_shared<Pistache::Http::Endpoint>(addr);
    endpoint->init(opts);
    endpoint->setHandler(Pistache::Http::make_handler<lvt::ui::http::pistache::PistacheServer::PistacheServerHandler>(*this));
    endpoint->serve();
}

void PistacheServer::stop() {
    if (endpoint) {
        endpoint->shutdown();
    }
}

} // namespace pistache
} // namespace http
} // namespace ui
} // namespace lvt

