class Dummy{
  public:
  bool test;
  bool test2;
  int val;

  Dummy(bool t, bool t1, int value){
    test = t;
    test2 = t1;
    val = value;
  }

  void printStates(){
    Serial.print("Bool 1 "); if(test) Serial.println("true"); else Serial.println("false");
    Serial.print("Bool 2 "); if(test2) Serial.println("true"); else Serial.println("false");
    Serial.print("Integer 1 "); Serial.println(val);
  }
  void printEnter(){
    Serial.println("HEY YOU PRESSED ME");
  }
};

typedef void (Dummy::*Event) ();

class Setting : public Item{
  protected:
  
  Dummy *controller = NULL;
  Event event = NULL;

  public:

  Setting(char *title, Dummy *control, Event event) : Item(title){
    controller = control;
    this->event = event;
    type = SETTING;
  }

  Item *enter(){
    (controller->*event)();
    Item::enter();
  }
  
};

template <class T>
class ValueSetting : public Setting{

  private:

  T *value;

  public : 

  T tempValue;

  ValueSetting(char *title, T *bindValue, Dummy *control, Event event) : Setting(title, control, event){
    value = bindValue;
    tempValue = *value;
  }

  T getValue(){
    return *value;
  }
  T getTempValue(){
    return tempValue;
  }

  bool isSaved(){
    return (*value == tempValue);
  }

  virtual void left() = 0;
  virtual void right() = 0;

  Item *enter(){
    *value = tempValue;
    return Setting::enter();
  }
  
};

class BoolSetting : public ValueSetting<bool>{

  public:
  
  BoolSetting(char *title, bool *bindValue, Dummy *control, Event event) : ValueSetting<bool>(title, bindValue, control, event){
    type = BOOLSETTING;
  }
  
  void left(){
    tempValue = !tempValue;
  }
  void right(){
    left();
  }
};
class IntSetting : public ValueSetting<int>{
  public:

  IntSetting(char *title, int *bindValue, Dummy *control, Event event) : ValueSetting<int>(title, bindValue, control, event){
    type = INTSETTING;
  }
  
  void left(){
    if(tempValue - 1 >= 0) tempValue--;
  }
  void right(){
    if(tempValue + 1 <= 9) tempValue++;
  }
};

