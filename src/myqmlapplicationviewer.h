/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef MYQMLAPPLICATIONVIEWER_H
#define MYQMLAPPLICATIONVIEWER_H

#include <QDeclarativeView>
#include <QVariant>
#include "aiplayer.h"

#ifdef SUPPORT_2P_GAME
#include <qmobilityglobal.h>
#include <qbluetoothuuid.h>
#include <qbluetoothlocaldevice.h>

QTM_USE_NAMESPACE

class GameServer;
class GameClient;
class ServerSelector;
#endif


/*!
  \class MyQmlApplicationViewer
  \brief QDeclarativeView derived class to handle creating of the Qt Quick UI
         and to handle the communication between Qt Quick and Qt code.
*/
class MyQmlApplicationViewer : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit MyQmlApplicationViewer(QWidget *parent = 0);
    void connectSignalsAndSlots();

private:

#ifdef SUPPORT_2P_GAME
    GameServer *server;
    GameClient *client;
    QString localName;
    ServerSelector* serverSelector;
    QBluetoothLocalDevice* localDevice;
#endif

    AIPlayer *aiPlayer;

public slots:
    // For QML
    void endGame();
    void connect1PSignalsAndSlots();
#ifdef SUPPORT_2P_GAME
    void connect2PClientSignalsAndSlots();
    void connect2PServerSignalsAndSlots();

    void serverSelected(int);
    bool isBtOn();
    void turnBtOn();
signals:
    void bluetoothOn();
    void bluetoothOff();
#endif
};

#endif // MYQMLAPPLICATIONVIEWER_H
