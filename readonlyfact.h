#ifndef READONLYFACT_H
#define READONLYFACT_H

#include "Fact/Fact.h"

class ReadOnlyFact: public Fact
{
    Q_OBJECT
public:
    ReadOnlyFact(QObject *parent = nullptr,
                 const QString &name = QString(),
                 const QString &title = QString(),
                 const QString &descr = QString(),
                 FactBase::Flags flags = FactBase::Flags(NoFlags),
                 const QString &icon = QString());

    Q_INVOKABLE bool setValue(const QVariant &value) override;

    void setValueForce(const QVariant &value);
};

#endif //READONLYFACT_H