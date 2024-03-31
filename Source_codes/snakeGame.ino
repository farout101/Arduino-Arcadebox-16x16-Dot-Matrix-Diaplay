#include <LedControl.h>
#include <Wire.h>
#include "MD_Parola.h"
#include "MD_MAX72xx.h"
#include "SPI.h"

// Pin configuration for MAX7219
const int DIN_PIN = 13;     // Data pin
const int CS_PIN = 11;      // Chip select pin
const int CLK_PIN = 12;     // Clock pin
const int NUM_DEVICES = 4;  // Number of MAX7219 devices

const int buzzerPin = 9;

//joystick pins
#define VRX_PIN A0  // Arduino pin connected to VRX pin
#define VRY_PIN A1  // Arduino pin connected to VRY pin

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

// Snake properties
const int INITIAL_LENGTH = 3;
const int MAX_LENGTH = 256;
int snakeX[MAX_LENGTH], snakeY[MAX_LENGTH];
int snakeLength = INITIAL_LENGTH;
int snakeDirection = 3;  // 0: left, 1: up, 2: right, 3: down

// Food properties
int foodX, foodY, delay_value=500;
bool eatFood=false;

// Game properties
int score = 0;
bool gameOver = false, paused = true;
byte transfer;

//Set up joystick values
int xValue = 0;  // To store value of the X axis
int yValue = 0;  // To store value of the Y axis

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);
// Sprite definitions:
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // pacman pursued by a ghost
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
};

const uint8_t F_WAVE = 14;
const uint8_t W_WAVE = 14;
const uint8_t PROGMEM wave[F_WAVE * W_WAVE] =  // triangular wave / worm
{
  0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10,
  0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20,
  0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40,
  0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
  0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
  0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
  0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10,
  0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08,
  0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04,
  0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x02,
  0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
  0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04,
  0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08,
};

const uint8_t F_ROLL1 = 4;
const uint8_t W_ROLL1 = 8;
const uint8_t PROGMEM roll1[F_ROLL1 * W_ROLL1] =  // rolling square
{
  0xff, 0x8f, 0x8f, 0x8f, 0x81, 0x81, 0x81, 0xff,
  0xff, 0xf1, 0xf1, 0xf1, 0x81, 0x81, 0x81, 0xff,
  0xff, 0x81, 0x81, 0x81, 0xf1, 0xf1, 0xf1, 0xff,
  0xff, 0x81, 0x81, 0x81, 0x8f, 0x8f, 0x8f, 0xff,
};

const uint8_t F_ROLL2 = 4;
const uint8_t W_ROLL2 = 8;
const uint8_t PROGMEM roll2[F_ROLL2 * W_ROLL2] =  // rolling octagon
{
  0x3c, 0x4e, 0x8f, 0x8f, 0x81, 0x81, 0x42, 0x3c,
  0x3c, 0x72, 0xf1, 0xf1, 0x81, 0x81, 0x42, 0x3c,
  0x3c, 0x42, 0x81, 0x81, 0xf1, 0xf1, 0x72, 0x3c,
  0x3c, 0x42, 0x81, 0x81, 0x8f, 0x8f, 0x4e, 0x3c,
};

const uint8_t F_LINES = 3;
const uint8_t W_LINES = 8;
const uint8_t PROGMEM lines[F_LINES * W_LINES] =  // spaced lines
{
  0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00,
  0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
  0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
};

const uint8_t F_ARROW1 = 3;
const uint8_t W_ARROW1 = 10;
const uint8_t PROGMEM arrow1[F_ARROW1 * W_ARROW1] =  // arrow fading to center
{
  0x18, 0x3c, 0x7e, 0xff, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x00,
  0x18, 0x3c, 0x7e, 0xff, 0x00, 0x7e, 0x00, 0x00, 0x18, 0x00,
  0x18, 0x3c, 0x7e, 0xff, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x18,
};

const uint8_t F_ARROW2 = 3;
const uint8_t W_ARROW2 = 9;
const uint8_t PROGMEM arrow2[F_ARROW2 * W_ARROW2] =  // arrow fading to outside
{
  0x18, 0x3c, 0x7e, 0xe7, 0x00, 0x00, 0xc3, 0x00, 0x00,
  0x18, 0x3c, 0x7e, 0xe7, 0xe7, 0x00, 0x00, 0x81, 0x00,
  0x18, 0x3c, 0x7e, 0xe7, 0x00, 0xc3, 0x00, 0x00, 0x81,
};

const uint8_t F_SAILBOAT = 1;
const uint8_t W_SAILBOAT = 11;
const uint8_t PROGMEM sailboat[F_SAILBOAT * W_SAILBOAT] =  // sail boat
{
  0x10, 0x30, 0x58, 0x94, 0x92, 0x9f, 0x92, 0x94, 0x98, 0x50, 0x30,
};

