#include "flightrequestcreator.h"

// "maxAltitudeAgl": 82,
//         "zonalAreaCode": "A54618",
//         "scheduledDateTimeStart": "2021-02-13T13:22:15.303Z",
//         "scheduledDateTimeEnd": "2021-02-15T13:22:15.303Z",
//         "reserveDateTimeStart": "2021-02-13T13:22:15.303Z",
//         "reserveDateTimeEnd": "2021-02-15T13:22:15.303Z",
//         "location": "Russia",
//         "supervisorName": "Dispatcher",

FlightRequestCreator::FlightRequestCreator(Fact *parent):
    HttpApiBase(parent, "create_request", "Create", "", Fact::Group, "plus-circle-outline")
{
    f_drones = new Fact(this, "drones", "Drone", "Choose your drone", Fact::Enum, "airplane");

    f_createButton = new Fact(this, "create_button", "Create", "", Fact::Apply | Fact::Action);
    f_createStatus = new Fact(this, "create_status", "", "", Fact::NoFlags);

    connect(this, &Fact::triggered, this, &FlightRequestCreator::onTriggered);
    connect(&m_drones, &Drones::dronesReceived, this, &FlightRequestCreator::onDronesReceived);
}

void FlightRequestCreator::onRequestFinished(QNetworkReply *reply)
{

}

void FlightRequestCreator::onTriggered()
{
    f_createStatus->setVisible(false);
    setTitle("Choose your drone");
    m_drones.setBearerToken(getBearerToken());
    m_drones.request();
}

void FlightRequestCreator::onDronesReceived()
{
    QList<QString> enumStrings;
    for(auto f: m_drones.f_drones) {
        enumStrings.append(QString("%1 (%2)").arg(f->title(), f->descr()));
    }
    f_drones->setEnumStrings(enumStrings);
}