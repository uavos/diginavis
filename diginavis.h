#pragma once

#include "Fact/Fact.h"
#include "Vehicles/Vehicle.h"
#include "asyncclient.h"
#include "flightplan.h"
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
    virtual ~Diginavis();

    Fact *f_isConnected;
    Fact *f_status;
    Fact *f_lastSync;
    Flightplan *f_flightplan;
    Authorization *f_authorization;

    Fact *f_myDrones;
    DroneCreator *f_droneCreator;
    Drones *f_drones;

    Fact *f_myRequests;
    FlightRequests *f_requests;
    FlightRequestCreator *f_requestCreator;

private:
    std::shared_ptr<AsyncClient> m_client;
    Vehicle *m_vehicle = nullptr;

private slots:
    void onIsConnectedChanged();
    void onStatusChanged();
    void onCurrentVehicleChanged();
    void onTrackerSynced();
};
