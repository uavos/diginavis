#include "authorization.h"

#include <QNetworkRequest>
#include <QNetworkReply>

Authorization::Authorization(Fact *parent):
    Fact(parent, "authorization", "Authorization", "", Fact::NoFlags, "login")
{
    m_timer.setSingleShot(false);
    m_timer.setInterval(5000);
    m_tokenExpireTimer.setSingleShot(true);
    connect(&m_manager, &QNetworkAccessManager::finished, this, &Authorization::onRequestFinished);
    connect(&m_timer, &QTimer::timeout, this, &Authorization::onTimerTimeout);
    connect(&m_tokenExpireTimer, &QTimer::timeout, this, &Authorization::requestBearerToken);
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
    QString body = QString("grant_type=client_credentials"
                           "&scope=openid"
                           "&client_id=%1"
                           "&client_secret=%2")
                       .arg(CLIENT_ID)
                       .arg(CLIENT_SECRET);
    m_manager.post(request, body.toUtf8());
}

void Authorization::onRequestFinished(QNetworkReply *reply)
{
    auto doc = QJsonDocument::fromJson(reply->readAll());
    auto root = doc.object();
    int expiresSecs = root["expires_in"].toInt();
    m_bearerToken = root["access_token"].toString();
    if(!m_bearerToken.isEmpty()) {
        setValue("OK");
        m_tokenExpireTimer.start(std::clamp(expiresSecs, 10, expiresSecs - 10) * 1000);
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
