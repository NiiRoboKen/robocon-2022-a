#include <PS4BT.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
/*
  pwm_pin dir_pinの中身
  [0] 左前のモータ
  [1] 左後ろのモータ
  [2] 右後ろのモータ
  [3] 右前のモータ
*/

int pwm_pin[] = {2, 4, 5, 6};
int dir_pin[] = {26, 32, 38, 44};
int Lx;
int Ly;
int Rx;
int Ry;
float Lrad;
float Rrad;
uint8_t Lpower;
uint8_t Rpower;

USB Usb;
BTD Btd(&Usb);
//PS4BT PS4(&Btd, PAIR);//初期接続
PS4BT PS4(&Btd);//二回目以降

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial);
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1);
  }
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(dir_pin[i], OUTPUT);
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}



void loop() {
  while (1) {
    Usb.Task();
    if (PS4.connected()) {
      Lx = PS4.getAnalogHat(LeftHatX) - 127;
      Ly = PS4.getAnalogHat(LeftHatY) - 127;
      Rx = PS4.getAnalogHat(RightHatX) - 127;
      Ry = PS4.getAnalogHat(RightHatY) - 127;

      if (Lx > 10 || Lx < -10 || Ly > 10 || Ly < -10) {
        Lrad = atan2(Lx, Ly);
        Lpower = map(sqrt(Lx * Lx + Ly * Ly), 11, 180, 0, 255);

        if (Lrad < -2.74 || Lrad > 2.74) {       //前
          Serial.println("Front");
          for (uint8_t i = 0; i < 4; i++) {
            analogWrite(pwm_pin[i], Lpower);
            digitalWrite(dir_pin[i], HIGH);
          }
        } else if (Lrad < 0.39 && Lrad > -0.39) { //後ろ
          Serial.println("Back");
          for (uint8_t i = 0; i < 4; i++) {
            analogWrite(pwm_pin[i], Lpower);
            digitalWrite(dir_pin[i], LOW);
          }
        } else if (Lrad < 1.96 && Lrad > 1.17) {  //右
          Serial.println("Right");
          for (uint8_t i = 0; i <= 3; i++) {
            analogWrite(pwm_pin[i], Lpower);
            if ((i + 1) % 2 == 0) {
              digitalWrite(dir_pin[i], LOW);
            } else if ((i + 1) % 2 == 1) {
              digitalWrite(dir_pin[i], HIGH);
            }
            //digitalWrite(dir_pin[0],LOW);
            //digitalWrite(dir_pin[1],HIGH);
            //digitalWrite(dir_pin[2],HIGH);
            //digitalWrite(dir_pin[3],LOW);
          }
        } else if (Lrad < -1.17 && Lrad > -1.96) { //左
          Serial.println("Left");
          for (uint8_t i = 0; i <= 3; i++) {
            analogWrite(pwm_pin[i], Lpower);
            if ((i + 1) % 2 == 0) {
              digitalWrite(dir_pin[i], HIGH);
            } else if ((i + 1) % 2 == 1) {
              digitalWrite(dir_pin[i], LOW);
            }
          }
        } else if (Lrad < -1.96 && Lrad > -2.74) { //左前
          Serial.println("Front Left");
          for (uint8_t i = 0; i <= 3; i++) {
            digitalWrite(dir_pin[i], HIGH);
            if ((i + 1) % 2 == 1) {
              analogWrite(pwm_pin[i], 0);
            } else {
              analogWrite(pwm_pin[i], Lpower);
            }
          }
        } else if (Lrad < 2.74 && Lrad > 1.96) {  //右前
          Serial.println("Front Right");
          for (uint8_t i = 0; i <= 3; i++) {
            digitalWrite(dir_pin[i], HIGH);
            if ((i + 1) % 2 == 1) {
              analogWrite(pwm_pin[i], Lpower);
            } else {
              analogWrite(pwm_pin[i], 0);
            }
          }
        } else if (Lrad < 1.17 && Lrad > 0.39) {  //右後ろ
          Serial.println("Back Right");
          for (uint8_t i = 0; i <= 3; i++) {
            digitalWrite(dir_pin[i], LOW);
            if ((i + 1) % 2 == 1) {
              analogWrite(pwm_pin[i], 0);
            } else {
              analogWrite(pwm_pin[i], Lpower);
            }
          }
        } else if (Lrad < -0.39 && Lrad > -1.17) { //左後ろ
          Serial.println("Back Left");
          for (uint8_t i = 0; i <= 3; i++) {
            digitalWrite(dir_pin[i], LOW);
            if ((i + 1) % 2 == 1) {
              analogWrite(pwm_pin[i], Lpower);
            } else {
              analogWrite(pwm_pin[i], 0);
            }
          }
        }
      } else if (Rx > 10 || Rx < -10 || Ry > 10 || Ry < -10) {
        Rrad = atan2(Rx, Ry);
        Rpower = map(sqrt(Rx * Rx + Ry * Ry), 11, 180, 0, 255);
        if (Rrad < 1.96 && Rrad > 1.17) {        //右旋回
          Serial.println("Turn Right");
          digitalWrite(dir_pin[0], HIGH); //L
          digitalWrite(dir_pin[1], HIGH);
          digitalWrite(dir_pin[2], LOW); //H
          digitalWrite(dir_pin[3], LOW);
          for (uint8_t i = 0; i <= 3; i++) {
            analogWrite(pwm_pin[i], Rpower);
          }
        } else if (Rrad < -1.17 && Rrad > -1.96) { //左旋回
          Serial.println("Turn Left");
          digitalWrite(dir_pin[0], LOW);
          digitalWrite(dir_pin[1], LOW);
          digitalWrite(dir_pin[2], HIGH);
          digitalWrite(dir_pin[3], HIGH);
          for (uint8_t i = 0; i <= 3; i++) {
            analogWrite(pwm_pin[i], Rpower);
          }
        }
      } else {                                    //停止
        for (uint8_t i = 0; i <= 3; i++) {
          analogWrite(pwm_pin[i], 0);
          digitalWrite(dir_pin[i], LOW);
        }
      }
      if (PS4.getButtonClick(PS)) {              //PSボタンを押したら切断
        Serial.print(F("\r\nPush PS"));
        PS4.disconnect();
      }
    } else {
      for (uint8_t i = 0; i <= 3; i++) {
        analogWrite(pwm_pin[i], 0);
        digitalWrite(dir_pin[i], LOW);      //安全装置
      }
    }
  }
}
