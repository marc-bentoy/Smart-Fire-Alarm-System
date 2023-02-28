// should be able to manipulate the blink rate of the LED
#define LED_PIN 7

unsigned long start_time = 0;
unsigned long curr_time = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    start_time = millis();
}

void loop() {
    if (start_time - curr_time >= 2000) {
        digitalWrite(LED_PIN, HIGH);
        curr_time = start_time;
        return;
    }
    digitalWrite(LED_PIN, LOW);
}