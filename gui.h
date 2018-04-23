#ifndef GUI_H
#define GUI_H

#include "sockets.h"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

const int WINDOW_SIZE = 1000;  // px

enum alignVertical { topV, centerV, botV };

enum alignHorizontal { leftH, centerH, rightH };

RenderWindow mainWindow;
Font font;

void start(int argc, char *argv[]) {
   if (argc != 3) {
      printf("Usage: %s <ip> <port>\n", argv[0]);
      exit(0);
   }
   init(argv[1], atoi(argv[2]));
   loginFromFile();
   char windowTitle[100];
   sprintf(windowTitle, "%s %s", argv[1], argv[2]);
   mainWindow.create(VideoMode(WINDOW_SIZE, WINDOW_SIZE), windowTitle);
   font.loadFromFile("../DroidSansMono.ttf");
}

void drawTextAtPosition(double x, double y, string text, Color color, int size, double playfieldRadius) {
   Text temp;
   temp.setFont(font);
   temp.setString(text);
   temp.setColor(color);
   temp.setCharacterSize(size);
   FloatRect tempRect = temp.getLocalBounds();
   double windowMultiplier = (static_cast<double>(WINDOW_SIZE) / (playfieldRadius * 2.0));
   x *= windowMultiplier;
   y *= windowMultiplier;
   x -= tempRect.width / 2.0;
   y -= tempRect.height / 2.0;
   temp.setPosition(x + playfieldRadius, y + playfieldRadius);
   mainWindow.draw(temp);
}

void drawText(string text, Color color, int size, alignVertical vertical, alignHorizontal horizontal, double margin) {
   Text temp;
   temp.setFont(font);
   temp.setString(text);
   temp.setColor(color);
   temp.setCharacterSize(size);
   FloatRect tempRect = temp.getLocalBounds();
   double posX = 0, posY = 0;
   switch (horizontal) {
      case leftH:
         posX = margin;
         break;
      case centerH:
         posX = (WINDOW_SIZE - tempRect.width) / 2;
         break;
      case rightH:
         posX = WINDOW_SIZE - tempRect.width - margin;
         break;
   }
   switch (vertical) {
      case topV:
         posY = margin;
         break;
      case centerV:
         posY = (WINDOW_SIZE - tempRect.height) / 2;
         break;
      case botV:
         posY = WINDOW_SIZE - tempRect.height - margin;
         break;
   }
   temp.setPosition(posX, posY);
   mainWindow.draw(temp);
}


void drawCircle(double x, double y, double radius, Color color, double playfieldRadius) {
   CircleShape shape;
   x += playfieldRadius;
   y += playfieldRadius;
   double windowMultiplier = (static_cast<double>(WINDOW_SIZE) / (playfieldRadius * 2.0));
   double pixelRadius = radius * windowMultiplier;
   shape.setRadius(pixelRadius);
   shape.setPosition((x - radius) * windowMultiplier, (y - radius) * windowMultiplier);
   shape.setFillColor(color);
   if (pixelRadius >= WINDOW_SIZE / 3) {
      shape.setPointCount(100);
   }
   mainWindow.draw(shape);
}


#endif  // GUI_H
