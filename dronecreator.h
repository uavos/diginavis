#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "Fact/Fact.h"

class DroneCreator: public Fact
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/uav-accountings";
    DroneCreator(Fact *parent = nullptr);

    Fact *f_serialNumber;
    Fact *f_type;
    Fact *f_model;
    Fact *f_engineCount;
    Fact *f_engineType;
    Fact *f_maxTakeOffWeight;

    Fact *f_createButton;

    Fact *f_createStatus;

    void setBearerToken(const QString &token);

private slots:
    void onTriggered();
    void onCreateTriggered();
    void onRequestFinished(QNetworkReply *reply);

private:
    QString m_bearerToken;
    QNetworkAccessManager m_network;
};