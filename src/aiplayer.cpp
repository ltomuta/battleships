/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QtCore>
#include <math.h>
#include "aiplayer.h"


AIPlayer::AIPlayer(QObject *parent) :
    QObject(parent)
{
    srand(QTime::currentTime().msec());
    resetState();
}


/*!
  Resets the game logic, this is called in beginning and on new game.
*/
void AIPlayer::resetState()
{
    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++)
            m_Shots[i][j] = NOT_SHOT;

    m_Ships.clear();
    m_OpponentShips[0] = SHIPS2;
    m_OpponentShips[1] = SHIPS3;
    m_OpponentShips[2] = SHIPS4;
    m_OpponentShips[3] = SHIPS5;
    m_HitShots.clear();
    m_LastShotSunkShip = false;
}


/*!
  Checks if the given ships does not overlap with other ships
  and adds the ship to container. If the ship overlaps the
  this will be removed from the container.
  The function will also check that the given ship has at least
  one empty cell to the next ship on every direction.
  To make the ship's places unpredictable we sometimes put the
  ships next to each other.
  Returns true if ship was succesfully place in to the game grid.
*/
bool AIPlayer::addShipToContainerIfPossible(Ship &ship)
{
    if (rand() % 8) {
        // Leave 1 cell space around the ship
        Ship ship1;
        Ship ship2;
        Ship ship3;

        if (ship.m_Portrait) {
            ship1 = Ship(ship.m_Length+2, ship.m_Col-1,
                         ship.m_Row-1, ship.m_Portrait);
            ship2 = Ship(ship.m_Length+2, ship.m_Col,
                         ship.m_Row-1, ship.m_Portrait);
            ship3 = Ship(ship.m_Length+2, ship.m_Col+1,
                         ship.m_Row-1, ship.m_Portrait);
        }
        else {
            ship1 = Ship(ship.m_Length+2, ship.m_Col-1,
                         ship.m_Row-1, ship.m_Portrait);
            ship2 = Ship(ship.m_Length+2, ship.m_Col-1,
                         ship.m_Row, ship.m_Portrait);
            ship3 = Ship(ship.m_Length+2, ship.m_Col-1,
                         ship.m_Row+1, ship.m_Portrait);
        }

        for (int i=0; i<m_Ships.count(); i++) {
            if (m_Ships[i].overlaps(ship1)) {
                return false;
            }
            if (m_Ships[i].overlaps(ship2)) {
                return false;
            }
            if (m_Ships[i].overlaps(ship3)) {
                return false;
            }
        }
    }
    else {
        // Allow ship to be next to a ship.
        for (int i=0; i<m_Ships.count(); i++) {
            if (m_Ships[i].overlaps(ship)) {
                return false;
            }
        }
    }

    m_Ships.push_back(ship);

    return true;
}


/*!
  Removes the given shot from the shot list that represents
  shots to ships that are not sunk yet.
*/
void AIPlayer::removeShotFromList(const Shot &shot)
{
    QList<Shot>::iterator it;
    for (it = m_HitShots.begin(); it < m_HitShots.end(); it++) {
        if (*it == shot) {
            m_HitShots.erase(it);
            return;
        }
    }
}


/*!
  Checks if the given shot has already been shot.
*/
bool AIPlayer::hasBeenShot(const Shot &shot, RESULT result) const
{
    if (m_Shots[shot.m_Col][shot.m_Row] == result) {
        return true;
    }

    return false;
}

