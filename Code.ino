
/*
   Collegare un display OLED ad Arduino

   Autore  : Andrea Lombardo
   Web     : http://www.lombardoandrea.com
   Post    : https://wp.me/p27dYH-OS
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Libreria per memoria non volatile
#include <EEPROM.h>

// Librerie per sleepmode e watchdog
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Libreria per suoni buzzer
#include <CuteBuzzerSounds.h>

// Librerie custom per Alegotchi
#include "Scrolling_menu.h"
#include "Alegotchi.h"
#include "Sprites.h"


/*
   Definisco le dimensioni del display, serviranno per creare l'istanza del display
   e magari riutilizzarle nel codice qualora dovessero servirmi queste informazioni
*/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUZZER_PIN 5
#define SWITCH_SCREEN 2
#define LEFT 11
#define SELECT 12
#define RIGHT 10

// Creo istanza del display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void setup() {

  Serial.begin(9600);
  Serial.println("BOOT");

  //Provo ad inizializzare il display all'indirizzo 0x3C (il tuo potrebbe essere diverso)
  if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("I2C ERROR");
    while (true);
  }

  // Pulisco il buffer
  display.clearDisplay();
  display.display();

  delay(100);

  pinMode(SWITCH_SCREEN, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);

  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3;
  WDTCSR |= _BV(WDIE);

  cute.init(BUZZER_PIN);

  Serial.println("START");
  delay(1000);

  if (digitalRead(SWITCH_SCREEN) == LOW){
    displayAle();
    displayMenuFunc();
    displayStats();
    display.display();
    cute.play(S_CONNECTION);
  }
}

//________________________ TAMAGOTCHI STATES

int state = 0;

// 0 = Home
// 1 = Sleep
// 2 = Dress
// 3 = Stats

char menuSections[][20] = {"Cut", "Dress", "Eat", "Stats", "Play", "Sleep"};

const unsigned char* menuIcons[] = {
  beard_icon,
  dress_icon,
  eat_icon,
  stats_icon,
  play_icon,
  sleep_icon
};

const unsigned char* bodyArray[] = {
  myBitmapbody_01,
  myBitmapbody_02,
  myBitmapbody_03,
  myBitmapbody_04
};

const unsigned char* menuDressIcons[] = {
  backHome,
  myBitmapbody_01,
  myBitmapbody_02,
  myBitmapbody_03,
  myBitmapbody_04
};

Scrolling_menu menu(58, 32, 6);
Scrolling_menu menuDress(58, 32, 5);
char activeState[20] = "Home";

bool sleeping = EEPROM.read(0);
  
int sleep = EEPROM.read(1);
int happiness = EEPROM.read(2);
int hunger = EEPROM.read(3);

int var_type = 0; // Serve per settare la tipologia di dato da recuperare (in questo caso int)
int age = EEPROM.get(10, var_type);
int beardLength = EEPROM.read(5);
int expression = EEPROM.read(6);
int clothing = EEPROM.read(7);

Alegotchi alegotchi(sleeping, sleep, happiness, hunger, age, beardLength, expression, clothing);


int sec = 0;
bool pressed = false;
int frameCount = 0;
int spriteOffset = -1;

void loop() {
  if (digitalRead(SWITCH_SCREEN) == HIGH){
    turnOff();
  }
  else{

    stateSelection();
    delay(10);
  }
  
}

void stateSelection() {
  if(state == 0){
    homeFunc();
  }else if (state == 1){
    sleepFunc();
  }else if (state == 2){
    dressFunc();    
  }else if (state == 3){
    statsFunc();    
  }
}

//__________________________________________________ STATES MANAGEMENT

void changeState(char section[20]){

  display.invertDisplay(true);
  delay(50);
  display.invertDisplay(false);
  
  Serial.println(section);
  if(strcmp(section, "Sleep") == 0){
    alegotchi.updateSleeping();
    alegotchi.updateHappiness(2);
    alegotchi.updateSleep(1);
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    state = 1;
  }else if(strcmp(section, "Play") == 0){
    alegotchi.updateHappiness(4);
    alegotchi.updateSleep(-2);
    alegotchi.updateHunger(-2);
    displayPlay();    
  }else if(strcmp(section, "Stats") == 0){
    displayAge();
    state = 3;
  }else if(strcmp(section, "Eat") == 0){
    alegotchi.updateHunger(5);
    alegotchi.updateHappiness(1);
    displayEat();
  }else if(strcmp(section, "Cut") == 0){
    alegotchi.updateBeardLength(0);
    if(alegotchi.beardLength >= 24){
      alegotchi.updateHappiness(10);
    }
    displayCut();
  }else if(strcmp(section, "Dress") == 0){
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    alegotchi.updateHappiness(2);
    displayMenuDress();
    state = 2;
  }else if(strcmp(section, "Home") == 0){
    frameCount = 50;
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    displayMenuFunc();
    displayAle();
    state = 0;
  }
  for (int i = 0; i < 20; i++){
   activeState[i] = section[i]; 
  }
  displayStats();
}

