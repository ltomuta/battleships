/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1

Image {
    id: container

    property int cellDim
    property string textColor
    property string fontSize: cellDim * 0.30

    property int ship2count: 4
    property int ship2left: 4
    property int ship3count: 3
    property int ship3left: 3
    property int ship4count: 2
    property int ship4left: 2
    property int ship5count: 1
    property int ship5left: 1

    width: container.cellDim * 5
    height: container.cellDim * 5.5
    source: "gfx/grid_10x3.png"
    fillMode: Image.PreserveAspectCrop
    smooth: true

    Item {
        id: shipInfo5

        width: container.cellDim * 5; height: container.cellDim

        SunkBar {
            y: 1
            width: parent.width - (parent.width * ship5left / ship5count)
            height: parent.height - 1
        }

        Image {
            anchors.bottom: parent.bottom
            width: parent.width
            source: "gfx/ship_x5_silh.png"
            smooth: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Item {
        id: shipInfo4

        anchors.right: shipInfo5.right

        y: container.cellDim * 1.5
        width: container.cellDim * 4; height: container.cellDim

        SunkBar {
            y: 1
            width: parent.width - (parent.width * ship4left / ship4count)
            height: parent.height - 1
        }

        Image {
            anchors.bottom: parent.bottom
            width: parent.width
            source: "gfx/ship_x4_silh.png"
            smooth: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Item {
        id: shipInfo3

        anchors.right: shipInfo4.right

        y: container.cellDim * 3
        width: container.cellDim * 3; height: container.cellDim

        SunkBar {
            y: 1
            width: parent.width - (parent.width * ship3left / ship3count)
            height: parent.height - 1
        }

        Image {
            anchors.bottom: parent.bottom
            width: parent.width
            source: "gfx/ship_x3_silh.png"
            smooth: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Item {
        id: shipInfo2

        anchors.right: shipInfo3.right

        y: container.cellDim * 4.5
        width: container.cellDim * 2.0; height: container.cellDim

        SunkBar {
            y: 1
            width: parent.width - (parent.width * ship2left / ship2count)
            height: parent.height - 1
        }

        Image {
            anchors.bottom: parent.bottom
            width: parent.width
            source: "gfx/ship_x2_silh.png"
            smooth: true
            fillMode: Image.PreserveAspectFit
        }
    }
}
