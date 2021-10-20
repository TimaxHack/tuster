#include "GyverButton.h"
#include "EEPROM.h"
#include <Multiservo.h>
#include <ServoDriverSmooth.h>
#include <LiquidCrystal_I2C.h>  // подключаем библу
// адрес дисплея 0x3f или 0x27
#include <Wire.h>
// размер дисплея 16x2 (поддерживаются и другие, например 20x4)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // адрес, столбцов, строк
///////////////////////////////////   CONFIGURATION   /////////////////////////////
#define BTN_PIN 52
#define BTN_PIN_VERCH 51
#define BTN_PIN_VNIZ 50 
#define POT_PIN 8  
#define TUM_PIN 15
constexpr uint8_t MULTI_SERVO_COUNT = 11;

enum Mode {START, WORK, DEMO, DEBUG, ENDING} mode;

ServoDriverSmooth smoothServo[MULTI_SERVO_COUNT];
Multiservo multiservo[MULTI_SERVO_COUNT];

GButton btn(BTN_PIN);
GButton btn1(BTN_PIN_VERCH);
GButton btn2(BTN_PIN_VNIZ);

void setup() {
  lcd.init();
   lcd.backlight();
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  btn.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  btn.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  btn.setClickTimeout(500);  // установка таймаута между кликами (по умолчанию 500 мс)  
  btn.setTickMode(AUTO);

  btn1.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  btn1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  btn1.setClickTimeout(500);  // установка таймаута между кликами (по умолчанию 500 мс)  
  btn1.setTickMode(AUTO);

  btn2.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  btn2.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  btn2.setClickTimeout(500);  // установка таймаута между кликами (по умолчанию 500 мс)  
  btn2.setTickMode(AUTO);

  // Перебираем значения моторов
  for (int count = 0; count < MULTI_SERVO_COUNT; count++) {
    // Подключаем сервомотор
    multiservo[count].attach(count);
    smoothServo[count].attach(count);
    smoothServo[count].setSpeed(20);
    smoothServo[count].setAccel(0.05);
  }

  while(analogRead(15)>=1010) {
    Serial.println("Никита, идиота кусок, выключи тумблер!!!");
  }
  setMode(START);
}

