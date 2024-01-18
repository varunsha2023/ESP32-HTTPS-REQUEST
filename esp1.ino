#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>

const char* ssid = "wifi1234";
const char* password = "12345678";

const String baseURL = "https://dei-convocation.onrender.com/broadcast/";
const String username = "admindei";         // Replace with your username
const String userPassword = "passworddei";  // Replace with your password

Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize MPR121
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1)
      ;
  }
}

void loop() {
  for (uint8_t i = 24; i < 37; i++) {
    if (cap.touched() & _BV(i - 24)) {
      // If pin i is touched, send the corresponding HTTP request
      sendHTTPRequest(i + 1);

      // Wait for a short delay to avoid multiple requests for the same touch
      delay(1000);
    }
  }
}

void sendHTTPRequest(int requestNumber) {
  Serial.print("Sending HTTP request for URL ");
  Serial.println(requestNumber);

  // Make HTTP GET request with the corresponding URL
  HTTPClient http;
  String url = baseURL + String(requestNumber);
  http.begin(url);

  // Add Basic Authentication headers
  String authHeader = "Basic " + base64::encode(username + ":" + userPassword);
  http.addHeader("Authorization", authHeader);

  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      // Process the payload as needed
    } else {
      Serial.println("HTTP request failed");
    }
  } else {
    Serial.println("Unable to connect to server");
  }

  http.end();
}
