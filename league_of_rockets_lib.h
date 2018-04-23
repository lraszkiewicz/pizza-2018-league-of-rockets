#ifndef MAZE_LIB_H
#define MAZE_LIB_H

#include <cmath>

#include "gui.h"

class GameConstants;
class Arena;
class Point;
class Ship;
class Rocket;
class Explosion;
class Asteroid;

void getArena(int &matches, int &current_match);

class GameConstants {
 public:
   double playfieldRadius = -1, spaceshipRadius = -1, explosionRadius = -1;
   int maxRocketCommands = -1, maxMatches = -1;
   double maxSpaceshipAccel = -1, maxRocketAccel = -1, maxRocketAngAccel = -1, maxRocketStartVelocity = -1,
          maxRocketStartAngAccel = -1;
   int spaceshipDestroyPoints = -1, rocketLaunchCooldown = -1, turns = -1;

   void getConstants();
};

GameConstants constants;

class Arena {
 public:
   int id, currentTurn, currentScore;

   void selectThisArena();
   void getState();
   void getShips();
   void getRockets();
   void getExplosions();
   void getAsteroids();

   vector<Ship> ships;
   vector<Rocket> rockets;
   vector<Explosion> explosions;
   vector<Asteroid> asteroids;

   int direction = 0;
   vector<bool> shipTargetsAsteroid = vector<bool>(50, false);
};

class Point {
 public:
   double x, y;

   Point(double x, double y);

   double length() {
      return sqrt(x * x + y * y);
   }

   double distance(Point p) {
      return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
   }
};

class Ship {
 public:
   int id;
   bool mine;
   Point position, velocity, acceleration;
   int turnsUntilRocket;

   Ship(int id, bool mine, double x, double y, double velX, double velY, double accelX, double accelY,
         int turnsUntilRocket);

   void setShipEngine(Point accel);
   void setShipBrake();
   void launchRocket(Point startVelocity, double startAngle, double startAngularVelocity);
};

class Rocket {
 public:
   int id;
   bool mine;
   Point position;
   double angle;
   Point velocity;
   double angularVelocity, enginePower, thrustersPower;

   Rocket(int id, bool mine, double x, double y, double angle, double velX, double velY, double angularVelocity,
         double enginePower, double thrustersPower);
};

class Explosion {
 public:
   int id;
   bool mine;
   Point position;

   Explosion(int id, bool mine, double x, double y);
};

class Asteroid {
 public:
   int id;
   Point position;
   double radius;
   int health, pointsForDamage;
   int newHealth;

   Asteroid(int id, double x, double y, double radius, int health, int pointsForDamage);
};

void getArena(int &matches, int &current_match) {
   qprintf("GET_ARENA\n");
   checkOk();
   qscanf("%d %d", &matches, &current_match);
}

void GameConstants::getConstants() {
   qprintf("GET_CONSTANTS\n");
   checkOk();
   qscanf("%lf%lf%lf%d%d%lf%lf%lf%lf%lf%d%d%d", &playfieldRadius, &spaceshipRadius, &explosionRadius,
         &maxRocketCommands, &maxMatches, &maxSpaceshipAccel, &maxRocketAccel, &maxRocketAngAccel,
         &maxRocketStartVelocity, &maxRocketStartAngAccel, &spaceshipDestroyPoints, &rocketLaunchCooldown, &turns);
   printf("Points for destroying a spaceship: %d\n", spaceshipDestroyPoints);
   printf("Spaceship radius: %lf\n", spaceshipRadius);
   printf("Playfield radius: %lf\n", playfieldRadius);
}

void Arena::selectThisArena() {
   qprintf("SELECT_ARENA %d\n", id);
   checkOk();
}

void Arena::getState() {
   qprintf("GET_STATE\n");
   checkOk();
   qscanf("%d%d", &currentTurn, &currentScore);
}

void Arena::getShips() {
   qprintf("GET_SHIPS\n");
   checkOk();
   ships.clear();
   int numberOfShips;
   qscanf("%d", &numberOfShips);
   for (int i = 0; i < numberOfShips; ++i) {
      int id, mine;
      double x, y, velX, velY, accelX, accelY;
      int turnsUntilRocket;
      qscanf("%d%d%lf%lf%lf%lf%lf%lf%d", &id, &mine, &x, &y, &velX, &velY, &accelX, &accelY, &turnsUntilRocket);
      ships.emplace_back(id, mine, x, y, velX, velY, accelX, accelY, turnsUntilRocket);
   }
}

