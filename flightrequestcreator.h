#pragma once

#include "httpapibase.h"
#include "drones.h"

class FlightRequestCreator: public HttpApiBase
{
    Q_OBJECT
public:
    FlightRequestCreator(Fact *parent = nullptr);

    Fact *f_drones;
    Fact *f_createButton;
    Fact *f_createStatus;

private:
    Drones m_drones;

private slots:
    void onTriggered();
    void onDronesReceived();
    void onRequestFinished(QNetworkReply *reply);
};