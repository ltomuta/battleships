/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1

Popup {
    id: serverSelector

    type: "serverSelector"
    buttonText: "Cancel"

    function addServer(serverIndex, deviceName, serverName) {
        serverModel.append({"serverIndex": serverIndex,
                               "deviceName": deviceName,
                               "serverName": serverName})
    }

    drawnHeight: main.height * 0.8

    Text {
        id: topic

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: height
        }

        text: serverModel.count ? "Select the opponent" : "Searching..."
        font.pixelSize: 30
        color: "white"
    }

    Item {
        id: listViewClippingArea

        anchors {
            top: topic.bottom; topMargin: topic.anchors.topMargin * 0.5
            left: parent.left; leftMargin: parent.width * 0.12
            right: parent.right; rightMargin: parent.width * 0.12
            bottom: parent.bottom; bottomMargin: 110
        }

        clip: true

        ListView {
            id: serverList

            property int delegateWidth: parent.width
            property int delegateHeight: delegateWidth * 0.4

            anchors.fill: parent

            spacing: 10
            model: serverModel
            delegate: MyButton {
                width: serverList.delegateWidth
                height: serverList.delegateHeight
                enabled: visible
                text: deviceName
                textSize: width * 0.1
                onClicked: {
                    root.openPopup("Connecting to " + deviceName + "...",
                        "connecting", "Cancel", false);
                    root.serverSelected(index);
                }
            }

            ListModel { id: serverModel }
        }

        Rectangle {
            id: scrollDecorator

            anchors {
                right: serverList.right; rightMargin: 3
            }
            y: serverList.visibleArea.yPosition * serverList.height +
               serverList.anchors.topMargin
            width: 4
            height: serverList.visibleArea.heightRatio * serverList.height

            opacity: 0.5
            color: "#cccccc"
            radius: 2
        }
    }
}
