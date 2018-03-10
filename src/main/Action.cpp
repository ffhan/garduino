#include "Action.h"

FunctionList::FunctionList(Control *control) {
	sys = control;
}

PromisePack::PromisePack(Control *sys, Promise promise, char *failMessage, bool complement = false) {
	this->sys = sys;
	this->failMessage = failMessage;
	this->promise = promise;
	this->complement = complement;
}

bool PromisePack::go() {
	if (complement) {
		if (1 - (sys->*promise)()) return true;
		Serial.println(failMessage);
		return false;
	}
	if ((sys->*promise)()) return true;
	Serial.println(failMessage);
	return false;
}

void FunctionList::add(PromisePack *promise) {
	FunctionNode *node = new FunctionNode();
	node->event = promise;
	node->sys = sys;
	node->count = len++;
	//Serial.println(node->count);

	//printItem(node->item);

	if (head == NULL) {
		head = node;
		tail = head;

		//printItem(node->item);
		return;
	}

	tail->next = node;

	tail = node;

	//printItem(node->item);
	return;
}

int FunctionList::getLen() {
	return len;
}

bool FunctionList::allTrue() {
	if (len == 0) {
		return true;
	}

	FunctionNode *head = this->head;
	PromisePack *item = head->event;

	//Serial.print("0"); Serial.println((sys->*item)());

	int i = 1;
	while (head) {

		if (!item->go()) return false;

		head = head->next;
		item = head->event;

		//Serial.print(i); Serial.println((sys->*item)());

		i++;
	}

	return true;
}

bool FunctionList::anyTrue() {
	if (len == 0) {
		return true;
	}

	FunctionNode *head = this->head;
	PromisePack *item = head->event;

	int i = 1;
	while (head) {

		if (item->go()) return true;

		head = head->next;
		item = head->event;

		i++;
	}

	return false;
}

Action::Action(Control *control, int code, Event event) {
	sys = control;
	this->code = code;
	list = new FunctionList(sys);
	this->event = event;
}

Action::Action(Control *control, int code, SetBit setBit, int *value) {
	sys = control;
	this->code = code;
	list = new FunctionList(sys);
	this->setBit = setBit;
	bindValue = value;
}

Action::Action(Control *control, int code, SetBit setBit) {
	sys = control;
	this->code = code;
	list = new FunctionList(sys);
	this->setBit = setBit;
}

void Action::bindToValue(int *value) {
	bindValue = value;
}

void Action::execute() {
	if (list->allTrue()) {
		if (event) {
			(sys->*event)();
		}
		else if (setBit) {
			(sys->*setBit)(*bindValue);
		}
	}
}

