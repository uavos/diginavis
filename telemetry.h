#pragma once

#include "Fact/Fact.h"
#include "asyncclient.h"

class Telemetry: public Fact
{
    Q_OBJECT
public:
    Telemetry(Fact *parent = nullptr);
    ~Telemetry();

    Fact *f_isConnected;
    Fact *f_status;
    Fact *f_lastSync;

    void setBearerToken(const QString &token);
    void startWork(const QString &requestUuid, const QString &droneUuid);

private slots:
    void onIsConnectedChanged();
    void onStatusChanged();
    void onTrackerSynced();

private:
    AsyncClient m_client;

    void updateValue();
};