/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QStringList>
#include <QTimer>
#include "gameclient.h"


GameClient::GameClient(QObject *parent)
    : QObject(parent),
      socket(0),
      retries(0)
{
}


GameClient::~GameClient()
{
    stopClient();
}


/*!
  Initializes the client and connects to the service.
*/
void GameClient::startClient(const QBluetoothServiceInfo &remoteService)
{
    if (socket || retries) {
        qDebug() << "Client already running!";
        return;
    }

    service = remoteService;
    retries = 3;

    QBluetoothAddress address = service.device().address();
    qDebug() << "Bluetooth address: " << address.toString();

    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
    qDebug() << "Create socket";

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)),
            this, SLOT(socketError(QBluetoothSocket::SocketError)));

    // Connect to the service a bit later so that the
    // "connecting" popup has some time to update on UI.
    // TODO: This should not be needed, but
    // socket->connectToService blocks our thread for a while
    // on Harmattan.
    QTimer::singleShot(500, this, SLOT(connectService()));
}

/*!
  Connects to the set service.
*/
void GameClient::connectService()
{
    socket->connectToService(service);
    qDebug() << "Connect to service done" << socket->peerName()
             << socket->state();
}

/*!
  Destroys the client. The possible connection to the server is closed.
*/
void GameClient::stopClient()
{
    if (socket) {
        QString logString("Disconnecting from server");
        qDebug() << logString;
        emit log(logString);

        socket->disconnectFromService();

        delete socket;
        socket = 0;
    }
}


/*!
  The UI reports that all of the ships are placed, send information about this
  to the server.
*/
void GameClient::myShipsPlaced()
{
    qDebug() << "Client is sending command \"shipsplaced\"!";

    emit log("Player 2 has placed all the ships.");

    QByteArray data = QString("shipsplaced").toAscii() + '\n'; // No arguments
    qint64 written = socket->write(data);

    qDebug() << "Client wrote " << written << " bytes.";
}


/*!
  The UI reports that the user has selected the place where to shoot.
  Send a message about the dropping of bomb to the server.
*/
void GameClient::dropBomb(int row, int column)
{
    emit log("Player 2 dropped a bomb to " + QString('A' + row) +
             QString(", ") + QString::number(column + 1));

    QString command("bombdropped:");
    command.append(QString::number(row)); // First argument
    command.append(":");
    command.append(QString::number(column)); // Second argument

    QByteArray data = command.toAscii() + '\n';
    socket->write(data);
}


/*!
  The UI reports about the enemy hit to the given coordinates.
  Send response to the server about the hit.
*/
void GameClient::gotHit(bool gotHit, int row, int column)
{
    if (gotHit) {
        emit log("Player 1 hits.");
    }
    else {
        emit log("Player 1 misses.");
    }

    QString command("hit:");
    command.append(QString::number(gotHit));
    command.append(QString(":%1:%2").arg(row).arg(column));

    QByteArray data = command.toAscii() + '\n';
    socket->write(data);
}


/*!
  The UI reports that the enemy shot sinked one of our ships.
  Send response to the server about the sinking ship.
*/
void GameClient::sinks(int length, int row, int column, bool portrait)
{
    emit log("The " + QString::number(length) + " long ship of Player 2 sinks.");

    QString command("sink:");
    command.append(QString::number(length) + ":");
    command.append(QString::number(row) + ":");
    command.append(QString::number(column) + ":");
    command.append(QString::number(portrait));

    QByteArray data = command.toAscii() + '\n';
    socket->write(data);
}


/*!
  Sends a message to the server that new game will be started.
*/
void GameClient::startGame()
{
    emit log("Player 2 starts a new game.");

    QString command("newgame");

    QByteArray data = command.toAscii() + '\n';
    socket->write(data);
}


/*!
  Receiver for the Bluetooth data. Handles the messages server has
  send to us and emits the corresponding signals for UI slots.
*/
void GameClient::readSocket()
{
    if (!socket) {
        qDebug() << "...but there is no socket!";
        return;
    }

    while (socket->canReadLine()) {
        QByteArray byteArray = socket->readLine().trimmed();
        QString line = QString::fromAscii(byteArray.constData(),
                                          byteArray.length());

        qDebug() << "Received line: " << line;

        QStringList pieces = line.split(":");
        QString command = pieces.at(0);

        if (!command.compare(QString("bombdropped"))) {
            QVariant row(pieces.at(1).toInt());
            QVariant column(pieces.at(2).toInt());

            emit log("Player 1 dropped a bomb to " +
                     QString('A' + row.toInt()) + QString(", ") +
                     QString::number(column.toInt() + 1));

            emit bombDropped(row, column);
        }
        else if (!command.compare(QString("shipsplaced"))) {
            qDebug() << "Client is recieving command \"shipsplaced\"!";

            emit log("Player 1 has placed all the ships.");

            emit shipsPlaced();
        }
        else if (!command.compare(QString("hit"))) {
            QVariant hit(pieces.at(1).toInt());
            QVariant row(pieces.at(2).toInt());
            QVariant column(pieces.at(3).toInt());

            if (hit.toBool()) {
                emit log("Player 2 hits.");
            }
            else {
                emit log("Player 2 misses.");
            }

            emit youHit(hit, row, column);
        }
        else if (!command.compare(QString("sink"))) {
            QVariant length(pieces.at(1).toInt());
            QVariant row(pieces.at(2).toInt());
            QVariant column(pieces.at(3).toInt());
            QVariant portrait(pieces.at(4).toInt());

            emit log("The " + length.toString() +
                     " long ship of Player 1 sinks.");

            emit sunk(length, row, column, portrait);
        }
        else if (!command.compare(QString("newgame"))) {
            emit log("Player 1 started a new game.");

            emit gameStarted();
        }
    }
}


/*!
  We have succesfully connected to the server.
*/
void GameClient::connected()
{
    QString logString = QString("Connected to server, socket state: %1")
            .arg(socket->state());
    qDebug() << logString;
    emit log(logString);

    retries = 0;

    emit connectedToServer(socket->peerName());
}


/*!
  Disconnected from the server.
*/
void GameClient::disconnected()
{
    QString logString = QString("Disconnected from server, state: %1")
            .arg(socket->state());
    qDebug() << logString;

    if (!retries) {
        emit log(logString);
        emit disconnectedFromServer();
    }
}

/*!
  There has been socket error in Bluetooth communication.
*/
void GameClient::socketError(QBluetoothSocket::SocketError error)
{
    QString string = QString("There was a socket error: %1").arg(error);

    qDebug() << string;
    emit log(string);

    if (retries > 0) {
        retries--;
        qDebug() << "Retries left: " << retries;
        QTimer::singleShot(500, this, SLOT(connectService()));
    } else {
        emit socketError();
    }
}

