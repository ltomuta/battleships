/*
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1
import Qt.labs.particles 1.0

Item {
    id: cell

    function reset() {
        exploded = false;
        missed = false;
        hitImage.scale = 0;
        shipImage.source = "";
        shipImage.length = 0;
        shipImage.opacity = 0;
    }

    function explode() {
        exploded = true;
        particles.source = "gfx/fireparticle.png";
        particles.burst(70);
        hitImage.scale = 1;
    }

    function miss() {
        missed = true;
        particles.source = "gfx/waterparticle.png";
        particles.burst(70);
        hitImage.scale = 1;
    }

    function showShip(length, portrait) {
        shipImage.length = length;
        shipImage.source = "gfx/ship_x" + length + ".png";
        shipImage.portrait = portrait;
        shipImage.opacity = 1.0;
    }

    // Don't use these from outside of this component
    property bool exploded: false
    property bool missed: false

    Image {
        id: shipImage

        property int length
        property bool portrait: true

        y: portrait ? 0 : parent.height

        rotation: portrait ? 0 : -90
        width: parent.width
        height: parent.height * length
        fillMode: Image.PreserveAspectFit
        transformOrigin: Item.TopLeft
        opacity: 0
        Behavior on opacity {
            PropertyAnimation {
                duration: 300
            }
        }

    }

    Image {
        id: hitImage

        anchors.fill: parent
        scale: 0
        opacity: 0.6
        z: 10
        source: parent.exploded ? "gfx/explosion.png"
                                : parent.missed ? "gfx/sunk.png"
                                                : ""
        Behavior on scale {
            PropertyAnimation {
                duration: 500
            }
        }
    }

    Particles {
        id: particles

        anchors.centerIn: parent
        width: 1; height: 1
        emissionRate: 0
        lifeSpan: 1000; lifeSpanDeviation: 600
        angle: 0; angleDeviation: 360;
        velocity: 30; velocityDeviation: 30
    }
}
