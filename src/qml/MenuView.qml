/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1

Image {
    id: container

    signal endGame();
    signal resumeGame();
    signal newGame1PBlue();
    signal newGame1PRed();
    signal newGame2PBlue();
    signal newGame2PRed();
    signal infoView();
    signal exit();

    property bool currentGame1P
    property bool hideMenuButtons: false
    property bool gameGoing: false

    // In windows we hide the 2P button
    property bool disable2PButtons

    width: 480; height: 854
    source: "gfx/water.png"
    fillMode: Image.Stretch
    smooth: true

    // Prevents mouse clicks on lower layers of QML elements
    MouseArea {
        anchors.fill: parent
    }

    Behavior on opacity {
        PropertyAnimation {
            duration: 250
        }
    }

    Image {
        id: logo

        anchors {
            top: parent.top; topMargin: parent.height * 0.1
            horizontalCenter: parent.horizontalCenter
        }

        width: parent.width * 0.85
        source: "gfx/battleships_logo_trans_512x512.png"
        smooth: true
        fillMode: Image.PreserveAspectFit
    }

    Image {
        id: panel1Player

        anchors {
            top: logo.bottom; topMargin: 5
            left: parent.left; leftMargin: 10
            right: parent.right; rightMargin: 10
        }

        height: 0.21 * parent.height
        source: "gfx/plate.png"
        smooth: true

        Text {
            id: player1Label

            anchors {
                top: parent.top; topMargin: 8
                left: parent.left; leftMargin: 35
            }

            text: "1 Player game"
            color: "#333333"
            font.pixelSize: 16
            style: Text.Sunken
            styleColor: "#888888"
            smooth: true
        }

        Item {
            anchors {
                top: player1Label.bottom; topMargin: 5
                left: parent.left; leftMargin: 10
                right: parent.right; rightMargin: 10
                bottom: parent.bottom; bottomMargin: 15
            }

            MyButton {
                id: button1PBlue

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.horizontalCenter
                    margins: 7
                }

                buttonImage: "gfx/blue_button.png"
                text: "Blue"
                onClicked: {
                    if (container.gameGoing) {
                        container.endGame();
                    }
                    container.currentGame1P = true;
                    container.gameGoing = true;
                    container.newGame1PBlue();
                }
            }

            MyButton {
                id: button1PRed

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.horizontalCenter
                    right: parent.right
                    margins: 7
                }

                buttonImage: "gfx/red_button.png"
                text: "Red"
                onClicked: {
                    if (container.gameGoing) {
                        container.endGame();
                    }
                    container.currentGame1P = true;
                    container.gameGoing = true;
                    container.newGame1PRed();
                }
            }
        }
    }


    Image {
        id: panel2Player

        anchors {
            top: panel1Player.bottom; topMargin: 10
            left: parent.left; leftMargin: 10
            right: parent.right; rightMargin: 10
        }

        height: panel1Player.height
        source: "gfx/plate.png"
        smooth: true

        Text {
            id: player2Label

            anchors {
                top: parent.top; topMargin: 8
                left: parent.left; leftMargin: 35
            }

            text: "2 Player game"
            color: "#333333"
            font.pixelSize: 16
            style: Text.Sunken
            styleColor: "#888888"
            smooth: true
        }

        Item {
            anchors {
                top: player2Label.bottom; topMargin: 5
                left: parent.left; leftMargin: 10
                right: parent.right; rightMargin: 10
                bottom: parent.bottom; bottomMargin: 15
            }

            MyButton {
                id: button2PBlue

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.horizontalCenter
                    margins: 7
                }

                enabled: !disable2PButtons
                buttonImage: "gfx/blue_button.png"
                text: "Blue"
                onClicked: {
                    if (container.gameGoing) {
                        container.endGame();
                    }
                    container.currentGame1P = false;
                    container.gameGoing = true;
                    container.newGame2PBlue();
                }
            }

            MyButton {
                id: button2PRed

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.horizontalCenter
                    right: parent.right
                    margins: 7
                }

                enabled: !disable2PButtons
                buttonImage: "gfx/red_button.png"
                text: "Red"
                onClicked: {
                    if (container.gameGoing) {
                        container.endGame();
                    }
                    container.currentGame1P = false;
                    container.gameGoing = true;
                    container.newGame2PRed();
                }
            }
        }
    }


    MyButton {
        id: resume

        anchors {
            top: panel2Player.bottom; topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        width: button1PBlue.width
        height: button1PBlue.height

        opacity: hideMenuButtons || !container.gameGoing ? 0 : 1
        text: "Resume"

        onClicked: {
            container.resumeGame();
        }
    }


    Image {
        id: closeMark

        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        width: 0.22 * parent.width
        height: width

        source: "gfx/closemark.png"
        smooth: true
        opacity: hideMenuButtons ? 0 : 1

        MouseArea {
            anchors.fill: parent

            scale: 1.5
            onPressed: {
                parent.anchors.leftMargin = -2
                parent.anchors.bottomMargin = -2
            }
            onReleased: {
                parent.anchors.leftMargin = 0
                parent.anchors.bottomMargin = 0
            }
            onClicked: container.exit();
        }
    }

    Image {
        id: info

        anchors {
            right: parent.right
            bottom: parent.bottom
        }

        width: 0.22 * parent.width
        height: width

        source: "gfx/info.png"
        smooth: true
        opacity: hideMenuButtons ? 0 : 1

        MouseArea {
            anchors.fill: parent

            scale: 1.5
            onPressed: {
                parent.anchors.rightMargin = -2
                parent.anchors.bottomMargin = -2
            }
            onReleased: {
                parent.anchors.rightMargin = 0
                parent.anchors.bottomMargin = 0
            }
            onClicked: container.infoView();
        }
    }
}
