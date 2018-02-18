/*
 * {\"measure\":{\"DateTime\":\"
 * 2/17/2018 14:00:00
 * \",\"SoilMoisture\":
 * 320
 * ,\"SoilDescription\":\"
 * Wet
 * \",\"AirHumidity\":
 * 65.0
 * ,\"AirTemperature\":
 * 22.5,
 * \"LightState\":
 * true
 * },\"deviceId\":\"
 * c815dc1d-8dda-437f-7d84-08d5745de76a
 * \"}\r\n
 */



int buildEntryJson(Fragmentor fp, Fragmentor tp, EProcessor ep, FloatProcessor flp, DateTime now, int soilMoisture, char *soilDescription, float airHumidity, float airTemperature, bool lightState){
  int len = 0;
  char ts[5];

  len += fp(entry1);
  //Serial.println(len);
  int t = now.year();
  for(int i = 10; i <= 1000; i = i * 10){
    if(t < i) len += fp(zero);
  }
  //Serial.println(len);
  
  sprintf(ts, "%d", t);
  len += tp(ts);
  len += fp(dash);
  //Serial.println(len);
  
  for(int z = 0; z < 5; z++){
    if(z == 0) t = now.month();
    else if(z == 1) t = now.day();
    else if(z == 2) t = now.hour();
    else if(z == 3) t = now.minute();
    else t = now.second();
    if(t < 10) fp(zero);
    sprintf(ts, "%d", t);
    len += tp(ts);
    if(z == 0) len += fp(dash);
    else if(z == 1) len += fp(space);
    else if(z == 2 || z == 3) len += fp(points);
  }
  
  //Serial.println(len);
  len += fp(entry2); //soil moisture
  //Serial.println(len);
  sprintf(ts, "%d", soilMoisture); //reuse ts char array
  len += tp(ts);

  
  len += fp(entry3); //soil description
  len += tp(soilDescription);
  
  len += fp(entry4); //air humidity
  len += flp(airHumidity);
  
  len += fp(entry5); // air temperature
  len += flp(airTemperature);
  
  len += fp(entry6); // light state
  if(lightState) len += tp("true");
  else len+= tp("false");

  len += fp(entry7); //device ID
  len += ep(460); //get device ID from EEPROM
  len += fp(entry8);
  //Serial.println(len);
  
  //Serial.println(len);
  return len;
}

int processFloat(float value){
  char front[4] = "";
  int len = 0;
  sprintf(front, "%d", (int) value);
  len += processValue(front);
  len += processValue(".");
  sprintf(front, "%d", round((value - (int) value) * 10));
  len += processValue(front);
  return len;
}
int floatLen(float value){
  char front[4] = "";
  int len = 0;
  sprintf(front, "%d", (int) value);
  len += valueLen(front);
  len += valueLen(".");
  sprintf(front, "%d", round((value - (int) value) * 10));
  len += valueLen(front);
  return len;
}

void buildGetCodeRoute(char *route){
  int index = 0;
  index += dumpFragment(codeGetFrnt, route);
  index += dumpEeprom(460, route + index);
  index += dumpFragment(codeGetBack, route + index);
  route[index - 2] = '\r';
  route[index - 1] = '\n';
  route[index] = '\0';
}

int buildCodeJson(Fragmentor fp, Fragmentor tp, EProcessor ep, DateTime now){
  //fp is fragment processor, tp is time processor. We use delegates because we have to ensure that we 
  //go through the same exact process for content-length and for writing out json.
  //now is needed to process code execution time.
  //ep is EEPROM processor, needed to get device Id out of EEPROM.
  int len = 0;
  char ts[5];

  len += fp(code1);
  //Serial.println(len);
  int t = now.year();
  for(int i = 10; i <= 1000; i = i * 10){
    if(t < i) len += fp(zero);
  }
  //Serial.println(len);
  
  sprintf(ts, "%d", t);
  len += tp(ts);
  len += fp(dash);
  //Serial.println(len);
  
  for(int z = 0; z < 5; z++){
    if(z == 0) t = now.month();
    else if(z == 1) t = now.day();
    else if(z == 2) t = now.hour();
    else if(z == 3) t = now.minute();
    else t = now.second();
    if(t < 10) fp(zero);
    sprintf(ts, "%d", t);
    len += tp(ts);
    if(z == 0) len += fp(dash);
    else if(z == 1) len += fp(space);
    else if(z == 2 || z == 3) len += fp(points);
  }
  //Serial.println(len);
  len += fp(code2);
  //Serial.println(len);
  len += ep(460); //460 is the device Id address.
  //Serial.println(len);
  len += fp(code3);
  //Serial.println(len);
  return len;
}


EthernetClient client;

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

int dumpEeprom(int index, char *dest){
  char buff;
  int i = 0;
  EEPROM.get(index, buff);
  while(buff != '\0'){
    index++;
    *(dest + i) = buff;
    i++;
    EEPROM.get(index, buff);
  }
  return i;
}

int _eeprom(int index, bool doWrite){
  char buff;
  EEPROM.get(index, buff);
  while(buff != '\0'){
    index++;
    if(doWrite){
      client.print(buff);
      Serial.print(buff);
    }
    EEPROM.get(index, buff);
  }
  return index;
}
int processEeprom(int index){
  return _eeprom(index, true) - index;
}

int eepromLen(int index){
  return _eeprom(index, false) - index;
}

