#ifndef DIGINAVIS_H
#define DIGINAVIS_H

#include "Fact/Fact.h"
#include "Vehicles/Vehicle.h"
#include "asyncclient.h"
#include "flightplan.h"
#include "readonlyfact.h"

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

private:
    std::shared_ptr<AsyncClient> m_client;
    Vehicle *m_vehicle = nullptr;

private slots:
    void onIsConnectedChanged();
    void onStatusChanged();
    void onCurrentVehicleChanged();
    void onTrackerSynced();
};

#endif // !DIGINAVIS_H