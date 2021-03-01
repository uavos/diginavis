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

    QVector<Fact*> f_requests;

private slots:
    void onTriggered();
    void onRequestFinished(QNetworkReply *reply) override;
};