#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN D2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

const char* THINGSBOARD_HOST = "******";
const char* ACCESS_TOKEN = "***********";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println();

  setup_wifi();

  client.setServer(THINGSBOARD_HOST, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  delay(2000); // Wait for 2 seconds between readings
  
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity(); // Read humidity as a percentage
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  publish_data(temperature, humidity);
}

void setup_wifi() {
  WiFi.begin("ssid", "pass");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP8266 Device", ACCESS_TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.println("Failed to connect to ThingsBoard, retrying...");
      delay(2000);
    }
  }
}

void publish_data(float temperature, float humidity) {
  // Create a JSON payload with the temperature and humidity data
  String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";

  // Publish the payload to the ThingsBoard MQTT broker
  client.publish("v1/devices/me/telemetry", (char*) payload.c_str());
}
