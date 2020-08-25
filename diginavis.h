#ifndef DIGINAVIS_H
#define DIGINAVIS_H

#include "Fact/Fact.h"
#include "asyncclient.h"
#include "flightplan.h"

class Diginavis: public Fact
{
    Q_OBJECT
public:
    Diginavis(Fact *parent = nullptr);
    virtual ~Diginavis();

    Fact *f_isConnected;
    Flightplan *f_flightplan;

private:
    std::shared_ptr<AsyncClient> m_client;

private slots:
    void onIsConnectedChanged();
};

#endif // !DIGINAVIS_H