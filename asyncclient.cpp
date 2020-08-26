#include "asyncclient.h"

#include <chrono>
#include <QDebug>
#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"
#include "Mission/Waypoint.h"

Tracker::Tracker(std::shared_ptr<grpc::Channel> channel):
    m_stub(TrackingService::NewStub(channel)),
    m_writer(m_stub->receive(&m_clientContext, &m_empty))
{
}

bool Tracker::write(float altitude, float latitude, float longitude, float gspeed, uint ts)
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
    request.set_mission_uuid(m_missionUuid.toStdString());

    if(m_writer->Write(request)) {
        return true;
    } else {
        std::cerr << "Can't write track" << std::endl;
        return false;
    }
}

void Tracker::setMissionUuid(const QString &uuid)
{
    m_missionUuid = uuid;
}

Tracker::~Tracker()
{
    m_writer->WritesDone();
    m_writer->Finish();
}

Registrator::Registrator(std::shared_ptr<grpc::Channel> channel,
                         const QString &deviceUuid, const QString &flightRequestUuid):
    m_stub(MissionService::NewStub(channel)),
    m_deviceUuid(deviceUuid),
    m_flightRequestUuid(flightRequestUuid)
{
}

std::optional<QString> Registrator::registerMission()
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

    auto result = m_stub->create(&clientContext, request, &response);
    if(result.ok()) {
        return QString::fromStdString(response.missionuuid());
    } else {
        std::cerr << result.error_message() << std::endl;
        std::cerr << result.error_details() << std::endl;
        return std::nullopt;
    }
}

void Registrator::updateMission(const QString &missionUuid)
{
    grpc::ClientContext clientContext;
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(3000, GPR_TIMESPAN)));
    MissionUpdateStatusAndCoordinatesRequest request;
    Empty response;
    request.set_missionuuid(missionUuid.toStdString());
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

    auto result = m_stub->updateWayPoints(&clientContext, request, &response);
    if(!result.ok()) {
        std::cerr << result.error_message() << std::endl;
        std::cerr << result.error_details() << std::endl;
    }
}

bool Registrator::updateStatus(const QString &missionUuid, MissionStatus status)
{
    grpc::ClientContext clientContext;
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(3000, GPR_TIMESPAN)));
    MissionUpdateStatusRequest request;
    Empty response;
    request.set_missionuuid(missionUuid.toStdString());
    request.set_status(status);

    auto result = m_stub->update(&clientContext, request, &response);
    if(!result.ok()) {
        std::cerr << result.error_message() << std::endl;
        std::cerr << result.error_details() << std::endl;
    }

    return result.ok();
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

void AsyncClient::setMissionUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    if(m_missionUuid != uuid) {
        m_missionUuid = uuid;
        emit flightplanUuidChanged();
    }
}

QString AsyncClient::getDeviceUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_deviceUuid;
}

void AsyncClient::setDeviceUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_deviceUuid = uuid;
}

QString AsyncClient::getMissionRequestUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_missionRequestUuid;
}

void AsyncClient::setMissionRequestUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_missionRequestUuid = uuid;
}

QString AsyncClient::getHost() const
{
    std::unique_lock locker(m_ioMutex);
    return m_host;
}

QString AsyncClient::getMissionUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_missionUuid;
}

AsyncClient::Status AsyncClient::getStatus() const
{
    std::unique_lock locker(m_ioMutex);
    return m_status;
}

void AsyncClient::setStatus(Status status)
{
    std::unique_lock locker(m_ioMutex);
    if(m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void AsyncClient::registerMission()
{
    std::unique_lock locker(m_ioMutex);
    m_tasks.push(RegisterMission);
}

void AsyncClient::updateMission()
{
    std::unique_lock locker(m_ioMutex);
    m_tasks.push(UpdateMission);
}

void AsyncClient::run()
{
    m_stop = false;
    setIsConnected(false);
    setStatus(None);
    while(!m_stop) {
        auto channel = grpc::CreateChannel(m_host.toStdString(), grpc::InsecureChannelCredentials());
        if(channel) {
            std::vector<float> altitudeBuffer;
            QElapsedTimer syncPoint;
            auto deadline = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(2, GPR_TIMESPAN));
            if(channel->WaitForConnected(deadline)) {
                setIsConnected(true);
                auto tracker = std::make_unique<Tracker>(channel);
                auto registrator = std::make_unique<Registrator>(channel, getDeviceUuid(), getMissionRequestUuid());

                while(!m_stop) {
                    auto task = getTask();
                    if(task) {
                        if(task.value() == RegisterMission) {
                            auto result = registrator->registerMission();
                            if(result) {
                                setMissionUuid(result.value());
                                syncPoint.start();
                                tracker->setMissionUuid(m_missionRequestUuid);
                                setStatus(Registered);
                            }
                        } else if(task.value() == UpdateMission && !getMissionUuid().isEmpty()) {
                            registrator->updateMission(getMissionUuid());
                        }
                    }

                    if(getStatus() != None && syncPoint.elapsed() > SYNC_INTERVAL) {
                        auto mandala = Vehicles::instance()->current()->f_mandala;
                        float altitude = mandala->valueByName("altitude").toFloat();
                        float gpsaltitude = mandala->valueByName("gps_hmsl").toFloat();
                        float latitude = mandala->valueByName("gps_lat").toFloat();
                        float longitude = mandala->valueByName("gps_lon").toFloat();
                        float gspeed = mandala->valueByName("gSpeed").toFloat();
                        uint ts = mandala->valueByName("dl_timestamp").toUInt();
                        if(tracker->write(gpsaltitude, latitude, longitude, gspeed, ts)) {
                            emit trackerSynced();
                            syncPoint.start();
                        }

                        altitudeBuffer.push_back(altitude);
                        if(altitudeBuffer.size() > 5)
                            altitudeBuffer.erase(altitudeBuffer.begin());

                        bool greater = std::all_of(altitudeBuffer.begin(), altitudeBuffer.end(), [](float v) {
                            return v > ALTITUDE_THRESHOLD;
                        });
                        bool less = std::all_of(altitudeBuffer.begin(), altitudeBuffer.end(), [](float v) {
                            return v < ALTITUDE_THRESHOLD;
                        });

                        if(getStatus() == Registered && greater) {
                            if(registrator->updateStatus(getMissionUuid(), IN_MOVE))
                                setStatus(InMove);
                        } else if(getStatus() == InMove && less) {
                            if(registrator->updateStatus(getMissionUuid(), LANDED))
                                setStatus(Landed);
                        }
                    }

                    if(channel->GetState(false) == GRPC_CHANNEL_SHUTDOWN) {
                        std::cerr << "Lost connection" << std::endl;
                        break;
                    }
                    msleep(1);
                }
            }
        } else
            std::cerr << "Can't create channel" << std::endl;
        setIsConnected(false);
        setStatus(None);
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
