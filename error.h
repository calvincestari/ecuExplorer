#pragma once

#include <vector>
#include <string>
using namespace std;

class CError  
{
public:
	CError();
	virtual ~CError();

	bool bDisplayError;
	vector <bool> bDisplayStack;
	string msg;
	int errorcount;

	void ReportError();
	void ReportSysError();
	void PushDisplayState(bool state) {bDisplayStack.push_back(bDisplayError); bDisplayError = state;} 
	void PopDisplayState() {bDisplayError = bDisplayStack[bDisplayStack.size()-1]; bDisplayStack.pop_back();} 
};