/*!
  Checks whether this position can contain a ship.
  This algorithm is used so that the AI will not shoot
  to a small areas where the ships that are left cannot
  exist.
*/
bool AIPlayer::canContainShip(const Shot &shot) const
{
    if (!hasBeenShot(shot, NOT_SHOT)) {
        return false;
    }

    // Find out the longest ship type there is still floating.
    int longestShipLength = 5;
    for (int i = 3; i >= 0; i--) {
        if (m_OpponentShips[i]) {
            longestShipLength = i+2;
            break;
        }
    }

    // Check if there is enough space in this column
    int freeSpaceLength = 1;
    for (int i = 1; i <= longestShipLength; i++) {
        if (shot.m_Col+i < DIM && m_Shots[shot.m_Col+i][shot.m_Row] == NOT_SHOT)
            freeSpaceLength++;
        else
            break;
    }
    if (freeSpaceLength >= longestShipLength)
        return true;
    for (int i = 1; i <= longestShipLength; i++) {
        if (shot.m_Col-i >= 0 && m_Shots[shot.m_Col-i][shot.m_Row] == NOT_SHOT)
            freeSpaceLength++;
        else
            break;
    }
    if (freeSpaceLength >= longestShipLength)
        return true;

    // Check if there is enough space in this row
    freeSpaceLength = 1;
    for (int i = 1; i <= longestShipLength; i++) {
        if (shot.m_Row+i < DIM && m_Shots[shot.m_Col][shot.m_Row+i] == NOT_SHOT)
            freeSpaceLength++;
        else
            break;
    }
    if (freeSpaceLength >= longestShipLength)
        return true;
    for (int i = 1; i <= longestShipLength; i++) {
        if (shot.m_Row-i >= 0 && m_Shots[shot.m_Col][shot.m_Row-i] == NOT_SHOT)
            freeSpaceLength++;
        else
            break;
    }
    if (freeSpaceLength >= longestShipLength)
        return true;

    // There isn't enough space for any ships
    return false;
}


/*!
  Places the ship of given length randomly.
*/
bool AIPlayer::placeShip(int length)
{
    Ship ship;
    ship.m_Length = length;
    ship.m_Hits = 0;

    int i=0;

    do {
        ship.m_Portrait = rand() % 2;

        if (ship.m_Portrait) {
            ship.m_Col = rand() % DIM;
            ship.m_Row = rand() % (DIM - ship.m_Length - 1);
        }
        else {
            ship.m_Col = rand() % (DIM - ship.m_Length - 1);
            ship.m_Row = rand() % DIM;
        }

        i++;
        if(i>500) {
            qDebug() << "The ships just won't shuffle! :-/ "
                     << "Error in AIPlayer's place ship logic!";
            return false;
        }
    }
    while (!addShipToContainerIfPossible(ship));

    return true;
}


/*!
  The enemy has places all ships, lets place ours and start the game.
*/
void AIPlayer::shipsPlaced()
{
    resetState();

    placeShip(5);

    for (int i=0; i<2; i++)
        placeShip(4);

    for (int i=0; i<3; i++)
        placeShip(3);

    for (int i=0; i<4; i++)
        placeShip(2);

    emit myShipsPlaced();
}


/*!
  The enemy shoots us in the given coordinates.
*/
void AIPlayer::bombDropped(int row, int column)
{
    bool hit = false;

    emit log(QString("You shoot to %1, %2")
             .arg(QString('A' + row))
             .arg(QString::number(column + 1)));

    for (int i = 0; i < SHIPS; i++) {
        if (m_Ships[i].testHit(row, column)) {
            m_Ships[i].m_Hits++;
            if (m_Ships[i].m_Hits == m_Ships[i].m_Length) {
                // The enemy sunk our ship, tell the details about the sunken
                // ship.
                emit sinks(QVariant(m_Ships[i].m_Length),
                           QVariant(m_Ships[i].m_Row),
                           QVariant(m_Ships[i].m_Col),
                           QVariant(m_Ships[i].m_Portrait));
            }

            hit = true;
            break;
        }
    }

    // Tell enemy if they hit or not
    emit gotHit(QVariant(hit), QVariant(row), QVariant(column));

    if (hit) {
        // The enemy hit us, do nothing, they will fire again
        return;
    }

    QTimer::singleShot(3500, this, SLOT(thinkWhereToShoot()));
}


