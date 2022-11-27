// 20Nov2022 - Scriptworkz
/* https://create.arduino.cc/projecthub/officine/interfacing-arduino-mkr-or-esp-via-mqtt-node-red-101-4833bc
 *  http://pdacontrolen.com/esp8266-public-broker-mqtt-mosca-io-node-red/
 *  MQTT Client Temperature Onewire
* More information about projects PDAControl 
* PDAControl English http://pdacontrolen.com 
* Mas informacion sobre proyectos PDAControl
* PDAControl Espanol http://pdacontrolen.com 
* Channel Youtube https://www.youtube.com/c/JhonValenciaPDAcontrol/videos 
* 
* under messageReceived (callback method) add actions to be done when a msg is received.
 * to publish, call client.publish(topic,msg)
*/


//#include <ESP8266WiFi.h> //if you are using nodemcu
#include <WiFi.h> //if you are using esp32
#include <PubSubClient.h>

// Set GPIO2 as an OUTPUT pin.
#define ledPin 2
  
// Timer
unsigned long lastMillis = 0;

// WiFi information
const char* ssid = "raspberrypi 2.4G";//"YOUR SSID";
const char* password = "123456788";//"YOUR SSID PASSWORD";

// MQTT Broker information
const char* mqtt_server = "192.168.0.166";// MQTT Broker IP ADDRESS";
const char *topic = "id1"; //Topic or ID for this microcontroller
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1990;


WiFiClient espClient;//A WiFi library that Set ESP32 as client
PubSubClient client(espClient);//A client library that support MQTT

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
  //Function to establish connection to MQTT Broker
  if (!client.connected()) {
    reconnect(); 
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 5000) {
      lastMillis = millis();
      client.publish("stationA/greet", "Hello World"); //PUBLISH TO TOPIC /hello MSG world order
      client.publish("stationA/info", "It's hot and windy."); //PUBLISH TO TOPIC /hello MSG world order
     /// Serial.println (" HelLo World ");
    }  
 
  client.loop();
}

