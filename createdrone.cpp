#include "createdrone.h"

Drones::Drones(Fact *parent):
    Fact(parent, "drones", "Drones", "", FactBase::Section)
{
    f_serialNumber = new Fact(this, "serial_number", "Serial number", "", Fact::Text);
    f_type = new Fact(this, "type", "Type", "", Fact::Text);
    f_model = new Fact(this, "model", "Model", "", Fact::Text);
    f_engineCount = new Fact(this, "engine_count", "Engine count", "", Fact::Int);
    f_engineCount->setMin(0);
    f_engineType = new Fact(this, "engine_type", "Engine type", "", Fact::Enum);
    f_engineType->setEnumStrings({"Gasoline", "Electric"});
    f_maxTakeOffWeight = new Fact(this, "max_takeoff_weight", "Max takeoff weight", "", Fact::Int);
    f_maxTakeOffWeight->setMin(0);

    f_createButton = new Fact(this, "create", "Create", "", Fact::Apply | Fact::Action);

    f_createStatus = new Fact(this, "create_result", "Result", "", Fact::Text);
    f_createStatus->setVisible(false);

    connect(&m_network, &QNetworkAccessManager::finished, this, &Drones::onRequestFinished);
    connect(f_createButton, &Fact::triggered, this, &Drones::onCreateTriggered);
    connect(this, &Fact::triggered, this, &Drones::onTriggered);
}

void Drones::setBearerToken(const QString &token)
{
    m_bearerToken = token;
}

void Drones::onTriggered()
{
    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(true);
        else
            f->setVisible(false);
    f_createButton->setVisible(true);
}

void Drones::onCreateTriggered()
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearerToken).toUtf8());
    request.setUrl(URL);

    QJsonObject root;
    QJsonObject applicant;
    applicant["firstName"] = QString("Никита");
    applicant["secondName"] = QString("Рябов");
    applicant["patronymicName"] = QString("Николаевич");
    applicant["dateOfBirth"] = QString("2001-05-08");
    applicant["placeOfBirth"] = QString("Казань");
    applicant["phoneNumber"] = QString("+79378462178");
    applicant["passportNumber"] = QString("9462832722");
    applicant["passportDateIssue"] = QString("2017-05-08");
    applicant["snils"] = QString("89764102");
    applicant["placeOfLiving"] = QString("Казань");
    applicant["email"] = QString("ryabov.nikita.ryab1299@mail.ru");
    QJsonObject uav;
    uav["serialNumber"] = f_serialNumber->value().toString();
    uav["type"] = f_type->value().toString();
    uav["model"] = f_model->value().toString();
    uav["engineCount"] = f_engineCount->value().toInt();
    uav["engineType"] = f_engineType->text();
    uav["maxTakeOffWeight"] = f_maxTakeOffWeight->value().toInt();
    root["applicant"] = applicant;
    root["uav"] = uav;
    QJsonDocument doc(root);
    m_network.post(request, doc.toJson());

    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(false);
        else
            f->setVisible(true);
    f_createStatus->setValue("Please wait...");
    f_createButton->setVisible(false);
}

void Drones::onRequestFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString uavUuid = doc.object()["data"].toObject()["uav"].toObject()["uavUuid"].toString();
    if(uavUuid.isEmpty())
        f_createStatus->setValue("FAIL");
    else
        f_createStatus->setValue("SUCCESS");
}