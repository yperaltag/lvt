#ifndef _LVT_UI_RPC_GRPCSERVER_H_
#define _LVT_UI_RPC_GRPCSERVER_H_

#include "interfaces/Server.h"

#include <grpcpp/grpcpp.h>

#include <memory>

namespace lvt {
namespace ui {
namespace rpc {

class GrpcServer : public lvt::Server {
public:
    GrpcServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port);
    
    void run() override;
    
    void stop() override;
    
private:
    class GrpcServerServiceImpl;
    
    std::unique_ptr<::grpc::ServerCompletionQueue> *cq;
    std::unique_ptr<::grpc::Server> server;
};

} // namespace rpc
} // namespace ui
} // namespace lvt

#endif // _LVT_UI_RPC_GRPCSERVER_H_
