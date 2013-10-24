/*
 * Copyright (c) 2011 Nokia Corporation.
 */

// The dimensions of the game grid
var dim = 10;

// Holds all of the ships
var ships = new Array();

// Holds the shots
var shots = new Array(dim);


for (var i = 0; i < dim; i++)
    shots[i] = new Array(dim);


// Clears the shots from shots array
function clearShots()
{
    for (var i = 0; i < dim; i++)
        for (var j = 0; j < dim ; j++)
            shots[i][j] = false;
}

// Adds ships to the ship array
function addShip(ship)
{
    ships.push(ship);
}


// Checks if we have already shot to given
// coordinate on enemy grid. This is used
// to prevent multiple shootings to the same
// coordinates.
function haveBeenShot(row, column)
{
    return shots[row][column];
}


// Compares if two ships overlaps
function doesShipsOverlap(s1, s2)
{
    var s1col, s1row;
    var s2col, s2row;

    for (var i=0; i<s1.length; i++) {
        for (var j=0; j<s2.length; j++) {
            if (s1.portrait) {
                s1col = s1.column;
                s1row = s1.row + i;
            }
            else {
                s1col = s1.column + i;
                s1row = s1.row;
            }

            if (s2.portrait) {
                s2col = s2.column;
                s2row = s2.row + j;
            }
            else {
                s2col = s2.column + j;
                s2row = s2.row;
            }

            if (s1col >= dim || s1row >= dim || s1col < 0 || s1row < 0) {
                console.log ("Some ship is outside the grid");
                return true;
            }

            if (s2col >= dim || s2row >= dim || s2col < 0 || s2row < 0) {
                console.log ("Some ship is outside the grid");
                return true;
            }


            if (s1col == s2col && s1row == s2row) {
                return true;
            }
        }
    }

    return false;
}


// Check if any of the ships in given array overlaps
// with each other.
function checkIfShipsOverlap(ships)
{
    for (var i=0; i<ships.length; i++) {
        for (var j=i+1; j<ships.length; j++) {
            if (doesShipsOverlap(ships[i], ships[j])) {
                return true;
            }
        }
    }

    return false;
}


// Shuffles the ships in the given array.
function shuffleShips(ships)
{
    if (ships.length < dim) {
        console.log("There are no ships to shuffle");
        return;
    }

    var shuffled = new Array();

    for (var i=0; i<ships.length; i++) {
        shuffled.push(ships[i]);

        var j=0;

        do {
            shuffled[i].shuffleShip();
            j++;
            if (j > 1000) {
                // If there is something wrong in the shuffling code
                // there will be forever loop here, so just to make
                // sure we don't jam the machine.
                console.log("The ships just won't shuffle :-/")
                return;
            }

        }
        while(checkIfShipsOverlap(shuffled))
    }

    delete shuffled;
}
