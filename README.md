# AM2315 Sensor MQTT Example

This project demonstrates how to use an AM2315 Temperature and Humidity sensor with an ESP32 microcontroller to publish sensor data to an MQTT server and control an LED based on received messages.

## Setup

1. Connect the AM2315 sensor to the ESP32.
2. Configure WiFi and MQTT settings.
3. Upload the sketch to your ESP32.
4. Monitor serial output for status and sensor readings.

## Dependencies

- **Adafruit_AM2315**
- **ArduinoJson**
- **WiFi**
- **PubSubClient**
- **Wire**

## Configuration

- **ssid**: WiFi network name.
- **pass**: WiFi password.
- **mqtt_serv**: MQTT server IP.
- **mqtt_port**: MQTT server port.
- **setTopic**: MQTT topic for sensor data.
- **subscribeTopic**: MQTT topic for LED control.
