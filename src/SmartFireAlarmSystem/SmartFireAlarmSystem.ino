#include <SoftwareSerial.h>

SoftwareSerial gsm(8, 9);

const int LED_PIN = 7;
const int BUTTON_PIN = 6;
const int BUZZER_PIN = 5;
const int FLAME_PIN = 6;
const int SMOKE_PIN = 4;

// ALARM CONSTANTS
const int ALARM_INTERVAL = 2000;
const int ALARM_DURATION = 20000;
const int CALL_DURATION = 20000;

unsigned long start_time = 0;
unsigned long serial_time = 0;
unsigned long current_time = 0;
unsigned long alarm_time = 0;
unsigned long call_time = 0;

// String BFP_NUM = "09239320152";
String BRGY_NUM = "09770651742";
String DEV_NUM = "09239320152";

bool shouldAlarm = false;
bool hasSentAlarmSMS = false;
bool hasCalledAlarm = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  gsm.begin(9600);
  delay(100);
}

void loop() {
  start_time = millis();

  // SERIAL DEBUG 
  if (start_time - serial_time >= 2000) {
    Serial.print("FLAME: ");
    Serial.println(digitalRead(FLAME_PIN));
    Serial.print("SMOKE: ");
    Serial.println(digitalRead(SMOKE_PIN));
    Serial.print("BUTTON: ");
    Serial.println(digitalRead(BUTTON_PIN));
    Serial.print("start_time: ");
    Serial.println(start_time);
    Serial.print("current_time: ");
    Serial.println(current_time);
    Serial.print("alarm_time: ");
    Serial.println(alarm_time);
    Serial.println("----------------");
    serial_time = start_time;
  }

  // if flame is detected
  if (((digitalRead(FLAME_PIN) == HIGH && digitalRead(SMOKE_PIN) == HIGH) || digitalRead(BUTTON_PIN) == HIGH) && !shouldAlarm) {
    Serial.println("!!!!");
    Serial.println("FLAME DETECTED OR FIRE ALARM PRESSED");
    Serial.println("!!!!");
    shouldAlarm = true;
    current_time = start_time;
    alarm_time = start_time;
    sendSMS();
    call();
  }

  if (!shouldAlarm) {
    return; 
  }

  if (start_time - call_time > CALL_DURATION) {
    gsm.print("ATH");
    call_time = start_time;
  }

  if (start_time - current_time > ALARM_DURATION) {
    Serial.println("*****");
    Serial.println("ALARM DURATION reached!");
    Serial.println("*****");

    // turn off alarms
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    shouldAlarm = false;
    return;
  }

  if (start_time - alarm_time >= ALARM_INTERVAL && start_time - alarm_time <= ALARM_INTERVAL * 2) {
    Serial.println("...");
    Serial.println("ALARM INTERVAL reached");
    Serial.println("...");
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    return;
  }

  if (start_time - alarm_time >= ALARM_INTERVAL) {
    alarm_time = start_time;
    return;
  }

  Serial.println("!!ALARMING!!");
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void sendSMS() {
  gsm.println("AT+CMGF=1");
  delay(100);
  gsm.println("AT+CMGS=\"09239320152\"\r");
  delay(100);
  gsm.println("[FIRE ALARM]");
  delay(100);
  gsm.println((char)26);
  delay(100);
}

void call() {
  gsm.println("AT");
  delay(500);
  gsm.println("ATD+639239320152;");
  gsm.println();
}

void endCall() {
  gsm.print("ATH");
  call_time = start_time;
  delay(100);
}
