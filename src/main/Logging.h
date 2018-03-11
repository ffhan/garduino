#ifndef __LOGGING_INCLUDED__
#define __LOGGING_INCLUDED__

class DateTime;
class Measuring;
class Control;
class WebController;

class Logging {

private:

	Control * sys;

public:

	//void initSD();
	void writeHeader();
	void printHeader();
	void logData(DateTime now, Measuring *measure);
	char *soilCondition(int soilValue);
	void printTime(DateTime now);

	Logging(Control *sys);
};

#endif
