#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

byte firstTouches[10];

void setup(void) {
  u8g2.begin();
  u8g2.setBusClock(40000000);
  u8g2.enableUTF8Print();

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
  u8g2.print("Hi");
  u8g2.setCursor(0, 36);
  u8g2.sendBuffer();
}

#define M_START 0
#define M_TOUCH 1
#define M_ACCEPT 2
#define M_PRERELEASE1 3
#define M_PRERELEASE2 4

byte mode = M_START;
unsigned int count = 0;
int prevKey = -1;
int key = -1;
int touchTimer = 0;
int preReleaseTimer1 = 0;
int preReleaseTimer2 = 0;
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

  switch(mode){
    case M_START:
    if(col != -1 && row != -1){ // trigger touch
      mode = M_TOUCH;
      key = col * 5 + (row - 5);
      Serial.println("touch");
    }
    break;
    case M_TOUCH:
    if(col == -1 && row == -1){ // release
      mode = M_PRERELEASE1;
      Serial.println("Pre Release 1");
    }else if(col != -1 && row != -1){ // touch
      int nowKey = col * 5 + (row - 5);
      if(nowKey == key){
        touchTimer ++;
        if(touchTimer > 1000){
          mode = M_ACCEPT;
          u8g2.setCursor(0, 12);
          u8g2.print(key);
          u8g2.sendBuffer();
 
          Serial.print("accept");
          Serial.println(key);
          touchTimer = 0;
        }
      }else{ // touch another key
        // restart another key timer
        touchTimer = 0;
        key = col * 5 + (row - 5);
      }
    }
    break;
    case M_PRERELEASE1:
    if(col == -1 && row == -1){ // release
      preReleaseTimer1 ++;
      if(preReleaseTimer1 > 2000){
        preReleaseTimer1 = 0;
        touchTimer = 0;
        Serial.println("start");
        mode = M_START;
        key = -1;
        prevKey = -1;
      }
    }else if(col != -1 && row != -1){ // touch
      int nowKey = col * 5 + (row - 5);
      if(nowKey == key){ // back to M_TOUCH
        mode = M_TOUCH;
        preReleaseTimer1 = 0;
        Serial.println("touch");
      }else{ // touch another key
        // restart another key timer
        mode = M_TOUCH;
        key = col * 5 + (row - 5);
        preReleaseTimer1 = 0;
        Serial.println("touch");
      }
    }
    break;
    case M_ACCEPT:
    if(col == -1 && row == -1){ // release
      mode = M_PRERELEASE2;
      Serial.println("PreRelease2");
    }else if(col != -1 && row != -1){
      int nowKey = col * 5 + (row - 5);
      if(nowKey == key){
      }else{
        // swipe another key?
        mode = M_TOUCH;
        prevKey = key;
        key = nowKey;
        Serial.print("touch");
        Serial.println(key);
      }
    }
    break;
    case M_PRERELEASE2:
    if(col == -1 && row == -1){ // release
      preReleaseTimer2 ++;
      if(preReleaseTimer2 > 6000){
        if(num > 8){
          num = 0;
          u8g2.clearBuffer();
          u8g2.setCursor(0, 36);
        }
        u8g2.setCursor(num * 12, 36);
        switch(prevKey){
          case -1:
          switch(key){
            case 6: u8g2.print("あ");break;
            case 7: u8g2.print("た");break;
            case 8: u8g2.print("ま");break;

            case 11: u8g2.print("か");break;
            case 12: u8g2.print("な");break;
            case 13: u8g2.print("や");break;
            case 14: u8g2.print("わ");break;

            case 16: u8g2.print("さ");break;
            case 17: u8g2.print("は");break;
            case 18: u8g2.print("ら");break;
            case 20: 
            if(num > 0){
              num --;
              u8g2.setDrawColor(0);
              u8g2.drawBox(num * 12, 24, 13, 13);
              u8g2.setDrawColor(1);
              num --;
            }
            break;
          }
          break;
          case 6:
          switch(key){
            case 1: u8g2.print("い");break;
            case 5: u8g2.print("う");break;
            case 11: u8g2.print("え");break;
            case 7: u8g2.print("お");break;
          }
          break;
          case 7:
          switch(key){
            case 2: u8g2.print("ち");break;
            case 6: u8g2.print("つ");break;
            case 12: u8g2.print("て");break;
            case 8: u8g2.print("と");break;
          }
          break;
          case 8:
          switch(key){
            case 3: u8g2.print("み");break;
            case 7: u8g2.print("む");break;
            case 13: u8g2.print("め");break;
            case 9: u8g2.print("も");break;
          }
          break;
          case 11:
          switch(key){
            case 6: u8g2.print("き");break;
            case 10: u8g2.print("く");break;
            case 16: u8g2.print("け");break;
            case 12: u8g2.print("こ");break;
          }
          break;
          case 12:
          switch(key){
            case 7: u8g2.print("に");break;
            case 11: u8g2.print("ぬ");break;
            case 17: u8g2.print("ね");break;
            case 13: u8g2.print("の");break;
          }
          break;
          case 13:
          switch(key){
            case 8: u8g2.print("い");break;
            case 12: u8g2.print("ゆ");break;
            case 18: u8g2.print("え");break;
            case 14: u8g2.print("よ");break;
          }
          break;
          case 14:
          switch(key){
            case 9: u8g2.print("を");break;
            case 13: u8g2.print("ん");break;
            case 19: u8g2.print("ー");break;
          }
          break;
          case 16:
          switch(key){
            case 11: u8g2.print("し");break;
            case 15: u8g2.print("す");break;
            case 21: u8g2.print("せ");break;
            case 17: u8g2.print("そ");break;
          }
          break;
          case 17:
          switch(key){
            case 12: u8g2.print("ひ");break;
            case 16: u8g2.print("ふ");break;
            case 22: u8g2.print("へ");break;
            case 18: u8g2.print("ほ");break;
          }
          break;
          case 18:
          switch(key){
            case 12: u8g2.print("り");break;
            case 16: u8g2.print("る");break;
            case 22: u8g2.print("れ");break;
            case 18: u8g2.print("ろ");break;
          }
          break;
        }
        u8g2.sendBuffer();
        num ++;

        preReleaseTimer1 = 0;
        preReleaseTimer2 = 0;
        touchTimer = 0;

        u8g2.setDrawColor(0);
        u8g2.drawBox(0, 0, 128, 13);
        u8g2.setDrawColor(1);
        u8g2.setCursor(0, 12);
        u8g2.print("---");
        u8g2.sendBuffer();
 
        mode = M_START;
        key = -1;
        prevKey = -1;
        Serial.println("start");
      }
    }else if(col != -1 && row != -1){ // touch
      int nowKey = col * 5 + (row - 5);
      if(nowKey == key){ // back to M_TOUCH
        mode = M_ACCEPT;
        preReleaseTimer2 = 0;
        Serial.println("touch");
      }else{ // touch another key
        // restart another key timer
        mode = M_TOUCH;
        prevKey = key;
        preReleaseTimer2 = 0;
        key = col * 5 + (row - 5);
        Serial.println("touch");
      }
    }
    break;
  }
  count ++;
}

