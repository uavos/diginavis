#include "flightrequests.h"

#include "App/App.h"

FlightRequests::FlightRequests(Fact *parent):
    HttpApiBase(parent, "flight_requests", "My flight requests", "", Fact::Group, "airplane-takeoff")
{
    connect(this, &FlightRequests::triggered, this, &FlightRequests::onTriggered);

    f_approved = new Fact(this, "approved", "Approved", "Ready to flight", Fact::Group, "check");
    f_pending = new Fact(this, "pending", "Pending", "Under consideration", Fact::Group, "progress-clock");
    f_declined = new Fact(this, "declined", "Declined", "Forbidden to flight", Fact::Group, "close");
}

void FlightRequests::onTriggered()
{
    auto request = makeRequest(URL);
    m_network.get(request);
}

void FlightRequests::onRequestFinished(QNetworkReply *reply)
{
    qDeleteAll(f_requests);

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    auto array = doc.object()["data"].toArray();
    for(auto flight: array) {
        auto flightplan = flight.toObject()["flightPlan"].toObject();
        QString uuid = flightplan["flightPlanUuid"].toString();
        QDateTime startTime = QDateTime::fromString(flightplan["scheduledDateTimeStart"].toString(), Qt::ISODate);
        QDateTime endTime = QDateTime::fromString(flightplan["scheduledDateTimeEnd"].toString(), Qt::ISODate);
        qDebug() << startTime.isNull() << startTime.isValid();
        QString status = flight.toObject()["processingStatus"].toString();
        QString model = flightplan["uavs"].toArray().first().toObject()["model"].toString();
        QString serialNumber = flightplan["uavs"].toArray().first().toObject()["serialNumber"].toString();
        Fact *parent = nullptr;
        if(status == "APPROVED")
            parent = f_approved;
        else if(status == "DECLINED")
            parent = f_declined;
        else
            parent = f_pending;
        QString title = QString("%1 - %2")
                            .arg(startTime.toString("dd.MM.yyyy(hh:mm:ss)"))
                            .arg(endTime.toString("dd.MM.yyyy(hh:mm:ss)"));
        QString description = QString("%1(%2)").arg(model).arg(serialNumber);
        Fact *request = new Fact(parent, uuid, title, description, Fact::NoFlags);
        f_requests.append(request);
    }
    f_approved->setValue(QString("%1").arg(f_approved->facts().size()));
    f_pending->setValue(QString("%1").arg(f_pending->facts().size()));
    f_declined->setValue(QString("%1").arg(f_declined->facts().size()));
    App::jsync(this);
}