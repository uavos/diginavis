#ifndef FLIGHTPLAN_H
#define FLIGHTPLAN_H

#include "Fact/Fact.h"
#include "flightplan.h"
#include "asyncclient.h"
#include "readonlyfact.h"

class Flightplan: public Fact
{
    Q_OBJECT
public:
    Flightplan(const std::shared_ptr<AsyncClient> &client, Fact *parent = nullptr);

    Fact *f_registration;
    Fact *f_droneUuid;
    Fact *f_country;
    ReadOnlyFact *f_uuid;

private:
    std::shared_ptr<AsyncClient> m_client;

private slots:
    void onFlightplanUuidChanged();
    void onDroneUuidValueChanged();
    void onCountryValueChanged();
};

#endif //FLIGHTPLAN_H