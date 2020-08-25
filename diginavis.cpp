#include "diginavis.h"

#include <grpc++/grpc++.h>
#include <QDebug>

#include "Mission.grpc.pb.h"

Diginavis::Diginavis(Fact *parent):
    Fact(parent, "diginavis", "Diginavis telemetry"),
    m_client(new AsyncClient(this))
{
    f_flightplan = new Flightplan(m_client, this);

    f_isConnected = new Fact(this, "isConnected", "No connection", "", Fact::NoFlags);
    f_isConnected->setActive(false);
    f_isConnected->setIcon("alert-circle-outline");

    m_client->setDeviceUuid("0f98f251-7bc5-4dcf-a8d1-836952304fc5");
    m_client->setFlighRequestUuid("393eea45-0094-49a9-b1ff-72ba560cd397");

    connect(m_client.get(), &AsyncClient::isConnectedChanged, this, &Diginavis::onIsConnectedChanged);

    m_client->start();
}

Diginavis::~Diginavis()
{
    m_client->stop();
    m_client->wait(5000);
}

void Diginavis::onIsConnectedChanged()
{
    if(m_client->isConnected()) {
        f_isConnected->setTitle("Connected to " + m_client->getHost());
        f_isConnected->setIcon("check");
    }
    else {
        f_isConnected->setTitle("No connection");
        f_isConnected->setIcon("alert-circle-outline");
    }
}