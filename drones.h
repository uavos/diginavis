#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "httpapibase.h"

class Drones: public HttpApiBase
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/v1/profile/my-uavs";
    Drones(Fact *parent = nullptr);

    QVector<Fact *> f_drones;

    void request();

private slots:
    void onTriggered();
    void onRequestFinished(QNetworkReply *reply) override;

signals:
    void dronesReceived();
};