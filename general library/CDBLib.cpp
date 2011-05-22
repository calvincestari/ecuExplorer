// -----------------------------------------------------------------------------------
//
// Function Module : CDblib.CPP
//
// Description: Database access utilities
//
// Author ElectroDynamics, April 1995
// Changes:-
//	ElectroDynamics - 11/3/96 - Updated for VC2.00 & C++ + Strict casting
//  ElectroDynamics - 08/12/96 - Converted to a C++ Object
//  ElectroDynamics  - 29/7/99 - Improved null checking in CString
// -----------------------------------------------------------------------------------
#ifndef DBNTWIN32
 #define DBNTWIN32
#endif
// -----------------------------------------------------------------------------------
// Global include files
// -----------------------------------------------------------------------------------	
#include <wfc.h>
//#include <sqlfront.h>
//#include <sqldb.h>
#include <CDBLib.h>
#include <CDump.h>

// -----------------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// External variables
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// Global Defines
// -----------------------------------------------------------------------------------
// Set the TDS packet size for efficient transfers
// defualt '512'
#define TDS_PACKETSIZE 512

// -----------------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------------
int err_handler (PDBPROCESS dbproc, INT severity,
    INT dberr, INT oserr, LPCSTR dberrstr, LPCSTR oserrstr);
int msg_handler (PDBPROCESS dbproc, DBINT msgno, INT msgstate,
    INT severity, LPCSTR msgtext, LPCSTR server,
    LPCSTR procedure, DBUSMALLINT line);

// Used by FIELD routines to identify the column name
#define DBLIB_FIND_COL(col_name)\
char * szColName;\
int i = 1;\
do\
  {\
   szColName = (char *)dbcolname(dbproc, i++);\
  }\
 while((szColName != NULL) && (strcmp(szColName,(LPCSTR)col_name) != 0));


// -----------------------------------------------------------------------------------
// Function:    CDBLIB
// Description: Constructor of the DBLIB Database handler class.
// parameters:  None
// return code: None
// -----------------------------------------------------------------------------------
CDBLIB::CDBLIB()
{
	m_fpSQLLogFile = fopen("SQLLog.txt","a");
	if (m_fpSQLLogFile == NULL)
	{
		DUMP("Unable to open SQL Log File",ERR);
		//return TRUE;
	}
}

// -----------------------------------------------------------------------------------
// Function:    ~CDBLIB
// Description: Constructor of the DBLIB Database handler class.
// parameters:  None
// return code: None
// -----------------------------------------------------------------------------------
CDBLIB::~CDBLIB()
{
	if (m_fpSQLLogFile != NULL)
	{
		fclose(m_fpSQLLogFile);
		m_fpSQLLogFile = NULL;
		//m_ulFileSize=0;
	}
}
// -----------------------------------------------------------------------------------
//
// Function : OpenDatabase
//
// Description: Opens a connection to a remote database server
//
// Return: dbproc, or 0L
//
// -----------------------------------------------------------------------------------
DBPROCESS * CDBLIB::OpenDatabase(CString * sUser, 
								CString * sPassword,
								CString * sApp,
								CString * sDatabase,
								CString * sServerName,
								CString * sWSName)
{
DBPROCESS *dbproc;
LOGINREC  *login;
CString sError;

	
	if (dbinit() == FAIL)
	{
		// Could not initialise private DB-Library structures
		// and retrieve international settings from the registry
		DUMP("Could not initialise DBLIB",ERR);
		return 0L;
	}

	// Allocate a LOGINREC structure for use in dbopen()
	if ((login = dblogin()) == NULL)
	{
		// Could not allocate a LOGINREC structure
		DUMP("Could not allocate a LOGINREC structure DBLIB",ERR);
		return 0L;
	}

	// Install Message and Error callback handlers
	dberrhandle(err_handler); // (int(__cdecl *)(void))
	dbmsghandle(msg_handler); //(int(__cdecl *)(void))

	// Set the workstation name in the LOGINREC structure
	// The workstation name shows up in the 'sysprocesses' table in the
	// 'master' database. Max len of 30 characters uses only 10
	DBSETLHOST(login,(LPCSTR)*sWSName); 
	// Sets the user name in the LOGINREC structure
	// Maximum length 30 characters
	DBSETLUSER(login,(LPCSTR)*sUser);
	// Sets the user SQL Server password in the LOGINREC structure
	// Maximum length 30 characters
	DBSETLPWD(login,(LPCSTR)*sPassword);
	// Set the application name in the LOGINREC structure
	// SQL Server uses the application name in its 'sysprocesses' table
	// to help identify your process.
	DBSETLAPP(login,(LPCSTR)*sApp);
	// Sets the name of the national language in the LOGINREC structure
	DBSETLNATLANG(login,NULL);
	// Set the TDS packet size for efficient transfers
	// defualt '512'
	DBSETLPACKET(login,TDS_PACKETSIZE);
	// Allocate and initialise a DBPROCESS structure
	if ((dbproc = dbopen(login,(LPCSTR)*sServerName))== NULL)
	{
		// Calls erro handler with more details
		sError.Format("Unable to connect: SQL Server[%s] is unavailable or does not exist DBLIB",(LPCSTR)*sServerName);
		DUMP((LPCSTR)sError,ERR);
		dbfreelogin (login );
		return 0L;
	}
	if (dbuse(dbproc,(LPCSTR)*sDatabase) == FAIL )
	{
		sError.Format("Unable to open database [%s] DBLIB",(LPCSTR)*sDatabase);
		DUMP((LPCSTR)sError,ERR);
		dbclose(dbproc);
		dbfreelogin (login );
		return 0L;
	}
	dbfreelogin (login );
	return dbproc;
}

