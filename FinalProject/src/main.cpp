#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char *ssid = "Redmi Note 8";           // Replace with your WiFi SSID
const char *password = "siromerodouble123"; // Replace with your WiFi password
WiFiClientSecure espClient;

// MQTT broker settings
const char *mqttServer = "3653c25602b04b5fa79a6836417632e7.s1.eu.hivemq.cloud"; // HiveMQ broker
const int mqttPort = 8883;                                                      // Secure MQTT port
const char *mqttUser = "esraa_123";                                             // MQTT username
const char *mqttPassword = "Me_br123";                                          // MQTT password

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
const long interval = 5000;       // Interval between sensor readings (5 seconds)

// Topics for MQTT
const char *flameTopic = "firefighter/flame";
const char *stateTopic = "firefighter/state";
const char *waterPumpTopic = "home/waterPump";
const char *servoTopic = "home/servo";

// Servo Initialization
Servo myservo;
int pos = 0;
bool fire = false; // Use bool for boolean values
boolean sensors_paused = false; // Flag to indicate if sensors are paused
// Pin definitions
#define ENA 33
#define IN1 17
#define IN2 5
#define ENB 35
#define IN3 18
#define IN4 19
#define flameL 36
#define flameM 39
#define flameR 34
#define buzzer 23
#define relaypump 16
// #define smoke 35
#define servoPin 32

// Speed control variable (0-255)
int Speed = 120; // Adjust the motor speed (0-255)

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
void publishStatus(const char *status);
void callback(char *topic, byte *payload, unsigned int length);
void connectToMQTT();
void forward();
void handleWaterPump(String message);
void fireActions(const char *action);
void handleServo(String message);
void handleFire(int flameLVal, int flameMVal, int flameRVal);

void setup()
{
  Serial.begin(115200);

  // Improved Wi-Fi setup with timeouts to avoid infinite loops
  setup_wifi();

  // MQTT setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // Set the MQTT callback function

  connectToMQTT();

  // Motor pins setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  analogWrite(ENA, 120); // Set motor speed once at the start
  analogWrite(ENB, 120); // Set motor speed once at the start

  _stop();

  // Flame sensor pins setup
  pinMode(flameL, INPUT);
  pinMode(flameM, INPUT);
  pinMode(flameR, INPUT);

  // Relay and motor pins setup
  pinMode(relaypump, OUTPUT);
  digitalWrite(relaypump, LOW); // Ensure the water pump is OFF at startup
  Serial.println("Relay pump set to OFF at startup (LOW).");

  // Buzzer pin setup
  pinMode(buzzer, OUTPUT);

  // Smoke sensor setup
  // pinMode(smoke, INPUT);

  // Servo setup
  myservo.attach(servoPin);
  myservo.write(90);
  // Delay to ensure stable startup state
  delay(1000);  // 1 second delay to ensure everything initializes properly
  Serial.println("Setup complete.");
}
// Function to set motor speed
void setMotorSpeed(int speed) {
    Speed = constrain(speed, 0, 255);
    analogWrite(ENA, Speed); // Set speed for right motor
    analogWrite(ENB, Speed); // Set speed for left motor
}
// Function for gradual acceleration
void softStart(int targetSpeed) {
    for (int speed = 0; speed <= targetSpeed; speed++) {
        setMotorSpeed(speed);
        delay(10); // Delay for smoother acceleration
    }
}

// Function for gradual deceleration
void softStop() {
    for (int speed = Speed; speed >= 0; speed--) {
        setMotorSpeed(speed);
        delay(10); // Delay for smoother deceleration
    }
}

void loop()
{

  // Reconnect if disconnected from MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop(); // Handle incoming/outgoing MQTT messages

  // Sensor readings
  // int smokeval = analogRead(smoke);
  int flameLVal = digitalRead(flameL);
  int flameMVal = digitalRead(flameM);
  int flameRVal = digitalRead(flameR);
  // Robot operation logic
  if (flameLVal == 1 && flameMVal == 1 && flameRVal == 1)
  {
    _stop();
    publishStatus("Idle");
  }
  else
  {
    handleFire(flameLVal, flameMVal, flameRVal);
    delay(100);
    while (fire)
    {
      Serial.println("Calling put_off_fire()...");
      put_off_fire();
      publishStatus("Fire extinguished");
    }
  }

  // Publish flame sensor readings
  char flameMsg[50];
  snprintf(flameMsg, sizeof(flameMsg), "L:%d M:%d R:%d", flameLVal, flameMVal, flameRVal);
  client.publish(flameTopic, flameMsg);


  delay(1000); // Adjust as needed
}
void fireActions(const char *action)
{
  delay(300);
  _stop();
  _buzzer();
  fire = true; // This should activate put_off_fire() in the next cycle
  publishStatus(action);
  Serial.println("Fire detected, fire flag set to true.");
}

