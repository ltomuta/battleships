/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1

Item {
    id: container

    property alias text: label.text
    property alias textSize: label.font.pixelSize
    property alias enabled: mouseArea.enabled
    property alias buttonImage: button.source


    signal clicked()

    Image {
        id: button

        width: parent.width; height: parent.height

        source: "gfx/gray_button.png"
        smooth: true

        Text {
            id: label

            anchors.centerIn: parent
            width: parent.width * 0.8
            horizontalAlignment: Text.AlignHCenter
            color: "#333333"
            font.bold: true
            font.weight: Font.DemiBold
            font.pixelSize: Math.max(parent.width, parent.height) * 0.18
            wrapMode: Text.WordWrap
            styleColor: "#666666"
            style: Text.Raised
            smooth: true
        }

        MouseArea {
            id: mouseArea

            anchors.fill: parent
            onPressed: {
                button.x = 1;
                button.y = 1;
                button.scale = 0.98
            }
            onReleased: {
                button.x = 0;
                button.y = 0;
                button.scale = 1
            }
            onClicked: container.clicked()
        }
    }

    Image {
        id: buttonReflection

        anchors.fill: button
        source: "gfx/button_reflection.png"
        opacity: 0.5
    }
}
