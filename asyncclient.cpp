#include "asyncclient.h"

#include <grpc++/grpc++.h>
#include <QDebug>

#include "Mission.grpc.pb.h"

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

void AsyncClient::setDeviceUuid(const QString &uuid)
{
    std::unique_lock locker(m_ioMutex);
    m_deviceUuid = uuid;
}

QString AsyncClient::getHost() const
{
    std::unique_lock locker(m_ioMutex);
    return m_host;
}

void AsyncClient::run()
{
    m_stop = false;
    setIsConnected(false);
    while(!m_stop) {
        auto channel = grpc::CreateChannel(m_host.toStdString(), grpc::InsecureChannelCredentials());
        if(channel) {
            auto deadline = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(2, GPR_TIMESPAN));
            if(channel->WaitForConnected(deadline)) {
                setIsConnected(true);
                auto missionStub = avtm::center::proto::MissionService::NewStub(channel);
                if(missionStub) {
                    grpc::ClientContext clientContext;
                    while(!m_stop) {
                        std::cout << channel->GetState(false) << std::endl;
                        if(channel->GetState(false) == GRPC_CHANNEL_SHUTDOWN)
                            break;
                        msleep(100);
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