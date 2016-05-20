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
#include "model.h"

#define LED D2                                           //กำหนดขาที่ต่อ LED เป็นขา D1
#define DHTPIN D4                                        //เลือกให้ DHT11 อยู่ที่ขา 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/*******************************************
 * Configgulation
 *******************************************/
 
 char* ssid          = "no_angel";
 char* password      = "o8o9237579";
static const char* host   = "api.thingspeak.com";
static const char* apiKey = "OJDJI20NWYIRFWWB";


WiFiClient wclient;
PubSubClient mqttClient(wclient);

float   tempurature[]     ={25,60};                              // Defalut temporature range;
bool    switchStatus_01   = false;

IPAddress mqttServer(54, 227, 37, 186);                 // Update these with values suitable for your network.
/*===========================================*/

unsigned char status_led=0;                                     // set up LED

void setup(){
  Serial.begin(115200);                                         // Setup console
  pinMode(LED, OUTPUT);                                         //กำหนด Pin ที่ต่อกับ LED เป็น Output
  delay(10);
  Serial.println("Temperature Sensor DHT11");
  connectionWIFI(ssid,password);

  mqttClient.setServer(mqttServer, 15507);
  mqttClient.setCallback(callback);  

  if (mqttClient.connect("ESP8266_MQTT", "ktbwgckp", "xc_om3Hl4iYn")){
      mqttClient.subscribe("/LED_ESP/dog/");
     Serial.println("ESP8266_MQTT connected ");
  }else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
  }
  
}

 
void loop(){
  delay(500);
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
    Serial.println(" %");
    delay(1000);
    //temporatureSetup();
    
    //OnOffLED(temperature);
    thingsSpeak(temperature,humidity);
    
  }
  delay(3000);
}


/*
 * Send data to ThingSpeak
 */
 void thingsSpeak(float temperature,float humitdity) {
    WiFiClient client;                                  // Use WiFiClient class to create TCP connections
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
         Serial.println("connection thingSpeak.com failed");
        return;
    }
    String url = "/update/";                            // We now create a URI for the request
    url += "?key=";                                     //  url += streamId;
    url += apiKey;
    url += "&field1=";
    url += temperature;
    url += "&field2=";
    url += humitdity;

              // This will send the request to the server
              
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
}
/*
 * Model
*/
/*void temporatureSetup(){
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(hostEDuck, httpPort)) {
    Serial.println("connection tempurature log sever failed");
    return;
  }
 
  
  String url = "/eDuck/led.json";                                                // We now create a URI for the request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +                         // This will send the request to the server
               "Host: "+hostEDuck+"\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  while(client.available()){                                                    // Read all the lines of the reply from server and print them to Serial
    String line = client.readStringUntil('\r');
    jsonData(line);
  }
}
*/
void jsonData(String text){
  //Serial.println(text);
  if (!text.startsWith("{\"t1\"",1)){
    return ;
  }  
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(text);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return ;
  }
  String tt1 = root["t1"];
  String tt2 = root["t2"];
  String tt3 = root["led1"];
  
  if (tt3 == "1"){  
    switchStatus_01 = true;
  }else{
    switchStatus_01 = false;
  }
  Serial.println("t1: "+tt1+" t2: "+tt2+" LED: "+tt3);
  tempurature[0] = tt1.toFloat();
  tempurature[1] = tt2.toFloat();  
}

void OnOffLED(float presentTempurature){

  boolean test = (tempurature[0] <= presentTempurature) && (presentTempurature <= tempurature[1]);
  
  if(switchStatus_01) test= test;
  else test= !test;
  
  //Serial.print("test ::");
  //Serial.println(test);
  
  if(test){
    digitalWrite(LED,HIGH); 
    Serial.println("LED ON");
  }else{
    digitalWrite(LED,LOW);
    Serial.println("LED OFF");
  }

}

