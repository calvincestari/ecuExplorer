/*/
	Macros.h (06/02/2004)
/*/
#pragma once

// includes
#include <wfc.h>

// registry macros
#define READREGDW(member, def_value, svalue)\
	reg.RegReadWithCreate(&member, def_value, svalue);	 
#define READREGS(member, def_value, svalue)\
	reg.RegReadWithCreate(&member, def_value, svalue);
#define DELREG(member)\
	reg.DeleteKey(member);
#define OPENREG(reg_name,module_name)\
	CRegistry reg;\
    CString sRegTemp;\
    reg.OpenWithCreate(reg_name,(LPCSTR)module_name);
#define CLOSEREG()\
	reg.Close();