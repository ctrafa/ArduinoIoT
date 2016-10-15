//This code was created by Rafael Teixeira (github.com/ctrafa)


// Libs
#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"

EthernetClient ethClient;

byte mac[]    = INSERT MAC ADDRESS HERE AS HEX LIKE: {0x72, 0x6a, 0xbb, 0x41, 0x88, 0xfb };
char macstr[] = INSERT MAC ADDRESS HERE AS STRING LIKE: "726abb4188fb";

byte ip[]     = {192, 168, 0, 67};
byte nameService[]     = {192, 168, 0, 6};
byte gateway[]     = {192, 168, 0, 1};
byte subnet[]     = {255, 255, 255, 0};


//Bluemix config
#define ORG "Qwqnmm" // CUSTOMIZE : your IoT Foundation Organisation
#define DEVICE_TYPE "arduino"
#define DEVICE_ID "arduinouno"

char SERVER[] = ORG ".messaging.internetofthings.ibmcloud.com"; //Define Bluemix server
int MQTT_PORT = 1883; //Define Bluemix server port
char CLIENT_ID[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID; //Define Bluemix client
char AUTHMETHOD[] = "use-token-auth"; //Define Bluemix authentication method
char AUTHTOKEN[] = "ptaWlMxEaSdwE7h39@S"; //Define Bluemix authentication token
char PUBLISH_TOPIC[] = "iot-2/evt/status/fmt/json"; //Define Watson IoT Platform publish topic
char SUBSCRIBE_TOPIC[] = "iot-2/cmd/alert/fmt/json";//"iot-2/type/arduino/id/arduinouno/evt/alert/fmt/json"; //Define Watson IoT Platform subscribe topic

void messageArrived(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message Received **************");
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
}
PubSubClient client(SERVER, MQTT_PORT, messageArrived, ethClient);


int led1 = 5;
int ldrpin = A3;
int dthpin = A4;
int gaspin = A5;
int delaytime = 1000;
#define DHTTYPE DHT11

DHT dht(dthpin, DHTTYPE);

float humidity;
float temperature;
int gas;
int ldr;


void setup()
{
  Serial.begin(9600);
  dht.begin();
  pinMode(led1, OUTPUT);
  if (Ethernet.begin(mac) == 0)  // Start in DHCP Mode
  {
    Serial.println("Failed to configure DHCP, using Static Mode");
    // If DHCP Mode failed, start in Static Mode
    Ethernet.begin(mac, ip, nameService, gateway, subnet);
  }

  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  reconnect();
}

void loop(void)
{
  if (!client.connected()) reconnect();
  readSensors();
  client.publish(PUBLISH_TOPIC, (char*) buildJson().c_str()); // Send a message to IoT Foundation
  delay(delaytime);
  client.loop();
}


String buildJson() {
  String data = "{";
  data += "\n";
  data += "\"d\": {";
  data += "\n";
  data += "\"device\": \"Arduino\",";
  data += "\n";
  data += "\"ldr\": ";
  data += (int)ldr;
  data += ",";
  data += "\n";
  data += "\"temperature (C)\": ";
  data += (int)temperature;
  data += ",";
  data += "\n";
  data += "\"humidity\": ";
  data += (int)humidity;
  data += ",";
  data += "\n";
  data += "\"gas\": ";
  data += (int)gas;
  data += "\n";
  data += "}";
  data += "\n";
  data += "}";
  return data;
}

void readSensors ()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  gas = analogRead(gaspin);
  ldr = analogRead(ldrpin);


  Serial.println("*********************************************");
  Serial.print("LDR = ");
  Serial.println(ldr);
  Serial.print("Gas = ");
  Serial.println(gas);
  Serial.print("Umidade: ");
  Serial.println(humidity);
  Serial.print("Temperatura: ");
  Serial.println(temperature);
}

void reconnect() {



  while (!client.connected()) { // If the Mqtt Client is not connected, so try to connect
    Serial.print("MQTT connection...");
    if (client.connect(CLIENT_ID, AUTHMETHOD, AUTHTOKEN)) { // Try to connect the Mqtt Client
      Serial.println("connected");
      client.subscribe(SUBSCRIBE_TOPIC);
    } else {
      Serial.println("failed, rc=" + client.state());
     
      byte server[] = { 64, 233, 187, 99 };
      if (ethClient.connect(server, 80)) {
        Serial.println("trying to connected to google.com");
      } else {
        Serial.println("unable to connect to google.com");
      }
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
}




