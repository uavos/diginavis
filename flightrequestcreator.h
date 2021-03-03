#pragma once

#include "httpapibase.h"
#include "drones.h"
#include "datetimefact.h"

class FlightRequestCreator: public HttpApiBase
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/flight-requests";
    FlightRequestCreator(Fact *parent = nullptr);

    Fact *f_drones;
    DateTimeFact *f_startDateTime;
    DateTimeFact *f_endDateTime;
    Fact *f_createButton;
    Fact *f_createStatus;
    Fact *f_missionSize;

private:
    Drones m_drones;

private slots:
    void onTriggered();
    void onCreateTriggered();
    void onDronesReceived();
    void onMissionSizeChanged();
    void onRequestFinished(QNetworkReply *reply);
};