//__________________________________________________ STATES FUNCTIONS

//____________________________ HOME

void homeFunc() {
    
  if(pressed == false){
        
    if(digitalRead(SELECT) == LOW){

      cute.play(S_CONNECTION);
      pressed = true;
      changeState(menuSections[menu.activeSection]);
      
    }else if (digitalRead(LEFT) == LOW){

      cute.play(S_CONNECTION);
      menu.moveLeft();
      displayMenuFunc();
      pressed = true;

    }else if (digitalRead(RIGHT) == LOW){

      cute.play(S_CONNECTION);
      menu.moveRight();
      displayMenuFunc();
      pressed = true;

    }
  }else if(digitalRead(SELECT) == HIGH && digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == HIGH){
    pressed = false;
  }
  displayAle(); 
  
}

//____________________________ SLEEP

void sleepFunc() {

  if(!alegotchi.sleeping){
    changeState("Home");
  }
    
  if(pressed == false){
        
    if(digitalRead(SELECT) == LOW){

      pressed = true;

      alegotchi.updateSleeping();
      alegotchi.updateHappiness(-3);
      changeState("Home");
      cute.play(S_FART1);       
      
    }
  }else if(digitalRead(SELECT) == HIGH && digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == HIGH){
    pressed = false;
  }
  displaySleep();

}

//____________________________ DRESS

void dressFunc(){

  if(pressed == false){
        
    if(digitalRead(SELECT) == LOW){

      cute.play(S_CONNECTION);
      pressed = true;

      if(menuDress.activeSection != 0){
        alegotchi.updateClothing(menuDress.activeSection - 1);
      }
      changeState("Home");
      
    }else if (digitalRead(LEFT) == LOW){

      cute.play(S_CONNECTION);
      menuDress.moveLeft();
      displayMenuDress();
      pressed = true;

    }else if (digitalRead(RIGHT) == LOW){

      cute.play(S_CONNECTION);
      menuDress.moveRight();
      displayMenuDress();
      pressed = true;

    }
    
  }else if(digitalRead(SELECT) == HIGH && digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == HIGH){
    pressed = false;
  }
}

//____________________________ DRESS

void statsFunc(){
  if(pressed == false){
        
    if(digitalRead(SELECT) == LOW){

      pressed = true;
      changeState("Home");
      cute.play(S_CONNECTION);       
      
    }
  }else if(digitalRead(SELECT) == HIGH && digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == HIGH){
    pressed = false;
  }  
}

//__________________________________________________ MAIN MENU

void displayMenuFunc(){
  int y = 17;
  int height = 16;

  display.fillRect(51, 17, 77, 47, SSD1306_BLACK);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  
  display.setTextSize(1);

  //PREV SECTION
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.prevSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.prevSection], 10, 10, 1);

  //ACTIVE SECTION
  y = y + height;
  display.drawRoundRect(menu.x - 5, y, 73, 16, 3, SSD1306_WHITE);
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.activeSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.activeSection], 10, 10, 1);

  //NEXT SECTION
  y = y + height;
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.nextSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.nextSection], 10, 10, 1);

  display.display();
}

//__________________________________________________ DRESS MENU