const uint8_t F_STEAMBOAT = 2;
const uint8_t W_STEAMBOAT = 11;
const uint8_t PROGMEM steamboat[F_STEAMBOAT * W_STEAMBOAT] =  // steam boat
{
  0x10, 0x30, 0x50, 0x9c, 0x9e, 0x90, 0x91, 0x9c, 0x9d, 0x90, 0x71,
  0x10, 0x30, 0x50, 0x9c, 0x9c, 0x91, 0x90, 0x9d, 0x9e, 0x91, 0x70,
};

const uint8_t F_HEART = 5;
const uint8_t W_HEART = 9;
const uint8_t PROGMEM heart[F_HEART * W_HEART] =  // beating heart
{
  0x0e, 0x11, 0x21, 0x42, 0x84, 0x42, 0x21, 0x11, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x1f, 0x0e,
  0x0e, 0x1f, 0x3f, 0x7e, 0xfc, 0x7e, 0x3f, 0x1f, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x1f, 0x0e,
  0x0e, 0x11, 0x21, 0x42, 0x84, 0x42, 0x21, 0x11, 0x0e,
};

const uint8_t F_INVADER = 2;
const uint8_t W_INVADER = 10;
const uint8_t PROGMEM invader[F_INVADER * W_INVADER] =  // space invader
{
  0x0e, 0x98, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x98, 0x0e,
  0x70, 0x18, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x18, 0x70,
};

const uint8_t F_ROCKET = 2;
const uint8_t W_ROCKET = 11;
const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // rocket
{
  0x18, 0x24, 0x42, 0x81, 0x99, 0x18, 0x99, 0x18, 0xa5, 0x5a, 0x81,
  0x18, 0x24, 0x42, 0x81, 0x18, 0x99, 0x18, 0x99, 0x24, 0x42, 0x99,
};

const uint8_t F_FBALL = 2;
const uint8_t W_FBALL = 11;
const uint8_t PROGMEM fireball[F_FBALL * W_FBALL] =  // fireball
{
  0x7e, 0xab, 0x54, 0x28, 0x52, 0x24, 0x40, 0x18, 0x04, 0x10, 0x08,
  0x7e, 0xd5, 0x2a, 0x14, 0x24, 0x0a, 0x30, 0x04, 0x28, 0x08, 0x10,
};

const uint8_t F_CHEVRON = 1;
const uint8_t W_CHEVRON = 9;
const uint8_t PROGMEM chevron[F_CHEVRON * W_CHEVRON] =  // chevron
{
  0x18, 0x3c, 0x66, 0xc3, 0x99, 0x3c, 0x66, 0xc3, 0x81,
};

const uint8_t F_WALKER = 5;
const uint8_t W_WALKER = 7;
const uint8_t PROGMEM walker[F_WALKER * W_WALKER] =  // walking man
{
  0x00, 0x48, 0x77, 0x1f, 0x1c, 0x94, 0x68,
  0x00, 0x90, 0xee, 0x3e, 0x38, 0x28, 0xd0,
  0x00, 0x00, 0xae, 0xfe, 0x38, 0x28, 0x40,
  0x00, 0x00, 0x2e, 0xbe, 0xf8, 0x00, 0x00,
  0x00, 0x10, 0x6e, 0x3e, 0xb8, 0xe8, 0x00,
};


void setup() {
  Serial.begin(9600);
  Serial.println("set up");
  for (int i = 0; i < NUM_DEVICES; i++) {
    lc.shutdown(i, false);  // Wake up MAX7219
    lc.setIntensity(i, 1);  // Set brightness (0-15)
    lc.clearDisplay(i);
  }

  // Initialize snake
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = i;
    snakeY[i] = 4;
    setLed(snakeX[i], snakeY[i]);
  }

  // Spawn initial food
  spawnFood();

  //Buzzer Pin
  pinMode(buzzerPin, OUTPUT);

  //I2C protocol set up
  Wire.begin(9);
  // // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();
  myDisplay.setSpriteData(pacman2, W_PMAN2, F_PMAN2, pacman2, W_PMAN2, F_PMAN2);
  myDisplay.displayText("", PA_CENTER, 50, 1000, PA_SPRITE, PA_SPRITE);
}

void requestEvent() {
  Serial.println("req event");
  Wire.write(score);
}
void receiveEvent(int bytes) {
  Serial.println("receive event");
  transfer = Wire.read();  // read one character from the I2C
  if (bitRead(transfer, 7) == 1) {
    paused = false;
    Serial.println("unnpaused");
  }
  if (bitRead(transfer, 7) == 0) {
    paused = true;
    Serial.println("paused");
  }
}

void loop() {
  Serial.println(paused);
  if (!gameOver && !paused) {
    moveSnake();
    
    checkCollision();
    checkFood();
    drawObjects();
    if(eatFood == false) {
      noTone(buzzerPin);
      delay(delay_value);  // Adjust snake speed
      tone(buzzerPin, 700);
      clearDisplay();
    } else {
      eatFood = false;
    }
    
  }
  else if(paused) {
    noTone(buzzerPin);
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
    }
  }
}

