#include <Arduino.h>
#include "Scrolling_menu.h"


Scrolling_menu::Scrolling_menu(int posX, int posY, int menuSize){
  _menuSize = menuSize - 1;
  x = posX;
  y = posY;
}

void Scrolling_menu::moveRight(){
  if (nextSection == _menuSize){
    nextSection = 0;
    activeSection++;
    prevSection++;
  }else if (activeSection == _menuSize){
    nextSection++;
    activeSection = 0;
    prevSection++;
  }else if (prevSection == _menuSize){
    nextSection++;
    activeSection++;
    prevSection = 0;
  }else {
    nextSection++;
    activeSection++;
    prevSection++;
  }
}

void Scrolling_menu::moveLeft(){
  if (prevSection == 0){
    nextSection--;
    activeSection--;
    prevSection = _menuSize;
  }else if (activeSection == 0){
    nextSection--;
    activeSection = _menuSize;
    prevSection--;
  }else if (nextSection == 0){
    nextSection = _menuSize;
    activeSection--;
    prevSection--;
  }else {
    nextSection--;
    activeSection--;
    prevSection--;
  }
}
