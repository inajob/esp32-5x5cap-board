#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

byte firstTouches[10];

void setup(void) {
  u8g2.begin();
  u8g2.setBusClock(40000000);
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function

  Serial.begin(115200);
  firstTouches[0] = touchRead(T0);
  firstTouches[1] = touchRead(T1);
  firstTouches[2] = touchRead(T2);
  firstTouches[3] = touchRead(T3);
  firstTouches[4] = touchRead(T4);
  firstTouches[5] = touchRead(T5);
  firstTouches[6] = touchRead(T6);
  firstTouches[7] = touchRead(T7);
  firstTouches[8] = touchRead(T8);
  firstTouches[9] = touchRead(T9);

  Serial.println("hello world");
  u8g2.setFont(u8g2_font_b12_t_japanese1);
  u8g2.setFontDirection(0);
  
  u8g2.clearBuffer();
  u8g2.setCursor(0, 0);
  u8g2.print("Hello World!");
  u8g2.setCursor(0, 12);
  u8g2.print("あいうえお");
  u8g2.setCursor(0, 36);
}

unsigned int count = 0;
int lastTouch = -1;
int pressTimer = 0;
int num = 0;
void loop(void) {
  byte touches[10];
  byte value[10];
  touches[0] = touchRead(T0);
  touches[1] = touchRead(T1);
  touches[2] = touchRead(T2);
  touches[3] = touchRead(T3);
  touches[4] = touchRead(T4);
  touches[5] = touchRead(T5);
  touches[6] = touchRead(T6);
  touches[7] = touchRead(T7);
  touches[8] = touchRead(T8);
  touches[9] = touchRead(T9);

  for(byte i = 0; i < 10; i ++){
    if(firstTouches[i] != 0){
      value[i] = 100 * touches[i] / firstTouches[i];
    }else{
      value[i] = touches[i];
    }
  }
  /*
  if(count == 10){
    u8g2.clearBuffer();
    u8g2.setCursor(0, 36);
    for(byte i = 0; i < 10; i ++){
      if(i == 5){
        u8g2.setCursor(0, 36 + 12);
      }
      u8g2.print((value[i])<50?"@":"-");
      u8g2.print(value[i]);
      u8g2.print(" ");
    }
    u8g2.sendBuffer();
    count = 0;
  }*/
  int col = -1;
  int row = -1;
  for(byte i = 0; i < 5; i ++){
    if(value[i] < 50){
      col = i;
    }
  }
  for(byte i = 5; i < 10; i ++){
    if(value[i] < 50){
      row = i;
    }
  }
  if(col != -1 && row != -1){
    int touchNo = col * 5 + (row - 5);
    if(lastTouch != -1){
      if(touchNo != lastTouch){
        Serial.print(lastTouch);
        Serial.print(" -> ");
        Serial.print(touchNo);
        Serial.println();
        pressTimer = 0;
      }else{
        pressTimer ++;
        if(pressTimer == 1000){
          if(num > 8){
            num = 0;
            u8g2.clearBuffer();
            u8g2.setCursor(0, 36);
          }
          Serial.print(touchNo);
          Serial.println(" touch accept");
          u8g2.print(touchNo);
          u8g2.print(" ");
          u8g2.sendBuffer();
          num ++;
        }
      }
    }else{
      Serial.print(touchNo);
      if(pressTimer < 2500){
        Serial.print(" chain");
      }else{
        Serial.print(" first");
        Serial.print(pressTimer);
      }
      Serial.println(" touch");
      pressTimer = 0;
    }
    lastTouch = touchNo;
  }else{
    if(lastTouch != -1){
      Serial.println("release");
      pressTimer = 0;
    }
    lastTouch = -1;
    pressTimer ++;
  }

  count ++;
}