void handleFire(int flameLVal, int flameMVal, int flameRVal)
{
  if (flameMVal == 0)
  {
    forward();
    delay(400);
    _stop();
    fireActions("In action - Front");
  }
  else if (flameLVal == 0)
  {
    left();
    delay(400);
    _stop();
    fireActions("In action - Left");
  }
  else if (flameRVal == 0)
  {
    right();
    delay(400);
    _stop();
    fireActions("In action - Right");
  }
}

// Wi-Fi connection function with timeout
void setup_wifi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000)
  { // Timeout after 15 seconds
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected to WiFi");
  }
  else
  {
    Serial.println("\nFailed to connect to WiFi");
  }
}

// MQTT connect function
void connectToMQTT()
{
  espClient.setCACert(root_ca); // Set CA Certificate for SSL connection

  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Firefighter", mqttUser, mqttPassword))
    {
      Serial.println("Connected to MQTT");
      client.subscribe(waterPumpTopic); // Subscribe to water pump control
      client.subscribe(servoTopic);     // Subscribe to servo control
    }
    else
    {
      Serial.print("Failed to connect, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// Reconnect function for MQTT
void reconnect()
{
  while (!client.connected())
  {
    connectToMQTT();
  }
}

// MQTT callback function
void callback(char *topic, byte *payload, unsigned int length)
{
  String message;
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.printf("Message received on topic: %s, Message: %s\n", topic, message.c_str());

  if (String(topic) == waterPumpTopic)
  {
    handleWaterPump(message);
  }
  else if (String(topic) == servoTopic)
  {
    handleServo(message);
  }
}

void handleWaterPump(String message)
{
  if (message == "on")
  {
    digitalWrite(relaypump, HIGH); // Turn the water pump ON
    Serial.println("Water pump turned ON");
  }
  else if (message == "off")
  {
    digitalWrite(relaypump, LOW); // Turn the water pump OFF
    Serial.println("Water pump turned OFF");
  }
}

void handleServo(String message)
{
  int angle = message.toInt();
  if (angle >= 0 && angle <= 180)
  {
    myservo.write(angle); // Set the servo to the specified angle
    Serial.printf("Servo angle set to: %d\n", angle);
  }
  else
  {
    Serial.println("Invalid servo angle");
  }
}

// Publish MQTT status
void publishStatus(const char *status)
{
  client.publish(stateTopic, status);
}

// Function to move forward with speed control
void forward()
{
  Serial.println("Moving forward.");
  analogWrite(ENA, Speed); // Set speed for right motor
  analogWrite(ENB, Speed); // Set speed for left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(200); // Move forward for a short duration
  _stop();
}

// Function to move backward
void backward()
{
  Serial.println("Moving backward.");
  analogWrite(ENA, Speed); // Set speed for right motor
  analogWrite(ENB, Speed); // Set speed for left motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(200); // Move backward for a short duration
  _stop();
}

// Function to turn right
void right()
{
  Serial.println("Turning right.");
  analogWrite(ENA, Speed); // Set speed for right motor
  analogWrite(ENB, Speed); // Set speed for left motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(200); // Turn for a short duration
  _stop();
}

// Function to turn left
void left()
{
  Serial.println("Turning left.");
  analogWrite(ENA, Speed); // Set speed for right motor
  analogWrite(ENB, Speed); // Set speed for left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(200); // Turn for a short duration
  _stop();
}

// Function to stop the robot
void _stop()
{
  Serial.println("Stopping motors.");
  analogWrite(ENA, 0); // Set speed for right motor
  analogWrite(ENB, 0); // Set speed for left motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Buzzer function
void _buzzer()
{
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

void put_off_fire()
{
  _stop();
  delay(100);
  Serial.println("Putting out fire...");

  digitalWrite(relaypump, HIGH); // Turn ON the water pump
  Serial.println("Relay pump ON (High). Water pump activated.");

  // delay(300); // Add a short delay between the relay and servo action

  // Sweep the servo to put out the fire
  for (pos = 10; pos <= 160; pos += 1)
  {
    myservo.write(pos);
    delay(25);
  }
  for (pos = 160; pos >= 10; pos -= 1)
  {
    myservo.write(pos);
    delay(25);
  }

  myservo.write(90);
  digitalWrite(relaypump, LOW); // Turn OFF the water pump
  Serial.println("Relay pump OFF (LOW). Water pump deactivated.");

  fire = false;
}