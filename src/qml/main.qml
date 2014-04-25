/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1
import 'gameLogic.js' as GameLogic;

AppWindow {
    id: root

    // The UI properties describing the game state.
    property color textColor: "white"
    property int dim: GameLogic.dim
    property bool shooting: false
    property bool yourTurn: false
    property bool youReady: false
    property bool opponentReady: false
    property bool btOn: false
    property int myShipsLeft
    property int opponentShipsLeft
    property bool no2PGame // set from Qt code
    property variant myGameGrid
    property variant enemyGameGrid
    property variant myShipInfo
    property variant enemyShipInfo

    // Opens the dynamically created pop up dialog
    function openPopup(text, type, buttonText, autoClose) {
        loader.source = "Popup.qml";
        loader.item.text = text;
        loader.item.type = type;
        loader.item.buttonText = buttonText;
        loader.item.autoClose = autoClose;
        loader.item.show();
    }

    // Handler for the button press in the pop up dialog
    function closePopup() {
        if (loader.item.type === "backToMenu") {
            root.endGame();
            menuView.gameGoing = false;
            menuView.opacity = 1;
        }
        else if (loader.item.type === "waitingClient") {
            root.endGame();
            menuView.gameGoing = false;
            menuView.opacity = 1;
        }
        else if (loader.item.type === "serverSelector") {
            root.endGame();
            menuView.gameGoing = false;
            menuView.opacity = 1;
        }
        else if (loader.item.type === "connecting") {
            root.endGame();
            menuView.gameGoing = false;
            menuView.opacity = 1;
        }

        loader.source = "";
    }

    // Resets the game logic and clears the own and enemy grid
    function newGame(blue) {
        if (blue) {
            console.log("Blue 1P started");
            myGameGrid = gameGridBlue;
            enemyGameGrid = gameGridRed;
            myShipInfo = shipInfoBlue;
            enemyShipInfo = shipInfoRed
        }
        else {
            console.log("Red 1P started");
            myGameGrid = gameGridRed;
            enemyGameGrid = gameGridBlue;
            myShipInfo = shipInfoRed;
            enemyShipInfo = shipInfoBlue;
        }

        myGameGrid.enemyGrid = false;
        enemyGameGrid.enemyGrid = true;

        GameLogic.clearShots();
        youReady = false
        yourTurn = false;
        opponentReady = false;
        shooting = false;
        turnText.text = "";

        playButton.opacity = 1;
        shuffleButton.opacity = 1;

        shipInfoBlue.ship2count = 4;
        shipInfoBlue.ship2left = 4;
        shipInfoRed.ship2count = 4;
        shipInfoRed.ship2left = 4;

        shipInfoBlue.ship3count = 3;
        shipInfoBlue.ship3left = 3;
        shipInfoRed.ship3count = 3;
        shipInfoRed.ship3left = 3;

        shipInfoBlue.ship4count = 2;
        shipInfoBlue.ship4left = 2;
        shipInfoRed.ship4count = 2;
        shipInfoRed.ship4left = 2;

        shipInfoBlue.ship5count = 1;
        shipInfoBlue.ship5left = 1;
        shipInfoRed.ship5count = 1;
        shipInfoRed.ship5left = 1;

        // Create ships only once in the application lifetime.
        if (GameLogic.ships.length === 0) {
            console.log("Creating ships");

            // Create ships
            for (var i=0; i<4; i++) {
                GameLogic.addShip(myGameGrid.createShip(2, i, 0, true));
            }

            for (i=0; i<3; i++) {
                GameLogic.addShip(myGameGrid.createShip(3, i, 2, true));
            }

            for (i=0; i<2; i++) {
                GameLogic.addShip(myGameGrid.createShip(4, i, 5, true));
            }

            for (i=0; i<1; i++) {
                GameLogic.addShip(myGameGrid.createShip(5, i+5, 0, true));
            }
        }

        for (i=0; i<GameLogic.ships.length; i++) {
            GameLogic.ships[i].gameGridParent = myGameGrid;
            GameLogic.ships[i].draggable = true;
            GameLogic.ships[i].hits = 0;
        }

        GameLogic.shuffleShips(GameLogic.ships);

        myShipsLeft = GameLogic.ships.length;
        opponentShipsLeft = myShipsLeft;

        // Clear the shots in both grids.
        gameGridBlue.clear();
        gameGridRed.clear();

        // Show my game grid on the beginning
        main.contentX = myGameGrid.x
    }

    /* The functions and signals for connecting to and communicating
       with other players.
    */
    signal serverSelected(int index)
    signal myShipsPlaced()
    signal dropBomb(int row, int column)
    signal gotHit(bool hit, int row, int column)
    signal sinks(int length, int row, int column, bool portrait)
    signal newGame1P()
    signal newGame2PClient()
    signal newGame2PServer()
    signal turnBtOn();
    signal stopBtDiscovery();
    signal endGame()

    onEndGame: log("Game ended.")

    // The enemy has just placed his / hers / its (AI) ships.
    function shipsPlaced() {
        opponentReady = true;

        if (youReady)
            turnAnimation.restart();

        openPopup(menuView.currentGame1P ? "Game begins"
                                         : "The opponent has\n placed all the ships.",
                  "placed", "", true);
    }

    // The enemy shoots us
    function bombDropped(row, column) {
        var ship;
        var hit = false;

        for (var i=0; i < GameLogic.ships.length; i++) {
            ship = GameLogic.ships[i];
            if (ship.dropBomb(row, column)) {
                if (ship.hits === ship.length) {
                    myShipsLeft--;

                    // Tell the enemy that our ship was sinked
                    sinks(ship.length, ship.row, ship.column, ship.portrait);

                    // Update our ship info
                    switch (ship.length) {
                    case 2:
                        myShipInfo.ship2left--;
                        break;
                    case 3:
                        myShipInfo.ship3left--;
                        break;
                    case 4:
                        myShipInfo.ship4left--;
                        break;
                    case 5:
                        myShipInfo.ship5left--;
                        break;
                    }
                }

                hit = true;
                break;
            }
        }

        // Tell the enemy if our ship was hit
        gotHit(hit, row, column);

        myGameGrid.grid.currentIndex = row * dim + column;

        if (hit) {
            myGameGrid.grid.currentItem.explode();
        }
        else {
            myGameGrid.grid.currentItem.miss();
            yourTurn = true;
        }
    }

    // Tells if we hit or miss the enemy ship
    function youHit(hit, row, column) {
        enemyGameGrid.grid.currentIndex = row * dim + column;
        if (hit) {
            enemyGameGrid.grid.currentItem.explode();
        }
        else {
            enemyGameGrid.grid.currentItem.miss();
            yourTurn = false;
        }
        shooting = false;
    }

    // Adds log to the in game console
    function log(logText) {
        myConsole.log(logText);
    }

    function connected(name) {
        console.log("You connected to server!");

        loader.source = "";
    }

    function otherConnected(name) {
        console.log("Other player connected!");

        loader.source = "";
        openPopup(name + " joined the game!", "connected", "", true);
    }

    function otherDisconnected() {
        console.log("Other player disconnected!");
        if (menuView.gameGoing) {
            openPopup("Game ended by the other player!", "backToMenu", "Close",
                      false);
        }
    }

    function socketError() {
        console.log("Disconnected!");
        if (loader.item.type === "connecting") {
            openPopup("Failed to connect!", "backToMenu", "Close", false);
        }
        else if (loader.item.type === "waitingClient") {
            openPopup("Failed to listen for incoming connections!",
                      "backToMenu", "Close", false);
        }
    }

    function bluetoothOff() {
        console.log("Bluetooth off!");
        btOn = false;
    }

    function bluetoothOn() {
        console.log("Bluetooth on!");
        btOn = true;
    }

    function serverFound(serverIndex, deviceName, serverName) {
        loader.item.addServer(serverIndex, deviceName, serverName);
    }

    // You just sunk the enemy chip.
    function sunk(length, row, column, portrait) {
        enemyGameGrid.grid.currentIndex = row * dim + column;
        enemyGameGrid.grid.currentItem.showShip(length, portrait);
        opponentShipsLeft--;

        // Update the enemy ship info
        switch(length) {
        case 2:
            enemyShipInfo.ship2left--;
            break;
        case 3:
            enemyShipInfo.ship3left--;
            break;
        case 4:
            enemyShipInfo.ship4left--;
            break;
        case 5:
            enemyShipInfo.ship5left--;
            break;
        }
    }

    focus: true
    width: 480; height: 854

    onYourTurnChanged: {
        if (yourTurn) {
            log("It is your turn.");
            if (!opponentReady)
                return;
        }
        else {
            log("It is the opponent's turn.");
            if (!youReady)
                return;
        }
        turnAnimation.restart();
    }

    onMyShipsLeftChanged: {
        if (!myShipsLeft) {
            log("You lost the game.");
            menuView.gameGoing = false;
            openPopup("YOU LOSE!", "backToMenu", "Close", false);
        }
    }

    onOpponentShipsLeftChanged: {
        if (!opponentShipsLeft) {
            log("You won the game.");
            menuView.gameGoing = false;
            openPopup("YOU WIN!", "backToMenu", "Close", false);
        }
    }

    SequentialAnimation {
        id: turnAnimation

        NumberAnimation {
            target: turnText
            property: "opacity"; from: 1; to: 0; duration: 400
        }

        PauseAnimation { duration: 400 }
        ScriptAction {
            script: {
                if (youReady && !opponentReady) {
                    turnText.text = "Waiting for opponent";
                }
                else if (yourTurn) {
                    main.contentX = enemyGameGrid.x;
                    turnText.text = "Your turn";
                } else {
                    main.contentX = myGameGrid.x;
                    turnText.text = "Opponent's turn";
                }
            }
        }
        PauseAnimation { duration: 1000 }

        NumberAnimation {
            target: turnText
            property: "opacity"; from: 0; to: 1; duration: 400
        }
    }

    Component.onCompleted:  {
        log("Battleships started.")
    }

    Image {
        id: background

        anchors.fill: main
        source: "gfx/background.png"
        fillMode: Image.TileHorizontally
    }

    Flickable {
        id: main

        anchors.fill: parent
        contentWidth: 2 * width
        flickDeceleration: 9999
        boundsBehavior: Flickable.StopAtBounds
        interactive: youReady && opponentReady && !gameGridBlue.pressing &&
                     !gameGridRed.pressing

        onMovementEnded: {
            if (contentX < width * 0.5)
                contentX = 0;
            else
                contentX = width;
        }

        Behavior on contentX {
            SequentialAnimation {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: 300
                }
            }
        }

        GameGrid {
            id: gameGridBlue

            width: main.width; height: width
            y: myConsole.barHeight
            dim: root.dim
            blueGrid: true

            onClicked: {
                if (yourTurn && opponentReady && !shooting) {
                    if(!GameLogic.haveBeenShot(row, column)) {
                        shooting = true;
                        dropBomb(row, column);
                        GameLogic.shots[row][column] = true;
                    }
                }
            }
        }

        ShipInfo {
            id: shipInfoBlue

            anchors {
                top: gameGridBlue.bottom; topMargin: gameGridBlue.cellDim
                left: gameGridBlue.left; leftMargin: gameGridBlue.cellDim / 2
            }

            cellDim: gameGridBlue.cellDim
            textColor: root.textColor
        }

        GameGrid {
            id: gameGridRed

            width: main.width; height: width
            x: main.width; y: gameGridBlue.y
            dim: root.dim
            blueGrid: false

            onClicked: {
                if (yourTurn && opponentReady && !shooting) {
                    if(!GameLogic.haveBeenShot(row, column)) {
                        shooting = true;
                        dropBomb(row, column);
                        GameLogic.shots[row][column] = true;
                    }
                }
            }
        }

        ShipInfo {
            id: shipInfoRed

            anchors {
                top: shipInfoBlue.top
                left: gameGridRed.left; leftMargin: gameGridRed.cellDim / 2
            }
            cellDim: gameGridRed.cellDim
            textColor: root.textColor
        }
    }

    Text {
        id: shipInfoLabel

        anchors {
            left: parent.left; leftMargin: gameGridBlue.cellDim / 2
        }

        y: gameGridBlue.y + gameGridBlue.height + 4
        font.pixelSize: main.height * 0.03
        color: "white"
        text: {
            var yourShips = main.contentX < (main.contentWidth / 4);
            if (gameGridBlue !== myGameGrid) {
                yourShips = !yourShips;
            }

            if (yourShips) {
                return "Your ships";
            }
            else {
                return "Opponent's ships";
            }
        }
    }

    Text {
        id: turnText

        anchors {
            right: parent.right; rightMargin: gameGridBlue.cellDim
        }

        y: gameGridBlue.y + gameGridBlue.height + 4
        font.pixelSize: main.height * 0.03
        color: "white"
        opacity: 1
        text: ""
    }

    MyButton {
        id: shuffleButton

        anchors {
            bottom: playButton.top; bottomMargin: gameGridBlue.cellDim / 2
            right: parent.right; rightMargin: gameGridBlue.cellDim / 2
        }

        width: gameGridBlue.cellDim * 4
        height: parent.height * 0.085
        text: "Shuffle"
        onClicked: GameLogic.shuffleShips(GameLogic.ships)
    }

    MyButton {
        id: playButton

        anchors {
            bottom: menuButton.top; bottomMargin: gameGridBlue.cellDim / 2
            right: parent.right; rightMargin: gameGridBlue.cellDim / 2
        }

        width: shuffleButton.width
        height: shuffleButton.height
        text: "Play"

        onClicked: {
            if(GameLogic.checkIfShipsOverlap(GameLogic.ships)) {
                root.openPopup("Ships overlap!\nRelocate your ships.",
                               "overlap", "", true);
                return;
            }

            youReady = true;

            // The fastest player gets the chance to shoot first
            yourTurn = !opponentReady;
            turnAnimation.restart();

            myShipsPlaced();

            // Prevent the dragging of ships.
            for (var i=0; i<GameLogic.ships.length; i++) {
                GameLogic.ships[i].draggable = false;
            }

            // Hide the button
            opacity = 0;
            shuffleButton.opacity = 0;
        }
    }

    MyButton {
        id: menuButton

        anchors {
            right: parent.right; rightMargin: gameGridBlue.cellDim / 2
            bottom: parent.bottom; bottomMargin: gameGridBlue.cellDim / 2
        }

        width: shuffleButton.width
        height: shuffleButton.height
        text: "Menu"

        onClicked: menuView.opacity = 1
    }

    Loader {
        id: loader
        anchors.centerIn: parent
    }

    Console {
        id: myConsole

        width: main.width; height: main.height * 0.6
        y: -height + barHeight
    }

    MenuView {
        id: menuView

        anchors.fill: parent

        disable2PButtons: root.no2PGame

        onResumeGame: {
            menuView.opacity = 0;
        }

        onEndGame: {
            root.endGame();
        }

        onNewGame1PBlue: {
            menuView.opacity = 0;
            root.newGame1P();
            root.newGame(true);
        }

        onNewGame1PRed: {
            menuView.opacity = 0;
            root.newGame1P();
            root.newGame(false);
        }

        onNewGame2PBlue: {
            menuView.opacity = 0;
            root.newGame2PClient();
            root.newGame(true);
            if (btOn) {
                loader.source = "ServerSelector.qml"
                loader.item.show();
            } else {
                openPopup("Turn the Bluetooth on and make the device discoverable",
                          "backToMenu", "Close", false);
            }
        }

        onNewGame2PRed: {
            menuView.opacity = 0;
            root.newGame2PServer();
            root.newGame(false);
            if (btOn) {
                openPopup("Waiting for a player to join...",
                          "waitingClient",
                          "Cancel", false);
            } else {
                openPopup("Turn the Bluetooth on and make the device discoverable",
                          "backToMenu", "Close", false);
            }
        }

        onInfoView: {
            hideMenuButtons = true;
            infoView.opacity = 1;
        }

        onExit: Qt.quit();
    }

    InfoView {
        id: infoView

        anchors {
            fill: parent
            margins: parent.width * 0.02
        }

        opacity: 0
        onClosed: {
            opacity = 0;
            menuView.hideMenuButtons = false;
        }
    }
}
