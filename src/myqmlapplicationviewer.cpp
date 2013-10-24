/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QtGui>
#include "myqmlapplicationviewer.h"


#ifdef SUPPORT_2P_GAME
#include "gameclient.h"
#include "gameserver.h"
#include "serverselector.h"

static const QLatin1String serviceUuid("11e719c0-7c79-11e0-819a-0800200c9a66");
#endif


MyQmlApplicationViewer::MyQmlApplicationViewer(QWidget *parent)
    : QDeclarativeView(parent),
#ifdef SUPPORT_2P_GAME
    server(0),
    client(0),
    serverSelector(0),
#endif
    aiPlayer(0)
{
#ifdef SUPPORT_2P_GAME
    localDevice = new QBluetoothLocalDevice();
#endif
}


/*!
  Ends all possible games currently going, and resets the game logic to the
  initial state.
*/
void MyQmlApplicationViewer::endGame()
{
    qDebug() << "Game ends";
#ifdef SUPPORT_2P_GAME
    delete server;
    server = 0;

    delete serverSelector;
    serverSelector = 0;

    delete client;
    client = 0;
#endif

    delete aiPlayer;
    aiPlayer = 0;
}


/*!
  Connects the minimum amount of signals and slots on the beginning of the
  of the application in order to make the menu system to work.
*/
void MyQmlApplicationViewer::connectSignalsAndSlots()
{
    QObject *qmlObject = rootObject();

#ifdef SUPPORT_2P_GAME
    qmlObject->setProperty("no2PGame", false);
#else
    qmlObject->setProperty("no2PGame", true);
#endif

    // Set the version number to infoview
    QObject *infoView = qmlObject->findChild<QObject*>("infoView");
    infoView->setProperty("version", qApp->applicationVersion());

    connect(qmlObject, SIGNAL(endGame()),
            this, SLOT(endGame()));
    connect(qmlObject, SIGNAL(newGame1P()),
            this, SLOT(connect1PSignalsAndSlots()));
#ifdef SUPPORT_2P_GAME
    connect(qmlObject, SIGNAL(newGame2PClient()),
            this, SLOT(connect2PClientSignalsAndSlots()));
    connect(qmlObject, SIGNAL(newGame2PServer()),
            this, SLOT(connect2PServerSignalsAndSlots()));
    connect(qmlObject, SIGNAL(turnBtOn()),
            this, SLOT(turnBtOn()));
    connect(this, SIGNAL(bluetoothOn()),
            qmlObject, SLOT(bluetoothOn()));
    connect(this, SIGNAL(bluetoothOff()),
            qmlObject, SLOT(bluetoothOff()));
#endif
    connect((QObject*)engine(), SIGNAL(quit()), qApp, SLOT(quit()));
}


/*!
  The user selected 1P game, 1P game signals and slots between the UI and AI
  game logic.
*/
void MyQmlApplicationViewer::connect1PSignalsAndSlots()
{
    aiPlayer = new AIPlayer(this);

    QObject *qmlObject = rootObject();

    connect(qmlObject, SIGNAL(myShipsPlaced()),
            aiPlayer, SLOT(shipsPlaced()));
    connect(qmlObject, SIGNAL(dropBomb(int, int)),
            aiPlayer, SLOT(bombDropped(int, int)));
    connect(qmlObject, SIGNAL(gotHit(bool, int, int)),
            aiPlayer, SLOT(youHit(bool, int, int)));
    connect(qmlObject, SIGNAL(sinks(int, int, int, bool)),
            aiPlayer, SLOT(sunk(int, int, int, bool)));

    connect(aiPlayer, SIGNAL(log(QVariant)),
            qmlObject, SLOT(log(QVariant)));
    connect(aiPlayer, SIGNAL(myShipsPlaced()),
            qmlObject, SLOT(shipsPlaced()));
    connect(aiPlayer, SIGNAL(dropBomb(QVariant, QVariant)),
            qmlObject, SLOT(bombDropped(QVariant, QVariant)));
    connect(aiPlayer, SIGNAL(gotHit(QVariant, QVariant, QVariant)),
            qmlObject, SLOT(youHit(QVariant, QVariant, QVariant)));
    connect(aiPlayer, SIGNAL(sinks(QVariant, QVariant, QVariant, QVariant)),
            qmlObject, SLOT(sunk(QVariant, QVariant, QVariant, QVariant)));
}


