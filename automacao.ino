/*
 * MAPA DOS PINOS DIGITAIS
 *  D0   = 16;
    D1   = 5;
    D2   = 4;
    D3   = 0;
    D4   = 2;
    D5   = 14;
    D6   = 12;
    D7   = 13;
    D8   = 15;
    D9   = 3;
    D10  = 1;
 * 
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "netvirtua";
const char* password = "s1e2n3h4a5";

ESP8266WebServer server(80);

boolean lampadaVitor = false;

void setup(void){
  //pino D7
  pinMode(13, OUTPUT);  
  //pino D5
  pinMode(14, OUTPUT);  

  // config static IP
  IPAddress ip(192, 168, 1, 199); // where xx is the desired IP Address
  IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/lampada/vitor", [](){    
   if(lampadaVitor == false) {
    lampadaVitor = true;
    digitalWrite(13, HIGH);
    server.send(200, "text/plain", "ligado");
   }else{
    lampadaVitor = false;
    digitalWrite(13, LOW);
    server.send(200, "text/plain", "desligado");
   }  
  });

  server.on("/rele/iluminacao/vitor", [](){
    if(lampadaVitor == false) {
        server.send(200, "text/plain", "false");
    }else{
        server.send(200, "text/plain", "true");
    }
  });

  server.on("/rele/garagem/porta", [](){
      server.send(200, "text/plain", "garagem ativado");
      digitalWrite(14, HIGH);
      delay(1000);
      digitalWrite(14, LOW);
  });
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void loop(void){
  server.handleClient();
}


