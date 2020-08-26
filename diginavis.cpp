#include "diginavis.h"

#include <grpc++/grpc++.h>
#include <QDebug>

#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"

#include "Mission.grpc.pb.h"

Diginavis::Diginavis(Fact *parent):
    Fact(parent, "diginavis", "Diginavis"),
    m_client(new AsyncClient(this)),
    m_vehicle(Vehicles::instance()->current())
{
    setIcon("cloud");
    f_flightplan = new Flightplan(m_client, this);

    f_status = new ReadOnlyFact(this, "status", "Status", "", Fact::Text);
    f_status->setIcon("state-machine");
    f_status->setValue("None");

    f_isConnected = new Fact(this, "isConnected", "No connection", "", Fact::NoFlags);
    f_isConnected->setActive(false);
    f_isConnected->setIcon("alert-circle-outline");

    m_client->setDeviceUuid("0f98f251-7bc5-4dcf-a8d1-836952304fc5");
    m_client->setMissionRequestUuid("393eea45-0094-49a9-b1ff-72ba560cd397");

    connect(m_client.get(), &AsyncClient::isConnectedChanged, this, &Diginavis::onIsConnectedChanged);
    connect(m_client.get(), &AsyncClient::statusChanged, this, &Diginavis::onStatusChanged);

    m_client->start();

    connect(Vehicles::instance(), &Vehicles::currentChanged, this, &Diginavis::onCurrentVehicleChanged);
    onCurrentVehicleChanged();
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
    } else {
        f_isConnected->setTitle("No connection");
        f_isConnected->setIcon("alert-circle-outline");
    }
}

void Diginavis::onStatusChanged()
{
    auto status = m_client->getStatus();
    if(status == AsyncClient::None)
        f_status->setValueForce("None");
    else if(status == AsyncClient::Registered)
        f_status->setValueForce("Registered");
    else if(status == AsyncClient::InMove)
        f_status->setValueForce("In move");
    else if(status == AsyncClient::Landed)
        f_status->setValueForce("Landed");
    else
        f_status->setValueForce("Unknown");
}

void Diginavis::onCurrentVehicleChanged()
{
    disconnect(m_vehicle->f_mission->f_upload, &Fact::triggered, m_client.get(), &AsyncClient::updateMission);
    m_vehicle = Vehicles::instance()->current();
    connect(m_vehicle->f_mission->f_upload, &Fact::triggered, m_client.get(), &AsyncClient::updateMission);
}