// -----------------------------------------------------------------------------------
//
// Function : CreateDynaset
//
// Description: Execute a query and hold result set
//
// Return: SUCCEED, or NO_MORE_RESULTS, or FAIL
//
// -----------------------------------------------------------------------------------
 
RETCODE CDBLIB::CreateDynaset(DBPROCESS * dbproc,CString * sQuery  )
{
	RETCODE iRet;
	try{
		 // Add text to the DBPROCESS command buffer
		 if (dbcmd(dbproc, (LPCSTR)*sQuery)== FAIL)
		{
			 if (m_fpSQLLogFile != NULL)
					fprintf(m_fpSQLLogFile,"%s\n",(LPCSTR)*sQuery);
			 return 0L;
		}
		 // Send a command batch to SQL Server
		 if (dbsqlexec(dbproc) == FAIL)
		 {
			 if (m_fpSQLLogFile != NULL)
					fprintf(m_fpSQLLogFile,"%s\n",(LPCSTR)*sQuery);
			 return 0L;
		 }
		 // Sets up the results of the next query
		iRet = dbresults(dbproc);
	}
	catch(...)
	{
		DUMP("Exception occurred in DB Create Dynaset",ERR);
		iRet = 0;
	}
 return iRet;
}

// -----------------------------------------------------------------------------------
//
// Function : FieldINT
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
// 13/3/96 - ElectroDynamics - Added NULL checking, removed problem wit i-- _Debug
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, int * lpDestination)
{
 int ii;
 void * jj;
 // Find the right column
 DBLIB_FIND_COL(sFieldName);
ii = dbdatlen(dbproc,i-1);
jj = dbdata(dbproc,i-1);

// Check for NULL
if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	*lpDestination = 0;
else
	*lpDestination = (int)*((int*)dbdata(dbproc,i));
}

// -----------------------------------------------------------------------------------
//
// Function : FieldINT
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
// 13/3/96 - ElectroDynamics - Added NULL checking, removed problem wit i-- _Debug
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, short * lpDestination)
{
 //int ii;
 //void * jj;
 // Find the right column
 DBLIB_FIND_COL(sFieldName);
//ii = dbdatlen(dbproc,i-1);
//jj = dbdata(dbproc,i-1);

// Check for NULL
if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	*lpDestination = 0;
else
	*lpDestination = (short)*((short*)dbdata(dbproc,i));
}

// -----------------------------------------------------------------------------------
//
// Function : FieldINT
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
// 13/3/96 - ElectroDynamics - Added NULL checking, removed problem wit i-- _Debug
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, BYTE * lpDestination)
{
 //int ii;
 //void * jj;
 // Find the right column
 DBLIB_FIND_COL(sFieldName);
//ii = dbdatlen(dbproc,i-1);
//jj = dbdata(dbproc,i-1);

// Check for NULL
if ((szColName == NULL) || (dbdatlen(dbproc,--i) < 0))
	*lpDestination = 0;
else
	*lpDestination = (BYTE)*((BYTE*)dbdata(dbproc,i));
}

// -----------------------------------------------------------------------------------
//
// Function : FieldBool
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
// 13/3/96 - ElectroDynamics - Added NULL checking, removed problem wit i-- _Debug
// 29/7/99 - ElectroDynamics - Improved null checking of CString
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, bool * lpDestination)
{
 //int ii;
 //void * jj;
 // Find the right column
 DBLIB_FIND_COL(sFieldName);
//ii = dbdatlen(dbproc,i-1);
//jj = dbdata(dbproc,i-1);

// Check for NULL
if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	*lpDestination = 0;
else
	*lpDestination = (bool)*((bool*)dbdata(dbproc,i));
}

// -----------------------------------------------------------------------------------
//
// Function : FieldTINT
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
//void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, short * lpDestination)
//{

// Find the right column
//DBLIB_FIND_COL(sFieldName);

