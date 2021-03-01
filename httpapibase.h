#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "Fact/Fact.h"

class HttpApiBase: public Fact
{
    Q_OBJECT
public:
    HttpApiBase(QObject *parent = nullptr,
                const QString &name = QString(),
                const QString &title = QString(),
                const QString &descr = QString(),
                FactBase::Flags flags = FactBase::Flags(NoFlags),
                const QString &icon = QString());

    void setBearerToken(const QString &token);
    QString getBearerToken() const;

protected:
    QNetworkAccessManager m_network;
    QNetworkRequest makeRequest(const QString &url, const QString &contentType = "application/json");

protected slots:
    virtual void onRequestFinished(QNetworkReply *reply) = 0;

private:
    QString m_bearerToken;
};