/*!
  Shuffles the given list to a random order.
*/
void AIPlayer::shuffleList(QList<DIRECTIONS> &list)
{
    if (list.size() < 2) {
        return;
    }

    if (list.size() == 2) {
        if (qrand() % 2) {
            qSwap(list[0], list[1]);
        }

        return;
    }

    for (int i=2; i<list.size(); i++) {
        qSwap(list[i], list[qrand() % i]);
    }
}


/*!
  Marks the given shot to the shot notes and shoots the enemy.
*/
void AIPlayer::shoot(const Shot &shot)
{
    m_Shots[shot.m_Col][shot.m_Row] = BOMB_ON_WAY;

    emit log(QString("AI shoots to %1, %2")
             .arg(QString('A' + shot.m_Row))
             .arg(QString::number(shot.m_Col + 1)));

    emit dropBomb(QVariant(shot.m_Row), QVariant(shot.m_Col));
}


/*!
  Tries to shoot to the left of the given shot.
*/
bool AIPlayer::tryToShootLeft(const Shot &shot, bool iterateHits)
{
    Shot leftShot = shot;

    if (iterateHits) {
        while (leftShot.m_Col > 0 && hasBeenShot(leftShot, HIT)) {
            leftShot.m_Col--;
        }
    }
    else if (leftShot.m_Col > 0) {
        leftShot.m_Col--;
    }

    if (hasBeenShot(leftShot, NOT_SHOT)) {
        shoot(leftShot);
        return true;
    }

    return false;
}


/*!
  Tries to shoot to the right of the given shot.
*/
bool AIPlayer::tryToShootRight(const Shot &shot, bool iterateHits)
{
    Shot rightShot = shot;

    if (iterateHits) {
        while (rightShot.m_Col < DIM-1 && hasBeenShot(rightShot, HIT)) {
            rightShot.m_Col++;
        }
    }
    else if (rightShot.m_Col < DIM-1) {
        rightShot.m_Col++;
    }

    if (hasBeenShot(rightShot, NOT_SHOT)) {
        shoot(rightShot);
        return true;
    }

    return false;
}


/*!
  Tries to shoot to the up of the given shot.
*/
bool AIPlayer::tryToShootUp(const Shot &shot, bool iterateHits)
{
    Shot upShot = shot;

    if (iterateHits) {
        while (upShot.m_Row > 0 && hasBeenShot(upShot, HIT)) {
            upShot.m_Row--;
        }
    }
    else if (upShot.m_Row > 0) {
        upShot.m_Row--;
    }

    if (hasBeenShot(upShot, NOT_SHOT)) {
        shoot(upShot);
        return true;
    }

    return false;
}


/*!
  Tries to shoot to the down of the given shot.
*/
bool AIPlayer::tryToShootDown(const Shot &shot, bool iterateHits)
{
    Shot downShot = shot;

    if (iterateHits) {
        while (downShot.m_Row < DIM-1 && hasBeenShot(downShot, HIT)) {
            downShot.m_Row++;
        }
    }
    else if (downShot.m_Row < DIM-1) {
        downShot.m_Row++;
    }

    if (hasBeenShot(downShot, NOT_SHOT)) {
        shoot(downShot);
        return true;
    }

    return false;
}


/*!
  Tries to determine the orientation of the ship by using the list
  of the hits to the ship(s) that is not sunk yet. This is not accurate
  because the hits might consist of many ships hits.
*/
bool AIPlayer::determineTheOrientationOfTheEnemyShip(bool &portrait)
{
    if (m_HitShots.size() < 2) {
        return false;
    }

    Shot s1 = m_HitShots[0];
    Shot s2 = m_HitShots[1];

    if (s1.m_Col == s2.m_Col && (abs(s1.m_Row - s2.m_Row) == 1)) {
        emit log("AI thinks that the ship is on portrait");
        portrait = true;
        return true;
    }

    if (s1.m_Row == s1.m_Row && (abs(s1.m_Col - s2.m_Col) == 1)) {
        emit log("AI thinks that the ship is on landscape");
        portrait = false;
        return true;
    }

    return false;
}