//if ((dbdatlen(dbproc,--i) < 0) || (szColName == NULL)) 
//	*lpDestination = 0;
//else
//	*lpDestination = (short)*dbdata(dbproc,i);
//}

// -----------------------------------------------------------------------------------
//
// Function : FieldDATE
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, DBDATEREC * lpDestination)
{
// Find the right column
DBLIB_FIND_COL(sFieldName);

if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	lpDestination = NULL;
else
	dbdatecrack(dbproc,lpDestination,(DBDATETIME *)dbdata(dbproc,i) ); 

}

// -----------------------------------------------------------------------------------
//
// Function : FieldDATE - CTime
//
// Description: Return a pointer to the referenced field data
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, CTime * lpDestination)
{
DBDATEREC  Date;
DBDATEREC * pDate;
pDate = &Date;

// Find the right column
DBLIB_FIND_COL(sFieldName);

if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	lpDestination = NULL;
else
{
	dbdatecrack(dbproc,pDate,(DBDATETIME *)dbdata(dbproc,i) );
	// Prevents error on a newly initialised date field!
	if (pDate->year == 1900) pDate->year = 1971;
#pragma message ("dt from DB resricted to > 1971" __FILE__)
    CTime tTemp(pDate->year,pDate->month,pDate->day,pDate->hour,pDate->minute,pDate->second,0);
    *lpDestination = tTemp;
}

}

// -----------------------------------------------------------------------------------
//
// Function : FieldMONEY
//
// Description: Return a DBMONEY field data
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, DBMONEY * lpDestination)
{

DBMONEY Money;
DBMONEY * lpMoney;
lpMoney = &Money;
// Find the right column
DBLIB_FIND_COL(sFieldName);

if ((szColName == NULL) || (dbdatlen(dbproc,--i) <= 0)) 
	lpDestination = NULL;
else
{
 lpMoney = (DBMONEY *)dbdata(dbproc, i);
 lpDestination->mnyhigh = lpMoney->mnyhigh;
 lpDestination->mnylow = lpMoney->mnylow;
}
}

// -----------------------------------------------------------------------------------
//
// Function : FieldCHAR
//
// Description: Return a character
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, CString * lpDestination)
{
DBINT iLen;
BYTE * lpByte;
//char * lpDest;
// Find the right column
DBLIB_FIND_COL(sFieldName);
iLen = dbdatlen(dbproc,--i);
if ((szColName == NULL) || (iLen <= 0))
	*lpDestination = "";
else
{
	lpByte = dbdata(dbproc, i); 
	*lpDestination=lpByte;
	//lpDest = lpDestination->GetBuffer(iLen+10);
	//memcpy(lpDest,lpByte,iLen);
	//lpDestination->ReleaseBuffer();//iLen
	*lpDestination=lpDestination->Left(iLen);
	lpDestination->TrimRight();
}
}

// -----------------------------------------------------------------------------------
//
// Function : FieldCHAR
//
// Description: Return a string from the referenced field data
//
// Return: pointer to variable or NULL
//
// -----------------------------------------------------------------------------------
void CDBLIB::Field(DBPROCESS * dbproc, CString sFieldName, char * lpDestination)
{
	char * lpByte;
	int iLen;
	 // Find the right column
	DBLIB_FIND_COL(sFieldName);
	iLen = dbdatlen(dbproc,--i);
	if ((szColName == NULL) || (iLen  <= 0)) 
		*lpDestination = '\0';
	else
	{
		lpByte=(char *)dbdata(dbproc, i);

		// Copy string to destination
		memcpy((char *)lpDestination,(char *)lpByte,iLen);

		// Null terminate string
		*(lpDestination+iLen--)='\0';

		// Trim spaces from RHS and pad with null
		while((lpDestination)[iLen] == ' ') lpDestination[iLen--] = '\0';
	}
}

// -----------------------------------------------------------------------------------
//
// Function : CloseDatabase
//
// Description: Close the current database
//
// Return: None
//
// -----------------------------------------------------------------------------------
void CDBLIB::CloseDatabase(DBPROCESS * dbproc)
 {
  dbclose(dbproc);
 }

