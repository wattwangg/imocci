#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESPFlashString.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

static const uint8_t PIN_D0 = 16;            //RX
static const uint8_t PIN_D1 = 5;            //TX
static const uint8_t PIN_D2 = 4;
static const uint8_t PIN_D3 = 0;        //SCL
static const uint8_t PIN_D4 = 2;        //SDA
static const uint8_t PIN_D5 = 14;       //SCK
static const uint8_t PIN_D6 = 12;       //MISO
static const uint8_t PIN_D7 = 13;       //MOSI
static const uint8_t PIN_D8 = 15;
static const uint8_t PIN_A0_ESP = 17;
static const uint8_t PIN_LED = 2;

/* Set these to your desired credentials. */
const char *ap_ssid = "ESPap";
const char *ap_password = "123456";

#define DEFAULT_SSID                  "orangepi"
#define SSID_FILEPATH                 "/ssid"
#define DEFAULT_SSID_PSW              "12345678"
#define SSID_PSW_FILEPATH                 "/ssid_psw"

static const uint16_t OFFVAL = 890; // low power vote
static const uint16_t ONVAL = 960; // 26v, hi power vote
static  uint16_t curVal = 0;

static bool ctrl_mode = true; // 电压判断执行模式
static bool ota_mode = false;
#define DEFAULT_MODE              "1" // 1: 自动 0：手动 2：ota
#define MODE_FILEPATH             "/mode"

String ipStr = "";
bool dnsIsBegin = false;

String udpmark = "";
#define DEFAULT_MARK              "esp_switch"
#define MARK_FILEPATH                 "/espmark"

unsigned int localPort = 8888;
WiFiUDP Udp;

ESP8266WebServer server(80);
int IO_Pin[] = {PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7, PIN_D8};
const int MAX_IO_PIN = 9;
void handleConoect() {
  if(server.args() > 0) {
      String ssid = server.arg(0);
      String psw = server.arg(1);
      Serial.print("ssid: ");
      Serial.print(ssid);
      Serial.println("");
      Serial.print("psw: ");
      Serial.println(psw);
      ESPFlashString ssidf(SSID_FILEPATH);
      ssidf.set(ssid);
      ESPFlashString pswf(SSID_PSW_FILEPATH);
      pswf.set(psw);
      server.send(200, "text/html", "<h1>recive wifi arg</h1>");
      delay(1000);
      ESP.restart();
  }
}
void handleSetMark() {
  if(server.args() > 0) {
      String tempudpmark = server.arg(0);
      Serial.print("set udpmark: ");
      Serial.print(tempudpmark);
      Serial.println("");
      if (tempudpmark.length() > 0) {
        udpmark = tempudpmark;
        ESPFlashString markf(MARK_FILEPATH);
        markf.set(udpmark);
      } 
  }
  server.send(200, "text/html", "<h1>recive mark set</h1>");
}
void handleMode() {
  if(server.args() > 0) {
      String modestr = server.arg(0);
      Serial.print("mode: ");
      Serial.print(modestr);
      Serial.println("");

      ESPFlashString ssidf(MODE_FILEPATH);
      ssidf.set(modestr);
      if (modestr.toInt() == 2) {
        ESP.restart();
      } else {
        if(ota_mode)
          ESP.restart();
        ctrl_mode = modestr.toInt() == 1 ? true : false;
      }
  }
  server.send(200, "text/html", "<h1>recive mode set</h1>");
}
void handleGetState() {
  String res = "{";
  //开关状态
  res.concat(String("\"switch_1_pin\":14,"));
  res.concat(String("\"switch_1_val\":")+String(digitalRead(14))+String(","));
  res.concat(String("\"switch_2_pin\":12,"));
  res.concat(String("\"switch_2_val\":")+String(digitalRead(12))+String(","));
  res.concat(String("\"switch_3_pin\":13,"));
  res.concat(String("\"switch_3_val\":")+String(digitalRead(13))+String(","));
  res.concat(String("\"switch_4_pin\":4,"));
  res.concat(String("\"switch_4_val\":")+String(digitalRead(4))+String(","));
  //模式
  String modestr = ota_mode ? " 2" : ctrl_mode ? "1" : "0";
  res.concat(String("\"mode\":")+modestr+String(","));
  //udp mark
  res.concat(String("\"mark\":\"")+String(udpmark));
  res.concat("\"}");
  server.send(200, "application/json", res);
}
void handleRoot() {
  if(server.args() > 0) {
    String argstr = server.arg(0);
    int index = argstr.indexOf(',');
    if(index != -1) {
      String pin = argstr.substring(0,index);
      String val = argstr.substring(index+1);
      Serial.print(pin);
      Serial.print("=>");
      Serial.println(val);
      int state = val.toInt() == LOW ? LOW : HIGH;
      digitalWrite(pin.toInt(),state);
    }
  }
  String res = "<h1>You are connected</h1><p>PIN define: </p>";
  res.concat("<p>d2->gpio4 switch_4</p>");
  res.concat("<p>d7->gpio13 switch_3</p>");
  res.concat("<p>d6->gpio12 switch_2</p>");
  res.concat("<p>d5->gpio14 switch_1</p>");
  String modestr = ota_mode ? "2" : ctrl_mode ? "1" : "0";
  res.concat(String("<p>current mode: ")+modestr+String("</p>"));
  res.concat(String("<p>curVal: ")+String(curVal)+String("</p>"));
  res.concat(String("<p>udpMark: ")+String(udpmark)+String("</p>"));
  res.concat(String("<p>interface: </p>/ :set pin, eg: /?cmd=0,1</p>") +
             String("/wifi :set object wifi args, eg: /wifi?ssid=xxx&psw=xxx</p>") +
             String("/mode :set work mode, eg:/mode?mode=1,1->auto,2->handle</p>") +
             String("/setmark :set udp mark, eg:/setmark?mark=switch_0000"));
  if (dnsIsBegin) {
    res.concat("<p>domain is:esp8266.local</p>");
  }
  if (ipStr != "") {
    res.concat("<p>IP address: ");
    res.concat(ipStr);
    res.concat("</p>");
  }
  server.send(200, "text/html", res);
}

