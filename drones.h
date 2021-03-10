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

    Fact *f_refresh;

    void request();

protected:
    void onJsonReceived(const QJsonDocument &doc) override;

private slots:
    void onTriggered();

signals:
    void dronesReceived();
};