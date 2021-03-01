#include "drones.h"

#include "App/App.h"

Drones::Drones(Fact *parent):
    Fact(parent, "drones", "My drones", "", Fact::Group, "airplane")
{
    connect(this, &Fact::triggered, this, &Drones::onTriggered);
    connect(&m_network, &QNetworkAccessManager::finished, this, &Drones::onRequestFinished);
}

void Drones::setBearerToken(const QString &token)
{
    m_bearerToken = token;
}

void Drones::onTriggered()
{
    QNetworkRequest request;
    request.setUrl(URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearerToken).toUtf8());
    m_network.get(request);
}

void Drones::onRequestFinished(QNetworkReply *reply)
{
    qDeleteAll(f_drones);
    f_drones.clear();
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    auto uavs = doc.object()["data"].toArray();
    for(auto uavref: uavs) {
        auto uav = uavref.toObject();
        const QString uuid = uav["uavUuid"].toString();
        const QString model = uav["model"].toString();
        const QString serial = uav["serialNumber"].toString();
        Fact *fact = new Fact(this, uuid, model, serial, Fact::NoFlags, "dots-horizontal");
        f_drones.append(fact);
    }
    App::instance()->jsync(this);
}