// -----------------------------------------------------------------------------------
//
// Function : err_handler
//
// Description: Process DBLib errors
//
// Return: INT_CANCEL
//
// -----------------------------------------------------------------------------------
int err_handler (PDBPROCESS dbproc, INT severity,
    INT dberr, INT oserr, LPCSTR dberrstr, LPCSTR oserrstr)
{
	CString sError;

/* ======================================================= */
 /* NOTE : Any other DB-Library calls made from inside this */
 /* handler CAN result in recursive calls.  To get around   */
 /* this problem re-assign the error handler to null and    */
 /* once the error handling is done assign the original     */
 /* error handler back to handle the next error.            */
 /* ======================================================= */

 //wprintf(L"Error - Dblib error report : %S",dberrstr);

 switch (severity)
  {
   case EXINFO     : // Don't worry just information
                     break;
   case EXUSER     : // User Error
                     break;
   case EXNONFATAL : // Don't worry a non fatal error
                     break;
   case EXCONVERSION : // Conversion error
                       break;
   case EXSERVER   : // The server has returned an error flag
                     break;
   case EXTIME     : // Timeout has occurred while waiting for results
	   //INT_CONTINUE	
                     break;
   case EXPROGRAM  : // An error has occurred in the application
                     break;
   case EXRESOURCE : // Running out of resources - dbproc may be dead
                     break;
   case EXCOMM     : // Failure in communication with server
					//DisconnectSql();
					// iLoop = 0;
					// while(!ConnectSql(IVRNo))
					// {
					// 	iLoop++;
					// 	if (iLoop == 3)
					//	{
				    // 		wprintf(L"SQL connect cannot be re-established");
					//		break;
					//	}
					// } 
                     break;
   case EXFATAL    : // Fatal error dbproc may be dead
                     break;
   case EXCONSISTENCY : // Internal software - notify MS
                        break;
  }

 
 //if (oserr != DBNOERR)
//	wprintf(L"Operating system error : %S",oserrstr);

// if ((dbproc == NULL) || (DBDEAD(dbproc)))
//	return INT_CANCEL;//INT_EXIT;

//#define INT_EXIT        0
//#define INT_CONTINUE    1
//#define INT_CANCEL      2
 // Ignore General SQL Error as it is reported again
	if(dberr != 10007)
	{
		sError.Format("DB-Library Error %i: %s", dberr, dberrstr);
		DUMP ((LPCSTR) sError,ERR);
		if (oserr != DBNOERR)
		{
			 sError.Format("Operating System Error %i: %s", oserr, oserrstr);
			 DUMP ((LPCSTR) sError,ERR);
		}
	}
    return (INT_CANCEL);
}

// -----------------------------------------------------------------------------------
//
// Function : msg_handler
//
// Description: Print SQL Server Messages
//
// Return: None
//
// -----------------------------------------------------------------------------------
int msg_handler (PDBPROCESS dbproc, DBINT msgno, INT msgstate,
    INT severity, LPCSTR msgtext, LPCSTR server,
    LPCSTR procedure, DBUSMALLINT line)
{
	CString sError;
	if(msgno != 5701)
	{
		sError.Format("SQL Server Message %ld: %s", msgno, msgtext);
		DUMP ((LPCSTR) sError,INFO); 
	}
    return (0);
}
// -----------------------------------------------------------------------------------
//
// Function : ColName
//
// Description: Returns the CString name of column n
//
// Return: Column Name
//
// -----------------------------------------------------------------------------------
CString CDBLIB::ColName(DBPROCESS * dbproc, int iCol)
{

	return (char *)dbcolname(dbproc, iCol);
}
// -----------------------------------------------------------------------------------
//
// Function : NextRow
//
// Description: Determines if there is another result set, then gets it
//
// Return: None
//
// -----------------------------------------------------------------------------------
int CDBLIB::NextRow(DBPROCESS * dbproc)
{
	return dbnextrow(dbproc);
}

// -----------------------------------------------------------------------------------
//
// Function : NumCols
//
// Description: Returns the number of columns in the result set, field's
//
// Return: Cols - 0 = None
//
// -----------------------------------------------------------------------------------
int CDBLIB::NumCols(DBPROCESS * dbproc)
{
	return dbnumcols(dbproc);
}

#ifdef _ICM_STAT_
// The following routines are ONLY provided for the ICM compiles of the
// stats server.  Symphony allows for many Connectors(HSL,RTD,SEI,etc) and
// multiple DB Connectors(SQL,ACCESS,etc) but in trying to retain the current
// ICM operational settings these multiple instances have been removed when
// compiling the ICM version.  However the calls to the DB routines have been
// changed to reflect the multiple connectors, and this causes problems during 
// the compile since they are no longer there.  Therefore these routines act
// as a time saver in the way that they mask the multiple connectors into
// singular connectors
void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, int * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, short * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, char * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, DBDATEREC * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, DBMONEY * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, CTime * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, BYTE * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, bool * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

void CDBLIB::Field(CString sSource,DB_POOL_REC * dbproc, CString sFieldName, CString * lpDestination)
{
	// Map to existing singular routine call - ignore CString
	Field(dbproc->hDBHandle,sFieldName,lpDestination);
}

int CDBLIB::CreateDynaset(CString sSource,DB_POOL_REC * pDBHandle,CString * sQuery)
{	
	// Map to existing singular routine call - ignore CString
	return CreateDynaset(pDBHandle->hDBHandle,sQuery);
}
#endif