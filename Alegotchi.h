/*
 * Alegotchi.h - Library for tamagotchi Ale instance.
 * Created by Alessandro Quets, Apri 04, 2022.
 * Relased into the public domain.
 */

#ifndef Alegotchi_h
#define Alegotchi_h

#include <Arduino.h>

class Alegotchi
{
  public:
  Alegotchi(bool _sleeping, int _sleep, int _happiness, int _hunger, int _age, int _beardLength, int _expression, int _clothing);

  bool sleeping; // address: 0
  
  int sleep; // address: 1
  int happiness; // address: 2
  int hunger; // address: 3

  int age; // address: 4
  int beardLength; // address: 5
  int expression; // address: 6
  int clothing; // address: 7

  void updateSleeping();

  void updateSleep(int sleepVal);
  void updateHappiness(int happinessVal);
  void updateHunger(int hungerVal);

  void updateAge();
  
  void updateBeardLength(int newBeardLength);
  void updateExpression(int newExpression);
  void updateClothing(int newClothing);

  int mapValue(int val, int barLength);
};

#endif
