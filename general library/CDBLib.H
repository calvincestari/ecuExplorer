// -----------------------------------------------------------------------------------
//
// Function Module : CDBLib.H
//
// Description: DBLIB Database access utilities
//
// Author Electrodynamics, CMS, April 1995
// -----------------------------------------------------------------------------------
#ifndef _cdblib_h_
#define _cdblib_h_

//#define DBNTWIN32
#include <sqlfront.h>
#include <sqldb.h>


// Structure Declarations
#ifdef _ICM_STAT_
struct DB_POOL_REC
{
	DBPROCESS * hDBHandle;
	enum DBState {DBLOGOUT,DBREADY,DBINUSE,DBERROR} iState;
	int iBusyCount;
	int iIdleCount;
	CString sSource;
#ifdef _DEBUG
	 char cID[20];
#endif
};
#endif


// -----------------------------------------------------------------------------------
// Class declarations
// ----------------------------------------------------------------------------------- 
class CDBLIB
{
public:
	DBPROCESS * OpenDatabase(CString * sUser, 
								CString * sPassword,
								CString * sApp,
								CString * sDatabase,
								CString * sServerName,
								CString * sWSName);
	RETCODE CreateDynaset(DBPROCESS * dbproc,CString * sQuery  );
	CDBLIB();
	~CDBLIB();
	void Field(DBPROCESS * dbproc, CString sFieldName, int * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, short * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, char * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, DBDATEREC * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, DBMONEY * lpDestination);
    void Field(DBPROCESS * dbproc, CString sFieldName, CTime * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, BYTE * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, bool * lpDestination);
	//void Field(DBPROCESS * dbproc, CString sFieldName, short * lpDestination);
	void Field(DBPROCESS * dbproc, CString sFieldName, CString * lpDestination);
	void CloseDatabase(DBPROCESS * dbproc);
	CString ColName(DBPROCESS * dbproc, int iCol);
	int NextRow(DBPROCESS * dbproc);
	int CDBLIB::NumCols(DBPROCESS * dbproc);
	FILE * m_fpSQLLogFile;

	// The following routines are ONLY provided for the ICM compiles of the
	// stats server.  Symphony allows for many Connectors(HSL,RTD,SEI,etc) and
	// multiple DB Connectors(SQL,ACCESS,etc) but in trying to retain the current
	// ICM operational settings these multiple instances have been removed when
	// compiling the ICM version.  However the calls to the DB routines have been
	// changed to reflect the multiple connectors, and this causes problems during 
	// the compile since they are no longer there.  Therefore these routines act
	// as a time saver in the way that they mask the multiple connectors into
	// singular connectors
#ifdef _ICM_STAT_
	int CDBLIB::CreateDynaset(CString sSource,DB_POOL_REC * pDBHandle,CString * sQuery);		
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, int * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, short * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, char * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, DBDATEREC * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, DBMONEY * lpDestination);
    void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, CTime * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, BYTE * lpDestination);
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, bool * lpDestination);	
	void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, CString * lpDestination);
#endif

};
#endif



