#pragma once

#include "Fact/Fact.h"

class FlightRequestApproved: public Fact
{
    Q_OBJECT
public:
    FlightRequestApproved(Fact *parent,
                          const QString &name,
                          const QString &title,
                          const QString &model,
                          const QString &serialNumber,
                          const QString &startTime,
                          const QString &endTime,
                          const QString &droneUuid);

    Fact *f_model;
    Fact *f_serialNumber;
    Fact *f_startTime;
    Fact *f_endTime;

    Fact *f_startWork;

private:
    QString m_requestUuid;
    QString m_droneUuid;

private slots:
    void onStartWorkTriggered();

signals:
    void workStarted(const QString &requestUuid, const QString &droneUuid);
};