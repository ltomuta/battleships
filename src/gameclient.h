/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QObject>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothsocket.h>

QTM_BEGIN_NAMESPACE
class QBluetoothSocket;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE


/*!
  \class GameClient
  \brief Handles the communication between the application and
         the server side via Bluetooth API.
*/
class GameClient : public QObject
{
    Q_OBJECT

public:
    explicit GameClient(QObject *parent = 0);
    ~GameClient();

    void startClient(const QBluetoothServiceInfo &remoteService);
    void stopClient();

signals:
    void shipsPlaced();
    void bombDropped(QVariant row, QVariant column);
    void youHit(QVariant youHit, QVariant row, QVariant column);
    void sunk(QVariant length, QVariant row, QVariant column, QVariant portrait);
    void gameStarted();
    void log(QVariant logText);
    void connectedToServer(const QVariant &name);
    void disconnectedFromServer();
    void socketError();

public slots:
    void myShipsPlaced();
    void dropBomb(int row, int column);
    void gotHit(bool gotHit, int row, int column);
    void sinks(int length, int row, int column, bool portrait);
    void startGame();

private slots:
    void connectService();
    void readSocket();
    void connected();
    void disconnected();
    void socketError(QBluetoothSocket::SocketError);

private:
    QBluetoothSocket *socket;
    QBluetoothServiceInfo service;
    int retries;
};

#endif // GAMECLIENT_H
