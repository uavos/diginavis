#pragma once

#include "Fact/Fact.h"
#include "Vehicles/Vehicle.h"
#include "asyncclient.h"
#include "telemetry.h"
#include "authorization.h"
#include "dronecreator.h"
#include "drones.h"
#include "flightrequests.h"
#include "flightrequestcreator.h"
#include "datetimefact.h"

class Diginavis: public Fact
{
    Q_OBJECT
public:
    Diginavis(Fact *parent = nullptr);

    Authorization *f_authorization;

    Fact *f_myDrones;
    DroneCreator *f_droneCreator;
    Drones *f_drones;

    Fact *f_myRequests;
    FlightRequests *f_requests;
    FlightRequestCreator *f_requestCreator;

    Telemetry *f_telemetry;

private:
    Vehicle *m_vehicle = nullptr;

};
