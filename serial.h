#pragma once

#include <wfc.h>
#include <string>
#include <vector>
using namespace std;
#include "Error.h"

class serial
{
public:
	string portname;
	int baudrate;

private:
	CError Error;
	int portnum;
	HANDLE hCom;
	int defaulttimeout;

	bool init();
	bool is_active();
	void OnPortError();

public:
	serial(void);
	~serial(void);
	bool open();
	void close();
	bool write(void *data,int len);
	bool read(void *data,int len);
	bool read_timeout(void *data,int len,int msTimeOut);
	bool read_and_echo(void *data,int len);
	bool read_timeout_and_echo(void *data,int len,int msTimeOut);
	bool flush();
	unsigned int get_status();
	bool set_status(unsigned int);
	void set_default_timeout(int to);
	vector <string> port_names();
	void set_baud(int baud);
	void set_port(string port);
};
