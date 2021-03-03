#include "diginavis.h"

#include <QDebug>

#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"

Diginavis::Diginavis(Fact *parent):
    Fact(parent, "diginavis", "Diginavis"),
    m_client(new AsyncClient(this)),
    m_vehicle(Vehicles::instance()->current())
{
    setIcon("cloud");
    f_flightplan = new Flightplan(m_client, this);

    f_authorization = new Authorization(this);

    f_drones = new Drones(this);
    f_droneCreator = new DroneCreator(f_drones);

    f_requests = new FlightRequests(this);
    f_requestCreator = new FlightRequestCreator(f_requests);

    f_status = new Fact(this, "status", "Status", "", Fact::Text);
    f_status->setIcon("format-list-bulleted");
    f_status->setValue("None");

    f_isConnected = new Fact(this, "isConnected", "Connection", "", Fact::Text);
    f_isConnected->setIcon("alert-circle-outline");
    f_isConnected->setValue("No");

    f_lastSync = new Fact(this, "last_sync", "Last sync", "", Fact::Text);
    f_lastSync->setIcon("sync");
    f_lastSync->setValue("N/A");

    connect(f_authorization, &Authorization::bearerTokenReceived, f_droneCreator, &DroneCreator::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_drones, &Drones::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_requestCreator, &FlightRequestCreator::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_requests, &FlightRequests::setBearerToken);
    f_droneCreator->setBearerToken(f_authorization->getBearerToken());
    f_drones->setBearerToken(f_authorization->getBearerToken());
    f_requests->setBearerToken(f_authorization->getBearerToken());
    f_requestCreator->setBearerToken(f_authorization->getBearerToken());

    connect(m_client.get(), &AsyncClient::isConnectedChanged, this, &Diginavis::onIsConnectedChanged);
    connect(m_client.get(), &AsyncClient::statusChanged, this, &Diginavis::onStatusChanged);
    connect(m_client.get(), &AsyncClient::trackerSynced, this, &Diginavis::onTrackerSynced);

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
        f_isConnected->setValue("Yes");
        f_isConnected->setIcon("check");
    } else {
        f_isConnected->setValue("No");
        f_isConnected->setIcon("alert-circle-outline");
    }
}

void Diginavis::onStatusChanged()
{
    auto status = m_client->getStatus();
    if(status == AsyncClient::None)
        f_status->setValue("None");
    else if(status == AsyncClient::Registered)
        f_status->setValue("Registered");
    else if(status == AsyncClient::InMove)
        f_status->setValue("In move");
    else if(status == AsyncClient::Landed)
        f_status->setValue("Landed");
    else
        f_status->setValue("Unknown");
}

void Diginavis::onCurrentVehicleChanged()
{
    disconnect(m_vehicle->f_mission->f_upload, &Fact::triggered, m_client.get(), &AsyncClient::updateMission);
    m_vehicle = Vehicles::instance()->current();
    connect(m_vehicle->f_mission->f_upload, &Fact::triggered, m_client.get(), &AsyncClient::updateMission);
}

void Diginavis::onTrackerSynced()
{
    QString str = QTime::currentTime().toString("hh-mm-ss:zzz");
    f_lastSync->setValue(str);
}