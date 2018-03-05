#include "ValueSetting.h"

template <class T>
ValueSetting<T>::ValueSetting(char *title, T *bindValue, Dummy *control, Event event) : Setting(title, control, event){
	value = bindValue;
	tempValue = *value;
}

template <class T>
T ValueSetting<T>::getValue(){
	return *value;
}
template <class T>
T ValueSetting<T>::getTempValue(){
	return tempValue;
}

template <class T>
bool ValueSetting<T>::isSaved(){
	return (*value == tempValue);
}

template <class T>
Item *ValueSetting<T>::enter(){
	*value = tempValue;
	return Setting::enter();
}

template class ValueSetting<bool>;
template class ValueSetting<int>;
