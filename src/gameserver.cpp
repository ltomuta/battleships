/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <QStringList>
#include <qrfcommserver.h>
#include <qbluetoothsocket.h>
#include "gameserver.h"

// The service Uuid for our service.
static const QLatin1String serviceUuid("11e719c0-7c79-11e0-819a-0800200c9a66");


GameServer::GameServer(QObject *parent)
    : QObject(parent),
      rfcommServer(0),
      clientSocket(0)
{
}


GameServer::~GameServer()
{
    stopServer();
}


/*!
  Creates RFCOMM server and makes it listen for incoming connections.
*/
void GameServer::startServer()
{
    if (rfcommServer) {
        qDebug() << "Server already started";
        return;
    }

    // Create the server.
    rfcommServer = new QRfcommServer(this);
    connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));

    if (!rfcommServer->listen()) {
        emit log("Server is not listening");
        qDebug() << "ERROR: rfcommServer is not listening!";
    }

    QString logString = QString("Server is using port %1")
            .arg(rfcommServer->serverPort());

    emit log(logString);
    qDebug() << logString;

    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

    // Class Uuuid must contain at least 1 entry.
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    // Service name, description and provider.
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("BattleShips game"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, tr("The game server for BattleShips"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("Nokia"));
    serviceInfo.setServiceAvailability(1);

    qDebug() << serviceInfo.attribute(QBluetoothServiceInfo::ServiceName).toString();
    qDebug() << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();


    // Set the Service UUID set.
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));

    // Service Discoverability.
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));


    // Protocol descriptor list.
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;

    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));

    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    // Register service.
    if (serviceInfo.registerService()) {
        emit log("Server registered");
        emit log("Waiting for clients to connect");

        qDebug() << "Server registered";
    }
    else {
        emit log("Failed to register service");
        qDebug() << "ERROR: Failed to register service!";
    }
}


/*!
  Stops the RFCOMM server.
*/
void GameServer::stopServer()
{
    // Unregister service
    serviceInfo.unregisterService();

    emit log("Server unregistered");

    // Close sockets
    if (clientSocket) {
        clientSocket->disconnectFromService();

        delete clientSocket;
        clientSocket = 0;
    }

    // Close server
    delete rfcommServer;
    rfcommServer = 0;
}


/*!
  Sends the "shipsplaced" message to the peer.
*/
void GameServer::myShipsPlaced()
{
    emit log("Player 1 has placed all the ships.");

    qDebug() << "Server is sending command \"shipsplaced\"!";

    QByteArray data = QString("shipsplaced").toAscii() + '\n'; // No arguments

    qDebug() << "Socket state before write: " << clientSocket->state();

    qint64 bytesWritten = clientSocket->write(data);
    qDebug() << "Wrote " << bytesWritten << " bytes to socket.";

    qDebug() << "Socket state after write: " << clientSocket->state();
}


/*!
  Sends the "bombdropped" message to the peer.
*/
void GameServer::dropBomb(int row, int column)
{
    emit log("Player 1 dropped a bomb to " + QString('A' + row) + ", " +
             QString::number(column + 1));

    qDebug() << "Player 1 dropped a bomb to " << QString('A' + row) << ", "
             << column + 1;

    QString command("bombdropped:");
    command.append(QString::number(row)); // First argument
    command.append(":");
    command.append(QString::number(column)); // Second argument

    QByteArray data = command.toAscii() + '\n';
    qint64 bytesWritten = clientSocket->write(data);
    qDebug() << "Wroted " << bytesWritten << " to bytes to socket.";
}


/*!
  Sends the "hit" message to the peer.
*/
void GameServer::gotHit(bool gotHit, int row, int column)
{
    if (gotHit)
        log("Player 2 hits.");
    else
        log("Player 2 misses.");

    QString command("hit:");
    command.append(QString::number(gotHit));
    command.append(QString(":%1:%2").arg(row).arg(column));

    QByteArray data = command.toAscii() + '\n';
    clientSocket->write(data);
}


