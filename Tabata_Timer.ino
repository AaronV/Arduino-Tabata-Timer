// LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);

// Start Button
const int startButtonPin = 13;
const int stopButtonPin = 8;
int lastStartButtonState = 0;
int lastStopButtonState = 0;

// Buzzer
const int buzzerPin = 9;

// Tabata
#include <SimpleTimer.h>
SimpleTimer timer;
boolean tabataRunning = false;
int tabataPosition = 0;
int countdown = 0;
const int tabataLength = 16;
const int tabataDefinition[] = {
  10,     // warmup
  20, 10, // 1
  20, 10, // 2
  20, 10, // 3
  20, 10, // 4
  20, 10, // 5
  20, 10, // 6
  20, 10, // 7
  20      // 8
};

void setup() {
  Serial.begin(9600);

  // LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Tabata Timer");

  // Timer
  pinMode(startButtonPin, INPUT);
  timer.setInterval(1000, checkTabataTimer);
}

// Buttons
void listenForButtonPress(int buttonPin, int lastButtonState, void (*f)(void)) {
  int buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState && buttonState == HIGH) {
    (*f)();
  }
  lastButtonState = buttonState;
}

void loop() {
  listenForButtonPress(startButtonPin, lastStartButtonState, startTabata);
  listenForButtonPress(stopButtonPin, lastStopButtonState, stopTabata);
  timer.run();
}

void printStatus() {
  lcd.setCursor(0,1);
  if (tabataRunning && countdown >= 0) {
    lcd.print("  ");
    lcd.setCursor(0,1);
    lcd.print(countdown);
  } else {
    lcd.print("Ready");
  }
}

// Tabata Timer
void checkTabataTimer() {
  Serial.print("Uptime (s): ");
  Serial.println(millis() / 1000);
  if (tabataRunning) {
    countdown--;
    if (countdown <= 0) {
      advanceTabata();
    }
    if (countdown <= 3) {
      beep();
    }
  }
  printStatus();
}

void startTabata() {
  Serial.println("Start Tabata");
  tabataRunning = true;
  tabataPosition = -1;
  beep();
  advanceTabata();
}

void stopTabata() {
  Serial.println("Stop Tabata");
  clearNumbers();
  tabataRunning = false;
  tone(buzzerPin, 900, 100);
  delay(200);
  tone(buzzerPin, 900, 100);
  delay(200);
  tone(buzzerPin, 900, 100);
}

void advanceTabata() {
  tabataPosition++;
  if (tabataPosition >= tabataLength) {
    stopTabata();
  } else {
    startCountdown(tabataDefinition[tabataPosition]);
  }
}

void startCountdown(int duration) {
  clearNumbers();
  int finishTime = millis() / 1000 + duration;
  countdown = finishTime - millis() / 1000;
  Serial.print("Position: ");
  Serial.print(tabataPosition);
  Serial.print(" - Starting ");
  Serial.print(duration);
  Serial.println(" second countdown");
}

void clearNumbers() {
  lcd.setCursor(0,1);
  lcd.print("                ");
}

// Buzzer
void beep() {
  tone(buzzerPin, 900, 500);
}

