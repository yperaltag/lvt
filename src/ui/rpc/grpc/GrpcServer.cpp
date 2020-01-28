#include "GrpcServer.h"

#include "lvt.grpc.pb.h"

namespace lvt {
namespace ui {
namespace rpc {

class CallData {
public:
    CallData(Lvt::AsyncService* service, ::grpc::ServerCompletionQueue* cq, std::shared_ptr<lvt::UiListener> listener)
    : service(service), cq(cq), listener(listener) {
        
    }
    
    virtual ~CallData() {}

    virtual void proceed() = 0;

protected:
    virtual void requestCall() = 0;
    
    virtual CallData *newCallData(Lvt::AsyncService* service, ::grpc::ServerCompletionQueue* cq, std::shared_ptr<lvt::UiListener> listener) = 0;
    
    virtual void processCall() = 0;
    
    Lvt::AsyncService *service;
    ::grpc::ServerCompletionQueue *cq;
    ::grpc::ServerContext ctx;
    
    std::shared_ptr<lvt::UiListener> listener;
};

class SingleCallData : public CallData {
public:
    SingleCallData(Lvt::AsyncService* service, ::grpc::ServerCompletionQueue* cq, std::shared_ptr<lvt::UiListener> listener)
    : CallData(service, cq, listener), status(CallStatus::CREATE) {
        
    }
    
    void proceed() override {
        if (status == CallStatus::CREATE) {
            status = CallStatus::PROCESS;

            requestCall();
        }
        else if (status == CallStatus::PROCESS) {
            newCallData(service, cq, listener)->proceed();

            status = CallStatus::FINISH;
            
            processCall();
        }
        else {
            delete this;
        }
    }

private:
    enum class CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status;
};

#define CALLDATA_PROTOTYPE_WITHOUT_RESPONDER(CallName, RequestType, ResponseType, BaseClass)                                                                       \
    public: \
        CallName##Data(Lvt::AsyncService* service, ::grpc::ServerCompletionQueue* cq, std::shared_ptr<lvt::UiListener> listener) \
        : BaseClass(service, cq, listener), responder(&ctx) { \
            \
        } \
\
    private: \
        RequestType request; \
        ResponseType response; \
\
    protected: \
        void requestCall() override { \
            service->Request##CallName(&ctx, &request, &responder, cq, cq, this); \
        } \
        \
        CallData *newCallData(Lvt::AsyncService* service, ::grpc::ServerCompletionQueue* cq, std::shared_ptr<lvt::UiListener> listener) override { \
            return new CallName##Data(service, cq, listener);                                                                                \
        } \

#define CALLDATA_PROTOTYPE_WITH_RESPONDER(CallName, RequestType, ResponseType, BaseClass)                                                                       \
    private: \
        ::grpc::ServerAsyncResponseWriter<ResponseType> responder; \
\
    CALLDATA_PROTOTYPE_WITHOUT_RESPONDER(CallName, RequestType, ResponseType, BaseClass)

#define CALLDATA_PROTOTYPE_WITH_STREAM_RESPONDER(CallName, RequestType, ResponseType, BaseClass)                                                                       \
    private: \
        ::grpc::ServerAsyncWriter<ResponseType> responder; \
\
    CALLDATA_PROTOTYPE_WITHOUT_RESPONDER(CallName, RequestType, ResponseType, BaseClass)

class CreateVehicleData : public SingleCallData {

    CALLDATA_PROTOTYPE_WITH_RESPONDER(CreateVehicle, VehicleLocation, VehicleId, SingleCallData)
    
    void processCall() override {
        double latitude = request.latitude();
        double longitude = request.longitude();
        
        listener->onCreateVehicle(latitude, longitude, [this] (lvt::UiStatusCode statusCode, const std::string &id) {
            auto status = ::grpc::Status::CANCELLED;
            
            if (statusCode == lvt::UiStatusCode::Created) {
                response.set_id(id);
                
                status = ::grpc::Status::OK;
            }
            
            responder.Finish(response, status, this);
        });
    }
};

class GetVehicleData : public SingleCallData {

    CALLDATA_PROTOTYPE_WITH_RESPONDER(GetVehicle, VehicleId, VehicleValues, SingleCallData)
    
    void processCall() override {
        auto &id = request.id();
        
        listener->onGetVehicle(id, [this, &id] (lvt::UiStatusCode statusCode, double latitude, double longitude) {
            auto status = ::grpc::Status::CANCELLED;
            
            if (statusCode == lvt::UiStatusCode::Ok) {
                response.set_id(id);
                response.set_latitude(latitude);
                response.set_longitude(longitude);
                
                status = ::grpc::Status::OK;
            }
            
            responder.Finish(response, status, this);
        });
    }
};

class UpdateVehicleData : public SingleCallData {

