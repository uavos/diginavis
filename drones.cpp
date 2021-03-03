#include "drones.h"

#include "App/App.h"

Drones::Drones(Fact *parent):
    HttpApiBase(parent, "drones", "Drones", "", Fact::Group, "airplane")
{
    f_refresh = new Fact(this, "refresh", "Refresh", "", Fact::Action | Fact::Apply, "refresh");

    connect(parent, &Fact::triggered, this, &Drones::onTriggered);
    connect(f_refresh, &Fact::triggered, this, &Drones::onTriggered);
}

void Drones::onTriggered()
{
    QNetworkRequest request = makeRequest(URL);
    m_network.get(request);
}

void Drones::request()
{
    QNetworkRequest request = makeRequest(URL);
    m_network.get(request);
}

void Drones::onRequestFinished(QNetworkReply *reply)
{
    qDeleteAll(f_drones);
    f_drones.clear();
    App::jsync(this);
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    auto uavs = doc.object()["data"].toArray();
    for(auto uavref: uavs) {
        auto uav = uavref.toObject();
        const QString uuid = uav["uavUuid"].toString();
        const QString model = uav["model"].toString();
        const QString serial = uav["serialNumber"].toString();
        Fact *fact = new Fact(this, uuid, model, serial, Fact::NoFlags, "dots-horizontal");
        fact->setProperty("uuid", uuid);
        f_drones.append(fact);
    }
    App::jsync(this);
    emit dronesReceived();
    setValue(uavs.size());
}