#include <SPI.h>
#include <Ethernet.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <avr/pgmspace.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,1);
const char *server = "garduino20180215111317.azurewebsites.net";
//const char server[] PROGMEM = {"garduino20180215111317.azurewebsites.net"};
//const char token[] PROGMEM = {"Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJhZG1pbkBhZG1pbi5vcmciLCJqdGkiOiI1Mzk1ZDZlMS02MmMwLTRmNjMtYmMzOS1jOTEyZWFkMDhhYmIiLCJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1laWRlbnRpZmllciI6ImI4M2VmZDU0LTVlN2ItNDIwMy04NTNjLWRjODk2MTczNTkxZSIsImV4cCI6MTUxOTAzNjY3NiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3Q6NDQzOTUiLCJhdWQiOiJodHRwczovL2xvY2FsaG9zdDo0NDM5NSJ9.6MeaXtkiLi-qNvSYHSO9JMvbvo7FwpB8_9_pf9D4fFI\r\n"};
const char host[] PROGMEM = {"Host: garduino20180215111317.azurewebsites.net\r\n"};
const char contType[] PROGMEM = {"Content-Type: application/json\r\n"};
const char cacheCtrl[] PROGMEM = {"Cache-Control: no-cache\r\n"};
const char newLine[] PROGMEM = {"\r\n"};
const char contLen[] PROGMEM = {"Content-Length: "};
const char connection[] PROGMEM = {"Connection: close\r\n"};
const char deviceId[] PROGMEM = {"c815dc1d-8dda-437f-7d84-08d5745de76a"};

const char actnFrnt[] PROGMEM = {"\"action\":"}; //action front phrase, "action":
const char actnBack[] PROGMEM = {",\""}; //action back phrase
const char minPhrs[] PROGMEM = {"\""}; //miminal phrase, only " char.
EthernetClient client;

RTC_DS3231 rtc;

/* LEGACY CODE USED TO TRANSPORT TOKEN FROM ROM TO EEPROM.
void saveToken(){
  char buff;
  char test;
  int i = 0;
  do{
    buff = pgm_read_byte_near(token + i);
    EEPROM.put(i, buff);
    EEPROM.get(i, test);
    Serial.print(test);
    i++;
  }while(buff != '\0');
  Serial.println(F("success."));
}*/

void printEeprom(int index){
  char buff;
  EEPROM.get(index, buff);
  while(buff != '\0'){
    index++;
    Serial.print(buff);
    EEPROM.get(index, buff);
  }
}

void writeFragment(char *fragment){ // fragment NEEDS string escape char.
  char buff = pgm_read_byte_near(fragment);
  int i = 0;
  while(buff != '\0'){
    i++;
    client.print(buff);
    Serial.print(buff);
    buff = pgm_read_byte_near(fragment + i);
  }
}

int fragmentLen(char *fragment){
  char buff = pgm_read_byte_near(fragment);
  int i = 0;
  while(buff != '\0'){
    i++;
    buff = pgm_read_byte_near(fragment + i);
  }
  return i;
}

bool connectToServ(){
  int f = client.connect(server, 80);
  Serial.println(f);
  return f;
}

void writeConnection(){
  writeFragment(connection);
}

void writeJwtToken(){
  char buff;
  int i = 0;
  EEPROM.get(i, buff);
  while(buff != '\0'){
    i++;
    client.print(buff);
    Serial.print(buff);
    EEPROM.get(i, buff);
  }
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
      writeConnection();
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
  client.print(strlen(json));
  Serial.print(strlen(json));
  writeNewLine();
  writeNewLine();
  client.print(json); //for now, later we'll be using buffer write.
  Serial.print(json);
}

//USE FOR POST & PUT
void poster(char *route, bool writeToken, char *json){
  if (connectToServ()) {
      printMakeHttp();

      writeRoute(route);
      writeHost();
      if(writeToken) writeJwtToken();
      writeConnection();
      writeContentType();
      writeCacheType();
      writeContent(json);
      writeNewLine();
      printOk();
    } 
    else {
      // kf you didn't get a connection to the server:
      printConnFailed();
  }
}

void getMyId(){ //working
  char call[] = "GET /api/Device/call/Garduino HTTP/1.1\r\n";
  getter(call);
  parseResponse(minPhrs, minPhrs, (int*) NULL, true, 460);
  printEeprom(460);
}

