#include "flightrequests.h"

#include "App/App.h"
#include "flightrequestapproved.h"

FlightRequests::FlightRequests(Fact *parent):
    HttpApiBase(parent, "flight_requests", "Flight requests", "", Fact::Group, "airplane-takeoff")
{
    f_approved = new Fact(this, "approved", "Approved", "Ready to flight", Fact::Group, "check");
    f_pending = new Fact(this, "pending", "Pending", "Under consideration", Fact::Group, "progress-clock");
    f_declined = new Fact(this, "declined", "Declined", "Forbidden to flight", Fact::Group, "close");

    f_refresh = new Fact(this, "refresh", "Refresh", "", Fact::Action | Fact::Apply, "refresh");

    connect(parent, &FlightRequests::triggered, this, &FlightRequests::onTriggered);
    connect(f_refresh, &Fact::triggered, this, &FlightRequests::onTriggered);
}

void FlightRequests::onTriggered()
{
    auto request = makeRequest(URL);
    m_network.get(request);
}

void FlightRequests::onRequestFinished(QNetworkReply *reply)
{
    qDeleteAll(f_requests);
    f_requests.clear();

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    auto array = doc.object()["data"].toArray();
    for(auto flight: array) {
        auto flightplan = flight.toObject()["flightPlan"].toObject();
        QString uuid = flightplan["flightPlanUuid"].toString();
        QDateTime startTime = QDateTime::fromString(flightplan["scheduledDateTimeStart"].toString(), Qt::ISODate);
        QDateTime endTime = QDateTime::fromString(flightplan["scheduledDateTimeEnd"].toString(), Qt::ISODate);
        QString status = flight.toObject()["processingStatus"].toString();
        auto uav = flightplan["uavs"].toArray().first().toObject();
        QString model = uav["model"].toString();
        QString serialNumber = uav["serialNumber"].toString();
        QString uavUuid = uav["uavUuid"].toString();
        QString title = QString("%1 - %2")
                            .arg(startTime.toString("dd.MM.yyyy(hh:mm:ss)"))
                            .arg(endTime.toString("dd.MM.yyyy(hh:mm:ss)"));
        QString description = QString("%1(%2)").arg(model).arg(serialNumber);
        Fact *request = nullptr;
        if(status == "APPROVED") {
            auto approvedRequest = new FlightRequestApproved(f_approved, uuid, title, model, serialNumber,
                                                             startTime.toString("dd.MM.yyyy(hh:mm:ss)"),
                                                             endTime.toString("dd.MM.yyyy(hh:mm:ss)"), uavUuid);
            connect(approvedRequest, &FlightRequestApproved::workStarted, this, &FlightRequests::workStarted);
            request = approvedRequest;

        } else if(status == "DECLINED") {
            request = new Fact(f_declined, uuid, title, description, Fact::NoFlags);
        } else {
            request = new Fact(f_pending, uuid, title, description, Fact::NoFlags);
        }
        f_requests.append(request);
    }
    f_approved->setValue(QString("%1").arg(f_approved->facts().size()));
    f_pending->setValue(QString("%1").arg(f_pending->facts().size()));
    f_declined->setValue(QString("%1").arg(f_declined->facts().size()));
    App::jsync(this);

    setValue(array.size());
}