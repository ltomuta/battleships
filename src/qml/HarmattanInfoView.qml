/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: container
    objectName: "infoView"

    signal closed();

    // Property is set from Qt code
    property string version

    width: 480; height: 854

    Behavior on opacity {
        PropertyAnimation {
            duration: 250
        }
    }

    Image {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: backButton.top; bottomMargin: 5
        }

        source: "gfx/popup_bg_480x854px.png"
        smooth: true

        Item {
            id: clippingArea

            anchors {
                fill: parent
                topMargin: parent.height * 0.07
                bottomMargin: anchors.topMargin
                leftMargin: parent.width * 0.07
                rightMargin: parent.width * 0.04
            }
            clip: true

            Flickable {
                id: flickable

                anchors {
                    fill: parent
                    rightMargin: 8
                }

                contentWidth: width
                contentHeight: text.height

                Text {
                    id: text

                    width: flickable.width
                    color: "black"
                    font.pointSize: 14
                    text: "<h2>Battleships " + container.version + "</h2>" +
                          "<p>Battleships is a Nokia example application that " +
                          "demonstrates the use of Qt Mobility Bluetooth API.</p>" +
                          "<p>For more information about the project, see " +
                          "<a href=\"https://projects.developer.nokia.com/" +
                          "battleships\">" +
                          "https://projects.developer.nokia.com/battleships</a>." +
                          "</p>" +
                          "<p>The Battleships provides two game modes: " +
                          "<ul>1P game, where the opponent is AI-controlled</ul>" +
                          "<ul>2P game, where the opponent is human player " +
                          "via a Bluetooth connection</ul></p>" +
                          "<h3>1P Game</h3>" +
                          "<p>The 1P game begins by placing your ships on the " +
                          "game grid. The ships are already randomly placed, but " +
                          "you are able to reposition them by dragging and " +
                          "double tapping (or by long pressing) to change their " +
                          "orientation. You can also press the Shuffle ships " +
                          "button to ramdomly re-position all of the ships. " +
                          "When you are ready, press the Play button to begin " +
                          "the game.</p>" +
                          "<p>Your and enemy ships are presented on a blue-hued " +
                          "and a red-hued grid. This makes it easy regognice " +
                          "which grid you are looking at.</p>" +
                          "<p>When it is your turn, shoot the enemy ships by " +
                          "tapping one of the grid cells. The view will " +
                          "automatically switch to the enemy grid when it is " +
                          "the enemy's turn. You may also pan the view between " +
                          "your own and the enemy's grid by flicking the grid " +
                          "horizontally. The game ends when all the ships of one " +
                          "of the players have been sunk.</p>" +
                          "<h3>2P game</h3>" +
                          "<p>The 2P game is available in Harmattan and Symbian " +
                          "devices, in Windows build, the 2P Game options are " +
                          "disabled in the menu.</p>" +
                          "<p>To start the 2P game, you must select either Red " +
                          "or Blue grid, the Red one is always the server and " +
                          "Blue the client. The client will begin to search for " +
                          "phones in the vicinity of the phone. " +
                          "All found mobile phones will be listed in the " +
                          "clients view. Select a phone to create a Bluetooth " +
                          "connection between the client and the server.</p>" +
                          "<p>You may now place your ships on the game grid. " +
                          "When you are done, tap the Play button to start the " +
                          "game.</p>"

                    wrapMode: Text.Wrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }

            Rectangle {
                id: scrollDecorator

                anchors.right: parent.right

                y: flickable.visibleArea.yPosition * flickable.height +
                   flickable.anchors.topMargin
                width: 4
                height: flickable.visibleArea.heightRatio * flickable.height

                opacity: 0.5
                color: "black"
                radius: 2
            }
        }
    }

    Image {
        id: backButton

        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        sourceSize.width: parent.width * 0.2
        source: "gfx/back_arrow.svg"
        smooth: true

        Behavior on scale {
            PropertyAnimation {
                duration: 40
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: parent.scale = 0.9
            onReleased: parent.scale = 1.0
            onClicked: container.closed();
        }
    }
}