uint32_t myTimer = 0;
int knot = 0, angle;
void loop() {
  lcd.setCursor(4, 1);
  uint16_t multiservo[7].getCurrent().const;
  lcd.print(multiservo[7].readVoltageCurrent());
  Serial.print(vost);
    //lcd.print(uint16_t getVoltage() const);
  
  // put your main code here, to run repeatedly:
  if (mode==START){
    lcd.setCursor(1, 0);  
    lcd.print("START");
    
    for (int i = 0; i < 3; i++) {
      multiservo[i].write(90);
      smoothServo[i].setCurrentDeg(90);
    }
    multiservo[3].write(90);
    smoothServo[3].setCurrentDeg(90);
    multiservo[4].write(180-90);
    smoothServo[4].setCurrentDeg(180-90);
    multiservo[5].write(0);
    smoothServo[5].setCurrentDeg(0);
    if (analogRead(15)>=1010){
    setMode(WORK);
    
    }
  }
  
  if (mode==WORK){ 
    lcd.setCursor(1, 0); 
    lcd.print("WORK");
    
    for (int i = 0; i < 3; i++) {
      smoothServo[i].setTargetDeg(0);
    }
    smoothServo[3].setTargetDeg(45);
    smoothServo[4].setTargetDeg(180-45);
    smoothServo[5].setTargetDeg(120);
    updateServos();
    if ((analogRead(8) >= 1023) & (btn.isHold())) setMode(DEBUG);
    if (btn.isDouble()) setMode(DEMO);
    if (analogRead(15)<=10)setMode(ENDING);


  
  
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char raeder;
  
    while (btSerial.available() > 0 && newData == false)
    {
      delay(10);
      raeder = btSerial.read();
      //Serial.print(rc);
   
      delay(10);
  
      if (recvInProgress == true) {
        if (raeder != endMarker) {  
          if (raeder == startMarker) continue;
          receivedChars[ndx] = raeder;
          ndx++;
          if (ndx >= numChars) {
            ndx = numChars - 1;
          }
        }
        else {
          receivedChars[ndx] = '\0'; // terminate the string
          recvInProgress = false;
          ndx = 0;
          newData = true;
        }
      }
  
      else if (raeder == startMarker) {
        recvInProgress = true;
      }
    }
   }
  
  
  
   
  
  char * strtokIndx; // this is used by strtok() as an index
  
    strtokIndx = strtok(tempChars, ",");     // get the first part - the string
    xRecieve1 = atoi(strtokIndx); // convert this part to a int, atoi =  перевод в int
  
    strtokIndx = strtok(NULL, ",");
    xRecieve2 = atoi(strtokIndx);
  
    strtokIndx = strtok(NULL, ",");
    yRecieve1 = atoi(strtokIndx);
  
    strtokIndx = strtok(NULL, ",");
    yRecieve2 = atoi(strtokIndx);
  
    strtokIndx = strtok(NULL, ",");
    flexRecieve1 = atoi(strtokIndx);
  
    strtokIndx = strtok(NULL, ",");
    flexRecieve2 = atoi(strtokIndx);



    
  
  
  if (mode==DEMO){
    lcd.print("DEMO");
    updateServos();
    if (btn.isDouble()) setMode(WORK);
  }
  
  if (mode==DEBUG){
    

    if (btn1.isPress()) {
      knot++;
      if (knot>=6) knot = 0;
    }
    else if (btn2.isPress()){
      knot--;
      if (knot<0) knot = 5;
      }
    if (btn.isPress()) { 
    switch (knot){
      case 0:smoothServo[0].setTargetDeg(angle);smoothServo[1].setTargetDeg(angle);smoothServo[2].setTargetDeg(angle); 
      break;
      case 1:smoothServo[3].setTargetDeg(angle);smoothServo[4].setTargetDeg(180 - angle);
      break;
      case 2:smoothServo[5].setTargetDeg(angle);
      break;
      case 3:smoothServo[6].setTargetDeg(angle);smoothServo[7].setTargetDeg(180-angle);
      break;
      case 4:smoothServo[9].setTargetDeg(angle);
      break;
      case 5:smoothServo[10].setTargetDeg(angle);smoothServo[11].setTargetDeg(angle);
      break;      
      }       
    }
   
    if (millis() - myTimer >= 50) {
    lcd.setCursor(0, 0);
    angle = map(analogRead(8), 0, 1023, 0, 180);
    myTimer = millis();
    switch (knot){
      case 0:    
        //Serial.print("Плечо-поворот: ");Serial.println(angle);
        lcd.print("Shoulder yaw: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      case 1:
        //Serial.print("Плечо-подъем: "); Serial.println(angle);
        lcd.print("Shoulder pitch: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      case 2:
        //Serial.print("Локоть-подъем: ");Serial.println(angle);
        lcd.print("Elbow pitch: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      case 3:
        //Serial.print("Кисть-подъем: ");Serial.println(angle);
        lcd.print("Wrist pitch: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      case 4:
        //Serial.print("Кисть-поворот: ");Serial.println(angle);
        lcd.print("Wrist roll: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      case 5:
        //Serial.print("Сжатие: ");Serial.println(angle);
        lcd.print("Fist: ");lcd.setCursor(0, 1);lcd.print(angle);
      break;
      
    }}
    
    
    
    
    updateServos();
    if ((analogRead(8) <= 10) & (btn.isHold())) setMode(WORK);
  }
  lcd.print(" ");
  if (mode==ENDING){
    updateServos();
    for (int i = 0; i < 3; i++) {
      smoothServo[i].setTargetDeg(90);
    }
    smoothServo[3].setTargetDeg(90);
    smoothServo[4].setTargetDeg(180-90);
    smoothServo[5].setTargetDeg(0);
  }


}

void updateServos(){
  for (int i = 0; i < MULTI_SERVO_COUNT; i++) {
    smoothServo[i].tick();   
    multiservo[i].write(smoothServo[i].getCurrentDeg());
  }
}

void setMode(Mode m){
  switch (m){
    case START: Serial.println("----- GEFEST IS START -----");
    break;
    case WORK:Serial.println("----- GEFEST WORKS HARD, harder then Nikita, hard like Sonya -----");
    break;
    case DEMO:Serial.println("----- DEMO MODE -----");
    break;
    case DEBUG:Serial.println("----- DEBUG MODE -----");
    lcd.print("DEBUG");
    delay(1000);
    break;
    case ENDING:Serial.println("----- ENDING -----");
    break;
  }
  mode=m;
}
