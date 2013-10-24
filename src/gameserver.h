/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QObject>
#include <QVariant>
#include <qbluetoothsocket.h>
#include <qbluetoothserviceinfo.h>

QTM_BEGIN_NAMESPACE
class QRfcommServer;
//class QL2capServer;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE


/*!
  \class GameServer
  \brief GameServer is responsible of handling the server side communication on
         Bluetooth communication.
*/
class GameServer : public QObject
{
    Q_OBJECT
public:
    explicit GameServer(QObject *parent = 0);
    ~GameServer();

    void startServer();
    void stopServer();

signals:
    void shipsPlaced();
    void bombDropped(QVariant row, QVariant column);
    void youHit(QVariant youHit, QVariant row, QVariant column);
    void sunk(QVariant length, QVariant row, QVariant column,
              QVariant portrait);
    void gameStarted();
    void log(QVariant logText);
    void opponentConnected(const QVariant &name);
    void opponentDisconnected();
    void socketError();

public slots:
    void myShipsPlaced();
    void dropBomb(int row, int column);
    void gotHit(bool gotHit, int row, int column);
    void sinks(int length, int row, int column, bool portrait);
    void startGame();
    void clientConnected();

private slots:

    void clientDisconnected();
    void socketError(QBluetoothSocket::SocketError);
    void readSocket();

private:
    //QL2capServer *l2capServer;
    QRfcommServer *rfcommServer;
    QBluetoothServiceInfo serviceInfo;
    QBluetoothSocket *clientSocket;
};

#endif // GAMESERVER_H
