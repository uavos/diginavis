#include "asyncclient.h"

#include <chrono>
#include <QDebug>
#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"
#include "Mission/Waypoint.h"
#include "App/AppLog.h"

SecureService::SecureService(const QString &bearerToken):
    m_bearerToken(bearerToken)
{
}

QString SecureService::getBearerToken() const
{
    return m_bearerToken;
}

void SecureService::setBearerToken(const QString &bearerToken)
{
    m_bearerToken = bearerToken;
}

Tracker::Tracker(std::shared_ptr<grpc::Channel> channel, const QString &bearerToken):
    SecureService(bearerToken),
    m_stub(TrackingService::NewStub(channel))
{
    m_clientContext.AddMetadata("authorization", "Bearer " + getBearerToken().toStdString());
    m_clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(500, GPR_TIMESPAN)));
    m_writer = m_stub->receive(&m_clientContext, &m_empty);
}

bool Tracker::write(float altitude, float latitude, float longitude, float gspeed, uint64_t ts)
{
    auto msecs = std::chrono::milliseconds(ts);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(msecs);
    auto nsecs = std::chrono::nanoseconds(msecs) - std::chrono::nanoseconds(secs);

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
        apxMsgW() << "Diginavis: Can't write track";
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

Registrator::Registrator(std::shared_ptr<grpc::Channel> channel, const QString &bearerToken):
    SecureService(bearerToken),
    m_stub(MissionService::NewStub(channel))
{
}

std::optional<QString> Registrator::registerMission(const QString &droneUuid, const QString &flightRequestUuid)
{
    grpc::ClientContext clientContext;
    clientContext.AddMetadata("authorization", "Bearer " + getBearerToken().toStdString());
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(3000, GPR_TIMESPAN)));
    MissionRequest request;
    MissionResponse response;
    request.set_deviceuuid(droneUuid.toStdString());
    request.set_flightrequestuuid(flightRequestUuid.toStdString());
    auto waypoints = Vehicles::instance()->current()->f_mission->f_waypoints;
    if(!qgetenv("DIGINAVIS_DEBUG").isEmpty())
        std::cout << "WAYPOINTS" << std::endl;
    for(int i = 0; i < waypoints->size(); i++) {
        auto waypoint = dynamic_cast<Waypoint *>(waypoints->child(i));
        if(waypoint) {
            auto point = request.add_coordinates();
            point->set_altitude(waypoint->f_altitude->value().toFloat());
            point->set_latitude(waypoint->f_latitude->value().toFloat());
            point->set_longitude(waypoint->f_longitude->value().toFloat());
            if(!qgetenv("DIGINAVIS_DEBUG").isEmpty())
                std::cout << point->latitude() << " " << point->longitude() << " " << point->altitude() << std::endl;
        }
    }

    auto result = m_stub->create(&clientContext, request, &response);
    if(result.ok()) {
        return QString::fromStdString(response.missionuuid());
    } else {
        apxMsgW() << "Diginavis: " << QString::fromStdString(result.error_message());
        return std::nullopt;
    }
}

void Registrator::updateMission(const QString &missionUuid)
{
    grpc::ClientContext clientContext;
    clientContext.AddMetadata("authorization", "Bearer " + getBearerToken().toStdString());
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
        apxMsgW() << "Diginavis: " << QString::fromStdString(result.error_message());
    }
}

bool Registrator::updateStatus(const QString &missionUuid, MissionStatus status)
{
    grpc::ClientContext clientContext;
    clientContext.AddMetadata("authorization", "Bearer " + getBearerToken().toStdString());
    clientContext.set_deadline(gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(3000, GPR_TIMESPAN)));
    MissionUpdateStatusRequest request;
    Empty response;
    request.set_missionuuid(missionUuid.toStdString());
    request.set_status(status);

    auto result = m_stub->update(&clientContext, request, &response);
    if(!result.ok()) {
        apxMsgW() << "Diginavis: " << QString::fromStdString(result.error_message());
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

void AsyncClient::setBearerToken(const QString &token)
{
    std::unique_lock locker(m_ioMutex);
    m_bearerToken = token;
}

QString AsyncClient::getBearerToken() const
{
    std::unique_lock locker(m_ioMutex);
    return m_bearerToken;
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

QString AsyncClient::getDroneUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_droneUuid;
}

void AsyncClient::setDroneUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_droneUuid = uuid;
}