/*!
  Sends the "sink" message to the peer.
*/
void GameServer::sinks(int length, int row, int column, bool portrait)
{
    emit log("The " + QString::number(length) + " long ship of Player 1 sinks.");

    QString command("sink:");
    command.append(QString::number(length) + ":");
    command.append(QString::number(row) + ":");
    command.append(QString::number(column) + ":");
    command.append(QString::number(portrait));

    QByteArray data = command.toAscii() + '\n';
    clientSocket->write(data);
}


/*!
  Sends the "newgame" message to the peer.
*/
void GameServer::startGame()
{
    emit log("Player 1 starts a new game.");

    QString command("newgame");

    QByteArray data = command.toAscii() + '\n';
    clientSocket->write(data);
}


/*!
  Handles the incoming connection from the client. Connects required signals
  and slots of the new connection in order to receive data from the client.
*/
void GameServer::clientConnected()
{
    qDebug() << "clientConnected";

    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket || clientSocket) {
        qDebug() << "Client already connected, rejecting new connection.";

        emit log("Client aready connected, rejecting new connection");

        socket->close();
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)),
            this, SLOT(socketError(QBluetoothSocket::SocketError)));

    clientSocket = socket;

    QString logString = QString("Client connected: %1").arg(socket->peerName());
    qDebug() << logString;

    emit log(logString);

    emit opponentConnected(socket->peerName());
}


/*!
  Handles the disconnection of the client.
*/
void GameServer::clientDisconnected()
{
    QString logString("Client disconnected");

    qDebug() << logString;

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());

    if (!socket)
        return;

    emit log(logString);

    emit opponentDisconnected();

    clientSocket->deleteLater();
    clientSocket = 0;
}


/*!
  Receives data from the socket.
*/
void GameServer::readSocket()
{
    qDebug() << "Server is recieving data!";

    if (!clientSocket)
        return;

    while (clientSocket->canReadLine()) {
        QByteArray byteArray = clientSocket->readLine().trimmed();
        QString line = QString::fromAscii(byteArray.constData(),
                                          byteArray.length());
        QStringList pieces = line.split(":");
        QString command = pieces.at(0);

        if (!command.compare(QString("bombdropped"))) {
            QVariant row(pieces.at(1).toInt());
            QVariant column(pieces.at(2).toInt());

            emit log("Player 2 dropped a bomb to " +
                     QString('A' + row.toInt()) + QString(", ") +
                     QString::number(column.toInt() + 1));

            emit bombDropped(row, column);
        }
        else if (!command.compare(QString("shipsplaced"))) {
            qDebug() << "Server is receiving command \"shipsplaced\"!";

            emit log("Player 2 has placed all the ships.");

            emit shipsPlaced();
        }
        else if (!command.compare(QString("hit"))) {
            QVariant hit(pieces.at(1).toInt());
            QVariant row(pieces.at(2).toInt());
            QVariant column(pieces.at(3).toInt());

            if (hit.toBool()) {
                emit log("Player 1 hits.");
            }
            else {
                emit log("Player 1 misses.");
            }

            emit youHit(hit, row, column);
        }
        else if (!command.compare(QString("sink"))) {
            QVariant length(pieces.at(1).toInt());
            QVariant row(pieces.at(2).toInt());
            QVariant column(pieces.at(3).toInt());
            QVariant portrait(pieces.at(4).toInt());

            emit log("The " + length.toString() + " long ship of Player 2 sinks.");

            emit sunk(length, row, column, portrait);
        }
        else if (!command.compare(QString("newgame"))) {
            emit log("Player 2 started a new game.");

            emit gameStarted();
        }
        else {
            qDebug() << "Received unknown command: " << command;

            emit log(QString("Received unknown command: %1").arg(command));
        }
    }
}


/*!
  Handles the socket error.
*/
void GameServer::socketError(QBluetoothSocket::SocketError error)
{
    QString errorString = QString("There was a socket error %1").arg(error);

    qDebug() << errorString;

    emit log(errorString);

    emit socketError();
}