void displayMenuDress(){
  display.fillRect(1, 17, 127, 47, SSD1306_BLACK);

  // PREV CLOTHING
  display.drawBitmap(3, 17, menuDressIcons[menuDress.prevSection], 32, 46, SSD1306_WHITE);

  // DISPLAY ALE WITH CHANGING CLOTHING
  display.drawRoundRect(39, 16, 50, 48, 5, SSD1306_WHITE);
  
  int spriteX = 39 + 10;
  
  if(menuDress.activeSection != 0){
    display.fillRect(spriteX, 18, 32, 46, SSD1306_BLACK);
    display.drawBitmap(spriteX, 18, menuDressIcons[menuDress.activeSection], 32, 46, SSD1306_WHITE);
  
    display.drawBitmap(spriteX, 18, feet_bg, 32, 46, SSD1306_BLACK);
    display.drawBitmap(spriteX, 18, feet, 32, 46, SSD1306_WHITE);
    
    display.drawBitmap(spriteX, 18, head_bg, 32, 46, SSD1306_BLACK);
    display.drawBitmap(spriteX, 18, head, 32, 46, SSD1306_WHITE);
    
    if(alegotchi.beardLength >= 24){
      display.drawBitmap(10, 18 + spriteOffset, beard_bg, 32, 46, SSD1306_BLACK);
      display.drawBitmap(10, 18 + spriteOffset, beard, 32, 46, SSD1306_WHITE);
    }
    
    display.drawBitmap(spriteX, 18, eyes, 32, 46, SSD1306_BLACK);
  }else{
    display.fillRect(spriteX, 18, 32, 46, SSD1306_BLACK);
    display.drawBitmap(spriteX, 18, menuDressIcons[menuDress.activeSection], 32, 46, SSD1306_WHITE);
  }
  
  
  // NEXT CLOTHING
  display.drawBitmap(93, 17, menuDressIcons[menuDress.nextSection], 32, 46, SSD1306_WHITE);
  
  display.display();
}

//__________________________________________________ ALE SPRITES

void displayAle(){

  if(frameCount >= 50){
    display.drawRoundRect(0, 16, 50, 48, 5, SSD1306_WHITE);
    
    display.fillRect(10, 18, 32, 46, SSD1306_BLACK);
    
    display.drawBitmap(10, 18 + spriteOffset, bodyArray[alegotchi.clothing], 32, 46, SSD1306_WHITE);
  
    display.drawBitmap(10, 18, feet_bg, 32, 46, SSD1306_BLACK);
    display.drawBitmap(10, 18, feet, 32, 46, SSD1306_WHITE);

    display.drawBitmap(10, 18 + spriteOffset, head_bg, 32, 46, SSD1306_BLACK);
    display.drawBitmap(10, 18 + spriteOffset, head, 32, 46, SSD1306_WHITE);
    
    if(alegotchi.beardLength >= 24){
      display.drawBitmap(10, 18 + spriteOffset, beard_bg, 32, 46, SSD1306_BLACK);
      display.drawBitmap(10, 18 + spriteOffset, beard, 32, 46, SSD1306_WHITE);
    }

    display.drawBitmap(10, 18 + spriteOffset, eyes, 32, 46, SSD1306_BLACK);
      
    display.display();
    if(spriteOffset == -1){
      spriteOffset = -2;
    }else{   
      spriteOffset = -1;
    }
    frameCount = 0;
  }
  frameCount++;
    
}

//__________________________________________________ SLEEPING SPRITES

void displaySleep(){

  if(frameCount >= 50){
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(64 - spriteOffset, 33 - spriteOffset);
    display.println("Z");
    display.setCursor(59 - !spriteOffset, 46 - !spriteOffset);
    display.println("Z");
    display.setTextSize(2);
    display.setCursor(47 - spriteOffset, 26 - !spriteOffset);
    display.println("Z");
      
    display.display();
    spriteOffset = !spriteOffset;
    frameCount = 0;
  }
  frameCount++;
    
}

//__________________________________________________ EAT ANIMATION

void selectEatFrame(int x, int y, int n){
  if(alegotchi.beardLength >= 24){
    if(n == 1){
      display.drawBitmap(x, y, eat_anim_head_b_1, 45, 40, SSD1306_WHITE);
    }else{
      display.drawBitmap(x, y, eat_anim_head_b_2, 45, 40, SSD1306_WHITE);
    }  
  }else{
    if(n == 1){
      display.drawBitmap(x, y, eat_anim_head_n_1, 45, 40, SSD1306_WHITE);
    }else{
      display.drawBitmap(x, y, eat_anim_head_n_2, 45, 40, SSD1306_WHITE);
    }    
  }
  
}

void displayEat(){

  int startX = 30;
  int startY = 20;

  display.fillRect(0, 16, 128, 48, SSD1306_BLACK); // Pulisco lo schermo

  // Frame 1
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_1, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 2
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2); 
  display.display();

  delay(500);

  // Frame 3
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_2, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 4
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2);
  display.display();

  delay(500);

  // Frame 5
  display.fillRect(0, 17, 128, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_3, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 6
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2);
  display.display();

  delay(500);

  // Frame 7
  display.fillRect(0, 17, 128, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.display();

  delay(500);
  cute.play(S_JUMP);
  delay(500);

  changeState("Home");
  
}

//__________________________________________________ PLAY ANIMATION