QString AsyncClient::getFlightRequestUuid() const
{
    std::unique_lock locker(m_ioMutex);
    return m_flightRequestUuid;
}

void AsyncClient::setFlightRequestUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_flightRequestUuid = uuid;
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

std::unique_ptr<Tracker> AsyncClient::createTracker(std::shared_ptr<grpc::Channel> channel) const
{
    auto tracker = std::make_unique<Tracker>(channel, getBearerToken());
    tracker->setMissionUuid(getMissionUuid());
    return tracker;
}

std::unique_ptr<Registrator> AsyncClient::createRegistrator(std::shared_ptr<grpc::Channel> channel) const
{
    return std::make_unique<Registrator>(channel, getBearerToken());
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
    std::vector<float> altitudeBuffer;
    while(!m_stop) {
        grpc::ChannelArguments args;
        args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 5000);
        args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);
        auto creds = grpc::SslCredentials(grpc::SslCredentialsOptions());
        // auto creds = grpc::InsecureChannelCredentials();
        auto channel = grpc::CreateCustomChannel(m_host.toStdString(), creds, args);
        if(channel) {
            QElapsedTimer syncPoint;
            syncPoint.start();
            auto deadline = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(2, GPR_TIMESPAN));
            if(channel->WaitForConnected(deadline)) {
                setIsConnected(true);
                auto tracker = createTracker(channel);
                auto registrator = createRegistrator(channel);

                while(!m_stop) {
                    const auto bearerToken = getBearerToken();
                    if(bearerToken != tracker->getBearerToken())
                        tracker = createTracker(channel);
                    if(bearerToken != registrator->getBearerToken())
                        registrator = createRegistrator(channel);

                    auto task = getTask();
                    if(task) {
                        if(task.value() == RegisterMission) {
                            auto result = registrator->registerMission(getDroneUuid(), getFlightRequestUuid());
                            if(result) {
                                setMissionUuid(result.value());
                                tracker->setMissionUuid(result.value());
                                setStatus(Registered);
                            }
                        } else if(task.value() == UpdateMission && !getMissionUuid().isEmpty()) {
                            registrator->updateMission(getMissionUuid());
                        }
                    }

                    if(getStatus() != None && syncPoint.elapsed() > SYNC_INTERVAL) {
                        auto mandala = Vehicles::instance()->current()->f_mandala;
                        float altitude = mandala->fact("est.pos.altitude")->value().toFloat();
                        float latitude = mandala->fact("est.pos.lat")->value().toFloat();
                        float longitude = mandala->fact("est.pos.lon")->value().toFloat();
                        float gspeed = mandala->fact("est.pos.speed")->value().toFloat();
                        uint64_t ts = QDateTime::currentDateTime().toMSecsSinceEpoch();
                        if(tracker->write(altitude, latitude, longitude, gspeed, ts)) {
                            emit trackerSynced();
                        }
                        syncPoint.start();

                        altitudeBuffer.push_back(altitude);
                        if(altitudeBuffer.size() > 5)
                            altitudeBuffer.erase(altitudeBuffer.begin());

                        bool greater = std::all_of(altitudeBuffer.begin(), altitudeBuffer.end(), [](float v) {
                            return v > ALTITUDE_THRESHOLD;
                        });
                        bool less = std::all_of(altitudeBuffer.begin(), altitudeBuffer.end(), [](float v) {
                            return v < ALTITUDE_THRESHOLD;
                        });

                        const auto status = getStatus();
                        if(status == Registered && greater) {
                            if(registrator->updateStatus(getMissionUuid(), IN_MOVE))
                                setStatus(InMove);
                        } else if(status == InMove && less) {
                            if(registrator->updateStatus(getMissionUuid(), LANDED))
                                setStatus(Landed);
                        }
                    }

                    deadline = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(2, GPR_TIMESPAN));
                    if(!channel->WaitForConnected(deadline)) {
                        apxMsgW() << "Diginavis: Lost connection";
                        break;
                    }
                    msleep(1);
                }
            } else {
                apxMsgW() << "Diginavis: Not connected";
            }
        } else
            apxMsgW() << "Diginavis: Can't create channel";
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
