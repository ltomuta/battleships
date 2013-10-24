/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1

Image {
    id: gameGrid

    signal clicked(int row, int column);

    // Dimension of the grid
    property int dim
    property alias cellDim: grid.cellWidth
    property alias grid: grid
    property bool enemyGrid: false
    property bool blueGrid

    property bool pressing: false

    function createShip(length, column, row, portrait) {
        var component = Qt.createComponent("Ship.qml");
        grid.currentIndex = column + row * dim;

        if (grid.currentIndex === -1) {
            console.log("ERROR! Ship will be placed outside the grid");
        }

        var ship = component.createObject(grid.currentItem);
        ship.length = length;
        ship.cellDim = grid.cellWidth;
        ship.column = column;
        ship.row = row;
        ship.portrait = portrait;
        ship.gameGridParent = gameGrid;

        return ship;
    }

    function shipDragged(ship) {
        var object = ship.mapToItem(grid, 0, 0);

        if (ship.portrait) {
            grid.currentIndex = grid.indexAt(object.x + grid.cellWidth / 2,
                                             object.y + grid.cellHeight / 2);
        }
        else {
            grid.currentIndex = grid.indexAt(object.x + grid.cellWidth / 2,
                                             object.y - grid.cellHeight / 2);
        }

        if (grid.currentIndex !== -1) {

            var column = grid.currentIndex % dim;
            var row = Math.floor(grid.currentIndex / dim);

            // Prevent placing the ship outside the grid
            if (ship.portrait) {
                if (row + ship.length > dim) {
                    row = dim - ship.length;
                    grid.currentIndex = row * dim + column;
                }
            }
            else {
                if (column + ship.length > dim) {
                    column = dim - ship.length;
                    grid.currentIndex = row * dim + column;
                }
            }

            ship.column = grid.currentIndex % dim;
            ship.row = Math.floor(grid.currentIndex / dim);
            ship.parent = grid.currentItem;
        }
    }


    function shipShuffled(ship) {
        grid.currentIndex = ship.row * dim + ship.column;
        ship.parent = grid.currentItem;
    }


    // Clears the grid from ships and explosions.
    function clear() {
        for (var i = 0; i < grid.model; i++) {
            grid.currentIndex = i;
            grid.currentItem.reset();
        }
    }

    source: "gfx/water.png"
    smooth: true

    Rectangle {
        anchors.fill: parent
        rotation: gameGrid.blueGrid ? -90 : 90

        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: gameGrid.blueGrid ? "#aa218c21" : "#aa8c2121"
            }
            GradientStop { position: 1.00; color: "transparent" }
        }
    }

    Row {
        id: numbers

        x: grid.x; y: grid.y - grid.cellHeight

        Repeater {
            model: dim

            Text {
                width: grid.cellWidth; height: grid.cellHeight
                text: index + 1
                color: textColor
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignBottom
            }
        }
    }

    Column {
        id: letters

        x: grid.x - grid.cellWidth / 2; y: grid.y

        Repeater {
            model: dim

            Text {
                width: grid.cellWidth / 2; height: grid.cellHeight
                text: String.fromCharCode(index + 65)
                color: textColor
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Image {
        x: grid.x; y: grid.y
        width: grid.cellWidth * dim
        height: grid.cellHeight * dim
        smooth: true
        source: "gfx/grid.png"
    }

    GridView {
        id: grid

        anchors {
            top: parent.top; topMargin: 40
            left: parent.left; leftMargin: 20
            right: parent.right; rightMargin: 20
            bottom: parent.bottom
        }
        cellWidth: width / dim; cellHeight: cellWidth
        boundsBehavior: Flickable.StopAtBounds
        model: dim * dim
        delegate: Cell {
            id: cell

            width: grid.cellWidth; height: grid.cellHeight
        }

        Rectangle {
            id: columnHighlight

            width: parent.cellWidth; height: parent.height
            opacity: 0

            Behavior on opacity {
                PropertyAnimation {
                    duration: 100
                }
            }
        }

        Rectangle {
            id: rowHightlight

            width: parent.width; height: parent.cellHeight
            opacity: 0

            Behavior on opacity {
                PropertyAnimation {
                    duration: 100
                }
            }
        }
    }

    MouseArea {
        id: clickHandler

        property int row
        property int column

        function resolveCoordinate() {
            row = Math.floor(mouseY / grid.cellHeight);
            column = Math.floor(mouseX / grid.cellWidth);
        }

        function highlightCoordinate() {
            if (row >= dim || column >= dim ||
                row < 0 || column < 0) {
                return;
            }

            columnHighlight.x = column * grid.cellWidth
            rowHightlight.y = row * grid.cellHeight

            columnHighlight.opacity = 0.2
            rowHightlight.opacity = 0.2
        }

        function hideHighlight() {
            columnHighlight.opacity = 0
            rowHightlight.opacity = 0
        }

        anchors.fill: grid
        enabled: gameGrid.enemyGrid
        onPressed: {
            if (!enemyGrid) {
                return;
            }

            resolveCoordinate();
            highlightCoordinate();
            gameGrid.pressing = true;
        }

        onMousePositionChanged: {
            if (!enemyGrid) {
                return;
            }

            resolveCoordinate();
            highlightCoordinate();
        }

        onReleased: {
            if (!enemyGrid) {
                return;
            }

            hideHighlight();
            gameGrid.pressing = false;
        }

        onClicked: {
            if (!enemyGrid) {
                return
            }

            gameGrid.clicked(row, column);
        }
    }
}
