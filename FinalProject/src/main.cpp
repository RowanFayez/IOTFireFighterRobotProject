#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "M1M2#";  // Replace with your WiFi SSID
const char* password = "Mn123456Mn#";  // Replace with your WiFi password
WiFiClientSecure espClient;

// MQTT broker settings
const char* mqttServer = "3653c25602b04b5fa79a6836417632e7.s1.eu.hivemq.cloud";  // HiveMQ broker
const int mqttPort = 8883;  // Secure MQTT port
const char* mqttUser = "esraa_123";  // MQTT username
const char* mqttPassword = "Me_br123";  // MQTT password

WiFiClientSecure espClient;  
PubSubClient client(espClient);

// HiveMQ Cloud Let's Encrypt CA certificate
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Global Variables for Display and Timing
unsigned long previousMillis = 0; // Store last update time
const long interval = 5000; // Interval between sensor readings (5 seconds)
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
// Topics for MQTT
const char* flameTopic = "firefighter/flame";
const char* smokeTopic = "firefighter/smoke";
const char* stateTopic = "firefighter/state";
const char* waterPumpTopic = "home/waterPump";
const char* servoTopic = "home/servo";

// Servo Initialization 
Servo myservo;
int pos = 0;
bool fire = false;  // Use `bool` for boolean values

// Pin definitions
#define IN1 17
#define IN2 5
#define IN3 18
#define IN4 19
#define flameL 36
#define flameM 39
#define flameR 34
#define buzzer 23
#define relaypump 16
#define smoke 35
#define servoPin 32

// Function declarations
void _stop();
void forward();
void backward();
void right();
void left();
void _buzzer();
void put_off_fire();
void setup_wifi();
void reconnect();
void publishStatus(const char* status);
void callback(char* topic, byte* payload, unsigned int length);  
void connectToMQTT();
void forward();

void setup() {
  Serial.begin(115200);

  // Improved Wi-Fi setup with timeouts to avoid infinite loops
  setup_wifi();

  // MQTT setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);  // Set the MQTT callback function

  connectToMQTT();

  // Motor pins setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  _stop();

  // Flame sensor pins setup
  pinMode(flameL, INPUT);
  pinMode(flameM, INPUT);
  pinMode(flameR, INPUT);

  // Buzzer pin setup
  pinMode(buzzer, OUTPUT);

  // Smoke sensor setup
  pinMode(smoke, INPUT);

  // Servo setup
  myservo.attach(servoPin);
  myservo.write(90);
}

void loop() {
  // Reconnect if disconnected from MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Handle incoming/outgoing MQTT messages

  // Sensor readings
  int smokeval = analogRead(smoke);
  int flameLVal = digitalRead(flameL);
  int flameMVal = digitalRead(flameM);
  int flameRVal = digitalRead(flameR);

  // Publish flame sensor readings
  char flameMsg[50];
  snprintf(flameMsg, sizeof(flameMsg), "L:%d M:%d R:%d", flameLVal, flameMVal, flameRVal);
  client.publish(flameTopic, flameMsg);

  // Publish smoke sensor readings
  char smokeMsg[50];
  snprintf(smokeMsg, sizeof(smokeMsg), "%d", smokeval);
  client.publish(smokeTopic, smokeMsg);

  // Robot operation logic
  if (flameLVal == 1 && flameMVal == 1 && flameRVal == 1) {
    _stop();
    publishStatus("Idle");
  } else {
    handleFire(flameLVal, flameMVal, flameRVal);
  }

  delay(1000);  // Adjust as needed
}

void handleFire(int flameLVal, int flameMVal, int flameRVal) {
  if (flameMVal == 0) {
    forward();
    fireActions("In action - Front");
  } else if (flameLVal == 0) {
    left();
    fireActions("In action - Left");
  } else if (flameRVal == 0) {
    right();
    fireActions("In action - Right");
  }

  if (fire) {
    put_off_fire();
    fire = false;
    publishStatus("Fire extinguished");
  }
}

void fireActions(const char* action) {
  delay(300);
  _stop();
  _buzzer();
  fire = true;
  publishStatus(action);
}

// Wi-Fi connection function with timeout
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {  // Timeout after 15 seconds
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

// MQTT connect function
void connectToMQTT() {
  espClient.setCACert(root_ca);  // Set CA Certificate for SSL connection

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Firefighter", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
      client.subscribe(waterPumpTopic);  // Subscribe to water pump control
      client.subscribe(servoTopic);      // Subscribe to servo control
    } else {
      Serial.print("Failed to connect, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// Reconnect function for MQTT
void reconnect() {
  while (!client.connected()) {
    connectToMQTT();
  }
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.printf("Message received on topic: %s, Message: %s\n", topic, message.c_str());

  if (String(topic) == waterPumpTopic) {
    handleWaterPump(message);
  } else if (String(topic) == servoTopic) {
    handleServo(message);
  }
}

void handleWaterPump(String message) {
  if (message == "on") {
    digitalWrite(relaypump, HIGH); // Turn the water pump ON
    Serial.println("Water pump turned ON");
  } else if (message == "off") {
    digitalWrite(relaypump, LOW); // Turn the water pump OFF
    Serial.println("Water pump turned OFF");
  }
}

void handleServo(String message) {
  int angle = message.toInt();
  if (angle >= 0 && angle <= 180) {
    myservo.write(angle); // Set the servo to the specified angle
    Serial.printf("Servo angle set to: %d\n", angle);
  } else {
    Serial.println("Invalid servo angle");
  }
}

// Publish MQTT status
void publishStatus(const char* status) {
  client.publish(stateTopic, status);
}

// Stop motor function
void _stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Move forward function
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Move backward function
void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Turn right function
void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Turn left function
void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Buzzer function
void _buzzer() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

// Fire extinguishing function
void put_off_fire() {
  digitalWrite(relaypump, HIGH);  // Activate water pump
  delay(5000);  // Keep water pump on for 5 seconds
  digitalWrite(relaypump, LOW);   // Deactivate water pump
}

