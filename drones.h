#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "Fact/Fact.h"

class Drones: public Fact
{
    Q_OBJECT
public:
    const QString URL = "https://project-one.io/api/v1/profile/my-uavs";
    Drones(Fact *parent = nullptr);
    void setBearerToken(const QString &token);
    
    QVector<Fact*> f_drones; 

private slots:
    void onTriggered();
    void onRequestFinished(QNetworkReply *reply);

private:
    QString m_bearerToken;
    QNetworkAccessManager m_network;
};