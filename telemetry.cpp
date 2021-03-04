#include "telemetry.h"

Telemetry::Telemetry(Fact *parent):
    Fact(parent, "telemetry", "Telemetry", "", Fact::Group, "cloud-upload-outline")
{
    f_status = new Fact(this, "status", "Status", "", Fact::NoFlags);
    f_status->setIcon("format-list-bulleted");
    f_status->setValue("None");

    f_isConnected = new Fact(this, "isConnected", "Connection", "", Fact::NoFlags);
    f_isConnected->setIcon("alert-circle-outline");
    f_isConnected->setValue("No");

    f_lastSync = new Fact(this, "last_sync", "Last sync", "", Fact::NoFlags);
    f_lastSync->setIcon("sync");
    f_lastSync->setValue("N/A");

    connect(&m_client, &AsyncClient::isConnectedChanged, this, &Telemetry::onIsConnectedChanged);
    connect(&m_client, &AsyncClient::statusChanged, this, &Telemetry::onStatusChanged);
    connect(&m_client, &AsyncClient::trackerSynced, this, &Telemetry::onTrackerSynced);
}

void Telemetry::setBearerToken(const QString &token)
{
    if(!token.isEmpty()) {
        m_client.setBearerToken(token);
        if(!m_client.isRunning())
            m_client.start();
    }
}

Telemetry::~Telemetry()
{
    m_client.stop();
    m_client.wait(5000);
}

void Telemetry::onIsConnectedChanged()
{
    if(m_client.isConnected()) {
        f_isConnected->setValue("Yes");
        f_isConnected->setIcon("check");
    } else {
        f_isConnected->setValue("No");
        f_isConnected->setIcon("alert-circle-outline");
    }
    updateValue();
}

void Telemetry::onStatusChanged()
{
    auto status = m_client.getStatus();
    if(status == AsyncClient::None)
        f_status->setValue("None");
    else if(status == AsyncClient::Registered)
        f_status->setValue("Registered");
    else if(status == AsyncClient::InMove)
        f_status->setValue("In move");
    else if(status == AsyncClient::Landed)
        f_status->setValue("Landed");
    else
        f_status->setValue("Unknown");
    updateValue();
}

void Telemetry::onTrackerSynced()
{
    QString str = QTime::currentTime().toString("hh-mm-ss:zzz");
    f_lastSync->setValue(str);
}

void Telemetry::updateValue()
{
    QString value;
    if(m_client.isConnected())
        value = "Connected, " + f_status->value().toString();
    else
        value = "Disconnected";
    setValue(value);
}

void Telemetry::startWork(const QString &requestUuid, const QString &droneUuid)
{
    m_client.setFlightRequestUuid(requestUuid);
    m_client.setDroneUuid(droneUuid);
    m_client.registerMission();
}