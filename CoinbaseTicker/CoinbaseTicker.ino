#include "ESP8266WiFi.h"
#include "WiFiClientSecure.h"
#include "ArduinoJson.h"
#include <MD_MAX72xx.h>
#include <stdio.h>
#include <SPI.h>

/*
 http://api.pro.coinbase.com/products/BTC-USD/ticker
 Expected http response
 
 {
 "trade_id":52342926,
 "price":"6201.54000000",
 "size":"0.00216765",
 "bid":"6201.53",
 "ask":"6201.54",
 "volume":"5318.00903573",
 "time":"2018-10-13T00:32:56.268000Z"
 }
 
 */

// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTD(x) Serial.println(x, DEC)

#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)

#endif


// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

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
const int httpsPort = 443;


void scrollText(char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

  PRINTS("\nScrolling text");
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

    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, httpsPort)) {
        scrollText("Connection failed!");
        Serial.println("connection failed");
    }

    
    Serial.print("requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");
    
    Serial.println("request sent");
    
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
    }
    String line = client.readStringUntil('\n');
  // Allocate JsonBuffer
   const size_t bufferSize = JSON_OBJECT_SIZE(7) + 150;
   DynamicJsonBuffer jsonBuffer(bufferSize);
    
    // Parse JSON object
    JsonObject& object = jsonBuffer.parseObject(line);
    if (!object.success()) {
        Serial.println(F("Parsing failed!"));
        scrollText("JSON Parse Failed!");
    }
    return object;
}

String cryptoURL [] {
  "/products/BTC-USD/ticker",
  "/products/ETH-USD/ticker",
  "/products/LTC-USD/ticker", 
  "/products/BCH-USD/ticker", 
  "/products/ETC-USD/ticker",
};

void getCryptoPrices() {

      for (int index = 0; index < 5; index++) {
        char *currentCrypto;
        switch (index) {
            case 0:
                currentCrypto = "BTC $";
                break;
            case 1:
                currentCrypto = "ETH $";
                break;
            case 2:
                currentCrypto = "LTC $";
                break;
            case 3:
                currentCrypto = "BCH $";
                break;
            case 4:
                currentCrypto = "ETC $";
                break;
            default:
                break;
        }
        JsonObject& root = getObject(cryptoURL[index]);
        Serial.println("reply was:");
        Serial.println("==========");
        Serial.println(F("Response:"));
        Serial.print("Trade Id: ");
        Serial.println(root["trade_id"].as<char*>());
        Serial.print("Price: ");
        Serial.println(root["price"].as<char*>());
        Serial.println("==========");
        Serial.println("closing connection");
        
        float cryptoPrice = root["price"].as<float>();
        Serial.println(cryptoPrice);
        char cryptoArray[64];
        
        snprintf(cryptoArray, sizeof cryptoArray, "%.2f", cryptoPrice);
        char *str2 = cryptoArray;
        strcat (currentCrypto, str2);
        Serial.println(currentCrypto);
        scrollText(currentCrypto);
        
    }
}


void setup() {
  mx.begin();
#if  DEBUG
  Serial.begin(115200);
  connectToWIFI();
#endif
  PRINTS("\n[MD_MAX72XX Test & Demo]");
}

void loop() {
    getCryptoPrices();
    
//    scrollText("1 Bitcoin equals $5,490.01 United States Dollar");
//    scrollText("1 Litecoin equals $40.44 United States Dollar");
}