void displayPlay(){
  int startX = 50;
  int startY = 17;
  int loop = 2;
  
  for (int i = 0; i <= loop; i++) {
    // Frame 1
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK); // Pulisco lo schermo 
    display.drawBitmap(startX, startY, play_anim_1, 27, 48, SSD1306_WHITE);
    display.display(); 
    delay(200);

    // Frame 2
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_2, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 3
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_3, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 4
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_4, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 5
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_5, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);
  }
  
  delay(500);
  cute.play(S_JUMP);
  delay(500);

  changeState("Home");
  
}

//__________________________________________________ CUT ANIMATION

void displayCut(){
  int startX = 45;
  int startY = 24;
  int loop = 2;
  
  for (int i = 0; i <= loop; i++) {
    // Frame 1
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK); // Pulisco lo schermo 
    display.drawBitmap(startX, startY, cut_anim_1, 37, 28, SSD1306_WHITE);
    display.display(); 
    delay(500);

    // Frame 2
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, cut_anim_2, 37, 28, SSD1306_WHITE);
    display.display();
    delay(500);

  }
  
  delay(500);
  cute.play(S_JUMP);
  delay(500);

  changeState("Home");
  
}

//__________________________________________________ DISPLAY AGE

void displayAge(){
  display.fillRect(0, 16, 128, 48, SSD1306_BLACK);

  display.setTextSize(1);
  
  display.setCursor(10,20);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Name");
  display.setTextColor(SSD1306_WHITE);
  display.println(" Asnelandre");
  
  display.setCursor(10,32);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Age");
  display.setTextColor(SSD1306_WHITE);
  display.print(" ");
  display.print(alegotchi.age/24);
  display.println(" days");

  display.setCursor(10,44);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Weight");
  display.setTextColor(SSD1306_WHITE);
  display.println(" 67 kg");
}


//__________________________________________________ STATS DISPLAY

void displayStats(){

  display.fillRect(1, 1, 127, 15, SSD1306_BLACK);
  display.setTextSize(1);

  // HAPPINESS STATS
  display.setCursor(2,5);
  display.println("H");
  display.drawRoundRect(9, 6, 27, 6, 1, SSD1306_WHITE);
  int H = alegotchi.mapValue(alegotchi.happiness, 27);
  display.fillRect(10, 7, H, 4, SSD1306_WHITE);

  // SLEEP STATS
  display.setCursor(43,5);
  display.println("S");
  display.drawRoundRect(50, 6, 27, 6, 1, SSD1306_WHITE);
  int S = alegotchi.mapValue(alegotchi.sleep, 27);
  display.fillRect(51, 7, S, 4, SSD1306_WHITE);

  // HUNGER/FOOD STATS
  display.setCursor(85,5);
  display.println("F");
  display.drawRoundRect(92, 6, 27, 6, 1, SSD1306_WHITE);
  int F = alegotchi.mapValue(alegotchi.hunger, 27);
  display.fillRect(93, 7, F, 4, SSD1306_WHITE);
  
  display.display();
}

//__________________________________________________ WATCHDOG AND SLEEP FUNCTIONS
int hour = 450;
ISR (WDT_vect) {
  
    sec ++; // sec = 450 --> è passata un'ora
    Serial.print("sec: ");
    Serial.println(sec);
    
    if(sec > hour){
      sec = 0;
      
      alegotchi.updateAge();
      Serial.println(alegotchi.age);
      
      alegotchi.updateHappiness(-1);
      alegotchi.updateHunger(-1);
      alegotchi.updateBeardLength(alegotchi.beardLength + 1);
      if(alegotchi.sleeping){
        alegotchi.updateSleep(3);        
      }else{
        alegotchi.updateSleep(-1);        
      }      
      
    }
}

void turnOn() {
  Serial.println("TURN ON");
  delay(2000);
}

void turnOff() {
  display.clearDisplay();
  display.display();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(2), turnOn, LOW);
  sleep_mode();

  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));

  // Ogni ora se uno stato è basso suona per attirare l'attenzione
  // Solo se lo switch dello schermo è spento
  if((alegotchi.sleep == 0 || alegotchi.hunger == 0 || alegotchi.happiness == 0) && sec >= hour && digitalRead(SWITCH_SCREEN) == HIGH){
    cute.play(S_CONFUSED);
  }

  
  if (digitalRead(SWITCH_SCREEN) == LOW){
    if(state == 0){
      frameCount = 50;
      displayAle();
      displayMenuFunc();
    }
    
    displayStats();
    display.display();
    cute.play(S_CONNECTION);
  }
}
