/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1

Item {
    id: myConsole

    property int speed: 0
    property int lastY: y
    property alias barHeight: bar.height
    property int upY: bar.height - myConsole.height

    function log(textToLog) {
        var date = new Date();

        var hours = date.getHours();
        if (hours < 10)
            hours = "0" + hours

        var minutes = date.getMinutes();
        if (minutes < 10)
            minutes = "0" + minutes

        var seconds = date.getSeconds();
        if (seconds < 10)
            seconds = "0" + seconds
        logText.text += "\n[" + hours + ":" + minutes + ":" + seconds + "] "
                        + textToLog;

        // Restrict the count of console log lines.
        if (logText.lineCount > 30) {
            var lineCount = 0;
            for (var i=logText.text.length-1; i>=0; i--) {
                if (logText.text[i] === "\n") {
                    lineCount++;
                    if (lineCount == 30) {
                        logText.text = logText.text.substring(i);
                        return;
                    }
                }
            }
        }
    }

    onYChanged: {
        speed = lastY - y;
        lastY = y;

        if (y == upY) {
            arrow.downEndPosition = false;
            speed = 0;
        }
        else if (y == 0) {
            arrow.downEndPosition = true;
            speed = 0;
        }
    }

    Rectangle {
        id: consoleBackground

        opacity: 0.6
        color: "black"
        width: parent.width
        height: parent.height - bar.height
    }

    Text {
        id: logText

        color: textColor
        font.pixelSize: 16
        wrapMode: Text.Wrap
        anchors {
            left: parent.left; leftMargin: 0.02 * parent.width
            right: parent.right; rightMargin: 0.02 * parent.width
            bottom: bar.top; bottomMargin: 0.5 * bar.height
        }
    }

    Image {
        id: bar

        width: parent.width; height: 0.08 * main.height
        source: "gfx/top_bar.png"
        y: parent.height-height

        Image {
            id: topic

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 0.05 * parent.width
            }

            height: parent.height * 0.7
            width: height * 4.83
            source: "gfx/battleships_name_trans.png"
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        Image {
            id: arrow

            property bool downEndPosition: false

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 0.02 * parent.width
            }

            source: "gfx/top_arrow.png"
            rotation: downEndPosition ? 180 : 0
            smooth: true

            Behavior on rotation {
                RotationAnimation {
                    duration: 350
                    direction: RotationAnimation.Shortest
                    easing.type: Easing.OutQuart
                }
            }
        }
    }

    PropertyAnimation {
        id: up

        target: myConsole
        property: "y"
        to: upY
    }

    PropertyAnimation {
        id: down

        target: myConsole
        property: "y"
        to: 0
    }

    MouseArea {
        anchors.fill: myConsole

        drag {
            target: myConsole
            axis: Drag.YAxis
            minimumY: upY
            maximumY: 0
        }

        onPressed: {
            down.stop();
            up.stop();
        }

        onReleased: {
            if (drag.active == false) {
                if (Math.round(myConsole.y) == myConsole.upY) {
                    down.duration = 1500 / 5;
                    down.start();
                }
                else {
                    up.duration = 1500 / 5;
                    up.start();
                }
            }
            else if (speed < 0) {
                down.duration = -1500 / (0.5 * speed)
                down.start();
            }
            else if (speed > 0) {
                up.duration = 1500 / (0.5 * speed)
                up.start();
            }
        }
    }
}
