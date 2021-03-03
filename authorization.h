#pragma once

#include <QTimer>
#include <QNetworkAccessManager>

#include "Fact/Fact.h"

class Authorization: public Fact
{
    Q_OBJECT
public:
    const QString URL = "https://auth.project-one.io/auth/realms/reg-asp/protocol/openid-connect/token";
    Authorization(Fact *parent = nullptr);

    QString getBearerToken() const;

private:
    QTimer m_timer;
    QString m_bearerToken;
    QNetworkAccessManager m_manager;

    void requestBearerToken();

private slots:
    void onRequestFinished(QNetworkReply *reply);
    void onTimerTimeout();

signals:
    void bearerTokenReceived(const QString &token);
};