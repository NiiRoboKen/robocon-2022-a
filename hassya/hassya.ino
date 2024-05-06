#include <PS4BT.h>
#include <usbhub.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);


int dir[4] = { 22, 23, 24, 25 };
int pwm[4] = { 2, 3, 4, 5 };
int Lsp,Rsp = 0;

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial)
    ;  // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1)
      ;  // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  for (int i = 0; i < 4; i++) {
    pinMode(dir[i], OUTPUT);
    pinMode(pwm[i], OUTPUT);
  }
}

void loop() {
  Usb.Task();
  if (PS4.connected()) {

    if (Lsp < 35 && Lsp >= 0) {
      if (PS4.getButtonClick(UP)) {  //upキーを押したとき左のspeedをあげる
        Lsp = Lsp + 1;
      }
    }
    if (Lsp <= 35 && Lsp > 0) {
      if (PS4.getButtonClick(DOWN)) {  //左のspeedを下げる
        Lsp = Lsp - 1;
      }
    }
    if (Rsp < 35 && Rsp >= 0) {
      if (PS4.getButtonClick(TRIANGLE)) {  //upキーを押したとき右のspeedをあげる
        Rsp = Rsp + 1;
      }
    }
    if (Rsp <= 35 && Rsp > 0) {
      if (PS4.getButtonClick(CROSS)) {  //右のspeedを下げる
        Rsp = Rsp - 1;
      }
    }
    if (PS4.getButtonClick(L1)) {  //左を止める
      Lsp = 0;
      for (int i = 0; i < 4; i++) {
        analogWrite(pwm[i], LOW);
        digitalWrite(dir[i], LOW);
      }
    }    
    if (PS4.getButtonClick(R1)) {  //右を止める
      Rsp = 0;
      for (int i = 0; i < 4; i++) {
        analogWrite(pwm[i], LOW);
        digitalWrite(dir[i], LOW);
      }
    }
    if (PS4.getButtonPress(CIRCLE)) {  //丸ボタンで右装填
      digitalWrite(pwm[3], HIGH);
    }
    if (PS4.getButtonPress(RIGHT)) {  //右ボタンで左装填
      digitalWrite(pwm[2], HIGH);
    }
    if (PS4.getButtonPress(SQUARE)) {  //四角ボタンで右ベルト停止
      digitalWrite(pwm[3], LOW);
    }
    if (PS4.getButtonPress(LEFT)) {  //左ボタンで左ベルト停止
      digitalWrite(pwm[2], LOW);
    }
    /*if (PS4.getButtonClick(L2)) {  //左
      LR = 0;
    }
    if (PS4.getButtonClick(R2)) {  //右
      LR = 1;
    }*/

    PS4.setLed(240, 0, 145);
    //Serial.print(sp);
    //Serial.print("\n");

    analogWrite(pwm[1], Lsp);
    analogWrite(pwm[0], Rsp);
    
    if (PS4.getButtonClick(PS)) {  //PSボタンを押したら切断
      Serial.print(F("\r\nPush PS"));
      PS4.disconnect();
    }
  } else {  //切断時停止制御
    for (int i = 0; i < 4; i++) {
      digitalWrite(dir[i], LOW);
      analogWrite(pwm[i], LOW);
    }
    Lsp = 0; Rsp = 0;
  }
}