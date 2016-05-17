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
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"

#define LED D2                                           //กำหนดขาที่ต่อ LED เป็นขา D1
#define DHTPIN D4                                        //เลือกให้ DHT11 อยู่ที่ขา 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/*******************************************
 * Configgulation
 *******************************************/
 
const char* ssid          = "no_angel";
const char* password      = "o8o9237579";
static const char* host   = "api.thingspeak.com";
static const char* apiKey = "OJDJI20NWYIRFWWB";
WiFiServer server(80);                                          //กำหนดใช้งาน TCP Server ที่ Port 80

/*===========================================*/

unsigned char status_led=0;                                     // set up LED

void setup(){
  Serial.begin(115200);                                         // Setup console
  pinMode(LED, OUTPUT);                                         //กำหนด Pin ที่ต่อกับ LED เป็น Output
  delay(10);
  Serial.println("Temperature Sensor DHT11");
  connectionWIFI();
}

 
void loop(){
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();


  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println(" *C");
    thingsSpeak(temperature,humidity);
  }
  delay(1000);
}

/* 
 * Connection Wifi
 */
void connectionWIFI(){
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);                                                   //เชื่อมต่อกับ AP
    Serial.println("MAC adress:");
    Serial.println(WiFi.macAddress());
    
    while (WiFi.status() != WL_CONNECTED)                                     //รอการเชื่อมต่อ
    {
          delay(500);
          Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");                                         //แสดงข้อความเชื่อมต่อสำเร็จ  
    server.begin();                                                           //เปิด TCP Server
    Serial.println("Server started");
    Serial.println(WiFi.localIP());                                           // แสดงหมายเลข IP ของ Server    
    
}
/*
 * Send data to ThingSpeak
 */
 void thingsSpeak(float temperature,float humitdity) {
    WiFiClient client;                                  // Use WiFiClient class to create TCP connections
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
         Serial.println("connection failed");
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


