// 25Nov2022 - Scriptworkz

//#include <ESP8266WiFi.h> //if you are using nodemcu
#include <WiFi.h> //if you are using esp32
#include <PubSubClient.h> //MQTT library

// include the Adafruit_BME280 header file.
#include <Adafruit_BME280.h>

// instance of the Adafruit_BME280 class named as bme.
Adafruit_BME280 bme;

// Use ledPin instead of GPIO2
#define ledPin 2
  
// Timer
unsigned long lastMillis = 0;

// WiFi information
const char* ssid = "YOUR SSID";
const char* password = "YOUR SSID PASSWORD";

// MQTT Broker information
const char* mqtt_server = "MQTT Broker IP ADDRESS";
const char *topic = "id1"; //Topic or ID for this microcontroller
const char *mqtt_username = ""; //Blank if no username at broker
const char *mqtt_password = ""; //Blank if no password at broker
const int mqtt_port = 1991;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() { 
  Serial.begin(115200);

  // Set ledPin as Output
  pinMode(ledPin, OUTPUT);
  // Switch off Led 
  digitalWrite(ledPin, HIGH);

  // Connecting to a MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //function call for WiFi connection
  setup_wifi(); 
 
  Serial.println("WiFi Connected...");
  Serial.print("MQTT Server IP Address:");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(String(mqtt_port)); 
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP()); 

  // Initialize BME280 sensor and if the connection is not successful,
  // print the failed status to the Serial Monitor.
  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }
}
 
//connecting to WiFI - 2.4GHz only
void setup_wifi() {
  delay(10);
  // Start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

// Instructions received from broker 
void callback(char* topic, byte* payload, unsigned int length) {
  /* To display message sent from broker */
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  /* This section is for controlling output: LED, servo etc */
  /* ------------------------------------ */
  Serial.println();
  if ((char)payload[0] == 't') {
    Serial.println("ON");
    digitalWrite(ledPin, LOW);
  } else {
     Serial.println("OFF");
    digitalWrite(ledPin, HIGH);
  }
 /* ------------------------------------ */
}

void reconnect() {
 // Loop until we're reconnected
  while (!client.connected()) {
    String client_id = "esp32-client-1";
    client_id += String(WiFi.macAddress());
    Serial.print("Attempting MQTT connection...");
 // Attempt to connect
    if (client.connect("ESP32Client",mqtt_username, mqtt_password)) {
        Serial.println("MQTT broker connected!");
        client.subscribe(topic); // Topic at this ESP32
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
  }
 // publish and subscribe
 client.publish(topic, "Hi EMQX I'm ESP32 ^^");
 client.subscribe(topic);
}

void loop() {
  //https://www.programmingelectronics.com/dtostrf/
  // dtostrf(float_value, minima_length, number_digits_after_decimal, where_to_store_string)
  char altitudeValue[15], pressureValue[15],tempValue[15], humidValue[15];
  dtostrf(bme.readAltitude(1013.25),8, 2, altitudeValue); // convert float to char 
  dtostrf(bme.readPressure(),4, 2, pressureValue); // convert float to char 
  dtostrf(bme.readHumidity(),4, 2, humidValue); // convert float to char 
  dtostrf(bme.readTemperature(),4, 2, tempValue); // convert float to char 

  //Function to establish connection to MQTT Broker
  if (!client.connected()) {
    reconnect(); 
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 5000) {
      lastMillis = millis();
      client.publish("stationA/altitude", altitudeValue); 
      client.publish("stationA/pressure", pressureValue);
      client.publish("stationA/humid", humidValue);
      client.publish("stationA/temp", tempValue);
      
      //Will display at Serial Monitor with baudrate of 115200
      Serial.print("Pressure: ");
      Serial.print(bme.readAltitude(1013.25));
      Serial.print ("m Altitude:");
      Serial.print(bme.readPressure());
      Serial.print("Pa Humidity: ");
      Serial.print(bme.readHumidity());
      Serial.print ("%RH Temperature:");
      Serial.print(bme.readTemperature());
      Serial.println("C");
    }  
 
  client.loop();
}