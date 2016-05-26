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
float              tempurature[]  ={25,60};                              // Defalut temporature range;

void setup(){
  Serial.begin(115200);                                         // Setup console
  pinMode(LED, OUTPUT);                                         //กำหนด Pin ที่ต่อกับ LED เป็น Output
  delay(10);
  Serial.println("Temperature Sensor DHT11");
  connectionWIFI();

  mqttClient.setServer(mqttServer, 15507);
  mqttClient.setCallback(callback);

  Serial.println("------ Load config -------");
  loadConfigulation();
  Serial.println("--------------------------"); 
}
 
void loop(){
  delay(500);
  if (!mqttClient.connected()) {
    MQTTConnect();
  }
  mqttClient.loop();                                          // Mqtt รออ่านข้อมูลจาก CloudMQTT
  
  delay(2500);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  delay(500);
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("--------------------------------------------------------------------------- READ sensor DHT11 Fail");
    
  } else {
    Serial.print("LED status :");
    Serial.print(digitalRead(LED));
    Serial.print("  Temperature: "); 
    Serial.print(temperature);
    Serial.print(" *C");
    Serial.print(" \t");
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %   =>");
    thingsSpeak(temperature,humidity);

    MQTTtoCloud();                               // JSON data to MQTT Cloud 
  }
}

void MQTTConnect() {
  
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266_MQTT", "pitbull", "123456789")) {         // Attempt to connect
      Serial.println("cloudmqtt connected ");                                 // Once connected, publish an announcement..
      mqttClient.subscribe("/FARM_1/LED01/");                                 // อ่านหัวข้อ Topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      connectionWIFI();
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  String message="";
  Serial.print("MQTT Read =>");
  Serial.print(topic);
  Serial.print(" => ");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message );
    if (payload[0] == 48){
    status_led=0;
    digitalWrite(LED,LOW); 
    Serial.println("LED OFF");
    MQTTtoCloud();
  }else if(payload[0] == 49){
    status_led=1;
    digitalWrite(LED,HIGH); 
    Serial.println("LED ON");
    MQTTtoCloud();
  }
}



 void thingsSpeak(float temperature,float humitdity) {
    WiFiClient client_1;                                  // Use WiFiClient class to create TCP connections

    if (!client_1.connect(host, 80)) {
         Serial.print(host);
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
    client_1.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("Send data to thingSpeak.com success");
}

void MQTTtoCloud(){
    String dataJson = "{\"temp\":\""+(String)dht.readTemperature()+"\",\"Hum\":\""+(String)dht.readHumidity()+"\",\"led1\":"+digitalRead(LED)+"}";
    char charBuf[50];
    dataJson.toCharArray(charBuf,50);
    mqttClient.publish("/FARM_1/LED01/",charBuf );
    Serial.print("MQTT Send ! =>");
    Serial.print(dataJson);
    Serial.println(" ");
}

void connectionWIFI(){
    Serial.print("Wifi Connecting to SSD:");
    Serial.println(ssid);
    WiFi.begin(ssid, password);                                                   //เชื่อมต่อกับ AP
    Serial.print("MAC adress:");
    Serial.println(WiFi.macAddress());
    int count =0; 
    
    while (WiFi.status() != WL_CONNECTED)                                     //รอการเชื่อมต่อ
    {
          delay(500);
          Serial.print(".");
          ++count;
          if(count >5000){
            ESP.reset();
            return;
          }
    }
    Serial.println("");
    Serial.print("WiFi connected IP:");                                         //แสดงข้อความเชื่อมต่อสำเร็จ  
    Serial.println(WiFi.localIP());                                           // แสดงหมายเลข IP ของ Server    
    Serial.println("<<  <<< << Network READY >>>>> >>>  >>");
}

void loadConfigulation(){
  WiFiClient client;    
  IPAddress configServer(192,168,1,29);            // Update JSON log for control   
  const int httpPort = 80;
  if (!client.connect(configServer, httpPort)) {
    Serial.println("**** load config failed ****");
  }
  
  String url = "/eDuck/led.json";                                                // We now create a URI for the request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +                         // This will send the request to the server
               "Host: gigadoy\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.available()){                                                    // Read all the lines of the reply from server and print them to Serial
    String line = client.readStringUntil('\r');
    jsonData(line);
  }
  switchLED(status_led);
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


void switchLED(int onOff){
    
    digitalWrite(LED,onOff);          
    if (onOff ==0){
      Serial.println("LED OFF");
      status_led =0;
    }else if(onOff == 1){
      Serial.println("LED ON");
      status_led =1;
    }
}
