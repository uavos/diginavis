#include "asyncclient.h"

#include <chrono>
#include <QDebug>
#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"
#include "Mission/Waypoint.h"

Tracker::Tracker(std::shared_ptr<grpc::Channel> channel, const QString &uuid):
    m_stub(TrackingService::NewStub(channel)),
    m_writer(m_stub->receive(&m_clientContext, &m_empty)),
    m_uuid(uuid)
{
}

void Tracker::write(float altitude, float latitude, float longitude, float gspeed, uint ts)
{
    auto msecs = std::chrono::milliseconds(ts);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(msecs);
    auto nsecs = std::chrono::nanoseconds(msecs) - std::chrono::nanoseconds(secs);

    grpc::ClientContext clientContext;
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(500, GPR_TIMESPAN)));
    TrackingRequest request;
    request.mutable_point()->set_altitude(altitude);
    request.mutable_point()->set_latitude(latitude);
    request.mutable_point()->set_longitude(longitude);
    request.set_speed(gspeed);
    request.mutable_time()->set_seconds(secs.count());
    request.mutable_time()->set_nanos(nsecs.count());
    request.set_mission_uuid(m_uuid.toStdString());

    if(!m_writer->Write(request))
        std::cerr << "Can't write track" << std::endl;
    else {
        std::cout << request.DebugString() << std::endl;
    }
}

Tracker::~Tracker()
{
    m_writer->WritesDone();
    m_writer->Finish();
}

AsyncClient::AsyncClient(QObject *parent):
    QThread(parent)
{
}

void AsyncClient::stop()
{
    m_stop = true;
}

bool AsyncClient::isConnected() const
{
    return m_isConnected;
}

void AsyncClient::setIsConnected(bool b)
{
    if(m_isConnected != b) {
        m_isConnected = b;
        emit isConnectedChanged();
    }
}

void AsyncClient::setFlightplanUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    if(m_flightplanUuid != uuid) {
        m_flightplanUuid = uuid;
        emit flightplanUuidChanged();
    }
}

void AsyncClient::setDeviceUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_deviceUuid = uuid;
}

void AsyncClient::setFlighRequestUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_flightRequestUuid = uuid;
}

QString AsyncClient::getHost() const
{
    std::unique_lock locker(m_ioMutex);
    return m_host;
}

QString AsyncClient::getFlightplanUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_flightplanUuid;
}

void AsyncClient::registerFlightplan()
{
    std::unique_lock locker(m_ioMutex);
    m_tasks.push(RegisterFlighplan);
}

void AsyncClient::run()
{
    m_stop = false;
    setIsConnected(false);
    QTime syncPoint;
    while(!m_stop) {
        auto channel = grpc::CreateChannel(m_host.toStdString(), grpc::InsecureChannelCredentials());
        if(channel) {
            auto deadline = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(2, GPR_TIMESPAN));
            if(channel->WaitForConnected(deadline)) {
                setIsConnected(true);
                auto missionStub = MissionService::NewStub(channel);
                std::unique_ptr<Tracker> tracker;

                if(missionStub) {
                    while(!m_stop) {
                        auto task = getTask();
                        if(task) {
                            if(task.value() == RegisterFlighplan) {
                                rpcRegisterFlightplan(missionStub);
                                syncPoint.start();
                                tracker = std::make_unique<Tracker>(channel, m_flightplanUuid);
                            }
                        }

                        if(!m_flightplanUuid.isEmpty() && syncPoint.elapsed() > 600) {
                            auto mandala = Vehicles::instance()->current()->f_mandala;
                            float altitude = mandala->valueByName("gps_hmsl").toFloat();
                            float latitude = mandala->valueByName("gps_lat").toFloat();
                            float longitude = mandala->valueByName("gps_lon").toFloat();
                            float gspeed = mandala->valueByName("gSpeed").toFloat();
                            uint ts = mandala->valueByName("dl_timestamp").toUInt();
                            tracker->write(altitude, latitude, longitude, gspeed, ts);
                            syncPoint.start();
                        }

                        if(channel->GetState(false) == GRPC_CHANNEL_SHUTDOWN)
                            break;
                        msleep(1);
                    }
                } else
                    std::cerr << "Can't create stub" << std::endl;
            }
        } else
            std::cerr << "Can't create channel" << std::endl;
        setIsConnected(false);
        msleep(1000);
    }
}

std::optional<int> AsyncClient::getTask()
{
    std::unique_lock locker(m_ioMutex);
    if(!m_tasks.empty()) {
        int task = m_tasks.front();
        m_tasks.pop();
        return task;
    } else
        return std::nullopt;
}

void AsyncClient::rpcRegisterFlightplan(const std::unique_ptr<MissionService::Stub> &service)
{
    grpc::ClientContext clientContext;
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(3000, GPR_TIMESPAN)));
    MissionRequest request;
    MissionResponse response;
    request.set_deviceuuid(m_deviceUuid.toStdString());
    request.set_flightrequestuuid(m_flightRequestUuid.toStdString());
    auto waypoints = Vehicles::instance()->current()->f_mission->f_waypoints;
    for(int i = 0; i < waypoints->size(); i++) {
        auto waypoint = dynamic_cast<Waypoint *>(waypoints->child(i));
        if(waypoint) {
            auto point = request.add_coordinates();
            point->set_altitude(waypoint->f_altitude->value().toFloat());
            point->set_latitude(waypoint->f_latitude->value().toFloat());
            point->set_longitude(waypoint->f_longitude->value().toFloat());
        }
    }

    auto status = service->create(&clientContext, request, &response);
    if(status.ok()) {
        setFlightplanUuid(QString::fromStdString(response.missionuuid()));
    } else {
        std::cerr << status.error_message() << std::endl;
        std::cerr << status.error_details() << std::endl;
    }
}
