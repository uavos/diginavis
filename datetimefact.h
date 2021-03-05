#pragma once

#include "Fact/Fact.h"

class DateTimeFact: public Fact
{
    Q_OBJECT
public:
    DateTimeFact(Fact *parent = nullptr,
                 const QString &name = QString(),
                 const QString &title = QString(),
                 const QString &descr = QString(),
                 const QString &icon = QString(),
                 qint64 autoAddSecs = 0);

    QDateTime getDateTime() const;
    void setDateTime(const QDateTime &dateTime);

    Fact *f_year;
    Fact *f_month;
    Fact *f_day;
    Fact *f_hour;
    Fact *f_minutes;

private:
    qint64 m_autoAddSecs = 0;

private slots:
    void onTriggered();
    void onChildValueChanged();
};