#ifndef DIGINAVIS_H
#define DIGINAVIS_H

#include "Fact/Fact.h"
#include "asyncclient.h"

class Diginavis: public Fact
{
    Q_OBJECT
public:
    Diginavis(Fact *parent = nullptr);

    Fact *f_isConnected;

private:
    AsyncClient m_client;

private slots:
    void onIsConnectedChanged();
};

#endif // !DIGINAVIS_H