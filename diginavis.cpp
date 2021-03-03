#include "diginavis.h"

#include <QDebug>

#include "Vehicles/Vehicles.h"
#include "Mission/VehicleMission.h"

Diginavis::Diginavis(Fact *parent):
    Fact(parent, "diginavis", "Diginavis"),
    m_vehicle(Vehicles::instance()->current())
{
    setIcon("cloud");

    f_authorization = new Authorization(this);

    f_myDrones = new Fact(this, "my_drones", "My drones", "", Fact::Group, "airplane");
    f_droneCreator = new DroneCreator(f_myDrones);
    f_drones = new Drones(f_myDrones);

    f_myRequests = new Fact(this, "my_requests", "My requests", "", Fact::Group, "airplane-takeoff");
    f_requestCreator = new FlightRequestCreator(f_myRequests);
    f_requests = new FlightRequests(f_myRequests);

    f_telemetry = new Telemetry(this);

    connect(f_authorization, &Authorization::bearerTokenReceived, f_droneCreator, &DroneCreator::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_drones, &Drones::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_requestCreator, &FlightRequestCreator::setBearerToken);
    connect(f_authorization, &Authorization::bearerTokenReceived, f_requests, &FlightRequests::setBearerToken);
    f_droneCreator->setBearerToken(f_authorization->getBearerToken());
    f_drones->setBearerToken(f_authorization->getBearerToken());
    f_requests->setBearerToken(f_authorization->getBearerToken());
    f_requestCreator->setBearerToken(f_authorization->getBearerToken());
}
