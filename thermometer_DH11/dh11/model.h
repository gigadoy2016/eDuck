/*-----------------------------------------------------------
                   MQTT Section.
-------------------------------------------------------------*/

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print(topic);
  Serial.print(" => ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/* 
 * Connection Wifi
 */

void connectionWIFI(char* ssid,char* password ){
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
