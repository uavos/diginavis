#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "httpapibase.h"

class DroneCreator: public HttpApiBase
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/v1/uav-accountings";
    // const QString URL = "https://project-one.io/api/v1/uav-accountings/auto";
    DroneCreator(Fact *parent = nullptr);

    Fact *f_serialNumber;
    Fact *f_type;
    Fact *f_model;
    Fact *f_engineCount;
    Fact *f_engineType;
    Fact *f_maxTakeOffWeight;

    Fact *f_createButton;

    Fact *f_createStatus;

private slots:
    void onTriggered();
    void onCreateTriggered();
    void onRequestFinished(QNetworkReply *reply) override;
};