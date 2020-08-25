#ifndef ASYNCCLIENT_H
#define ASYNCCLIENT_H

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
    Tracker(std::shared_ptr<grpc::Channel> channel, const QString &uuid);
    void write(float altitude, float latitude, float longitude, float gspeed, uint ts);
    ~Tracker();

private:
    QString m_uuid;
    grpc::ClientContext m_clientContext;
    std::unique_ptr<TrackingService::Stub> m_stub;
    std::unique_ptr<grpc::ClientWriter<TrackingRequest>> m_writer;
    Empty m_empty;
};

class AsyncClient: public QThread
{
    Q_OBJECT
public:
    AsyncClient(QObject *parent = nullptr);
    void stop();
    void setDeviceUuid(const QString &uuid);
    void setFlighRequestUuid(const QString &uuid);
    bool isConnected() const;
    QString getHost() const;
    QString getFlightplanUuid() const;

    void registerFlightplan();

protected:
    void run() override;

private:
    enum {
        RegisterFlighplan,
    };
    QString m_host = "winavis.d1.project-one.io:6565";
    QString m_deviceUuid;
    QString m_flightRequestUuid;
    QString m_flightplanUuid;
    std::atomic_bool m_stop{false};
    std::atomic_bool m_isConnected{false};
    std::queue<int> m_tasks;
    mutable std::mutex m_ioMutex;

    void setIsConnected(bool b);
    void setFlightplanUuid(const QString &uuid);

    std::optional<int> getTask();

    void rpcRegisterFlightplan(const std::unique_ptr<MissionService::Stub> &service);

signals:
    void isConnectedChanged();
    void flightplanUuidChanged();
};

#endif // !ASYNCCLIENT_H