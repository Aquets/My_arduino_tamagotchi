#include <Arduino.h>
#include <EEPROM.h>
#include "Alegotchi.h"

Alegotchi::Alegotchi(bool _sleeping, int _sleep, int _happiness, int _hunger, int _age, int _beardLength, int _expression, int _clothing){
  sleeping = _sleeping;
  sleep = _sleep;
  happiness = _happiness;
  hunger = _hunger;
  age = _age;
  beardLength = _beardLength;
  expression = _expression;
  clothing = _clothing;
}

// SLEEP/WAKE-UP FUNCTION
void Alegotchi::updateSleeping(){
  sleeping = !sleeping;
  EEPROM.write(0, sleeping);
}

//__________________________________ UPDATE STATS FUNCTIONS

void Alegotchi::updateSleep(int sleepVal){
  sleep += sleepVal;
  if(sleep < 0){
    sleep = 0;
  }else if(sleep > 24){
    sleep = 24;
  }
  EEPROM.write(1, sleep);
}

void Alegotchi::updateHappiness(int happinessVal){
  happiness += happinessVal;
  if(happiness < 0){
    happiness = 0;
  }else if(happiness > 24){
    happiness = 24;
  }
  EEPROM.write(2, happiness);
}

void Alegotchi::updateHunger(int hungerVal){
  hunger += hungerVal;
  if(hunger < 0){
    hunger = 0;
  }else if(hunger > 24){
    hunger = 24;
  }
  EEPROM.write(3, hunger);
}

//__________________________________ UPDATE APPEARANCE FUNCTIONS

void Alegotchi::updateAge(){
  age ++;
  EEPROM.put(10, age);
}

void Alegotchi::updateBeardLength(int newBeardLength){
  beardLength = newBeardLength;
  EEPROM.write(5, beardLength);
}

void Alegotchi::updateExpression(int newExpression){
  expression = newExpression;
  EEPROM.write(6, expression);
}

void Alegotchi::updateClothing(int newClothing){
  clothing = newClothing;
  EEPROM.write(7, clothing);
}

//__________________________________ MAP VALUES TO PIXELS

int Alegotchi::mapValue(int val, int barLength){
  int statLength = (val * barLength-2) / 24;
  return statLength;
}