void Arena::getRockets() {
   qprintf("GET_ROCKETS\n");
   checkOk();
   rockets.clear();
   int numberOfRockets;
   qscanf("%d", &numberOfRockets);
   for (int i = 0; i < numberOfRockets; ++i) {
      int id, mine;
      double x, y, angle, velX, velY, angularVelocity, enginePower, thrustersPower;
      qscanf("%d%d%lf%lf%lf%lf%lf%lf%lf%lf", &id, &mine, &x, &y, &angle, &velX, &velY, &angularVelocity, &enginePower,
            &thrustersPower);
      rockets.emplace_back(id, mine, x, y, angle, velX, velY, angularVelocity, enginePower, thrustersPower);
   }
}

void Arena::getExplosions() {
   qprintf("GET_EXPLOSIONS\n");
   checkOk();
   explosions.clear();
   int numberOfExplosions;
   qscanf("%d", &numberOfExplosions);
   for (int i = 0; i < numberOfExplosions; ++i) {
      int id, mine;
      double x, y;
      qscanf("%d%d%lf%lf", &id, &mine, &x, &y);
      explosions.emplace_back(id, mine, x, y);
   }
}

void Arena::getAsteroids() {
   qprintf("GET_ASTEROIDS\n");
   checkOk();
   asteroids.clear();
   int numberOfAsteroids;
   qscanf("%d", &numberOfAsteroids);
   for (int i = 0; i < numberOfAsteroids; ++i) {
      int id;
      double x, y, radius;
      int health, pointsForDamage;
      qscanf("%d%lf%lf%lf%d%d", &id, &x, &y, &radius, &health, &pointsForDamage);
      asteroids.emplace_back(id, x, y, radius, health, pointsForDamage);
   }
}

Point::Point(double x, double y) : x(x), y(y) {}

Ship::Ship(int id, bool mine, double x, double y, double velX, double velY, double accelX, double accelY,
      int turnsUntilRocket)
      : id(id), mine(mine), position(x, y), velocity(velX, velY), acceleration(accelX, accelY),
        turnsUntilRocket(turnsUntilRocket) {}

void Ship::setShipEngine(Point accel) {
   qprintf("SET_SHIP_ENGINE %d %.5lf %.5lf\n", id, accel.x, accel.y);
   checkOk();
}

void Ship::setShipBrake() {
   qprintf("SET_SHIP_BRAKE %d\n", id);
   checkOk();
}

void Ship::launchRocket(Point startVelocity, double startAngle, double startAngularVelocity) {
   qprintf("LAUNCH_ROCKET %d %.5lf %.5lf %.5lf %.5lf\n", id, startVelocity.x, startVelocity.y, startAngle,
         startAngularVelocity);
   checkOk();
   qprintf("SET_ENGINE 1.0\n");
   qprintf("END\n");
   checkOk();
   int rocketId;
   qscanf("%d", &rocketId);
}

Rocket::Rocket(int id, bool mine, double x, double y, double angle, double velX, double velY, double angularVelocity,
      double enginePower, double thrustersPower)
      : id(id), mine(mine), position(x, y), angle(angle), velocity(velX, velY), angularVelocity(angularVelocity),
        enginePower(enginePower), thrustersPower(thrustersPower) {}

Explosion::Explosion(int id, bool mine, double x, double y) : id(id), mine(mine), position(x, y) {}

Asteroid::Asteroid(int id, double x, double y, double radius, int health, int pointsForDamage)
      : id(id), position(x, y), radius(radius), health(health), pointsForDamage(pointsForDamage), newHealth(health) {}

int getTurnsLeft() {
   qprintf("TURNS_LEFT\n");
   checkOk();
   int tmp;
   qscanf("%d", &tmp);
   return tmp;
}

int move(int moveX, int moveY, int *x, int *y) {
   qprintf("MOVE %d %d\n", moveX, moveY);
   int code = checkOk();
   if (code == 0) {
      *x += moveX;
      *y += moveY;
   }
   return code;
}

#endif  // MAZE_LIB_H
