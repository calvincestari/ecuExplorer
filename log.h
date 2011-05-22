#pragma once

#include <string>
using namespace std;

#include "dialogEcuFlash.h"

class log
{
private:
	FILE* hLogfile;
	int logMask;
	bool bLogFile;
	bool bLogDebug;
	bool bTimeStamp;
	bool bTimeRelative;
	string filename;
	
	int tLogStart;
	int tLogLast;

	bool bLogDialog;
	dialogEcuFlash* pDialog;

public:
	log(void);
	~log(void);
	void set_filename(string _filename);
	void set_timestamp(bool _timestamp);
	void set_mask(int _mask);
	bool open();
	bool close();
	bool print(IN ULONG _level, PCHAR Format, ...);
	bool print(PCHAR Format, ...);
	void set_dialog(dialogEcuFlash* lpDialog);
};
