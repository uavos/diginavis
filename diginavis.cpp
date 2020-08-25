#include "diginavis.h"

#include <grpc++/grpc++.h>
#include <QDebug>

#include "Mission.grpc.pb.h"

Diginavis::Diginavis(Fact *parent):
    Fact(parent, "diginavis", "Diginavis telemetry"),
    m_client(this)
{
    f_isConnected = new Fact(this, "isConnected", "No connection", "", Fact::NoFlags);
    f_isConnected->setActive(false);
    f_isConnected->setIcon("alert-circle-outline");

    m_client.setDeviceUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));

    connect(&m_client, &AsyncClient::isConnectedChanged, this, &Diginavis::onIsConnectedChanged);

    m_client.start();
    // auto channel = grpc::CreateChannel("winavis.d1.project-one.io:6565", grpc::InsecureChannelCredentials());
    // qDebug() << channel->GetState(false);
    // auto stub = avtm::center::proto::MissionService::NewStub(channel);
    // grpc::ClientContext clientContext;
    // avtm::center::proto::MissionRequest request;
    // auto deviceuuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    // auto fruuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    // std::cout << deviceuuid << std::endl;
    // std::cout << fruuid << std::endl;
    // request.set_deviceuuid(deviceuuid);
    // request.set_flightrequestuuid(fruuid);

    // avtm::center::proto::MissionResponse response;
    // grpc::Status status = stub->create(&clientContext, request, &response);
    // if(status.ok()) {
    //     qDebug() << "OK";
    // } else {
    //     std::cout << status.error_message() << std::endl;
    //     std::cout << status.error_details() << std::endl;
    // }
    // qDebug() << QString::fromStdString(response.missionuuid());
    // qDebug() << "5";
}

void Diginavis::onIsConnectedChanged()
{
    if(m_client.isConnected()) {
        f_isConnected->setTitle("Connected to " + m_client.getHost());
        f_isConnected->setIcon("check");
    }
    else {
        f_isConnected->setTitle("No connection");
        f_isConnected->setIcon("alert-circle-outline");
    }
}