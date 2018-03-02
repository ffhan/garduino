typedef void (*Event) ();

template <class T>
class Setting : public Item{

  private:

  T *value;

  Event event = NULL;

  public : 

  T tempValue;

  Setting(char *title, T *bindValue, Event event) : Item(title){
    value = bindValue;
    tempValue = *value;
    this->event = event;
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
    Serial.println("entered.");
    *value = tempValue;
    event();
    return this;
  }
  
};

class BoolSetting : public Setting<bool>{

  public:
  
  BoolSetting(char *title, bool *bindValue, Event event) : Setting<bool>(title, bindValue, event){
    type = BOOLSETTING;
  }
  
  void left(){
    Serial.println("left");
    tempValue = !tempValue;
  }
  void right(){
    left();
  }
};
class IntSetting : public Setting<int>{
  public:

  IntSetting(char *title, int *bindValue, Event event) : Setting<int>(title, bindValue, event){
    type = INTSETTING;
  }
  
  void left(){
    if(tempValue - 1 >= 0) tempValue--;
  }
  void right(){
    if(tempValue + 1 <= 9) tempValue++;
  }
};
//todo: implement TriggerSetting (doesn't hold a value, only calls a specific action.)
