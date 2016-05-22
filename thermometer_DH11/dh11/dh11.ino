/*
 * 2016/05/17
 * Creater xanio
 * Sensor DH11
 * LED RED
 * Risistence 10K
 * nodeMCU ESP8266
 * 
 */

#include "DHT.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"

#define   LED D2                                           //กำหนดขาที่ต่อ LED เป็นขา D1
#define   DHTPIN D4                                        //เลือกให้ DHT11 อยู่ที่ขา 4
#define   DHTTYPE DHT11
DHT       dht(DHTPIN, DHTTYPE);

/*******************************************
 * Configgulation
 *******************************************/
 
const char*         ssid          = "no_angel";
const char*         password      = "o8o9237579";

static const char*  host          = "api.thingspeak.com";
static const char*  apiKey        = "OJDJI20NWYIRFWWB";

WiFiClient          wclient;
PubSubClient        mqttClient(wclient);
IPAddress           mqttServer(54, 227, 37, 186);           // Update these with values suitable for your network.

/*===========================================*/

unsigned int       status_led     = 0;                          // set up LED
char*              topicMQTT      = "/FARM_1/LED01/";
String             messageMQTT    = "0";

void setup(){
  Serial.begin(115200);                                         // Setup console
  pinMode(LED, OUTPUT);                                         //กำหนด Pin ที่ต่อกับ LED เป็น Output
  delay(10);
  Serial.println("Temperature Sensor DHT11");
  connectionWIFI();

  mqttClient.setServer(mqttServer, 15507);
  mqttClient.setCallback(callback);

  Serial.println("------ Load config -------");

  
  Serial.println("--------------------------"); 
}
 
void loop(){
  messageMQTT = status_led;
  mqttClient.loop();                                          // Mqtt รออ่านข้อมูลจาก CloudMQTT
  delay(500);
  
  if (!mqttClient.connected()) {
    MQTTConnect();
  }
  char buff[2];
  messageMQTT.toCharArray(buff, 2);
  /*if(mqttClient.publish(topicMQTT,buff)){
    Serial.print("MQTT pubilsh ->");
    Serial.print(topicMQTT);
    Serial.print("||msg: ");
    Serial.print(messageMQTT);
    Serial.println(" OK");
  }else{
    Serial.println("MQTT Fail");
  }*/
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
    
  } else {
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.print(" *C");
    Serial.print(" \t");
    
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %   =>");
    delay(1000);
    thingsSpeak(temperature,humidity);
    
  }
  delay(1000);
}

void MQTTConnect() {
  
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266_MQTT", "ktbwgckp", "xc_om3Hl4iYn")) {     // Attempt to connect
      Serial.println("cloudmqtt connected ");                                 // Once connected, publish an announcement..
      
      mqttClient.subscribe("/FARM_1/LED01/");                                   // อ่านหัวข้อ Topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print(topic);
  Serial.print(" => ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
    if (payload[0] == 48){
    status_led=0;
    digitalWrite(LED,LOW); 
    Serial.println("LED OFF");
  }else if(payload[0] == 49){
    status_led=1;
    digitalWrite(LED,HIGH); 
    Serial.println("LED ON");
  }
}

 void thingsSpeak(float temperature,float humitdity) {
    WiFiClient client_1;                                  // Use WiFiClient class to create TCP connections

    if (!client_1.connect(host, 80)) {
         Serial.println("connection thingSpeak.com failed");
         Serial.println(host);
        return;
    }
    String url = "/update/";                            // We now create a URI for the request
    url += "?key=";                                     //  url += streamId;
    url += apiKey;
    url += "&field1=";
    url += temperature;
    url += "&field2=";
    url += humitdity;              
    client_1.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("Send data to thingSpeak.com success");
}

void connectionWIFI(){
    Serial.print("Wifi Connecting to SSD:");
    Serial.println(ssid);
    WiFi.begin(ssid, password);                                                   //เชื่อมต่อกับ AP
    Serial.print("MAC adress:");
    Serial.println(WiFi.macAddress());
    
    while (WiFi.status() != WL_CONNECTED)                                     //รอการเชื่อมต่อ
    {
          delay(500);
          Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected IP:");                                         //แสดงข้อความเชื่อมต่อสำเร็จ  
    Serial.println(WiFi.localIP());                                           // แสดงหมายเลข IP ของ Server    
    Serial.println("<<  <<< <<Network READY>>>>> >>>  >>");
}
