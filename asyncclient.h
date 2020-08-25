#ifndef ASYNCCLIENT_H
#define ASYNCCLIENT_H

#include <QThread>
#include <mutex>
#include <QGeoCoordinate>
#include "Vehicles/Vehicles.h"

class AsyncClient: public QThread
{
    Q_OBJECT
public:
    AsyncClient(QObject *parent = nullptr);
    void stop();
    void setDeviceUuid(const QString &uuid);
    bool isConnected() const;
    QString getHost() const;

protected:
    void run() override;

private:
    QString m_host = "winavis.d1.project-one.io:6565";
    QString m_deviceUuid;
    std::atomic_bool m_stop{false};
    std::atomic_bool m_isConnected{false};
    mutable std::mutex m_ioMutex;

    void setIsConnected(bool b);

signals:
    void isConnectedChanged();
};

#endif // !ASYNCCLIENT_H