    CALLDATA_PROTOTYPE_WITH_RESPONDER(UpdateVehicle, VehicleValues, google::protobuf::Empty, SingleCallData)
    
    void processCall() override {
        auto &id = request.id();
        double latitude = request.latitude();
        double longitude = request.longitude();
        
        listener->onModifyVehicle(lvt::Vehicle{id, latitude, longitude}, [this] (lvt::UiStatusCode statusCode) {
            auto status = ::grpc::Status::CANCELLED;
            
            if (statusCode == lvt::UiStatusCode::Ok) {
                status = ::grpc::Status::OK;
            }
            
            responder.Finish(response, status, this);
        });
    }
};

class DeleteVehicleData : public SingleCallData {

    CALLDATA_PROTOTYPE_WITH_RESPONDER(DeleteVehicle, VehicleId, google::protobuf::Empty, SingleCallData)
    
    void processCall() override {
        auto &id = request.id();
        
        listener->onDeleteVehicle(id, [this] (lvt::UiStatusCode statusCode) {
            auto status = ::grpc::Status::CANCELLED;
            
            if (statusCode == lvt::UiStatusCode::Ok) {
                status = ::grpc::Status::OK;
            }
            
            responder.Finish(response, status, this);
        });
    }
};

class GetVehiclesInAreaData : public CallData {

    CALLDATA_PROTOTYPE_WITH_STREAM_RESPONDER(GetVehiclesInArea, Area, VehicleValues, CallData)
    
    void processCall() override {
        double fromLatitude = request.fromlatitude();
        double toLatitude = request.tolatitude();
        double fromLongitude = request.fromlongitude();
        double toLongitude = request.tolongitude();
        
        listener->onGetVehiclesInArea(fromLatitude, toLatitude, fromLongitude, toLongitude, [this] (lvt::UiStatusCode statusCode, const std::vector<lvt::Vehicle> &vehicles) {
            if (statusCode == lvt::UiStatusCode::Ok) {
                sendVehicles(vehicles);
            }
            else {
                responder.Finish(::grpc::Status::CANCELLED, this);
            }
        });
    }
    
public:
    void proceed() override {
        if (status == CallStatus::CREATE) {
            status = CallStatus::PROCESS;

            requestCall();
        }
        else if (status == CallStatus::PROCESS) {
            newCallData(service, cq, listener)->proceed();

            status = CallStatus::FINISH;
            
            processCall();
        }
        else if (status == CallStatus::STREAM_PROCESS) {
            sendSingleVehicle();
        }
        else {
            delete this;
        }
    }
    
private:
    void sendVehicles(const std::vector<lvt::Vehicle> &vehicles) {
        this->vehicles = vehicles;
        
        vehiclesItr = this->vehicles.begin();
        status = CallStatus::STREAM_PROCESS;
        
        sendSingleVehicle();
    }
    
    void sendSingleVehicle() {
        if (vehiclesItr != vehicles.end()) {
            response.set_id(vehiclesItr->id);
            response.set_latitude(vehiclesItr->latitude);
            response.set_longitude(vehiclesItr->longitude);
            
            vehiclesItr ++;
            
            responder.Write(response, this);
        }
        else {
            status = CallStatus::FINISH;
            responder.Finish(::grpc::Status::OK, this);
        }
    }

    enum class CallStatus { CREATE, PROCESS, FINISH, STREAM_PROCESS };
    CallStatus status = CallStatus::CREATE;
    
    std::vector<lvt::Vehicle> vehicles;
    std::vector<lvt::Vehicle>::const_iterator vehiclesItr;
};

GrpcServer::GrpcServer(std::shared_ptr<lvt::UiListener> listener, std::uint16_t port)
: lvt::Server(listener, port), cq(nullptr) {
    
}
    
void GrpcServer::run() {
    Lvt::AsyncService service;

    ::grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:" + std::to_string(port), ::grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    cq = new std::unique_ptr<::grpc::ServerCompletionQueue>(builder.AddCompletionQueue());

    server = builder.BuildAndStart();
    
    (new CreateVehicleData(&service, cq->get(), listener))->proceed();
    (new GetVehicleData(&service, cq->get(), listener))->proceed();
    (new UpdateVehicleData(&service, cq->get(), listener))->proceed();
    (new DeleteVehicleData(&service, cq->get(), listener))->proceed();
    (new GetVehiclesInAreaData(&service, cq->get(), listener))->proceed();
    
    void* tag;
    bool ok = true;
    
    while ((*cq)->Next(&tag, &ok) && ok) {
        static_cast<CallData *>(tag)->proceed();
    }
}

void GrpcServer::stop() {
    if (server) {
        server->Shutdown();
    }
    
    if (cq) {
        (*cq)->Shutdown();
    }
}

} // namespace rpc
} // namespace ui
} // namespace lvt
