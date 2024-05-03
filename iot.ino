#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// WiFi credentials
const char* ssid = "WiFiSSID";
const char* password = "WiFiPassword";

// ThingSpeak credentials
const char* thingSpeakAddress = "api.thingspeak.com";
const String apiKey = "ThingSpeakAPIKey";

// Ultrasonic sensor pins
const int trigPin = D5; // GPIO pin connected to the sensor's trigger pin
const int echoPin = D6; // GPIO pin connected to the sensor's echo pin

// Threshold distance for obstacle detection (in cm)
const int obstacleThreshold = 20;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  long duration, distance;
  
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance (in cm) based on the speed of sound
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if obstacle is detected
  if (distance < obstacleThreshold) {
    Serial.println("Obstacle detected!");
  } else {
    Serial.println("No obstacle detected.");
  }

  // Send distance to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    // Create a client for ThingSpeak
    WiFiClient client;
    
    // Prepare the ThingSpeak update
    ThingSpeak.setField(1, distance); // Field 1 corresponds to the distance data
    int statusCode = ThingSpeak.writeFields(YourChannelNumber, apiKey);

    // Check if update was successful
    if (statusCode == 200) {
      Serial.println("Data sent to ThingSpeak successfully");
    } else {
      Serial.print("Error sending data to ThingSpeak. HTTP error code ");
      Serial.println(statusCode);
    }
  } else {
    Serial.println("WiFi connection lost. Unable to send data to ThingSpeak");
  }

  // Wait before sending next update
  delay(5000); // Send data every 5 seconds (adjust as needed)
}
