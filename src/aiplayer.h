/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QObject>
#include <QVariant>
#include "math.h"

/*!
  \class AIPlayer
  \brief Implements the AI for 1P opponent.
*/
class AIPlayer : public QObject
{
    Q_OBJECT
public:
    // The grid dimension and count of ships in the game
    enum {
        DIM = 10,
        SHIPS = 10,
        SHIPS2 = 4,
        SHIPS3 = 3,
        SHIPS4 = 2,
        SHIPS5 = 1
    };

    /*!
      \class Ship
      \brief Holds the information about one AI players ship such
             as ship length, orientation, position and hit count.
    */
    class Ship {
    public:
        int m_Length;
        int m_Col;
        int m_Row;
        bool m_Portrait;
        int m_Hits;

        /*!
          Tests if given coordinate hits to this ship.
        */
        bool testHit(int row, int col) const {
            if (m_Portrait)  {
                if (m_Col == col && m_Row <= row && row < m_Row + m_Length) {
                    return true;
                }
            }
            else if (m_Row == row && m_Col <= col && col < m_Col + m_Length) {
                return true;
            }

            return false;
        }

        /*!
          Tests if given ship overlaps with this ship.
        */
        bool overlaps(const Ship &s) const {
            for (int i=0; i<m_Length; i++) {
                if (m_Portrait) {
                    if (s.testHit(m_Row + i, m_Col)) {
                        return true;
                    }
                }
                else {
                    if (s.testHit(m_Row, m_Col + i)) {
                        return true;
                    }
                }
            }

            return false;
        }

        /*!
          Default constructor.
        */
        Ship()
        {
        }

        /*!
          Constructs ship.
        */
        Ship(int length, int col, int row, bool portrait)
            : m_Length(length),
              m_Col(col),
              m_Row(row),
              m_Portrait(portrait),
              m_Hits(0)
        {
        }
    };

    /*! \class Shot
        \brief Represents a single shot to a specific coordinate.
               Knows how to check if two Shots are equal.
    */
    class Shot {
    public:

        int m_Col;
        int m_Row;

        Shot()
            : m_Col(-1),
              m_Row(-1)
        {
        }

        Shot(int col, int row)
            : m_Col(col),
              m_Row(row)
        {
        }

        /*!
          Checks the equality of this and given shot.
        */
        bool operator==(const Shot &shot) const {
            if (shot.m_Col == m_Col && shot.m_Row == m_Row) {
                return true;
            }

            return false;
        }
    };

    explicit AIPlayer(QObject *parent = 0);
    bool placeShip(int length);
    bool addShipToContainerIfPossible(Ship &ship);

private:
    // Holds different shots types to grid cells.
    enum RESULT {
        NOT_SHOT = 0,
        BOMB_ON_WAY = 1,
        HIT = 2,
        MISS = 3
    };

    // Enum to differentiate the shooting directions.
    enum DIRECTIONS {
        LEFT, RIGHT, UP, DOWN
    };

    // Flag to tell that our last shot sunk the enemy ship.
    // This is used to notify youHit slot that the shot
    // will not be placed into the m_HitShots list.
    bool m_LastShotSunkShip;

    // Our ships.
    QList<Ship> m_Ships;

    // Our shots
    RESULT m_Shots[DIM][DIM];

    // Lists of shots that are hits to a ship(s) that are still floating.
    QList<Shot> m_HitShots;

    // Count of enemy ships left:
    // Index    Ship length
    // 0        2
    // 1        3
    // 2        4
    // 3        5
    int m_OpponentShips[4];
    void resetState();
    void removeShotFromList(const Shot &shot);
    void shuffleList(QList<DIRECTIONS> &list);
    bool determineTheOrientationOfTheEnemyShip(bool &portrait);
    bool hasBeenShot(const Shot &shot, RESULT result) const;
    void shoot(const Shot &shot);
    bool tryToShootLeft(const Shot &shot, bool iterateHits = false);
    bool tryToShootRight(const Shot &shot, bool iterateHits = false);
    bool tryToShootUp(const Shot &shot, bool iterateHits = false);
    bool tryToShootDown(const Shot &shot, bool iterateHits = false);
    bool canContainShip(const Shot &shot) const;

signals:
    void log(QVariant logText);
    void myShipsPlaced();
    void dropBomb(QVariant row, QVariant column);
    void gotHit(QVariant gotHit, QVariant row, QVariant column);
    void sinks(QVariant length, QVariant row, QVariant column, QVariant portrait);

public slots:
    void shipsPlaced();
    void bombDropped(int row, int column);
    void youHit(bool youHit, int row, int column);
    void sunk(int length, int row, int column, bool portrait);

    void thinkWhereToShoot();
};

#endif // AIPLAYER_H
