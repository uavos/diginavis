#include "authorization.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

Authorization::Authorization(Fact *parent):
    Fact(parent, "authorization", "Authorization", "", Fact::Text)
{
    m_timer.setSingleShot(false);
    m_timer.setInterval(5000);
    connect(&m_manager, &QNetworkAccessManager::finished, this, &Authorization::onRequestFinished);
    connect(&m_timer, &QTimer::timeout, this, &Authorization::onTimerTimeout);
    requestBearerToken();
    m_timer.start();
    setValue("Not authorized");
}

QString Authorization::getBearerToken() const
{
    return m_bearerToken;
}

void Authorization::requestBearerToken()
{
    QNetworkRequest request(URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString body = "grant_type=client_credentials"
                   "&scope=openid"
                   "&client_id=client_app_5b88c43f-6376-41b0-ab04-172ebaf0efa2"
                   "&client_secret=3720d4d9-ff62-40e3-85bb-9e70bb8276f8";
    m_manager.post(request, body.toUtf8());
}

void Authorization::onRequestFinished(QNetworkReply *reply)
{
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    m_bearerToken = doc.object()["access_token"].toString();
    if(!m_bearerToken.isEmpty()) {
        setValue("OK");
        emit bearerTokenReceived(m_bearerToken);
    } else {
        setValue("Not authorized");
    }
}

void Authorization::onTimerTimeout()
{
    if(m_bearerToken.isEmpty()) {
        requestBearerToken();
    }
}
