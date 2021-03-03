#pragma once

#include <grpc++/grpc++.h>
#include <queue>
#include <QThread>
#include <mutex>
#include <QGeoCoordinate>
#include "Mission.grpc.pb.h"
#include "Tracking.grpc.pb.h"

using namespace avtm::center::proto;

class Tracker
{
public:
    Tracker(std::shared_ptr<grpc::Channel> channel);
    bool write(float altitude, float latitude, float longitude, float gspeed, uint64_t ts);
    void setMissionUuid(const QString &uuid);
    ~Tracker();

private:
    QString m_missionUuid;
    grpc::ClientContext m_clientContext;
    std::unique_ptr<TrackingService::Stub> m_stub;
    std::unique_ptr<grpc::ClientWriter<TrackingRequest>> m_writer;
    Empty m_empty;
};

class Registrator
{
public:
    Registrator(std::shared_ptr<grpc::Channel> channel);

    std::optional<QString> registerMission();
    void setDroneUuid(const QString &uuid);
    void setFlightRequestUuid(const QString &uuid);
    void updateMission(const QString &missionUuid);
    bool updateStatus(const QString &missionUuid, MissionStatus status);

private:
    QString m_droneUuid;
    QString m_flightRequestUuid;
    std::unique_ptr<MissionService::Stub> m_stub;
};

class AsyncClient: public QThread
{
    Q_OBJECT
public:
    static const int SYNC_INTERVAL = 600;
    static const int ALTITUDE_THRESHOLD = 3;
    enum Status {
        None,
        Registered,
        InMove,
        Landed
    };
    AsyncClient(QObject *parent = nullptr);
    void stop();
    QString getDroneUuid() const;
    void setDroneUuid(const QString &uuid);
    QString getFlightRequestUuid() const;
    void setFlightRequestUuid(const QString &uuid);
    bool isConnected() const;
    QString getHost() const;
    QString getMissionUuid() const;
    Status getStatus() const;

    void registerMission();
    void updateMission();

protected:
    void run() override;

private:
    enum {
        RegisterMission,
        UpdateMission
    };
    const QString m_host = "winavis.project-one.io:6565";
    QString m_droneUuid;
    QString m_flightRequestUuid;
    QString m_missionUuid;
    std::atomic_bool m_stop{false};
    std::atomic_bool m_isConnected{false};
    std::queue<int> m_tasks;
    Status m_status = None;
    mutable std::mutex m_ioMutex;

    void setIsConnected(bool b);
    void setMissionUuid(const QString &uuid);
    void setStatus(Status status);

    std::optional<int> getTask();

signals:
    void isConnectedChanged();
    void flightplanUuidChanged();
    void statusChanged();
    void trackerSynced();
};
