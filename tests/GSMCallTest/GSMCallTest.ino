#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(2, 3); // RX, TX

String phoneNumber1 = "+123456789"; // Replace with first phone number
String phoneNumber2 = "+987654321"; // Replace with second phone number
int callDuration = 10; // Call duration in seconds

void setup() {
  gsmSerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // Check if a call is in progress
  if (checkCallStatus()) {
    Serial.println("Call in progress");
  } else {
    Serial.println("No call in progress");
    
    // Make a conference call to both phone numbers
    if (makeConferenceCall()) {
      Serial.println("Conference call initiated");
      delay(callDuration * 1000); // Wait for the call to complete
      hangUpCall();
      Serial.println("Conference call ended");
    } else {
      Serial.println("Failed to initiate conference call");
    }
  }
  
  delay(1000);
}

bool makeCall(String phoneNumber) {
  gsmSerial.print("ATD");
  gsmSerial.print(phoneNumber);
  gsmSerial.println(";");
  
  // Wait for the call to connect
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    if (checkCallStatus()) {
      return true;
    }
    delay(500);
  }
  
  return false;
}

bool checkCallStatus() {
  gsmSerial.println("AT+CPAS");
  
  // Wait for the response
  String response = waitForResponse(1000);
  
  if (response.indexOf("+CPAS: 0") != -1) {
    return true;
  } else {
    return false;
  }
}

bool makeConferenceCall() {
  // Initiate the first call
  if (!makeCall(phoneNumber1)) {
    return false;
  }
  
  // Add the second call to the conference
  gsmSerial.print("AT+CHLD=2");
  gsmSerial.println(phoneNumber2);
  
  // Wait for the call to connect
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    if (checkCallStatus()) {
      return true;
    }
    delay(500);
  }
  
  return false;
}

void hangUpCall() {
  gsmSerial.println("ATH");
}

String waitForResponse(int timeout) {
  String response = "";
  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    if (gsmSerial.available() > 0) {
      response += gsmSerial.readString();
      startTime = millis(); // Reset the timeout
    }
  }
  return response;
}