void moveSnake() {
  Serial.println("move snake");
  int newHeadX = snakeX[0];
  int newHeadY = snakeY[0];

  readJoystick();
  // Move the head in the current direction
  switch (snakeDirection) {
    case 0:  // Right
      newHeadX++;
      break;
    case 1:  // Up
      newHeadY--;
      break;
    case 2:  // Left
      newHeadX--;
      break;
    case 3:  // Down
      newHeadY++;
      break;
  }

  // Shift the body
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Move the head
  snakeX[0] = newHeadX;
  snakeY[0] = newHeadY;

  // Draw the snake
}


void drawObjects() {
  //Serial.println("draw obj");
  for (int i = 0; i < snakeLength; i++) {
    setLed(snakeX[i], snakeY[i]);
  }
  setLed(foodX, foodY);
}


void checkCollision() {
  //Serial.println("check collision");
  // Check if the snake hits the walls or itself
  if (snakeX[0] < 0 || snakeX[0] >= 16 || snakeY[0] < 0 || snakeY[0] >= 16) {
    gameOver = true;
    displayGameOver();
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      displayGameOver();
    }
  }
}

void checkFood() {
  //Serial.println("check food");
  // Check if the snake eats the food
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    eatFood = true;
    // Increase score and spawn new food
  
    score++;
    if(delay_value > 50) {
      delay_value = delay_value - 50;
    }
    tone(buzzerPin, 2200);
    spawnFood();
    delay(delay_value/2);
    // Increase snake length (if not at maximum)
    if (snakeLength < MAX_LENGTH) {
      snakeLength++;
    }
    noTone(buzzerPin);
  }
}

void spawnFood() {
  //Serial.println("spawn food");
  // Generate random coordinates for the food
  foodX = random(0, 16);
  foodY = random(0, 16);

  // Check if the food spawns on the snake, if yes, regenerate
  for (int i = 0; i < snakeLength; i++) {
    if (foodX == snakeX[i] && foodY == snakeY[i]) {
      spawnFood();
      return;
    }
  }

  // Draw the food
}

void clearDisplay() {
  Serial.println("clear display");
  for (int i = 0; i < NUM_DEVICES; i++) {
    lc.clearDisplay(i);
  }
}

void setLed(int x, int y) {
  //Serial.println("set LED");
  int module, xOffset, yOffset;
  if (x > 7 && y > 7) {
    module = 0;
    xOffset = 8;
    yOffset = 8;
  } else if (y > 7) {
    module = 2;
    xOffset = 0;
    yOffset = 8;
  } else if (x > 7) {
    module = 1;
    xOffset = 8;
    yOffset = 0;
  } else {
    module = 3;
    xOffset = 0;
    yOffset = 0;
  }

  lc.setLed(module, x - xOffset, y - yOffset, true);
}

void displayGameOver() {
  Serial.println("displayGameover");
  // Display Game Over message
  clearDisplay();
  byte three[8] = { B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001 };
  byte zero[8] = { B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001 };
  byte one[8] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };
  byte two[8] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };


  for (int i = 0; i < 8; i++) {
    lc.setRow(1, i, one[i]);
    lc.setRow(2, i, two[i]);
    lc.setRow(3, i, three[i]);
    lc.setRow(0, i, zero[i]);
  }

  delay(1000);
  // Restart the game
  resetGame();
}

void resetGame() {
  Serial.println("reset");
  // Reset variables
  score = 0;
  gameOver = false;
  delay_value = 500;
  snakeLength = INITIAL_LENGTH;
  snakeDirection = 0;

  // Clear display
  clearDisplay();

  // Reinitialize snake
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = 5 - i;
    snakeY[i] = 6;
    //setLed(snakeX[i], snakeY[i]);
  }

  // Spawn initial food
  spawnFood();
}

void readJoystick() {
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  //button_state = digitalRead(button);

  if (yValue < xValue && yValue > 1023 - xValue && xValue > 600) {  
    // 0: left, 1: up, 2: right, 3: down
    snakeDirection = 2;
  } else if (yValue > xValue && yValue < 1023 - xValue && xValue < 400) {
    snakeDirection = 0;
  } else if (yValue > xValue && yValue > 1023 - xValue && yValue > 600) {
    snakeDirection = 1;
  } else if (yValue < xValue && yValue < 1023 - xValue && yValue < 400) {
    snakeDirection = 3;
  }
}

void displayPacman() {
  clearDisplay();
  byte three[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000110,B00000110,B00000110};
  byte zero[8] = {B01100000,B01100000,B01100000,B00000000,B00000000,B00000000,B00000000,B00000000};
  byte one[8] = {B00000110,B00000110,B00000110,B00000000,B00000000,B00000000,B00000000,B00000000};
  byte two[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B01100000,B01100000,B01100000};


  for (int i = 0; i < 8; i++) {
    lc.setRow(1, i, one[i]);
    lc.setRow(2, i, two[i]);
    lc.setRow(3, i, three[i]);
    lc.setRow(0, i, zero[i]);
  }
}

void makeSoundBuzz(int value) {
 
}
void removeSound() {
  
}