void setup() {
  //delay(20000);
  ESPFlashString ssid(SSID_FILEPATH, DEFAULT_SSID);
  ESPFlashString ssid_psw(SSID_PSW_FILEPATH, DEFAULT_SSID_PSW);
  ESPFlashString rmode(MODE_FILEPATH, DEFAULT_MODE);
  ESPFlashString switch_mark(MARK_FILEPATH, DEFAULT_MARK);
  for(int i =0; i < MAX_IO_PIN; i++) {
    pinMode(IO_Pin[i],OUTPUT);
    digitalWrite(IO_Pin[i],LOW);
  }
  
  if(rmode.get() == "2"){
    ota_mode = true;
  } else {
    ctrl_mode = rmode.get() == "1" ? true : false;
  }
  if(ota_mode == false) {
    udpmark = switch_mark.get();
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);
  
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    WiFi.begin(ssid.get(), ssid_psw.get());
    pinMode(A0, INPUT);
    pinMode(PIN_LED, OUTPUT);
  }
  else {
    Serial.println("Booting");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);
    
    WiFi.begin(ssid.get(), ssid_psw.get());
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESPFlashString ssidf(MODE_FILEPATH);
      ssidf.set("0");
      ESP.restart();
    }
    
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
      ESPFlashString ssidf(MODE_FILEPATH);
      ssidf.set("0");
      ESP.restart();
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
      ESPFlashString ssidf(MODE_FILEPATH);
      ssidf.set("0");
      ESP.restart();
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  server.on("/", handleRoot);
  server.on("/wifi", handleConoect);
  server.on("/mode", handleMode);
  server.on("/state",handleGetState);
  server.on("/setmark",handleSetMark);
  server.begin();
  Serial.println("HTTP server started");
  delay(2000);
}

void loop() {
  server.handleClient();
  if(ota_mode)
      ArduinoOTA.handle();
  else {
    delay(20);
    uint16_t temp = analogRead(A0);
    if (temp != 0 and temp != curVal) {
        curVal = temp;
        //Serial.println(curVal);
    }
    
    if (WiFi.status() == WL_CONNECTED){
      IPAddress ip = WiFi.localIP();
      String temp = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      if (ipStr != temp) {
        ipStr = temp;
        Serial.print("ip: ");
        Serial.println(ipStr);
      }
    }
    if (WiFi.status() == WL_CONNECTED && !dnsIsBegin) {
       if (!MDNS.begin("esp8266")) {
          Serial.println("Error setting up MDNS responder!");
          while (1) {
            delay(50);
          }
        }
        dnsIsBegin = true;
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
        Udp.begin(localPort);
    }
    if (dnsIsBegin) {
        MDNS.update();
    }
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                    packetSize,
                    Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                    Udp.destinationIP().toString().c_str(), Udp.localPort(),
                    ESP.getFreeHeap());
  
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      String broadcast_addr = String("{\"esp8266\":\"")+ipStr+String("\",\"mark\":\"")+udpmark+String("\"}");
      Udp.write(broadcast_addr.c_str());
      Udp.endPacket();
    }
    
    if (ctrl_mode) {
      if (curVal < OFFVAL) {
          digitalWrite(PIN_D5,LOW); // 关闭
      }
      if(curVal > ONVAL) {
         digitalWrite(PIN_D5,HIGH); // 打开
      }
    }
  }
}
