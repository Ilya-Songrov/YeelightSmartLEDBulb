#include "Bulb.hpp"

namespace YeelightBulb {

Bulb::Bulb(QObject *parent)
    : QObject{parent}
    , socket(new QTcpSocket())
    , queueRequests()
    , objResponse(nullptr)
    , id(0)
{
    connect(socket.get(), &QTcpSocket::disconnected, this, &Bulb::disconnected);
    connect(socket.get(), &QTcpSocket::stateChanged, this, &Bulb::stateChanged);
    connect(socket.get(), &QTcpSocket::errorOccurred, this, &Bulb::errorOccurred);
    connect(socket.get(), &QTcpSocket::readyRead, this, &Bulb::readFromBulb);
}

bool Bulb::connectToBulb(const QHostAddress& address, quint16 port)
{
    socket->connectToHost(address, port);
    const bool ret = socket->waitForConnected(3000);
    qDebug() << "print_function:" << __FUNCTION__ << "Socket connection:" << ret << Qt::endl;
    return ret;
}

void Bulb::resetConnection()
{
    socket->close();
    QThread::msleep(100);
    socket.reset(new QTcpSocket());
}

void Bulb::set_power(const bool power, const BulbEffect bulbEffect, const int duration)
{
    const QString powerStr = power ? "on" : "off";
    const QString bulbEffectStr = QVariant::fromValue(bulbEffect).toString().toLower();
    QJsonObject objCommand{
        {"id", ++id},
        {"method", "set_power"},
        {"params", QJsonArray({powerStr, bulbEffectStr, duration})},
    };
    sendToBulb(objCommand, QSharedPointer<QJsonObject>(new QJsonObject{
                                                           {"id", id},
                                                           {"result", QJsonArray({"ok"})},
                                                       }
                                                       ));
}

void Bulb::set_bright(const char brightness, const BulbEffect bulbEffect, const int duration)
{
    const QString bulbEffectStr = QVariant::fromValue(bulbEffect).toString().toLower();
    QJsonObject objCommand{
        {"id", ++id},
        {"method", "set_bright"},
        {"params", QJsonArray({brightness, bulbEffectStr, duration})},
    };
    sendToBulb(objCommand, QSharedPointer<QJsonObject>(new QJsonObject{
                                                           {"id", id},
                                                           {"result", QJsonArray({"ok"})},
                                                       }
                                                       ));
}

void Bulb::sendToBulb(const QJsonObject& objCommand, QSharedPointer<QJsonObject> objRes)
{
    qDebug() << "print_function:" << __FUNCTION__ << __LINE__ << " text: " << __LINE__ << Qt::endl;
    if (!objResponse.isNull()) {
        queueRequests.enqueue(qMakePair(objCommand, objRes));
        return;
    }
    objResponse = objRes;
    qDebug() << "print_function:" << __FUNCTION__ << objCommand << Qt::endl;
    socket->write(QJsonDocument(objCommand).toJson() + "\r\n");
}

void Bulb::readFromBulb()
{
    const QByteArray arr      = socket->readAll();
    const QJsonDocument doc   = QJsonDocument::fromJson(arr);
    const QJsonObject obj = doc.object();
    qDebug() << "print_function:" << __FUNCTION__ << arr << Qt::endl;
    const QString message = obj.value("error").toObject().value("message").toString();
    if (obj.value("method").toString() == "props") {
        // ignore this state
        return;
    }
    else if (!objResponse.isNull() && obj == *objResponse.get()) {
        // no action
    }
    else if(message == "client quota exceeded"){
        emit bulbErrorOccurred(ClientQuotaExceeded);
    }
    else {
        emit bulbErrorOccurred(ResponsesDoNotMatch);
    }
    objResponse.clear();
    runCommandInQueue();
}

void Bulb::runCommandInQueue()
{
    if (queueRequests.isEmpty()) {
        return;
    }
    QPair<QJsonObject, QSharedPointer<QJsonObject> > pair = queueRequests.dequeue();
    sendToBulb(pair.first, pair.second);
}

}
