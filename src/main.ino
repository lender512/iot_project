#include <WiFi.h>
#include <PubSubClient.h>

// Update these with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Update this with your MQTT broker IP address or hostname
const char* mqttServer = "your_MQTT_broker_address";
const int mqttPort = 1883;

// Update this with your MQTT topic
const char* mqttTopic = "your_MQTT_topic";

// Initialize the WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set MQTT server and callback function
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  // Connect to MQTT broker
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
      // Subscribe to the MQTT topic
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  // Maintain MQTT connection and handle incoming messages
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming message
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message content: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Add your logic here to handle the incoming message
}

void reconnect() {
  // Loop until reconnected
  while (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
      // Subscribe to the MQTT topic
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
