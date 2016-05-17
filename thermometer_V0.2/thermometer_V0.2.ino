

/*
 * 2015/10/29
 * Create by xanio
 * 
 *  module ESP8266
 *  Sensor DS18B20
 *  LED
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>


#define LED D2                                           //กำหนดขาที่ต่อ LED เป็นขา D1
#define ONE_WIRE_BUS D4                                  //ต่อเข้ากับ sensor วัดอุณหภูมิ DS18B20

const char *ssid =  "no_angel";                          // cannot be longer than 32 characters!
const char *pass =  "o8o9237579";   
unsigned char status_led=0;                              // set up LED


IPAddress mqttServer(107, 22, 157, 224);                 // Update these with values suitable for your network.
IPAddress logTempuratureServer(192,168,1,26);
//const char* host = "192.168.1.103";

WiFiServer server(80);                                   //กำหนดใช้งาน TCP Server ที่ Port 80
WiFiClient wclient;
PubSubClient mqttClient(wclient);

OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&ourWire);

String  thermo_1;
char*   thingSpeak        = "https://api.thingspeak.com/update?key=OJDJI20NWYIRFWWB";
char*   controllerHost    = "http://gigadoy";
float   tempurature[]     ={25,60};                              // Defalut temporature range;
bool    switchStatus_01   = false;

void setup(){
  Serial.begin(115200);                                         // Setup console
  pinMode(LED, OUTPUT);                                         //กำหนด Pin ที่ต่อกับ LED เป็น Output
  delay(10);
  Serial.println("Temperature Sensor DS18B20");

  tempSensors.begin();                                          //-( Start up the DallasTemperature library )
  connectionWIFI();
}

/*******************************************************
 *              Loop
 *******************************************************/
 
void loop(){
  temporatureControl();
  Serial.print("  IP : ");
  Serial.print(WiFi.localIP());
  
  tempSensors.requestTemperatures(); 
  Serial.print("  Tempurature C: ");
  Serial.println(tempSensors.getTempCByIndex(0));
  

 thermo_1 = String(tempSensors.getTempCByIndex(0));
 conditionOnOff(tempSensors.getTempCByIndex(0));
 
 char buff[6];
 thermo_1.toCharArray(buff, 6);
 thingsSpeak(thermo_1);
}


/*******************************************************
 *             End Loop
 *******************************************************/

 
void reconnect() {
  
  while (!mqttClient.connected()) {                                           // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266_MQTT", "ktbwgckp", "xc_om3Hl4iYn")) {     // Attempt to connect
      Serial.println("connected");                                            // Once connected, publish an announcement..
      
      mqttClient.subscribe("/LED_ESP/dog/");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
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

void connectionWIFI(){
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);                                                   //เชื่อมต่อกับ AP
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

void thingsSpeak(String t) {
    static const char* host = "api.thingspeak.com";
    static const char* apiKey = "OJDJI20NWYIRFWWB";

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
         Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/update/";
    //  url += streamId;
    url += "?key=";
    url += apiKey;
    url += "&field1=";
    url += t;

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
}

void sendDataLED(String t) {
    WiFiClient client;                                                    // Use WiFiClient class to create TCP connections
    const int httpPort = 80;
     
    if (!client.connect(controllerHost, httpPort)) {
         Serial.println("connection failed");
        return;
    }
    String url = "/tempControl.php?led1="+status_led;                                              // We now create a URI for the request

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + controllerHost + "\r\n" +
                 "Connection: close\r\n\r\n");
}
/*
 * รับข้อมูลจาก URL เพื่อแปลข้อมูล             
*/
void temporatureControl(){
  WiFiClient client;                                                            //Serial.println("connecting to tempurature server");
  const int httpPort = 80;
  if (!client.connect(logTempuratureServer, httpPort)) {
    Serial.println("connection tempurature log sever failed");
    return;
  }
  
  String url = "/eDuck/led.txt";                                                // We now create a URI for the request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +                         // This will send the request to the server
               "Host: gigadoy\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.available()){                                                    // Read all the lines of the reply from server and print them to Serial
    String line = client.readStringUntil('\r');
    jsonData(line);
  }
}

void findText(String text){
    char* words;
    text.toCharArray(words,text.length());
    
    if (text.startsWith("temperature1",1)){
      int firstClosingBracket = text.indexOf('[');
      int lastClosingBracket = text.indexOf(']');
      int middleClosingBracket = text.indexOf(',');
      
      tempurature[0] = text.substring(firstClosingBracket+1,middleClosingBracket).toFloat();
      tempurature[1] = text.substring(middleClosingBracket+1,lastClosingBracket).toFloat();
      //Serial.print("tempurature[0]");
      //Serial.println(tempurature[0]);
      //Serial.print("tempurature[1]");
      //Serial.println(tempurature[1]);
    }
}

void jsonData(String text){
if (!text.startsWith("{\"t1\"",1)){
  return ;
}  
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(text);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  String tt1 = root["t1"];
  String tt2 = root["t2"];
  String tt3 = root["led1"];
  
  if (tt3 == "1"){  switchStatus_01 = true;
  }else{switchStatus_01 = false;}
  
  tempurature[0] = tt1.toFloat();
  tempurature[1] = tt2.toFloat();

}

void conditionOnOff(float presentTempurature){

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
/*
  if((tempurature[0] <= presentTempurature) && (presentTempurature <= tempurature[1])){
    digitalWrite(LED,HIGH); 
    Serial.println("LED ON");
  }else{
    digitalWrite(LED,LOW);
    Serial.println("LED OFF");
  }
  */
}

