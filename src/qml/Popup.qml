/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1

MouseArea {
    id: popup

    property bool autoClose
    property string type
    property alias text: label.text
    property alias buttonText: button.text
    property alias buttonHeight: button.height
    property real drawnHeight: 0.4 * main.height
    property alias button: button
    default property alias children: drawnArea.children

    function show() {
        showAnimation.start();
    }

    anchors.centerIn: parent
    width: main.width; height: main.height

    onClicked: {
        if (button.text == "") {
            closePopup();
        }
    }

    SequentialAnimation {
        id: showAnimation

        ParallelAnimation {
            PropertyAnimation {
                target: dimming
                property: "opacity"
                to: 0.6
            }
            PropertyAnimation {
                target: drawnArea; property: "width"; to: 0.9 * main.width
                easing.type: Easing.InOutQuad
            }
            PropertyAnimation {
                target:  drawnArea; property: "height"; to: drawnHeight
                easing.type: Easing.InOutQuad
            }
        }

        ParallelAnimation {
            PropertyAnimation {
                target: button
                property: "opacity"
                to: button.text == "" ? 0 : 1
            }
            ColorAnimation {
                target: label
                property: "color"
                to: "white"
            }
        }

        PauseAnimation { duration: 1200 }

        ScriptAction {
            script: {
                if (popup.autoClose) {
                    closePopup();
                }
            }
        }
    }

    // Popup background
    Rectangle {
        id: dimming

        anchors.fill: parent
        color: "black"
        opacity: 0
    }

    Rectangle {
        id: drawnArea

        anchors.centerIn: parent
        width: 0; height: 0
        color: "black"
        smooth: true
        radius: 20
        opacity: 0.7
        border.width: 2
        border.color: "gray"

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#555555" }
            GradientStop { position: 0.7; color: "#666666" }
            GradientStop { position: 1.0; color: "#333333" }
        }
    }

    Text {
        id: label

        anchors {
            left: drawnArea.left; leftMargin: drawnArea.width * 0.1
            right: drawnArea.right; rightMargin: drawnArea.width * 0.1
            bottom: drawnArea.verticalCenter
        }

        color: "transparent"
        smooth: true
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
    }

    MyButton {
        id: button

        anchors {
            horizontalCenter: drawnArea.horizontalCenter
            bottom: drawnArea.bottom
            bottomMargin: height * 0.15
        }

        width: drawnArea.width * 0.7; height: width * 0.4
        text: "Close"
        onClicked: closePopup();
        opacity: 0
    }
}