void getCode(){ //working
  //temporarily coded-in deviceId until I program a http parser.
  char call[] = "GET /api/Code/latest/c815dc1d-8dda-437f-7d84-08d5745de76a HTTP/1.1\r\n";
  int code = 500; //if 500 then some error happened. parser has to change this value.
  getter(call);
  /*
   * Http response parser. If it screws something up it shouldn't screw up EEPROM values.
   * If it does, it'll screw up deviceId storage (460-496)
   * As long as code address exists parser won't write to EEPROM.
   */
  parseResponse(actnFrnt, actnBack, &code, false, 460);
  Serial.println(code);
}
void login(){ //working
  char json[] = "{\"Email\":\"admin@admin.org\",\"Password\":\"Fran_97Sokol\",\"RememberMe\":\"false\"}\r\n\r\n";
  char login[] = "POST /api/account HTTP/1.1\r\n";
  poster(login, false, json);
  parseResponse(minPhrs, minPhrs, (int*) NULL, true, 0); //store token between " and " to EEPROM address 0+.
}

void completeCode(){ //working
  char call[] = "PUT /api/Code/latest HTTP/1.1\r\n";
  char json[] = "{\"dateTime\":\"2018-02-15 13:36:00\",\"deviceId\":\"c815dc1d-8dda-437f-7d84-08d5745de76a\"}\r\n\r\n";
  poster(call, true, json);
  printResponse();
}

//TODO: fix

void postEntry(){ //working
  char call[] = "POST /api/entry HTTP/1.1\r\n";
  char json[] = "{\"measure\":{\"DateTime\":\"2/17/2018 14:00:00\",\"SoilMoisture\":320,\"SoilDescription\":\"Wet\",\"AirHumidity\":65.0,\"AirTemperature\":22.5,\"LightState\":true},\"deviceId\":\"c815dc1d-8dda-437f-7d84-08d5745de76a\"}\r\n\r\n";
  poster(call, true, json);
  printResponse();
}

void printResponse(){
  char c;
  while(client.connected()){
    while(client.available()){
      c = client.read();
      Serial.print(c);
    }
  }
  Serial.println(F("disconnecting."));
  client.stop();
}

void parseResponse(char *before, char *after, int *action, bool eepromWrite, int eepromIndex){
  char actStrg[30];
  bool done = false; //is search currently on
  bool found = false; //did we find the correct search phrase?
  byte index = 0; //index of current search within phrase
  byte searchSize = fragmentLen(before); // size of our phrase
  char c;
  int writeIndex = 0; //write head position.
  while(client.connected()){
    while(client.available()){
      c = client.read();
      if(done){
        //keep empty.
      }
      else if(c == pgm_read_byte_near(before + index) && !found){
        index++;
        if(index >= searchSize){
          found = true;
          index = 0; // reuse the index for after.
          searchSize = fragmentLen(after);
        }
      }
      else if(c != pgm_read_byte_near(before + index) && !found){
        index = 0; //reset the index because it's not what we were looking for.
      }
      else if(found){ // DON'T DELETE ELSE BECAUSE FOUND HAS TO START AT THE NEXT CYCLE.
        if(eepromWrite) {
          EEPROM.put(eepromIndex + writeIndex, c);
          }
        else{
          actStrg[writeIndex] = c;
        }
        writeIndex++;
        if(c != pgm_read_byte_near(after + index)){
          index = 0;
          
        }
        else if(c == pgm_read_byte_near(after + index)){
          index++;
          if(index >= searchSize){
            done = 1;
            if(eepromWrite) {EEPROM.put(eepromIndex + writeIndex - searchSize, '\0');
            }
            else actStrg[writeIndex - searchSize] = '\0';
          }
        }
      }
      Serial.print(c);
    }
  }

  Serial.println(F("disconnecting."));
  client.stop();

  if(eepromWrite) *action = atoi(actStrg);
  
  /*
  EEPROM.get(eepromIndex, c);
  index = 0;
  while(c != '\0'){
    index++;
    Serial.print(c);
    EEPROM.get(eepromIndex + index, c);
  }
  */
}

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
  
  postEntry();
}
void loop(){
  return;
  char c;
  while(client.connected()){
    while(client.available()){
      c = client.read();
      Serial.print(c);
    }
  }
  Serial.println(F("disconnecting."));
  client.stop();
  delay(50000);
  getMyId();
  /*
if(b = client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();

    delay(5000);

    getMyId();
  }
*/
}


