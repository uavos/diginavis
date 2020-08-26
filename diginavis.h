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

    Fact *f_isConnected;
    ReadOnlyFact *f_status;
    Flightplan *f_flightplan;

private:
    std::shared_ptr<AsyncClient> m_client;
    Vehicle *m_vehicle = nullptr;

private slots:
    void onIsConnectedChanged();
    void onStatusChanged();
    void onCurrentVehicleChanged();
};

#endif // !DIGINAVIS_H