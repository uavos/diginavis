#include "flightrequestapproved.h"

FlightRequestApproved::FlightRequestApproved(Fact *parent,
                                             const QString &name,
                                             const QString &title,
                                             const QString &model,
                                             const QString &serialNumber,
                                             const QString &startTime,
                                             const QString &endTime,
                                             const QString &droneUuid):
    Fact(parent, name, title, QString("%1 (%2)").arg(model).arg(serialNumber), Fact::Group),
    m_requestUuid(name),
    m_droneUuid(droneUuid)
{
    f_model = new Fact(this, "model", "Model", "", Fact::NoFlags);
    f_model->setValue(model);
    f_serialNumber = new Fact(this, "serial_number", "Serial number", "", Fact::NoFlags);
    f_serialNumber->setValue(serialNumber);
    f_startTime = new Fact(this, "start_time", "Start time", "", Fact::NoFlags);
    f_startTime->setValue(startTime);
    f_endTime = new Fact(this, "end_time", "End time", "", Fact::NoFlags);
    f_endTime->setValue(endTime);
    f_startWork = new Fact(this, "start_work", "Start work", "", Fact::Action | Fact::Apply | Fact::CloseOnTrigger, "");

    connect(f_startWork, &Fact::triggered, this, &FlightRequestApproved::onStartWorkTriggered);
}

void FlightRequestApproved::onStartWorkTriggered()
{
    emit workStarted(m_requestUuid, m_droneUuid);
}