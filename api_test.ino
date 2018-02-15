#include <SPI.h>
#include <Ethernet.h>
#include "RTClib.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,1);
char server[] = "garduino20180215111317.azurewebsites.net";
EthernetClient client;

RTC_DS3231 rtc;

char host[] = "Host: garduino20180215111317.azurewebsites.net \r\n";
//expires every 2 days
char jwtToken[] = "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJhZG1pbkBhZG1pbi5vcmciLCJqdGkiOiJhYjJhMjk3Mi02YzM0LTQzMzgtODVjYi0zZDIzZjAwNDQ3YzMiLCJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1laWRlbnRpZmllciI6Ijk5NjQ5MzQzLTFmMDgtNDYyNS1iY2JmLTY4ZTVkZWNiYjZjYyIsImV4cCI6MTUxODgzNTYxOCwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3Q6NDQzOTUiLCJhdWQiOiJodHRwczovL2xvY2FsaG9zdDo0NDM5NSJ9.wTDZubyjx1MnNV98wIKPFN9Z0qjN8AXFLq7vlGogAec\r\n";
char contentType[] = "Content-Type: application/json\r\n";
/*
 * POST /api/account HTTP/1.1
Host: garduino20180215032159.azurewebsites.net
Content-Type: application/json
Cache-Control: no-cache
Postman-Token: 4a14b695-9487-266f-5ad4-2ae34ea48414

{
 "Email" : "admin@admin.org",
  "Password" : "Fran_97Sokol",
  "RememberMe" : "false"
}
*/

void getter(char route[]){
  if (client.connect(server, 80)) {
      Serial.print("Make a HTTP request ... ");

      client.write(route);
      client.write(host);
      client.write(jwtToken);
      client.write("Cache-Control: no-cache\r\n\r\n\n");
      client.write("\r\n\r\n");
      Serial.println("ok");

      Serial.write(route);
      Serial.write(host);
      Serial.write(jwtToken);
      Serial.write("Cache-Control: no-cache\r\n\r\n\n");
      Serial.write("\r\n\r\n");

    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
  }
}
//USE FOR POST & PUT
void poster(char route[], char token[], char json[]){
  if (client.connect(server, 80)) {
      Serial.print("Make a HTTP request ... ");      

      client.write(route);
      client.write(host);
      client.write(token);
      client.write(contentType);
      client.write("Cache-Control: no-cache\r\n");
      client.write("Content-Length: ");
      client.print(strlen(json));
      client.write("\r\n\r\n");
      client.write(json);
      Serial.println("ok");

      Serial.write(route);
      Serial.write(host);
      Serial.write(token);
      Serial.write(contentType);
      Serial.write("Cache-Control: no-cache\r\n");
      Serial.write("Content-Length: ");
      Serial.print(strlen(json));
      Serial.write("\r\n\r\n");
      Serial.write(json);
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
  }
}

void getMyId(){
  char call[] = "GET /api/Device/call/Garduino HTTP/1.1\r\n";
  getter(call);
}

void getCode(){
  //temporarily coded-in deviceId until I program a http parser.
  char call[] = "GET /api/Code/latest/c815dc1d-8dda-437f-7d84-08d5745de76a HTTP/1.1\r\n";
  getter(call);
}
void login(){
  char json[] = "{\"Email\":\"admin@admin.org\",\"Password\":\"Fran_97Sokol\",\"RememberMe\":\"false\"}\r\n\r\n";
  char login[] = "POST /api/account HTTP/1.1\r\n";
  poster(login, "\r\n", json);
}

void completeCode(){
  char call[] = "PUT /api/Code/latest HTTP/1.1\r\n";
  char json[] = "{\"dateTime\":\"2018-02-15 13:36:00\",\"deviceId\":\"c815dc1d-8dda-437f-7d84-08d5745de76a\"}\r\n\r\n";
  poster(call, jwtToken, json);
}

//TODO: fix
void postEntry(){
  char call[] = "POST /api/entry HTTP/1.1\r\n";
  char json[] = "\"measure\":{\"DateTime\":\"1/10/2018 14:00:00\",\"SoilMoisture\":320,\"SoilDescription\":\"Wet\",\"AirHumidity\":65.0,\"AirTemperature\":22.5,\"LightState\":true},\"deviceId\":\"69093aed-dc43-4e5b-b769-08d5741ccd16\"}\r\n\r\n";
  poster(call, jwtToken, json);
}
void setup() {
  Serial.begin(1200);  
  
  rtc.begin();

  Serial.print("Setup LAN ... ");
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Ethernet.begin(mac);
  Serial.println("ok");
  delay(1000);


  postEntry();
    
}

void loop(){

if(client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    delay(5000);

    getMyId();
  }

}

