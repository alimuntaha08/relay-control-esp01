//Follow me at https://web.facebook.com/kae.digitalsolutions
//Author:Ali Muntaha

//Hardware dapat dibeli disini:

//https://shopee.co.id/product/169084802/24064620899  
//Saklar listrik dengan aplikasi smartphone android 220V 10A satu chanel

//https://shopee.co.id/product/169084802/25814164085 
//Paket Esp01+relay+power supply 220v+box+kabel+connector komplit tinggal merakit DIY cocok untuk belajar merakit sendiri

//https://shopee.co.id/product/169084802/25264108902 
//ESP8266 wifi dengan Relay esp01+relay+aplikasi relay control dan source code esp Esp01

//********************************************************************//


#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHT11PIN 2  // 2=>GPIO0 ESP01
#define DHTTYPE DHT11 
DHT dht(DHT11PIN, DHTTYPE);
long lastMsg = 0; 

//change with your parameter 
String model="SWITCH_TEMP";
String mcode ="0001";
String scode =model+mcode;
String ver="1.0.1";
String ver_relay="4.0";  //Change with 1.0 or 4.0 , 1.0=active high, 4.0=active low (with opto) 

const char* ssid = "YOUR_SSID";//type your ssid
const char* password = "WIFI_PASSWORD";//type your password
 
int relayPIN = 0; // GPIO.0 of ESP01S

WiFiServer server(80);
 
void setup() {
      Serial.begin(115200);
      delay(10); 
     
      pinMode(relayPIN, OUTPUT);
      digitalWrite(relayPIN, HIGH);
    
      dht.begin();   //DHT Begin 
    
      // Connect to WiFi network
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
    
      WiFi.mode(WIFI_STA);
      WiFi.hostname(scode);
      delay(10);
    
      WiFi.begin(ssid, password);
       
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");
       
      // Start the server
      server.begin();
      Serial.println("Server started");
     
      // Print the IP address
      Serial.print("Use this URL to connect: ");
      Serial.print("http://");
      Serial.print(WiFi.localIP());
      Serial.println("/");    
}
 
void loop() { 

      long now = millis(); 
       
        if (now - lastMsg > 3000) { //3 sec
          lastMsg = now;
          float temp_dht = dht.readTemperature(); // Read temperature as Celsius
          float humid = dht.readHumidity();   
          if (!isnan(temp_dht) || !isnan(humid)) {
            Serial.print("Temp:");
            Serial.print(temp_dht);
            Serial.print(" Humid:");
            Serial.println(humid);
          }else{
            Serial.println("Failed to read from DHT sensor");
            return;
          }       
        }              
            
        // Check if a client has connected
        WiFiClient client = server.available();
        if (!client) {
          return;
        }
         
        // Wait until the client sends some data
        Serial.println("New client");
        while(!client.available()){
          delay(1);
        }
         
        // Read the first line of the request
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html"); 
        client.println(""); 
        
        // Match the request            
        if (request.indexOf("/get?cmd=temp") != -1) {   
          float temp_dht = dht.readTemperature(); // Read temperature as Celsius
          float humid = dht.readHumidity();
          String ptr;;    
            ptr +=(float)temp_dht;
            ptr +="#";
            ptr +=(float)humid;       
            client.print(ptr);
        }    

        if (request.indexOf("/relay?cmd=off") != -1) {
            if(ver_relay=="4.0"){
                digitalWrite(relayPIN, HIGH);
                client.print("OFF");
            }else{
                digitalWrite(relayPIN, LOW);
                client.print("OFF");
            } 
        } 
        
        if (request.indexOf("/relay?cmd=on") != -1){ 
           if(ver_relay=="4.0"){
                digitalWrite(relayPIN, LOW);
                client.print("ON");
            }else{
                digitalWrite(relayPIN, HIGH);
                client.print("ON");
            } 
        }
       
        delay(1);
        Serial.println("Client disonnected");
        Serial.println("");
 
}