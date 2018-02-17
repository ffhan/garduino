#include <SPI.h>
#include <Ethernet.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <avr/pgmspace.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,1);
const char *server = "garduino20180215111317.azurewebsites.net";
//const char server[] PROGMEM = {"garduino20180215111317.azurewebsites.net"};
const char token[] PROGMEM = {"Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJhZG1pbkBhZG1pbi5vcmciLCJqdGkiOiJhYjJhMjk3Mi02YzM0LTQzMzgtODVjYi0zZDIzZjAwNDQ3YzMiLCJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1laWRlbnRpZmllciI6Ijk5NjQ5MzQzLTFmMDgtNDYyNS1iY2JmLTY4ZTVkZWNiYjZjYyIsImV4cCI6MTUxODgzNTYxOCwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3Q6NDQzOTUiLCJhdWQiOiJodHRwczovL2xvY2FsaG9zdDo0NDM5NSJ9.wTDZubyjx1MnNV98wIKPFN9Z0qjN8AXFLq7vlGogAec\r\n"};
const char host[] PROGMEM = {"Host: garduino20180215111317.azurewebsites.net\r\n"};
const char contType[] PROGMEM = {"Content-Type: application/json\r\n"};
const char cacheCtrl[] PROGMEM = {"Cache-Control: no-cache\r\n"};
const char newLine[] PROGMEM = {"\r\n"};
const char contLen[] PROGMEM = {"Content-Length: "};
EthernetClient client;

RTC_DS3231 rtc;
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

void writeFragment(char *fragment){
  char buff;
  int i = 0;
  do{
    buff = pgm_read_byte_near(fragment + i);
    client.print(buff);
    Serial.print(buff);
    i++;
  } while(buff != '\0');
}

bool connectToServ(){
  int f = client.connect(server, 80);
  Serial.println(f);
  return f;
}

void writeJwtToken(){
  writeFragment(token);
}

void writeRoute(char *route){
  client.print(route);
  Serial.print(route);
}
void writeHost(){
  writeFragment(host);
}
void writeContentType(){
  writeFragment(contType);
}
void writeCacheType(){
  writeFragment(cacheCtrl);
}

void writeNewLine(){
  writeFragment(newLine);
}

void printMakeHttp(){
  Serial.println(F("Make a HTTP request ... "));
}
void printOk(){
  Serial.println(F("ok"));
}
void printConnFailed(){
  Serial.println(F("connection failed"));
}

void getter(char *route){
  if (connectToServ()) {
      printMakeHttp();

      writeRoute(route);
      writeHost();
      writeJwtToken();
      writeCacheType();
      writeNewLine();
      printOk();
    } 
    else {
      // kf you didn't get a connection to the server:
      printConnFailed();
  }
}


void writeContent(char *json){
  writeFragment(contLen);
  client.print(strlen(*json));
  writeNewLine();
  client.print(*json); //for now, later we'll be using buffer write.
}

//USE FOR POST & PUT
void poster(char *route, bool writeToken, char *json){
  if (connectToServ()) {
      printMakeHttp();

      writeRoute(route);
      writeHost();
      if(writeToken) writeJwtToken();
      writeContentType();
      writeCacheType();
      writeContent(json);
      printOk();
    } 
    else {
      // kf you didn't get a connection to the server:
      printConnFailed();
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
  poster(login, false, json);
}

void completeCode(){
  char call[] = "PUT /api/Code/latest HTTP/1.1\r\n";
  char json[] = "{\"dateTime\":\"2018-02-15 13:36:00\",\"deviceId\":\"c815dc1d-8dda-437f-7d84-08d5745de76a\"}\r\n\r\n";
  poster(call, true, json);
}

//TODO: fix
/*
void postEntry(){
  char call[] = "POST /api/entry HTTP/1.1\r\n";
  char json[] = "\"measure\":{\"DateTime\":\"1/10/2018 14:00:00\",\"SoilMoisture\":320,\"SoilDescription\":\"Wet\",\"AirHumidity\":65.0,\"AirTemperature\":22.5,\"LightState\":true},\"deviceId\":\"69093aed-dc43-4e5b-b769-08d5741ccd16\"}\r\n\r\n";
  poster(call, true, json);
}
*/
void setup() {
  Serial.begin(9600);  

  //expires every 2 days, size is 454. Later hold only string before token and then strcpy everything into this.
  //store into EEPROM
  //EEPROM.put(0,);
  
  rtc.begin();

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  Serial.print(F("Setup LAN ... "));
  // give the Ethernet shield a second to initialize:
  if(Ethernet.begin(mac) == 0){
    Serial.print(F("didn't begin. trying static ip.. "));
    Ethernet.begin(mac,ip);
  }
  printOk();
  delay(6000);


  getMyId();
    
}

void loop(){

if(client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();

    delay(50000);

    getMyId();
  }

}


