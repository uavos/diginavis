#include "datetimefact.h"

DateTimeFact::DateTimeFact(Fact *parent,
                           const QString &name,
                           const QString &title,
                           const QString &descr,
                           const QString &icon,
                           qint64 autoAddSecs):
    Fact(parent, name, title, descr, Fact::Group, icon),
    m_autoAddSecs(autoAddSecs)
{
    f_year = new Fact(this, "year", "Year", "", Fact::Int);
    f_year->setMin(QDateTime::currentDateTime().toUTC().date().year());

    f_month = new Fact(this, "month", "Month", "", Fact::Int);
    f_month->setMin(QDateTime::currentDateTime().toUTC().date().month());

    f_day = new Fact(this, "day", "Day", "", Fact::Int);
    f_day->setMin(QDateTime::currentDateTime().toUTC().date().day());

    f_hour = new Fact(this, "hour", "Hour", "", Fact::Int);
    f_hour->setMin(0);
    f_hour->setMax(23);

    f_minutes = new Fact(this, "minutes", "Minutes", "", Fact::Int);
    f_minutes->setMin(0);
    f_minutes->setMax(59);

    connect(parent, &Fact::triggered, this, &DateTimeFact::onTriggered);
    for(auto f: facts())
        connect(f, &Fact::valueChanged, this, &DateTimeFact::onChildValueChanged);
}

QDateTime DateTimeFact::getDateTime() const
{
    QDateTime result(QDate(f_year->value().toInt(), f_month->value().toInt(), f_day->value().toInt()),
                     QTime(f_hour->value().toInt(), f_minutes->value().toInt()), Qt::UTC);
    return result;
}

void DateTimeFact::setDateTime(const QDateTime &dateTime)
{
    f_year->setValue(dateTime.date().year());
    f_month->setValue(dateTime.date().month());
    f_day->setValue(dateTime.date().day());
    f_hour->setValue(dateTime.time().hour());
    f_minutes->setValue(dateTime.time().minute());
    setValue(dateTime.toString("dd.MM.yyyy-hh:mm:ss"));
}

void DateTimeFact::onTriggered()
{
    QDateTime next = QDateTime::currentDateTime().toUTC().addSecs(m_autoAddSecs);
    setDateTime(next);
}

void DateTimeFact::onChildValueChanged()
{
    setValue(getDateTime().toString("dd.MM.yyyy-hh:mm:ss"));
}