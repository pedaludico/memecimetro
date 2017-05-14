#include "ui_unny.h"

#include <Arduino.h>
#include <avr/sleep.h>
#include "sprites_gen.h"
#include "ui_common.h"
#include "LCD_Functions.h"

// #include "unny_layout.h"
#include "ema_layout.h"
int lastActionTime;
/*
     /.--------. <<<<*>>>>  .--------.
    / .-----.  \  ,#####,  /  .-----. \
   / /{{{{{{{`. \ #_   _# / .`}}} }}}\ \
  / /{{ {{{ {{ ".\|6` `6|/."}} }}}} }}\ \
 { {{ {{{ {{{{ {{;|  u  |;} }} }} }} }}} }
 { {{{{ {{{{ {{{{ \  =  /}}} }} }} }}} } }
 { { {{{ {{{{{ {{{|\___/|} }} }} }}}} }} }
 { {{{ {{{ {{____/:     :\____}}}}}} }}} }
 { {{{{.'"""`.-===-\   /-===-.`"""'.}} } }
 { {{{/           ""-.-""           \}}} }
   \ \/'       ＡＮＧＥＬＩＴＯ        '\/ /
*/

int currentTime() {
  return millis()/1000;
}

int currentSeconds() {
  return (millis()/1000) % 60;
}

int currentMinutes() {
  return ((millis()/1000) / 60) % 60;
}

int currentHours() {
  return (((millis()/1000) / 60) / 60) % 60;
}

void wakeUpNow() {
  Serial.println("-----------------------WAKING UP-----------------");
  analogWrite(9, 255);
  lastActionTime = currentTime();
}

void sleepNow() {
  clearDisplay(0);
  analogWrite(9, 0);
  updateDisplay();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0,wakeUpNow, LOW);
  sleep_mode();
  sleep_disable();
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  detachInterrupt(0);
}

void checkSleepTime() {
  Serial.println("lastActionTime:");
  Serial.println(lastActionTime);
  if(currentTime() >= (lastActionTime + sleepThreshold)) {
    Serial.println("-[=[   SLEEP TIME   ]=]-");
    sleepNow();
  }
}

void registerActionTime() {
  Serial.println("registerActionTime");
  lastActionTime = currentTime();
}
// DRAW THE SPEED NUMBER
void setUnnySpeedBig(double speed) {
  char buf[16];
  dtostrf(speed, 4, 1, buf);
  if(SPEED_FONT_SIZE == 1){
    setBigText(buf, SPEED_NUMBER_X, SPEED_NUMBER_Y);
  }
  if(SPEED_FONT_SIZE == 2) {
    setBiggerText(buf, SPEED_NUMBER_X, SPEED_NUMBER_Y);
  }
}

// DRAW THE SPEED IMAGE
void setUnnySpeedCombo(double speed) {
  setUnnySpeedBig(speed);
  s_speedometer(global_clock % speedometer_len, SPEED_IMAGE_X, SPEED_IMAGE_Y);
}

// DRAW THE ICON SET
void setUnnyIcons() {
  int x = ICONSET_LEFT_X, y = ICONSET_LEFT_Y, i = 0;
  Serial.println(ICONSET_LEFT_Y);
  Serial.println(ICONSET_LEFT_X);
  s_crankset(global_clock % crankset_len, ICONSET_LEFT_X, y + (i++)*8);
  s_road(global_clock % road_len, x, y + (i++)*8);
  s_bars(global_clock % bars_len, x, y + (i++)*8);
  s_sandclock(global_clock % sandclock_len, x, y + (i++)*8);
}
void setnounnyIconsleft() {
  int x = ICONSET_LEFT_X, y = ICONSET_LEFT_Y, i = 0;
  s_crankset(global_clock/4 % crankset_len, x, y + (i++)*8);
  s_road(global_clock/4 % road_len, x, y + 4 + (i++)*8);
}
void setnounnyIconsright() {
  int x = ICONSET_RIGHT_X, y = ICONSET_RIGHT_Y, i = 0;
  s_bars(global_clock/4 % bars_len, x, y + (i++)*4);
  s_sandclock(global_clock/4 % sandclock_len, x, y + 4 + (i++)*8);
}


// DRAW THE CADENCE NUMBER
void setUnnyCadence() {
  if (global_clock % 6 == 0) {
    cadence = random(100);
  }
  char buf[10];
  snprintf(buf, 10, "%4d", cadence);
  setText(buf, CADENCE_NUMBER_X, CADENCE_NUMBER_Y);
}

// DRAW THE TIME NUMBER
void setUnnyTime() {
  char buf[10];
  snprintf(buf, 10, "%02d:%02d", currentMinutes(), currentSeconds());
  setText(buf, TIME_NUMBER_X, TIME_NUMBER_Y);
}

// DRAW THE DISTANCE NUMBER
void setUnnyDistance() {
  char buf[16];
  double dist = 1.2f*global_clock*global_clock;
  if (dist > 9999.9) {
    dist = 9999.9;
  }
  dtostrf(dist, 5, 1, buf);
  setText(buf, DISTANCE_NUMBER_X, DISTANCE_NUMBER_Y);
}

// RENDER THE HEAD-UP DISPLAY
void setUnnyHUD(double speed) {
  setCorners();

  setUnnySpeedCombo(speed); // X
  if(ICONSET_ROWS == 1) {
    setUnnyIcons(); //X
  } else {
    setnounnyIconsleft();
    setnounnyIconsright();
  }
  setUnnyCadence(); //X
  // MAX SPEED VA ACÁ
  setUnnyTime();
  setUnnyDistance(); //X
}
