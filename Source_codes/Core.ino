#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHT22_PIN 11
#include <EEPROM.h>

const int AbuttonPin = 8;
const int BbuttonPin = 9;
int x=0, pre_x = 0, A_btn_state, B_btn_state, address = 0;
bool paused = true;
unsigned int high_score;

DHT dht22(DHT22_PIN, DHT22);
LiquidCrystal_I2C lcd(0x27,16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void setup() {
  high_score = EEPROM.read(address);

  Serial.println("loop");
  dht22.begin(); // initialize the DHT22 sensor
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);
  lcd.init();
  lcd.clear();
  lcd.backlight();   
     // Make sure backlight is on
  pinMode(AbuttonPin, INPUT);

  // the last Cursor state
  // Print a message on both lines of the LCD.
  // lcd.setCursor(0,0);   Set cursor to character 2 on line 0
  // lcd.print("Score:");
  
}

void loop() {
  Serial.println("loop");
  A_btn_state = digitalRead(AbuttonPin);
  B_btn_state = digitalRead(BbuttonPin);

  //read humiity
  float humi  = dht22.readHumidity();
  // read temperature as Celsius
  int tempC = dht22.readTemperature();

  Wire.requestFrom(9, 1);     // request 6 bytes from slave device #9
  while (Wire.available()) {  // slave may send less than requested
    x = Wire.read();     // receive a byte as character
  }

  if(paused == false) {
    lcd.setCursor(0, 0);
    lcd.print("Score:");
    lcd.print(x);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Paused");
  }
  


  lcd.setCursor(9,1);
  lcd.print("temp:");
  lcd.print(tempC);
  lcd.setCursor(9,0);
  lcd.print("humi:");
  lcd.print(humi);
  
  lcd.setCursor(0,1);
  lcd.print("HS   :");
  lcd.print(high_score);

  //lcd.setCursor(6,1); 
  Serial.println(A_btn_state);
  Serial.println(B_btn_state);
  if(A_btn_state == 1) {
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(B10000000);              // sends x 
    Wire.endTransmission(); 
    paused = false;
  }
  if(B_btn_state == 1) {
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(B00000000);              // sends x 
    Wire.endTransmission(); 
    paused = true;
  }
  
  
  // if (isnan(humi) || isnan(tempC)) {
  //   Serial.println("Failed to read from DHT22 sensor!");
  // } else {
  //   Serial.print("DHT22# Humidity: ");
  //   Serial.print(humi);
  //   Serial.print("%");

  //   Serial.print("  |  "); 

  //   Serial.print("Temperature: ");
  //   Serial.print(tempC);
  //   Serial.print("°C ~ ");
  // }

  delay(200);
  if(pre_x != x or paused == true) {
    lcd.clear();
    pre_x = x;
  }
  if(high_score < x) {
    high_score = x;
    EEPROM.write(address, high_score);
  }
  
} 
