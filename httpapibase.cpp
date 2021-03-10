#include "httpapibase.h"

#include "App/AppLog.h"

HttpApiBase::HttpApiBase(QObject *parent,
                         const QString &name,
                         const QString &title,
                         const QString &descr,
                         FactBase::Flags flags,
                         const QString &icon):
    Fact(parent, name, title, descr, flags, icon)
{
    connect(&m_network, &QNetworkAccessManager::finished, this, &HttpApiBase::onRequestFinished);
}

void HttpApiBase::setBearerToken(const QString &token)
{
    m_bearerToken = token;
}

QString HttpApiBase::getBearerToken() const
{
    return m_bearerToken;
}

QNetworkRequest HttpApiBase::makeRequest(const QString &url, const QString &contentType)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearerToken).toUtf8());
    return request;
}

void HttpApiBase::onRequestFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    if(reply->error() == QNetworkReply::NoError) {
        if(!data.isEmpty()) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(data, &error);
            if(error.error == QJsonParseError::NoError) {
                onJsonReceived(doc);
            } else {
                apxMsgW() << "Diginavis: Can't parse json";
                apxMsgW() << QString::fromUtf8(data);
            }
        } else {
            apxMsgW() << "Diginavis: empty http reply";
        }
    } else {
        apxMsgW() << QString("Diginavis: http error %1").arg(reply->error());
        apxMsgW() << QString::fromUtf8(data);
    }
}