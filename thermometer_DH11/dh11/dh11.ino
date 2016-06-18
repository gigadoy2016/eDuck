/*
 * 2016/05/17
 * Creater xanio          | MQTT   user:controller_1 pass:123456789
 * Sensor DH11            |
 * LED RED                |
 * Risistence 10K         |
 * nodeMCU ESP8266        |
 * 
 */

#include "DHT.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"
#include "LED.h"
#include "TimeInterval.h"
#include "connection.h"

#define                 DHTPIN D4                                        //เลือกให้ DHT11 อยู่ที่ขา 4
#define                 DHTTYPE DHT11
DHT                     dht(DHTPIN, DHTTYPE);

/*******************************************
 * Configgulation
 *******************************************/
 
#define                 WLAN_SSID       "no_angel"
#define                 WLAN_PASS       "o8o9237579"

#define                 MQTT_USER       "controller_01"
#define                 MQTT_PASSWORD   "123456789"
#define                 TOPIC_SUB       "DEVICE/01/command/"
#define                 TOPIC_PUB       "DEVICE/01/status/"

static const char*      HOST_CLOUD   = "api.thingspeak.com";
static const char*      apiKEY_CLOUD = "OJDJI20NWYIRFWWB";

WiFiClient              wclient;
PubSubClient            mqttClient(wclient);
IPAddress               mqttServer(54, 234, 232, 210);           // Update these with values suitable for your network.

LED                     led_01(D1);
LED                     led_02(D2);
LED                     led_03(D5);
LED                     led_04(D6);

TimeInterval            MQTT_time_publish(15000);

unsigned int            status_led     = 0;                          // set up LED
float                   tempurature[]  ={25,60};                     // Defalut temporature range;

/*---------------------
 *   setup system
 -----------------------*/

void setup(){
  Serial.begin(115200);                                         // Setup console
  Serial.println("Temperature Sensor DHT11");
  connectionWIFI(WLAN_SSID,WLAN_PASS);

  loadConfigDevice();               // ***Process 1****>>>>>>>>>Load config from server

  mqttClient.setServer("m11.cloudmqtt.com", 15507);
  mqttClient.setCallback(callback);
  MQTTConnect();
}
 
void loop(){
  mqttClient.loop(); 
  MQTTConnect();
                                                              // Mqtt รออ่านข้อมูลจาก CloudMQTT
  if(MQTT_time_publish.every(15000)){
    MQTTtoCloud();
  }
}

void MQTTConnect() {
  
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266_MQTT", MQTT_USER, MQTT_PASSWORD)) {         // Attempt to connect
      Serial.println("cloudmqtt connected ");                                  // Once connected, publish an announcement..
      mqttClient.subscribe(TOPIC_SUB);                                         // อ่านหัวข้อ Topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  String message="";
  Serial.print("MQTT Read      =>");
  Serial.print(topic);
  Serial.print("=> ");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  String device = root["deviceID"];
  if(device == "0001"){
    led_01.set((int)root["led_01"]);
    led_02.set((int)root["led_02"]);
    led_03.set((int)root["led_03"]);
    led_04.set((int)root["led_04"]);
    MQTTtoCloud();
  }
}


 void thingsSpeak(float temperature,float humitdity) {
    WiFiClient client_1;                                  // Use WiFiClient class to create TCP connections

    if (!client_1.connect(HOST_CLOUD, 80)) {
         Serial.print(HOST_CLOUD);
         Serial.println("connection failed");
        return;
    }
    String url = "/update/";                            // We now create a URI for the request
    url += "?key=";                                     //  url += streamId;
    url += apiKEY_CLOUD;
    url += "&field1=";
    url += temperature;
    url += "&field2=";
    url += humitdity;              
    client_1.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + HOST_CLOUD + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("thingSpeak.com =>Send!! success");
}

void MQTTtoCloud(){
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("!! ERROR DHT11 can't read data !!");    
    MQTT_time_publish.reset();                                         //Reset MQTT
        
  } else {
    String dataJson = "{\"temp\":\""+(String)temperature+"\",\"Hum\":\""+(String)humidity+"\",\"led_01\":"+led_01.readStatusLED()
                      +",\"led_02\":"+led_02.readStatusLED()
                      +",\"led_03\":"+led_03.readStatusLED()
                      +",\"led_04\":"+led_04.readStatusLED()+"}";
    thingsSpeak(temperature,humidity);
    char charBuf[200];
    dataJson.toCharArray(charBuf,200);
    mqttClient.publish(TOPIC_PUB,charBuf );
    Serial.print("MQTT Send !    =>");
    Serial.print("Topic:"+(String)TOPIC_PUB);
    Serial.println(charBuf);
  }
}

void loadConfigDevice(){
  WiFiClient  client;
  IPAddress   configServer(192,168,1,29);                     // Update JSON log for control   
  const int   httpPort = 80;
  if (!client.connect(configServer, httpPort)) {
    Serial.println(">>>>>>>>>>>> loading...");
  }
  
  String url = "/eDuck/led.json";                                                // We now create a URI for the request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +                         // This will send the request to the server
               "Host: gigadoy\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.available()){                                                    // Read all the lines of the reply from server and print them to Serial
    String line = client.readStringUntil('\r');
    jsonData(line);
  }
  led_01.set(status_led);
  Serial.print("{t1:");  Serial.print(tempurature[0]);  Serial.print(" t2:"); Serial.print(tempurature[1]);
  Serial.print(" LED:"); Serial.print(status_led); Serial.println("}");
}

void jsonData(String text){
if (!text.startsWith("{\"t1\"",1)){
  return ;
}  
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(text);
  if (!root.success()) {
    Serial.println("parseObject() failed");
  }
  String tt1 = root["t1"];
  String tt2 = root["t2"];
  String tt3 = root["led1"];
  status_led = tt3.toInt();
  
  tempurature[0] = tt1.toFloat();
  tempurature[1] = tt2.toFloat(); 
}
