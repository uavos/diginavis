#pragma once

#include "Fact/Fact.h"
#include "Vehicles/Vehicle.h"
#include "asyncclient.h"
#include "flightplan.h"
#include "readonlyfact.h"
#include "authorization.h"
#include "dronecreator.h"
#include "drones.h"
#include "flightrequests.h"
#include "flightrequestcreator.h"

class Diginavis: public Fact
{
    Q_OBJECT
public:
    Diginavis(Fact *parent = nullptr);
    virtual ~Diginavis();

    ReadOnlyFact *f_isConnected;
    ReadOnlyFact *f_status;
    ReadOnlyFact *f_lastSync;
    Flightplan *f_flightplan;
    DroneCreator *f_droneCreator;
    Authorization *f_authorization;
    Drones *f_drones;
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
