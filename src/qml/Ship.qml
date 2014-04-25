/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1

Image {
    id: ship

    property bool isDoubleClicked: false
    property int length: -1
    property int hits: 0

    property int row
    property int column
    property bool portrait: true

    property int cellDim
    property variant gameGridParent

    // Enable / disable the dragging
    property alias draggable: shipMouseArea.enabled

    function dropBomb(rowArg, columnArg) {
        if (portrait && column === columnArg && row <= rowArg
                && rowArg < row + length) {
            hits++;
            return true;
        }

        if (!portrait && row === rowArg && column <= columnArg
                && columnArg < column + length) {
            hits++;
            return true;
        }

        return false;
    }

    // Randomly places the ship and informs the game grid about the new place.
    function shuffleShip() {
        ship.portrait = Math.round(Math.random());
        if (ship.portrait) {
            ship.column = Math.floor(Math.random() * dim);
            ship.row = Math.floor(Math.random() * (dim - length - 1));
        }
        else {
            ship.column = Math.floor(Math.random() * (dim - length - 1));
            ship.row = Math.floor(Math.random() * dim);
        }

        gameGridParent.shipShuffled(ship);

        fixOffsetProblem();
    }

    function fixOffsetProblem() {
        if (ship.portrait) {
            ship.x = 0;
            ship.y = 0;
        }
        else {
            // Because the ship is rotated -90 degrees the top left
            // corner of the Image is not 0, 0.
            // We need to calculate its new position.
            ship.x = (ship.length-1) * 0.48 * grid.cellWidth;
            ship.y = (ship.length-1) * -0.5 * grid.cellHeight;
        }
    }

    width: cellDim * 1.1; height: length * cellDim
    source: length === -1 ? "" : "gfx/ship_x" + length + ".png"
    smooth: true
    rotation: portrait ? 0 : -90
    fillMode: Image.PreserveAspectFit

    Behavior on rotation { PropertyAnimation {
            duration: 100
        }
    }

    MouseArea {
        id: shipMouseArea

        anchors.fill: parent
        drag.target: parent

        onPressAndHold: {
            portrait = !portrait;

            ship.fixOffsetProblem();
        }

        onReleased: {
            if (isDoubleClicked) {
                isDoubleClicked = false;
                return;
            }

            gameGridParent.shipDragged(ship);

            ship.fixOffsetProblem();
        }

        onDoubleClicked: {
            isDoubleClicked = true;
            portrait = !portrait;

            ship.fixOffsetProblem();
        }
    }
}
