#include <chrono>
#include <queue>
#include <thread>
#include <cmath>

#include "league_of_rockets_lib.h"

int sign(double x) {
   if (abs(x) < 1e-6) {
      return 0;
   } else if (x < 0.0) {
      return -1;
   } else {
      return 1;
   }
}

int main(int argc, char *argv[]) {
   start(argc, argv);

   std::random_device rd;
   std::mt19937 gen(rd());

   while (true) {
      constants.getConstants();

      vector<Arena> arenas(static_cast<unsigned>(constants.maxMatches));
      vector<int> accel(static_cast<unsigned>(constants.maxMatches), -1);
      for (int i = 0; i < constants.maxMatches; ++i) {
         arenas[i].id = i + 1;
      }

      int selectedArenaIndex = 0;

      while (mainWindow.isOpen()) {
         int activeArenas, currentArena;
         getArena(activeArenas, currentArena);

         for (Arena &arena : arenas) {
            if (arena.id > activeArenas) {
               continue;
            }
            arena.selectThisArena();
            arena.getState();
            arena.getShips();
            arena.getRockets();
            arena.getExplosions();
            arena.getAsteroids();
            bool reverseDirection = false;
            Point *targetShip = nullptr;
            vector<bool> takenAsteroids(250, false);
            vector<bool> shipHalted(20, false);
            vector<Ship> enemyShips, myShips;
            for (Ship &ship : arena.ships) {
               if (ship.mine) {
                  myShips.push_back(ship);
               } else {
                  enemyShips.push_back(ship);
               }
            }
            if (arena.currentTurn < constants.turns / 2 && !enemyShips.empty()) {
               uniform_int_distribution<> dis(0, enemyShips.size() - 1);
               targetShip = &enemyShips[dis(gen)].position;
            }

            for (Ship &ship : myShips) {
               for (Ship &ship2 : myShips) {
                  if (ship.id != ship2.id
                      && ship.position.distance(ship2.position) < 2.0 * constants.spaceshipRadius + 10.0
                      && !shipHalted[ship2.id]) {
                     shipHalted[ship.id] = true;
                  }
               }
               if (ship.position.distance(Point(0, 0)) > 0.9 * constants.playfieldRadius
                   && sign(ship.position.x) == sign(ship.acceleration.x)) {
                  reverseDirection = true;
               }
               bool targetChosen = false;
               Point target(0, 0);
               if (targetShip != nullptr) {
                  targetChosen = true;
                  uniform_real_distribution<> dis(-constants.spaceshipRadius * 0.8, constants.spaceshipRadius * 0.8);
                  target.x = targetShip->x + dis(gen);
                  target.y = targetShip->y + dis(gen);
               }
               double bestAsteroidScore = 1e20;
               Asteroid *targetAsteroid = nullptr;
               bool tooClose = false;
               if (!targetChosen && !arena.asteroids.empty()) {
                  for (Asteroid &asteroid : arena.asteroids) {
                     if (ship.position.distance(asteroid.position)
                         < asteroid.radius + constants.explosionRadius + constants.spaceshipRadius + 15.0) {
                        tooClose = true;
                     }
                     double score = ship.position.distance(asteroid.position);
                     if (!enemyShips.empty()) {
                        score /= static_cast<float>(asteroid.pointsForDamage);
                     }
                     if (!takenAsteroids[asteroid.id] && (!targetChosen || score < bestAsteroidScore)) {
                        targetAsteroid = &asteroid;
                        target = asteroid.position;
                        bestAsteroidScore = score;
                        targetChosen = true;
                     }
                  }
               }
               if (targetAsteroid != nullptr) {
                  takenAsteroids[targetAsteroid->id] = true;
                  --targetAsteroid->newHealth;
                  if (enemyShips.empty()) {
                     if (!arena.shipTargetsAsteroid[ship.id]) {
                        arena.shipTargetsAsteroid[ship.id] = true;
                        ship.setShipBrake();
                     }
                     if (ship.position.distance(targetAsteroid->position)
                         > targetAsteroid->radius + constants.explosionRadius + constants.spaceshipRadius + 30.0) {
                        ship.setShipEngine(Point(targetAsteroid->position.x - ship.position.x,
                              targetAsteroid->position.y - ship.position.y));
                     } else {
                        ship.setShipEngine(Point(0, 0));
                        ship.setShipBrake();
                     }
                  }
               }
               if (ship.turnsUntilRocket == 0) {
                  Point velocity(target.x - ship.position.x, target.y - ship.position.y);
                  ship.launchRocket(velocity, copysign(acos(velocity.x / velocity.length()), velocity.y), 0);
               }
               if (tooClose || shipHalted[ship.id]) {
                  ship.setShipBrake();
                  ship.setShipEngine(Point(0, 0));
               }
            }
            if (!enemyShips.empty() && (reverseDirection || arena.direction == 0)) {
               if (arena.direction == 0) {
                  arena.direction = -1;
               } else {
                  arena.direction = -arena.direction;
               }
               for (Ship &ship : arena.ships) {
                  if (ship.mine) {
                     ship.setShipBrake();
                     ship.setShipEngine(Point(copysign(10000, arena.direction), 0));
                  }
               }
            }
         }

         // START: DRAWING
         if (selectedArenaIndex < 0 || selectedArenaIndex >= constants.maxMatches) {
            selectedArenaIndex = 0;
         }
         Arena *selectedArena = &arenas[selectedArenaIndex];

         mainWindow.clear();

         drawCircle(0, 0, constants.playfieldRadius, Color(128, 128, 128), constants.playfieldRadius);
         for (Asteroid &asteroid : selectedArena->asteroids) {
            drawCircle(
                  asteroid.position.x,
                  asteroid.position.y,
                  asteroid.radius,
                  Color(150, 75, 0),
                  constants.playfieldRadius);
            drawTextAtPosition(
                  asteroid.position.x,
                  asteroid.position.y,
                  to_string(asteroid.health) + "; " + to_string(asteroid.pointsForDamage),
                  Color::White,
                  12,
                  constants.playfieldRadius);
         }
         for (Explosion &explosion : selectedArena->explosions) {
            drawCircle(
                  explosion.position.x,
                  explosion.position.y,
                  constants.explosionRadius,
                  explosion.mine ? Color::Yellow : Color(255, 165, 0),
                  constants.playfieldRadius);
         }
         for (Ship &ship : selectedArena->ships) {
            drawCircle(
                  ship.position.x,
                  ship.position.y,
                  constants.spaceshipRadius,
                  ship.mine ? Color::Green : Color::Red,
                  constants.playfieldRadius);
         }
         for (Rocket &rocket : selectedArena->rockets) {
            drawCircle(
                  rocket.position.x,
                  rocket.position.y,
                  constants.playfieldRadius / 200.0,
                  rocket.mine ? Color(0, 102, 51) : Color(128, 0, 0),
                  constants.playfieldRadius);
         }

         string turnsLeftString = "Turn: " + to_string(selectedArena->currentTurn) + '/' + to_string(constants.turns);
         drawText(turnsLeftString, Color::Red, 30, botV, rightH, 25);

         string arenaString = "Arena: " + to_string(selectedArena->id) + '/' + to_string(activeArenas)
                              + '/' + to_string(constants.maxMatches);
         drawText(arenaString, Color::Red, 30, botV, leftH, 25);

         string scoreString = "Score: " + to_string(selectedArena->currentScore);
         drawText(scoreString, Color::Red, 30, topV, leftH, 25);

         mainWindow.display();
         // END: DRAWING

         // START: EVENTS
         Event ev;
         while (mainWindow.pollEvent(ev)) {
            if (ev.type == Event::Closed) {
               return 0;
            } else if (ev.type == Event::KeyPressed) {
               if (ev.key.code == Keyboard::Q) {
                  return 0;
               } else if (ev.key.code >= Keyboard::Num0 && ev.key.code <= Keyboard::Num9) {
                  selectedArenaIndex = ev.key.code - Keyboard::Num0 - 1;
               } else if (ev.key.code >= Keyboard::Numpad0 && ev.key.code <= Keyboard::Numpad9) {
                  selectedArenaIndex = ev.key.code - Keyboard::Numpad0 - 1;
               }
            }
         }
         // END: EVENTS

         wait();

         if (arenas[0].currentTurn >= constants.turns - 5)
            break;
      }
   }
}
