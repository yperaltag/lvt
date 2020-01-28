#ifndef _LVT_SERVER_H_
#define _LVT_SERVER_H_

#include "UiListener.h"

#include <memory>
#include <cstdint>

namespace lvt {

class Server {
public:
    Server(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port)
    : listener(listener), port(port) {
        
    }
    
    virtual void run() = 0;
    
    virtual void stop() = 0;
    
protected:
    std::shared_ptr<lvt::UiListener> listener;
    std::uint16_t port;
};

} // namespace lvt

#endif // _LVT_SERVER_H_
