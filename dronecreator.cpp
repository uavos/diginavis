#include "dronecreator.h"

#include "App/AppLog.h"

DroneCreator::DroneCreator(Fact *parent):
    HttpApiBase(parent, "create_drone", "Create drone", "", FactBase::Section, "plus-circle-outline")
{
    f_serialNumber = new Fact(this, "serial_number", "Serial number", "", Fact::Text);
    f_type = new Fact(this, "type", "Type", "", Fact::Text);
    f_model = new Fact(this, "model_", "Model", "", Fact::Text);
    f_engineCount = new Fact(this, "engine_count", "Engine count", "", Fact::Int);
    f_engineCount->setMin(0);
    f_engineType = new Fact(this, "engine_type", "Engine type", "", Fact::Enum);
    f_engineType->setEnumStrings({"Gasoline", "Electric"});
    f_maxTakeOffWeight = new Fact(this, "max_takeoff_weight", "Max takeoff weight", "", Fact::Int);
    f_maxTakeOffWeight->setMin(0);

    f_createButton = new Fact(this, "create", "Create", "", Fact::Apply | Fact::Action);

    f_createStatus = new Fact(this, "create_result", "", "", Fact::NoFlags);
    f_createStatus->setVisible(false);

    connect(f_createButton, &Fact::triggered, this, &DroneCreator::onCreateTriggered);
    connect(this, &Fact::triggered, this, &DroneCreator::onTriggered);
}

void DroneCreator::onTriggered()
{
    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(true);
        else
            f->setVisible(false);
    f_createButton->setVisible(true);
}

void DroneCreator::onCreateTriggered()
{
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

    QNetworkRequest request = makeRequest(URL);
    m_network.post(request, doc.toJson());

    for(auto f: facts())
        if(f != f_createStatus)
            f->setVisible(false);
        else
            f->setVisible(true);
    f_createStatus->setTitle("Please wait...");
    f_createButton->setVisible(false);
}

void DroneCreator::onJsonReceived(const QJsonDocument &doc)
{
    QString uavUuid = doc.object()["data"].toString();
    if(uavUuid.isEmpty()) {
        apxMsgW() << "Diginavis: " << QString::fromUtf8(doc.toJson());
        f_createStatus->setTitle("FAIL");
    }
    else
        f_createStatus->setTitle("Success. Please wait for moderator response.");
}