#include <ESP8266WiFi.h>
#include<EasyDDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"
#define BUTTON_BUILTIN 0
const int led = 2;

std::unique_ptr<ESP8266WebServer> server;

void handleRoot() {
 // server->send(200, "text/plain", "hello from esp8266!");
  server->send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
}
void handleLED();
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";
  for (uint8_t i = 0; i < server->args(); i++) {
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }
  server->send(404, "text/plain", message);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUTTON_BUILTIN, INPUT);
 EasyDDNS.service("noip");
 EasyDDNS.client("xxx","xxx","xxx");
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  //Serial.setDebugOutput(true);
  //WiFiManager
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
// Nombre y pw de wifi creado
  wifiManager.autoConnect("Light Control", "12345678");

//si se conecta a red wifi
  Serial.println("Conectado!)");
  
  server.reset(new ESP8266WebServer(WiFi.localIP(), 80));

  server->on("/", handleRoot);
  server->on("/LED", HTTP_POST, handleLED);

  server->on("/inline", []() {
    server->send(200, "text/plain", "Trabajando..");
  });

  server->onNotFound(handleNotFound);

  server->begin();
  Serial.println("Server HTTP iniciado");
  Serial.println(WiFi.localIP());
}

void handleButton(){
  int debounce = 50;
  if ( digitalRead(BUTTON_BUILTIN) == LOW ){
    delay(debounce);
    if(digitalRead(BUTTON_BUILTIN) == LOW ){
      WiFiManager wifiManager;      
      wifiManager.resetSettings();
      delay(1000);
      ESP.reset();
    }
  }
}


void loop() {
 
  server->handleClient();
  handleButton();
  EasyDDNS.update(10000);
  
  

}

void handleLED() {                          // If a POST request is made to URI /LED
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  server->sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server->send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
