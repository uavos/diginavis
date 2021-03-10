#pragma once

#include "httpapibase.h"

class FlightRequests: public HttpApiBase
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/v1/flight-requests";
    FlightRequests(Fact *parent = nullptr);

    Fact *f_approved;
    Fact *f_declined;
    Fact *f_pending;

    Fact *f_refresh;

    QVector<Fact *> f_requests;

private slots:
    void onTriggered();
    void onJsonReceived(const QJsonDocument &doc) override;

signals:
    void workStarted(const QString &requestUuid, const QString &droneUuid);
};