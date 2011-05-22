#ifndef _LEXANAL_H_
#define _LEXANAL_H_


/* ===================================================================== */
/* *****                       Lexical Analyser                    ***** */
/* *****                       Header File                         ***** */
/*
   This file is the header file for the lexical analyser.  It contains
   the definitions for the tokens,error codes and functions.

   Date   : 14/03/95
   Ver    : 1.00
   Author : Shawn  
   3/11/96 - Converted to C++ Electrodynamics                                                     */
/* ===================================================================== */

/* Compiler definitions for calling the CarriageReturns function to
   turn OFF or ON the carriage returns                                   */

#define CR_ON  1
#define CR_OFF 0

/* Compiler definitions for calling the Spaces function to
   turn OFF or ON the spaces                                             */

#define SP_ON  1
#define SP_OFF 0

/* Definition for the size of the token                                  */

#define TOKENSIZE 40

/* Definition for the size of the storage buffer when data is read from
   the file.  Recommend size be equal to a multiple od the physical
   sector size.                                                         */

#define STORAGESIZE 4096

/* Definition for set containing upper and lower versions of 
   the alphabet,  characters forming the numerical alphabet. 
   
   The blankset is used for skipping characters which are of
   no relevance to the tokens.  When the CarriageReturns 
   function is called with CR_ON then the carriage return char
   is allowed to pass through up to the level of the token
   building.
                                                                         */

#define EOFch '\x1a'
#define BLANKSET "\r\n "
#define NUMERALSET "0123456789"
#define ALPHAUPSET "ABCDEFGHIJKLMNOPQRSTUVWXYZ?"
#define ALPHALWSET "abcdefghijklmnopqrstuvwxyz"
#define FLOATSET ".eE"

/* Definition for a pointer to the marker data structure */

#define LPMARKERTYPE struct MarkerType *

/* Definition for data structure to hold a marker */

#define MARKERTYPE struct MarkerType

struct MarkerType
       {
		int     RemMarker;        // The char position we can return to
	   };

/* Defintion for pointer to the LexBufferType */

//#define LPLEXBUFTYPE struct LexBufferType *

/* Definition for data structure to hold the lexical variables */

//#define LEXBUFTYPE struct LexBufferType
/*
struct LexBufferType
	{
     char    BlankSet[4];	   // Set for holding the skippable chars
     char    NumeralSet[20];   // Set for the numbers
     char    AlphaUpSet[50];   // Upper case alphabet set
     char    AlphaLwSet[50];   // Lower case alphabet set
     char    FloatSet[10];      // Float identifier's set
	 int     LexErrNo;         // Analyser error no
	 CString sStorage;		   // Pointer to the buffer that must be analysed
     int     EndMarker;		   // Last char in the Storage
     int     CurrMarker;       // Current char in the Storage
	 char    CurrCh;           // Current char being examined
     int     LineNo;           // What line no is being analysed
    };

*/
/* Definition for the tokentype */

enum TokenType 
             {
              eofT,integerliteral,stringliteral,binaryoperator,
		      unaryoperator,openroundbracket,closeroundbracket,
		      opensquarebracket,closesquarebracket,
		      logoperator,reloperator,delimiter,command
             };

/* Definition for a pointer type to a token */

#define LPTOKEN struct Token *

/* Definition for the token which contains the type of token and a buffer.
   The reason for having both is when a stringliteral token type is found
   the actual string may differ ie different variable names.              */

#define TOKEN struct Token

struct Token
		   {
		    enum TokenType TType;
		    char TValue[TOKENSIZE];
		   };

/* Error codes */

#define LAENoProblems 0    // No problems have been encountered
#define LAEFileOpen -1     // An Error has occurred while opening
				           // the file
#define LAENoReverse -2    // The file pointer cannot return past
				           // the beginning of the file
#define	LAEInvalidCh -3    // The analyser has encountered an
				           // invalid character in the file


class CLexical
{
   public:
			CLexical();
			~CLexical();
			void SetScanBuffer(LPSTR lpszScan);
			void SetLineNo(int NewLineNo);
			void GetToken(TOKEN * pToken);
			int  GetLineNo(void);
			void SetMarker(void);
			void SetMarker(LPMARKERTYPE lpMarker);
			void ResetMarker(void);
			void ResetMarker(LPMARKERTYPE lpMarker);
			void DecrementMarker(void);
			int  GetMarker(void);
			void IncrementMarker(void);
			void MoveMarker(int iNewMarker);
			int  Find(CString sFind,int iFlag);
			void Spaces(int t);
			//TOKEN m_Token;
			char CurrCh;
			void NextCh(void);
			void PrevCh(void);

   //private:
			CString		m_sBlankSet;	 // Set for holding the skippable chars
			CString		m_sNumeralSet;   // Set for the numbers
			CString		m_sAlphaUpSet;   // Upper case alphabet set
			CString		m_sAlphaLwSet;   // Lower case alphabet set
			CString		m_sFloatSet;     // Float identifier's set
			int			m_iLexErrNo;     // Analyser error no
			CString		m_sStorage;		 // Pointer to the buffer that must be analysed
			int			m_iEndMarker;    // Last char in the Storage
			int			m_iCurrMarker;   // Current char in the Storage
										 // Current char being examined
			int			m_iLineNo;       // What line no is being analysed
			MARKERTYPE m_Marker;
			
			// Functions for getting and setting the error codes encountered by the
			// lexical analyser.  

			void SetLexErrNo(int NewErr);
			int  GetLexErrNo(void);

			// Function to open the character data from the file and to get a 
			// character in the forward and reverse directions in the file                    

			
			
			
			int  GetBufferNdx(void);
			void SetBufferNdx(int NewNdx);


			// Functions to set and get the current line number, skip blanks ie (LF,
			// CR and spaces), get a token and to turn carriage returns on or off 
			void SkipBlanks(void);  
			void CLexical::SkipChar(char cChar);
			void GetTok(TOKEN * pToken);      
			// Parameters are CR_ON and CR_OFF
			void CarriageReturns(int t);   
			// Remove a char from a string
			void RemoveChr(CString * lpsConstStr,char rchr);         
			// Parameters are SP_ON and SP_OFF
			                

			
};

#endif
