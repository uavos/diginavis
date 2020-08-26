#include "readonlyfact.h"

ReadOnlyFact::ReadOnlyFact(QObject *parent,
                           const QString &name,
                           const QString &title,
                           const QString &descr,
                           FactBase::Flags flags,
                           const QString &icon):
    Fact(parent, name, title, descr, flags, icon)
{
}

bool ReadOnlyFact::setValue(const QVariant &value)
{
    Q_UNUSED(value);
    return false;
}

void ReadOnlyFact::setValueForce(const QVariant &value)
{
    Fact::setValue(value);
}