#include "flightplan.h"

Flightplan::Flightplan(const std::shared_ptr<AsyncClient> &client, Fact *parent):
    Fact(parent, "flightplan", "Flightplan", "", Fact::Group),
    m_client(client)
{
    setIcon("airplane");
    f_registration = new Fact(this, "registration", "Registration", "", Fact::Apply | Fact::Action);

    f_uuid = new ReadOnlyFact(this, "uuid", "UUID", "", Fact::Text);
    // f_uuid->setEnabled(false);

    connect(m_client.get(), &AsyncClient::flightplanUuidChanged, this, &Flightplan::onFlightplanUuidChanged);
    connect(f_registration, &Fact::triggered, m_client.get(), &AsyncClient::registerMission);
}

void Flightplan::onFlightplanUuidChanged()
{
    f_uuid->setValueForce(m_client->getMissionUuid());
}