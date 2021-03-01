#include "httpapibase.h"

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