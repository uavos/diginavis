#include "flightrequestcreator.h"

#include <QGeoPolygon>
#include <QGeoRectangle>
#include <QGeoCoordinate>
#include "Vehicles/Vehicles.h"
#include "Vehicles/Vehicle.h"
#include "Mission/VehicleMission.h"
#include "Mission/Waypoint.h"
#include "App/AppLog.h"

FlightRequestCreator::FlightRequestCreator(Fact *parent):
    HttpApiBase(parent, "create_request", "Create", "", Fact::Group, "plus-circle-outline")
{
    f_drones = new Fact(this, "drones", "Drone", "Choose your drone", Fact::Enum, "airplane");
    f_startDateTime = new DateTimeFact(this, "start_date_time", "Start date time (UTC)", "", "airplane-takeoff", 5 * 60);
    f_endDateTime = new DateTimeFact(this, "end_date_time", "End date time (UTC)", "", "airplane-landing", 5 * 60 + 60 * 60 * 3);
    f_missionSize = new Fact(this, "mission_size", "Mission size", "", Fact::NoFlags, "earth");
    f_missionSize->setValue(0);

    f_createButton = new Fact(this, "create_button", "Create", "", Fact::Apply | Fact::Action);
    f_createStatus = new Fact(this, "create_status", "", "", Fact::NoFlags);

    auto mission = Vehicles::instance()->current()->f_mission;
    connect(this, &Fact::triggered, this, &FlightRequestCreator::onTriggered);
    connect(f_createButton, &Fact::triggered, this, &FlightRequestCreator::onCreateTriggered);
    connect(&m_drones, &Drones::dronesReceived, this, &FlightRequestCreator::onDronesReceived);
    connect(mission, &VehicleMission::missionSizeChanged, this, &FlightRequestCreator::onMissionSizeChanged);
}

void FlightRequestCreator::onRequestFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString flightRequetsUuid = doc.object()["data"].toString();
    if(flightRequetsUuid.isEmpty()) {
        apxMsgW() << "Diginavis: " << QString::fromUtf8(data);
        f_createStatus->setTitle("FAIL");
    } else
        f_createStatus->setTitle("Success. Please wait for moderator response.");
}

void FlightRequestCreator::onTriggered()
{
    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(true);
        else
            f->setVisible(false);
    f_createButton->setVisible(true);
    m_drones.setBearerToken(getBearerToken());
    m_drones.request();
}

void FlightRequestCreator::onCreateTriggered()
{
    QJsonObject root;
    root["flightDescription"] = "";
    QJsonObject flightplan;
    QJsonObject geometry1;
    geometry1["type"] = "Feature";
    QJsonObject geometry2;
    geometry2["type"] = "Polygon";
    QJsonArray coordinates;
    QJsonArray coordinates2;
    QGeoPolygon polygon;
    auto mission = Vehicles::instance()->current()->f_mission;
    auto waypoints = mission->f_waypoints;
    float maxAltitude = 0;
    for(int i = 0; i < waypoints->size(); i++) {
        auto waypoint = dynamic_cast<Waypoint *>(waypoints->child(i));
        if(waypoint) {
            auto altitude = waypoint->f_altitude->value().toFloat();
            if(altitude > maxAltitude)
                maxAltitude = altitude;
            polygon.addCoordinate(QGeoCoordinate(waypoint->f_longitude->value().toFloat(),
                                                 waypoint->f_latitude->value().toFloat()));
        }
    }
    auto geoRect = polygon.boundingGeoRectangle();
    coordinates2.append(QJsonArray({geoRect.topLeft().latitude(), geoRect.topLeft().longitude()}));
    coordinates2.append(QJsonArray({geoRect.topRight().latitude(), geoRect.topRight().longitude()}));
    coordinates2.append(QJsonArray({geoRect.bottomRight().latitude(), geoRect.bottomRight().longitude()}));
    coordinates2.append(QJsonArray({geoRect.bottomLeft().latitude(), geoRect.bottomLeft().longitude()}));
    coordinates2.append(QJsonArray({geoRect.topLeft().latitude(), geoRect.topLeft().longitude()}));
    coordinates.append(coordinates2);
    geometry2["coordinates"] = coordinates;
    geometry1["geometry"] = geometry2;
    flightplan["geometry"] = geometry1;
    flightplan["maxAltitudeAgl"] = maxAltitude;
    flightplan["zonalAreaCode"] = "A54618";
    auto startDateTime = f_startDateTime->getDateTime();
    auto endDateTime = f_endDateTime->getDateTime();
    flightplan["scheduledDateTimeStart"] = startDateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    flightplan["scheduledDateTimeEnd"] = endDateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    flightplan["reserveDateTimeStart"] = startDateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    flightplan["reserveDateTimeEnd"] = endDateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    flightplan["location"] = "Russia";
    flightplan["supervisorName"] = "Dispatcher";
    QJsonObject pilot;
    pilot["phoneNumber"] = "+79482647223";
    pilot["email"] = "galinvladaleks1995@mail.ru";
    pilot["firstName"] = "Владимир";
    pilot["secondName"] = "Галинский";
    pilot["patronymicName"] = "Александрович";
    flightplan["pilot"] = pilot;
    QJsonArray uavs;
    uavs.append(m_drones.f_drones[f_drones->value().toInt()]->property("uuid").toString());
    flightplan["uavs"] = uavs;
    root["flightPlan"] = flightplan;

    QJsonDocument doc(root);
    auto request = makeRequest(URL);
    m_network.post(request, doc.toJson());

    f_createStatus->setTitle("Please wait...");
    f_createButton->setVisible(false);
    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(false);
        else
            f->setVisible(true);
}

void FlightRequestCreator::onDronesReceived()
{
    QList<QString> enumStrings;
    for(auto f: m_drones.f_drones) {
        enumStrings.append(QString("%1 (%2)").arg(f->title(), f->descr()));
    }
    f_drones->setEnumStrings(enumStrings);
}

void FlightRequestCreator::onMissionSizeChanged()
{
    auto mission = Vehicles::instance()->current()->f_mission;
    f_missionSize->setValue(QString("%1").arg(mission->missionSize()));
}