#ifdef SUPPORT_2P_GAME
/*!
  The user selected 2P client game. This device will be the client, approriate
  signals and slots connections are made here.
*/
void MyQmlApplicationViewer::connect2PClientSignalsAndSlots()
{
    qDebug() << "Connecting 2P client Signals and Slots";

    if (!isBtOn())
        return;

    client = new GameClient(this);
    serverSelector = new ServerSelector(this);

    QObject *qmlObject = rootObject();

    connect(serverSelector, SIGNAL(serverFound(QVariant,QVariant,QVariant)),
            qmlObject, SLOT(serverFound(QVariant, QVariant, QVariant)));
    connect(serverSelector, SIGNAL(log(QVariant)),
            qmlObject, SLOT(log(QVariant)));
    connect(qmlObject, SIGNAL(serverSelected(int)),
            this, SLOT(serverSelected(int)));

    connect(qmlObject, SIGNAL(myShipsPlaced()),
            client, SLOT(myShipsPlaced()));
    connect(qmlObject, SIGNAL(dropBomb(int, int)),
            client, SLOT(dropBomb(int, int)));
    connect(qmlObject, SIGNAL(gotHit(bool, int, int)),
            client, SLOT(gotHit(bool, int, int)));
    connect(qmlObject, SIGNAL(sinks(int, int, int, bool)),
            client, SLOT(sinks(int, int, int, bool)));

    connect(client, SIGNAL(shipsPlaced()),
            qmlObject, SLOT(shipsPlaced()));
    connect(client, SIGNAL(bombDropped(QVariant, QVariant)),
            qmlObject, SLOT(bombDropped(QVariant, QVariant)));
    connect(client, SIGNAL(youHit(QVariant, QVariant, QVariant)),
            qmlObject, SLOT(youHit(QVariant, QVariant, QVariant)));
    connect(client, SIGNAL(sunk(QVariant, QVariant, QVariant, QVariant)),
            qmlObject, SLOT(sunk(QVariant, QVariant, QVariant, QVariant)));
    connect(client, SIGNAL(log(QVariant)),
            qmlObject, SLOT(log(QVariant)));
    connect(client, SIGNAL(connectedToServer(QVariant)),
            qmlObject, SLOT(connected(QVariant)));
    connect(client, SIGNAL(disconnectedFromServer()),
            qmlObject, SLOT(otherDisconnected()));
    connect(client, SIGNAL(socketError()),
            qmlObject, SLOT(socketError()));

    qDebug() << "Starting discovery";
    serverSelector->startDiscovery(QBluetoothUuid(serviceUuid));
}


/*!
  The user selected 2P server game. This device will be the server, appropriate
  signals and slots connections are made here.
*/
void MyQmlApplicationViewer::connect2PServerSignalsAndSlots()
{
    qDebug() << "Connecting 2P server Signals and Slots";

    if (!isBtOn())
        return;

    server = new GameServer(this);

    QObject *qmlObject = rootObject();

    connect(qmlObject, SIGNAL(myShipsPlaced()),
            server, SLOT(myShipsPlaced()));
    connect(qmlObject, SIGNAL(dropBomb(int, int)),
            server, SLOT(dropBomb(int, int)));
    connect(qmlObject, SIGNAL(gotHit(bool, int, int)),
            server, SLOT(gotHit(bool, int, int)));
    connect(qmlObject, SIGNAL(sinks(int, int, int, bool)),
            server, SLOT(sinks(int, int, int, bool)));

    connect(server, SIGNAL(shipsPlaced()),
            qmlObject, SLOT(shipsPlaced()));
    connect(server, SIGNAL(bombDropped(QVariant, QVariant)),
            qmlObject, SLOT(bombDropped(QVariant, QVariant)));
    connect(server, SIGNAL(youHit(QVariant, QVariant, QVariant)),
            qmlObject, SLOT(youHit(QVariant, QVariant, QVariant)));
    connect(server, SIGNAL(sunk(QVariant, QVariant, QVariant, QVariant)),
            qmlObject, SLOT(sunk(QVariant, QVariant, QVariant, QVariant)));
    connect(server, SIGNAL(log(QVariant)),
            qmlObject, SLOT(log(QVariant)));
    connect(server, SIGNAL(opponentConnected(QVariant)),
            qmlObject, SLOT(otherConnected(QVariant)));
    connect(server, SIGNAL(opponentDisconnected()),
            qmlObject, SLOT(otherDisconnected()));
    connect(server, SIGNAL(socketError()),
            qmlObject, SLOT(socketError()));

    server->startServer();
    // Temporary workaround for Qt issue QTMOBILITY-1850
    server->stopServer();
    server->startServer();
}


/*!
  Server was selected from the UI.
*/
void MyQmlApplicationViewer::serverSelected(int index)
{
    QBluetoothServiceInfo remoteServer = serverSelector->getServer(index);
    serverSelector->stopDiscovery();
    qDebug() << "Server selected:" << remoteServer.serviceName();

    qDebug() << "service info isValid: " << remoteServer.isValid();
    qDebug() << "service info isCompleted: " << remoteServer.isComplete();
    qDebug() << "service info service uuid: " << remoteServer.serviceUuid();

    qDebug() << "Connecting...";
    client->startClient(remoteServer);
}

/*!
  Checks if Bluetooth is currently powered on
*/
bool MyQmlApplicationViewer::isBtOn()
{
    QBluetoothLocalDevice::HostMode mode = localDevice->hostMode();

    qDebug() << "Current bt mode: " << mode;

    if (mode == QBluetoothLocalDevice::HostPoweredOff ||
        mode == QBluetoothLocalDevice::HostConnectable) {
        emit bluetoothOff();
        return false;
    }

    emit bluetoothOn();
    return true;
}

/*!
  Turn bt on and make the device discoverable
*/
void MyQmlApplicationViewer::turnBtOn()
{
    qDebug() << "Turning bt on";
    localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    emit bluetoothOn();
}

#endif