/*!
  The shooting logic. Contains three algorithms.
  1. If there are at least two hits to a ship that has not been sunk,
     determine the orientation of the ship, and shoot one of the endings
     of the ship.
  2. If 1. cannot be applied, shoot beside any possible place of the
     earlier hits to the ships that have not yet been sunk.
  3. If 2. cannot be applied, just shoot randomly.
*/
void AIPlayer::thinkWhereToShoot()
{

    emit log(QString("AI thinks where to shoot, hit shots %1")
             .arg(m_HitShots.size()));

    Shot shot;

    if (m_HitShots.size() > 0) {
        // There are hits to enemy ship that has not sunk yet, it
        // would be wise to try to hit near those hits.

        // Algorithm 1
        bool portrait;
        if (determineTheOrientationOfTheEnemyShip(portrait)) {
            QList<DIRECTIONS> directions;
            if (portrait) {
                directions << UP << DOWN;
            }
            else {
                directions << LEFT << RIGHT;
            }

            shuffleList(directions);

            for (int i=0; i<directions.size(); i++) {
                switch (directions[i]) {
                case UP:
                    if (tryToShootUp(m_HitShots[0], true)) {
                        return;
                    }
                    break;
                case DOWN:
                    if (tryToShootDown(m_HitShots[0], true)) {
                        return;
                    }
                    break;
                case LEFT:
                    if (tryToShootLeft(m_HitShots[0], true)) {
                        return;
                    }
                    break;
                case RIGHT:
                    if (tryToShootRight(m_HitShots[0], true)) {
                        return;
                    }
                    break;
                }
            }
        }

        // Algorithm 2
        QList<DIRECTIONS> directions;
        directions << LEFT << RIGHT << UP << DOWN;
        shuffleList(directions);

        QList<Shot>::iterator it;
        for(it = m_HitShots.begin(); it < m_HitShots.end(); it++) {
            for(int i=0; i<directions.size(); i++) {
                switch (directions[i]) {
                case LEFT:
                    if (tryToShootLeft(*it)) {
                        return;
                    }
                    break;
                case RIGHT:
                    if (tryToShootRight(*it)) {
                        return;
                    }
                    break;
                case UP:
                    if (tryToShootUp(*it)) {
                        return;
                    }
                    break;
                case DOWN:
                    if (tryToShootDown(*it)) {
                        return;
                    }
                    break;
                }
            }
        }
    }

    // Algorithm 3
    do {
        shot = Shot(rand() % DIM, rand() % DIM);
    }
    while (!canContainShip(shot));

    shoot(shot);
}


/*!
  We just hit the enemy's ship.
*/
void AIPlayer::youHit(bool youHit, int row, int column)
{
    if (row < 0 || row >= DIM || column < 0 || column >= DIM) {
        qDebug() << "ERROR: Enemy reported we hit outside the grid!";
        return;
    }

    if (youHit) {
        m_Shots[column][row] = HIT;
    }
    else {
        m_Shots[column][row] = MISS;
    }

    bool opponentShipsLeft = false;
    for (int i = 0; i < 4; i++)
        if (m_OpponentShips[i]) {
            opponentShipsLeft = true;
            break;
        }
    if (youHit && opponentShipsLeft) {
        if (!m_LastShotSunkShip) {
            m_HitShots.push_back(Shot(column, row));
        }
        else {
            m_LastShotSunkShip = false;
        }

        QTimer::singleShot(1600, this, SLOT(thinkWhereToShoot()));
    }
}


/*!
  The enemy reports that one of their ship has been sunk in given
  coordinates, length. Also the orientation of the ship is reported.
  The coordinates represents the top-left position of the ship.
*/
void AIPlayer::sunk(int length, int row, int column, bool portrait)
{
    // Remove the hits from m_HitShots of the sunken ship
    for (int i=0; i<length; i++) {
        if (portrait) {
            removeShotFromList(Shot(column, row + i));
        }
        else {
            removeShotFromList(Shot(column + i, row));
        }
    }

    m_LastShotSunkShip = true;

    m_OpponentShips[length-2]--;
}
