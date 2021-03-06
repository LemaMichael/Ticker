
// http://api.pro.coinbase.com/products/BTC-USD/ticker
// Here is the expected response: 
// 
// {
// "trade_id":52342926,
// "price":"6201.54000000",
// "size":"0.00216765",
// "bid":"6201.53",
// "ask":"6201.54",
// "volume":"5318.00903573",
// "time":"2018-10-13T00:32:56.268000Z"
// }
 

#include "ESP8266WiFi.h"
#include "WiFiClientSecure.h"
#include "ArduinoJson.h"
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <stdio.h>



// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8

#define CLK_PIN   D5 // or SCK
#define DATA_PIN  D7 // or MOSI
#define CS_PIN    D8 // or SS

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds


const char* ssid = "";
const char* password = "";

const char* host = "api.pro.coinbase.com";
const char* fingerprint = "9C B0 72 05 A4 F9 D7 4E 5A A4 06 5E DD 1F 1C 27 5D C2 F1 48";


// Allocate JsonBuffer
const size_t capacity = JSON_OBJECT_SIZE(7) + 252;
DynamicJsonBuffer jsonBuffer(capacity);

void scrollText(char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts
  mx.clear();

  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth)
        mx.setColumn(0, cBuf[i]);
      delay(DELAYTIME);
    }
  }
}

void connectToWIFI() {
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());    
}

JsonObject& getObject(String url) {

    // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure client;
    client.setTimeout(10000);
    Serial.print("connecting to ");
    Serial.println(host);
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    client.setFingerprint(fingerprint);
    if (!client.connect(host, 443)) {
        Serial.println("connection failed");
        scrollText("Connection failed!");
        // No further work should be done if the connection failed
        return jsonBuffer.parseObject(client);
    }
    Serial.println(F("Connected!"));

    // Send HTTP Request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
    Serial.println("request sent");
    
    // Check HTTP Status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      //client.stop();
      return jsonBuffer.parseObject(client);
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      //scrollText("Invalid Response");
    }
    
    // Parse JSON object
    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        //scrollText("JSON Parse Failed!");
    }
    
    // Disconnect
    client.stop();
    jsonBuffer.clear();
    return root;
}

String cryptoURL[] {
  "/products/BTC-USD/ticker",
  "/products/ETH-USD/ticker",
  "/products/LTC-USD/ticker", 
  "/products/BCH-USD/ticker", 
  "/products/ETC-USD/ticker"
};

String cryptoName[] {
  "BTC $",
  "ETH $",
  "LTC $",
  "BCH $",
  "ETC $"
};

void getCryptoPrices() {
   for (int index = 0; index < 5; index++) {
        JsonObject& root = getObject(cryptoURL[index]);
        Serial.println("==========");
        Serial.println(F("Response:"));
        Serial.print("Trade Id: ");
        Serial.println(root["trade_id"].as<char*>());
        Serial.print("Price: ");
        Serial.println(root["price"].as<char*>());
        float cryptoPrice = root["price"].as<float>();
        Serial.println(cryptoPrice);
        Serial.println("==========");
        String output = cryptoName[index] + String(cryptoPrice);
        Serial.println(output);
        
        char *cstr = new char[output.length() + 1];
        strcpy(cstr, output.c_str());
        scrollText(cstr);
        delete [] cstr;
    }
}


void setup() {
  mx.begin();
  Serial.begin(115200);
  connectToWIFI();
}

void loop() {
    getCryptoPrices();
    
//    scrollText("1 Bitcoin equals $5,490.01 United States Dollar");
//    scrollText("1 Litecoin equals $40.44 United States Dollar");
}
