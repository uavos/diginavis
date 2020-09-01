#include "flightplan.h"

#include "App/AppLog.h"

Flightplan::Flightplan(const std::shared_ptr<AsyncClient> &client, Fact *parent):
    Fact(parent, "flightplan", "Flightplan", "", Fact::Group),
    m_client(client)
{
    setIcon("airplane");
    f_registration = new Fact(this, "registration", "Registration", "", Fact::Apply | Fact::Action);

    //1546a666-6a07-4f16-9b5f-7f97747c1490
    f_droneUuid = new Fact(this, "drone_uuid", "Drone UUID", "", Fact::Text);
    f_droneUuid->setIcon("airplane");
    f_country = new Fact(this, "country", "Country", "", Fact::Enum);
    f_country->setEnumStrings({"Russia", "Austria", "Belarus"});
    f_country->setIcon("earth");
    f_uuid = new ReadOnlyFact(this, "uuid", "UUID", "", Fact::Text);

    connect(m_client.get(), &AsyncClient::flightplanUuidChanged, this, &Flightplan::onFlightplanUuidChanged);
    connect(f_registration, &Fact::triggered, m_client.get(), &AsyncClient::registerMission);
    connect(f_droneUuid, &Fact::valueChanged, this, &Flightplan::onDroneUuidValueChanged);
    connect(f_country, &Fact::valueChanged, this, &Flightplan::onCountryValueChanged);

    onCountryValueChanged();
}

void Flightplan::onFlightplanUuidChanged()
{
    f_uuid->setValueForce(m_client->getMissionUuid());
}

void Flightplan::onDroneUuidValueChanged()
{
    m_client->setDeviceUuid(f_droneUuid->value().toString());
}

void Flightplan::onCountryValueChanged()
{
    QMap<QString, QString> uuids = {{"Russia", "56907fdf-6d71-4ee4-a515-42053ef4e6df"},
                                    {"Austria", "d4088cf0-1cf2-478d-991c-4140c5a3587f"},
                                    {"Belarus", "0c63f64a-c774-4451-9ed6-62c201ae27d0"}};
    QString country = f_country->enumStrings()[f_country->value().toInt()];
    QString uuid = uuids.value(country);
    if(uuid.isEmpty())
        apxMsgW() << "Diginavis: Unknown country: " << country;
    m_client->setMissionRequestUuid(uuid);
}