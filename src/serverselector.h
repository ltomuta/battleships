/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef SERVERSELECTOR_H
#define SERVERSELECTOR_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <qbluetoothuuid.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothdevicediscoveryagent.h>

QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

QTM_USE_NAMESPACE


/*!
  \class ServerSelector
  \brief Handles the server side communication in 2P game.
*/
class ServerSelector : public QObject
{
    Q_OBJECT

public:
    explicit ServerSelector(QObject *parent = 0);
    ~ServerSelector();

    void startDiscovery(const QBluetoothUuid &uuid);
    QBluetoothServiceInfo getServer(int index);

private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QMap<int, QBluetoothDeviceInfo> m_discoveredDevices;
    QBluetoothUuid m_uuid;

signals:
    void serverFound(QVariant index, QVariant deviceName, QVariant serverName);
    void log(QVariant logText);

public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    void discoveryFinished();
    void stopDiscovery();
};

#endif // SERVERSELECTOR_H