int processValue(char *value){
  client.print(value);
  Serial.print(value);
  return strlen(value);
}
int valueLen(char *value){
  return strlen(value);
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

int dumpFragment(char *fragment, char *dest){
  char buff = pgm_read_byte_near(fragment);
  int i = 0;
  while(buff != '\0'){
    *(dest + i) = buff;
    i++;
    
    buff = pgm_read_byte_near(fragment + i);
  }
  return i;
}

int processFragment(char *fragment){
  char buff = pgm_read_byte_near(fragment);
  int i = 0;
  while(buff != '\0'){
    i++;
    client.print(buff);
    Serial.print(buff);
    buff = pgm_read_byte_near(fragment + i);
  }
  return i;
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

int strLen(char *str){
  char buff = *(str);
  int i = 0;
  int len = 0;
  while(buff != '\0'){
    if(buff != ' ') len += 1;
    i++;
    buff = *(str + i);
  }
  return len;
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
  writeFragment(route);
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

void writeStaticRoute(char *route){
  client.print(route);
  Serial.print(route);
}

void getter(char *route, bool staticPath){
  if (connectToServ()) {
      printMakeHttp();

      if(!staticPath) writeRoute(route);
      else writeStaticRoute(route);
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
void writePostContent(){
  writeFragment(contLen);
  DateTime now = rtc.now();
  int len = buildEntryJson(&fragmentLen, &valueLen, &eepromLen, &floatLen, sys.getTime(), sys.measure.soilMoisture, 
  logger.soilCondition(sys.measure.soilMoisture), sys.measure.humidity, sys.measure.temperature, sys.getLightingState());
  client.print(len);
  Serial.print(len);
  writeNewLine();
  writeNewLine();
  buildEntryJson(&processFragment, &processValue, &processEeprom, &processFloat, sys.getTime(), sys.measure.soilMoisture, 
  logger.soilCondition(sys.measure.soilMoisture), sys.measure.humidity, sys.measure.temperature, sys.getLightingState());
}

void writeCodeContent(){
  writeFragment(contLen);
  DateTime now = rtc.now();
  int len = buildCodeJson(&fragmentLen, &valueLen, &eepromLen, now);
  client.print(len);
  Serial.print(len);
  writeNewLine();
  writeNewLine();
  buildCodeJson(&processFragment, &processValue, &processEeprom, now);
}

void writeLoginContent(){
  writeFragment(contLen);
  int len = fragmentLen(loginJson);
  client.print(len);
  Serial.print(len);
  writeNewLine();
  writeNewLine();
  processFragment(loginJson);
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
void poster(char *route, bool writeToken, ContentWriter cw, bool staticPath){
  if (connectToServ()) {
      printMakeHttp();

      if(!staticPath) writeRoute(route);
      else writeStaticRoute(route);
      writeHost();
      if(writeToken) writeJwtToken();
      writeConnection();
      writeContentType();
      writeCacheType();
      cw();
      writeNewLine();
      printOk();
    } 
    else {
      // kf you didn't get a connection to the server:
      printConnFailed();
  }
}

void getMyId(){ //working
  
  getter(nameCall, false);
  parseResponse(minPhrs, minPhrs, (int*) NULL, true, false, 460);
  printEeprom(460);
}

int getCode(){ //working
  char route[80] = "test";
  buildGetCodeRoute(route);
  int code = 500; //if 500 then some error happened. parser has to change this value.
  getter(route, true);
  /*
   * Http response parser. If it screws something up it shouldn't screw up EEPROM values.
   * If it does, it'll screw up deviceId storage (460-496)
   * As long as code address exists parser won't write to EEPROM.
   */
  parseResponse(actnFrnt, actnBack, &code, false, false, 460);
  Serial.print("I parsed this: ");
  Serial.println(code);
  return code;
}

void login(){ //working
  //COMMENT OUT EEPROM PUT!
  //EEPROM.put(0, "Authorization: Bearer ");
  poster(loginPath, false, writeLoginContent, false);
  parseResponse(minPhrs, minPhrs, (int*) NULL, true, true, 22); //store token between " and " to EEPROM address 0+. Start from address 22 because Authorization_token takes that amount of space
}

void completeCode(){ //working
  poster(codeCall, true, writeCodeContent, false);
  printResponse();
}

//TODO: fix

void postEntry(){ //working
  
  poster(entryPost, true, writePostContent, false);
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

void parseResponse(char *before, char *after, int *action, bool eepromWrite, bool writeNewLine, int eepromIndex){
  char actStrg[30] = "";
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
            if(eepromWrite) {
              if(writeNewLine) EEPROM.put(eepromIndex + writeIndex - searchSize, "\r\n\0");
              else EEPROM.put(eepromIndex + writeIndex - searchSize, '\0');
            }
            else actStrg[writeIndex - searchSize] = '\0';
          }
        }
      }
      Serial.print(c);
    }
  }
  actStrg[29] = '\0';
  Serial.println(F("disconnecting."));
  client.stop();
  Serial.print("inside parse: ");
  Serial.println(actStrg);
  if(!eepromWrite){
    Serial.println(strLen(actStrg));
    if(strLen(actStrg) != 0){
      Serial.println("entered");
      *action = atoi(actStrg);
      Serial.println("exited");
    }
    else *action = 500;
  }
  Serial.print("which atoied to: ");
  Serial.println(*action);
  
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

