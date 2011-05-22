/*/
	dialogEcuQuery.h (2006.01.24)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "resource.h"
#include "serial.h"

class dialogEcuQuery : public CDialog
{
public:
	dialogEcuQuery();
	void UpdateProgress(LPCTSTR cBuffer);

	bool ready_port();
	bool dump_memory_to_file(string dumpfile,int dumpstart,int dumpend);
	bool dump_memory_to_file(FILE *fpd,int dumpstart,int dumpend);
	bool readaddresssingle_rsp(unsigned char *data, int addr, int* len);
	bool readaddresssingle_cmd_echo_check(unsigned char *data,int start,int datalen);
	unsigned char generate_checksum(unsigned char* data,int datalen);
	void dump_hex(unsigned char* data,unsigned short len,unsigned char header);
	bool write_buffer_echo_check(unsigned char *data,unsigned char chk,int datalen);
	bool read_buffer_varlen(unsigned char* rsp,unsigned char *buffer,int *len);

	enum {IDD = IDD_DIALOG_ECUQUERY};

	BOOL bQuery;
	void* lpParent;
	CFont* lpFont;
	int iAddress_start;
	int iAddress_stop;
	serial s;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCapture();
	virtual void OnClose();
	virtual BOOL OnInitDialog();
	virtual void OnKillFocus();
	
	DECLARE_MESSAGE_MAP()
};