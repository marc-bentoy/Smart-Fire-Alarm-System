#include <SoftwareSerial.h>

SoftwareSerial gsm(8, 9);

const int LED_PIN = 7;
const int BUTTON_PIN = 6;
const int BUZZER_PIN = 5;
const int FLAME_PIN = 4;
// const int SMOKE_PIN = 4;

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
String BRGY_NUM = "+639770651742";
String DEV_NUM = "+639239320152";
String FIRE_THRESHOLD_EXCEEDED_MSG = "🔥🔥FIRE ALARM🔥🔥";
String FIRE_ALARM_PRESSED_MSG = "🔽🔽 FIRE ALARM PRESSED 🔽🔽";
String FIRE_AND_SMOKE_DETECTED = "🔥💨FLAME AND SMOKE DETECTED🔥💨";

bool shouldAlarm = false;
bool hasSentAlarmSMS = false;
bool hasCalledAlarm = false;
bool isCalling = false;

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

  // if fire alarm button is pressed 
  if (digitalRead(BUTTON_PIN) == HIGH && !shouldAlarm) {
    Serial.println("🔽🔽 FIRE ALARM PRESSED 🔽🔽");
    shouldAlarm = true;
    current_time = start_time;
    alarm_time = start_time;
    sendSMS(FIRE_ALARM_PRESSED_MSG);
    call();
  }

  // if flame is detected
  if (digitalRead(FLAME_PIN) == LOW && !shouldAlarm) {
    Serial.println("🔥💨FLAME AND SMOKE DETECTED🔥💨");
    shouldAlarm = true;
    current_time = start_time;
    alarm_time = start_time;
    sendSMS(FIRE_THRESHOLD_EXCEEDED_MSG);

    if (!isCalling)
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
    stopCall();

    // turn off alarms
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    shouldAlarm = false;
    return;
  }

  if (start_time - alarm_time >= ALARM_INTERVAL && start_time - alarm_time <= ALARM_INTERVAL * 2) {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    return;
  }

  if (start_time - alarm_time >= ALARM_INTERVAL) {
    alarm_time = start_time;
    return;
  }

  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void sendSMS(String message) {
  gsm.println("AT+CMGF=1");
  delay(100);
  gsm.println("AT+CMGS=\"" + DEV_NUM + "\"\r");
  delay(100);
  gsm.println(message);
  delay(100);
  gsm.println((char)26);
  delay(100);
}

void call() {
  gsm.println("AT");
  delay(500);
  gsm.println("ATD" + DEV_NUM + ";");
  gsm.println();
  isCalling = true;
}

void stopCall() {
  gsm.print("ATH");
  call_time = start_time;
  delay(100);
  isCalling = false;
  Serial.println("*******");
  Serial.println("Call Alarm Ended");
  Serial.println("*******");
}
