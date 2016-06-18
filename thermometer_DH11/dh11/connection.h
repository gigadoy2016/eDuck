void connectionWIFI(char* WLAN_SSID,char* WLAN_PASS){
    Serial.print("Wifi Connecting to SSD:");
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);                                                   //เชื่อมต่อกับ AP
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

