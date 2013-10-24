/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QtCore>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include "serverselector.h"

QTM_USE_NAMESPACE

ServerSelector::ServerSelector(QObject *parent)
    : QObject(parent),
      m_discoveryAgent(NULL)
{
}


ServerSelector::~ServerSelector()
{
    delete m_discoveryAgent;
}


/*!
  Starts the discovery of the Bluetooth devices in visinity.
*/
void ServerSelector::startDiscovery(const QBluetoothUuid &uuid)
{
    delete m_discoveryAgent;
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));

    connect(m_discoveryAgent, SIGNAL(finished()),
            this, SLOT(discoveryFinished()));

    m_uuid = uuid;
    //m_discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::LimitedInquiry);
    m_discoveryAgent->start();

    qDebug() << "Searching...";

    emit log("Searching devices...");
}


/*!
  Stops the discovery of the Bluetooth devices in visinity.
*/
void ServerSelector::stopDiscovery()
{
    qDebug() << "STOP DISCOVERY!";

    emit log("Stopping searching of devices.");

    delete m_discoveryAgent;
    m_discoveryAgent = NULL;
}


/*!
  A device has been discovered. If the device is phone device
  we report the device to the UI by emitting signal serverFound.
*/
void ServerSelector::deviceDiscovered(const QBluetoothDeviceInfo & info)
{
    QString logString;
    logString = QString("Discovered device %1 %2").arg(info.name()).arg(info.address().toString());

    qDebug() << logString;
    emit log(logString);

#ifndef SHOW_ALL_BT_DEVICES
    if (info.majorDeviceClass() != QBluetoothDeviceInfo::PhoneDevice) {
        qDebug() << "Not a PhoneDevice, skipping";
        return;
    }
#endif // SHOW_ALL_BT_DEVICES

    QMapIterator<int, QBluetoothDeviceInfo> i(m_discoveredDevices);
    while (i.hasNext()) {
        i.next();
        if (info.address() == i.value().address())
            return;
    }

    int index = m_discoveredDevices.count();
    m_discoveredDevices.insert(index, info);

    // Display the address of devices without name
    QString deviceName;
    if (!info.name().length())
        deviceName = info.address().toString();
    else
        deviceName = info.name();

    emit serverFound(QVariant(index), QVariant(deviceName),
                     QVariant("BattleShips"));
}


/*!
  Getter for the QBluetoothServiceInfo of the discovered device by using index.
*/
QBluetoothServiceInfo ServerSelector::getServer(int index)
{
    QList<QBluetoothUuid> list;
    QBluetoothServiceInfo info;

    list.append(m_uuid);
    info.setDevice(m_discoveredDevices[index]);
    info.device().setServiceUuids(list, QBluetoothDeviceInfo::DataIncomplete);
    info.setServiceUuid(m_uuid);

    return info;
}


/*!
  The discovery has been finished. All of the found devices are now in the
  m_discoveredDevices. The discovery is restarted after a while.
*/
void ServerSelector::discoveryFinished()
{
    qDebug() << "Device searching finished";

    emit log("No more devices were found");

    emit log("Restart discovery");
    QTimer::singleShot(1000, m_discoveryAgent, SLOT(start()));
}
