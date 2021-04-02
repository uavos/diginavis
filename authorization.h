#pragma once

#include <QTimer>
#include <QNetworkAccessManager>

#include "Fact/Fact.h"

class Authorization: public Fact
{
    Q_OBJECT
public:
    const QString URL = "https://auth.project-one.io/auth/realms/reg-asp/protocol/openid-connect/token";
    const QString CLIENT_ID = "client_app_5b88c43f-6376-41b0-ab04-172ebaf0efa2";
    const QString CLIENT_SECRET = "3720d4d9-ff62-40e3-85bb-9e70bb8276f8";
    Authorization(Fact *parent = nullptr);

    QString getBearerToken() const;

private:
    QTimer m_timer;
    QTimer m_tokenExpireTimer;
    QString m_bearerToken;
    QNetworkAccessManager m_manager;

    void requestBearerToken();

private slots:
    void onRequestFinished(QNetworkReply *reply);
    void onTimerTimeout();

signals:
    void bearerTokenReceived(const QString &token);
};