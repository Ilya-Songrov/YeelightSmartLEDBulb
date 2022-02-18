#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPair>
#include <QQueue>

namespace YeelightBulb {

class Bulb : public QObject
{
    Q_OBJECT
public:
    enum BulbError{
        ClientQuotaExceeded,
        ResponsesDoNotMatch,
    };
    enum BulbEffect{
        Smooth,
        Sudden,
    };
    Q_ENUM(BulbEffect)

    explicit Bulb(QObject *parent = nullptr);

    bool connectToBulb(const QHostAddress& address, quint16 port);
    void resetConnection();

    void set_power(const bool power, const BulbEffect bulbEffect, const int duration);
    void set_bright(const char brightness, const BulbEffect bulbEffect, const int duration);

    QString getPeerHostPort();

signals:
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError);
    void bulbErrorOccurred(YeelightBulb::Bulb::BulbError bulbError);

private:
    void sendToBulb(const QJsonObject& objCommand, QSharedPointer<QJsonObject> objRes);
    void readFromBulb();
    void runCommandInQueue();

private:
    QScopedPointer<QTcpSocket> socket;
    QQueue<QPair<QJsonObject, QSharedPointer<QJsonObject> > > queueRequests;
    QSharedPointer<QJsonObject> objResponse;
    int id;


};

}
