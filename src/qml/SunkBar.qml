import QtQuick 1.1

Image {
    id: container

    property color sunkColor: "#77BB0000"

    source: "gfx/sunkbar.png"
    fillMode: Image.Tile

    Behavior on width {
        PropertyAnimation {
            duration: 1000
            easing.type: Easing.OutQuart
        }
    }
}
