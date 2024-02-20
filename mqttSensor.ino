// lib AM2315 is used for temp and humidity sensor
#include <Adafruit_AM2315.h>

// libs for MQTT
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


/***************************************************
  This is an example for the AM2315 Humidity + Temp sensor

  Designed specifically to work with the Adafruit AM2315 Sensor
  ----> https://www.adafruit.com/products/1293

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect RED of the AM2315 sensor to 5.0V
// Connect BLACK to Ground
// Connect WHITE to i2c clock | pin 22
// Connect YELLOW to i2c data | pin 21

Adafruit_AM2315 am2315;

const char* ssid = "AmpliFi";             // Name of the wifi network
const char* pass = "pass";                // WiFi password
const char* mqtt_serv = "192.168.181.2";  // IP address of mqtt server
const int mqtt_port = 1883;               // Port of mqtt server

const char* setTopic = "sensor/AM2315";   // sets clients topic to "sensor/AM2315"
const char* subscribeTopic = "led";       // subscribes to a topic "led"

#define LED_BUILTIN 2

WiFiClient espClient;
PubSubClient client(espClient);

String clientId = "ESP32Client-";  // Creating a random client ID example: ESP32Client-e26e

unsigned long lastMsg = 0;

void setup() {

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.printf("AM2315 Test!\n");

  while (!am2315.begin()) {
    Serial.println("Sensor not found, check wiring & pullups!");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(1000);
  }

  // Connecting to the WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("");
  Serial.printf("Wifi Connected, IP: %s\n", WiFi.localIP().toString().c_str());
  /*
    function with the toString() method to convert the IPAddress object to a string,
    and c_str() to convert the stringto a C-style string 
    (null-terminated character array), which can be used with the %s specifier in printf()
  */

  // Set MQTT Server
  client.setServer(mqtt_serv, mqtt_port);
  // setting callback to callback function
  client.setCallback(callback);


  // adding random string to the end.
  clientId += String(random(0xffff), HEX);
  Serial.println(clientId);
}

void loop() {
  // Check if connected to MQTT
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // creating JSON document doc
  StaticJsonDocument<80> doc;
  // adding device_id to doc
  doc["device_id"] = clientId;
  // creating object data
  JsonObject data = doc.createNestedObject("data");
  char output[100];

  // Delay between sending messages
  unsigned long now = millis();
  if (now - lastMsg > 20000) {
    Serial.printf("Time: %l \n", now - lastMsg);
    lastMsg = now;
    float temperature, humidity;

    // Check if reading data failed
    while (!am2315.readTemperatureAndHumidity(&temperature, &humidity)) {
      delay(500);
    }

    // Add variables to data object
    data["t"] = temperature;
    data["h"] = humidity;

    // Serialise JSON and send
    serializeJson(doc, output);
    Serial.println(output);

    // publishing using mqtt
    client.publish(setTopic, output);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe(subscribeTopic);
    } else {
      Serial.printf("failed, code= %i\n", client.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // Convert the payload into a string
  String payloadString = "";
  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }
  Serial.println(payloadString);

  // Compare the payload string with the string "ON"
  if (payloadString.equals("ON")) {
    // If they match, the received message is "ON"
    // Turn the LED on
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (payloadString.equals("OFF")) {
    // Otherwise, turn the LED off
    digitalWrite(LED_BUILTIN, LOW);
  }
}