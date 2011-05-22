/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Module  : Lexical Analyser                                     */
/*                                                                */
/* Desc    : This file contains functions for lexically analyzing */
/*           the a file and determining the tokens the file       */
/*           consists of.										  */
/* 																  */
/* Author  : Shawn 												  */
/*																  */
/* Date    : 14/03/1995											  */
/*																  */
/* History : 1. Creation of the file                              */
/*           2. Moved the global analyser variables into the      */
/*              LexBufferType so that this analyser can be used   */
/*              in a multi-tasking environment.                   */
/*                                                                */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */ 


/* ---------------------------------------------------------------------

   The lexical analyzer consists of three layers :-

                        -----------------------------

                             TOKENS

                        -----------------------------

                             CHARACTERS

                        -----------------------------

                             FILE AND BUFFER

                        -----------------------------                        

   Tokens
   ------

   The Tokens layer builds up a token from a sequence of characters.
   It deals with things such as delimiters (ie commas), 
   integerliteral (ie 23) etc. 

   Characters
   ----------

   The Characters layer deals with obtaining characters out of the buffers
   to give to the Tokens layer. 

   File and Buffer
   ----------------

   The File and Buffer layer opens the file if it can and loads the buffer.
   When the buffer is empty the nect buffer of characters is read from 
   the file.  If the end of the file is reached it will automatically 
   close the file.  The reason for using the buffer is minimize disk
   access and thus to speed up the analysis.

   --------------------------------------------------------------------- */


//#include <windows.h>
//#include <stdio.h>
#include <wfc.h>

// Application specific includes

#include <CLexical.h>

/* Definition to determine if an error must be signalled for illegal
   characters which the lexical analyser does not understand.          */

#define _UNALLOWCHAR_



CLexical::CLexical()
{
	 // Configure the sets for use by the analyser
	 //m_sBlankSet   = BLANKSET;
	 //m_sNumeralSet = NUMERALSET;
	 //m_sAlphaUpSet = ALPHAUPSET;
	 //m_sAlphaLwSet = ALPHALWSET;
	 //m_sFloatSet   = FLOATSET;

	 // Reset the current index to the start of file
	 //m_iCurrMarker = 0;  
	 //m_Marker.RemMarker = 0;

	 // Clear the error code flag
	 //m_iLexErrNo = LAENoProblems;
	m_iLineNo = 0;

}

CLexical::~CLexical()
{
}

/* ------------------------------------------------------------
   These functions enable carriage returns or spaces character 
   to pass through the skipblanks function to the token 
   building level.

   Carriage returns
   ----------------

   This feature is useful when columns of data are comma 
   delimited except for the last column which is delimited from
   first column by a carriage return.

   Spaces
   ------

   This feature is useful when scanning a static string such
   as "Fred was here" to produce one token(Fred was here) 
   instead (Fred)(was)(here) because then number of spaces
   between the words would be lost.
   ------------------------------------------------------------   */

/* ------------------------------------------------------------
   The RemoveChar function removes a single character from the 
   the input string.
   ------------------------------------------------------------ */

void CLexical::RemoveChr(CString * lpsConstStr,char rchr)
{
 int i;//,j;
 //char q;
 CString sTemp;

 //i = 0;
 //j = 0;

for (i=0; i < lpsConstStr->GetLength();i++)
{
	if (lpsConstStr->GetAt(i) == rchr)
	{
		sTemp += lpsConstStr->Left(i);
		*lpsConstStr = lpsConstStr->Right(lpsConstStr->GetLength() - i - 1);
	}
}
sTemp += *lpsConstStr;
*lpsConstStr = sTemp;
/*
 while (ConstStr[i] != '\0')
   if (ConstStr[i] != rchr)
      {
       q = ConstStr[i];
       ConstStr[j] = q;
       i++;
       j++; 
      }
   else
      {
       i++;
      } 
 ConstStr[j] = '\0';
 */
}

/* -------------------------------------------------------------
   Function to allow sChar through SkipBlanks
   ------------------------------------------------------------- */

void CLexical::SkipChar(char cChar)
{
 //switch (t)
 // {
 //  case CR_OFF : if (m_sBlankSet.Find(sChar) == -1)
 //                   m_sBlankSet += sChar;
 //                break;
 //  case CR_ON  : 
	if (m_sBlankSet.Find(cChar) == -1)
		RemoveChr(&m_sBlankSet,cChar);
 //                break;
 // }
}

/* -------------------------------------------------------------
   Function to allow carriage returns through SkipBlanks
   ------------------------------------------------------------- */

void CLexical::CarriageReturns(int t)
{
 switch (t)
  {
   case CR_OFF : if (m_sBlankSet.Find('\r') == -1)
                    m_sBlankSet +="\r";
                 break;
   case CR_ON  : if (m_sBlankSet.Find('\r') == -1)
                    RemoveChr(&m_sBlankSet,'\r');
                 break;
  }
}

/* ------------------------------------------------------------------
   Function to allow spaces through SkipBlanks
   SP_OFF - Don't allow spaces through
   SP_ON  - Do allow spaces through
   ------------------------------------------------------------------ */

void CLexical::Spaces(int t) 
{
 switch (t)//CString
  {
   case SP_OFF  : if (m_sBlankSet.Find(' ') == -1)
                     m_sBlankSet +=" ";
							m_sAlphaUpSet = m_sAlphaUpSet.Left(m_sAlphaUpSet.GetLength()-1);
                  break;
   case SP_ON   : if (m_sBlankSet.Find(' ') != -1)
                     RemoveChr(&m_sBlankSet,' ');
							m_sAlphaUpSet += ' ';
                  break; 
  }
}

/* -----------------------------------------------------------
   The Error handling functions allow a error condition to be
   set and retrieved.  If the error condition is non zero then
   no more tokens will be available.
   ----------------------------------------------------------- */
void CLexical::SetLexErrNo(int NewErr)
{
 m_iLexErrNo = NewErr;
}

int CLexical::GetLexErrNo(void)
{
 return m_iLexErrNo;
}


/* ----------------------------------------------------------------------
   Functions for opening the character stream, getting the next character
   and the previous character.
   ---------------------------------------------------------------------- */

void CLexical::SetScanBuffer(LPSTR lpszScan)
{
		 // Configure the sets for use by the analyser
	 m_sBlankSet   = BLANKSET;
	 m_sNumeralSet = NUMERALSET;
	 m_sAlphaUpSet = ALPHAUPSET;
	 m_sAlphaLwSet = ALPHALWSET;
	 m_sFloatSet   = FLOATSET;

	 // Reset the current index to the start of file
	 m_iCurrMarker = 0;  
	 m_Marker.RemMarker = 0;
	 m_iLineNo = 0;

	 // Clear the error code flag
	 m_iLexErrNo = LAENoProblems;

 // Get a copy of the pointer to the buffer holding the string to parse
 m_sStorage = lpszScan;

 // Set the endmarker to the number of chars read
 m_iEndMarker = m_sStorage.GetLength();//strlen(lpszScan);    
 
 // Reset the current index to the start of file
 m_iCurrMarker = 0;  
 m_Marker.RemMarker = 0;

 // Clear the error code flag
 m_iLexErrNo = LAENoProblems;
}

void CLexical::SetMarker(void)
{
 m_Marker.RemMarker = m_iCurrMarker;
}

void CLexical::SetMarker(LPMARKERTYPE lpMarker)
{
 lpMarker->RemMarker = m_iCurrMarker;
}

void CLexical::MoveMarker(int iNewMarker)
{
	m_iCurrMarker = iNewMarker;
}

void CLexical::ResetMarker(void)
{
 m_iCurrMarker = m_Marker.RemMarker;
}

void CLexical::DecrementMarker(void)
{
 m_iCurrMarker--;
}

void CLexical::IncrementMarker(void)
{
 m_iCurrMarker++;
}

int CLexical::GetMarker(void)
{
 return m_iCurrMarker;
}

void CLexical::ResetMarker(LPMARKERTYPE lpMarker)
{
 m_iCurrMarker = lpMarker->RemMarker;
}

void CLexical::NextCh(void)
{
 // if the last char in the buffer is reached then get the next buffer

 if (m_iCurrMarker >= m_iEndMarker)   
    {
	 CurrCh = EOFch;  // EOF char ASCII(26)
	 m_iCurrMarker++; // ElectroDynamics - To catch ends of strings properly
    }
 else // Assign the next char as the CurrMarker < EndMarker
   {
    CurrCh = m_sStorage.GetAt(m_iCurrMarker++);
   }
}


int CLexical::GetBufferNdx(void)
{
 return m_iCurrMarker;
}

void CLexical::SetBufferNdx(int NewNdx)
{
 m_iCurrMarker = NewNdx;
}


void CLexical::PrevCh(void)
{
 // If the current index is at the beginning of the buffer 
 if (m_iCurrMarker == 0)   
    {                  // then read the previous buffer
	 CurrCh = m_sStorage.GetAt(m_iCurrMarker);  // May have be removed
    }
 else // else current index is not at the beginning of the buffer
   {
    m_iCurrMarker--;
	// Trap end of string conditions
	if (m_iCurrMarker < m_iEndMarker )
		CurrCh = m_sStorage.GetAt(m_iCurrMarker);       // May have be removed
   }
}

/* ----------------------------------------------------------------------
   Functions for setting and getting the lineno
   ---------------------------------------------------------------------- */

void CLexical::SetLineNo(int NewLineNo)
{
 m_iLineNo = NewLineNo;
}

int CLexical::GetLineNo(void)
{
 return m_iLineNo;
}

/* ----------------------------------------------------------------------
   Function for skipping characters which are not required by the token
   builder.  These characters can be be LF,CR and an ordinary space.  If
   the CarriageReturns is called with CR_ON then the BlankSet will only
   contain LF and a space.

   The line number count is incremented on a CR.
   ---------------------------------------------------------------------- */

void CLexical::SkipBlanks(void )
{
 while (m_sBlankSet.Find(CurrCh) != -1)
   {
    if (CurrCh == '\r')            // If the current char is CR then
       SetLineNo(GetLineNo()+1);   // increment the line no
    NextCh();
   }
}

/* ----------------------------------------------------------------------
  This function returns either the start position or the end position 
  of the search string..
  ---------------------------------------------------------------------- */
int CLexical::Find(CString sFind,int iFlag)
{
int iPos;
	iPos = m_sStorage.Find((LPCSTR)sFind);
	if(iPos > -1)
	{
		if (iFlag)
			m_iCurrMarker = iPos + sFind.GetLength();
		else
			m_iCurrMarker = iPos;
		return iPos;
	}
	else
		return -1;
	return -1;
}

/* ----------------------------------------------------------------------
   This function GetTok builds a token.
   ---------------------------------------------------------------------- */

void CLexical::GetTok(TOKEN * pToken)
{
 int tempint;
 char TwoBuf[2];

 NextCh();                       // get the next char and reset token
 strcpy(pToken->TValue,"");
 SkipBlanks();                   // skip all the blanks
 switch (CurrCh)
  {
   case '{' : tempint = 1;      // What is between {...}
	          do
		        {
		         if (CurrCh == '{')
		            tempint++;
		         if (CurrCh == '}')
		            tempint--;
		        } while (tempint != 0);
	          GetTok(pToken);
	          break;

   case EOFch : strcpy(pToken->TValue,"ctrlZ");  //EOF 
		        pToken->TType = eofT;
		        break;

   case '(' : strcpy(pToken->TValue,"(");
	          pToken->TType = openroundbracket;
	          break;
   case ')' : strcpy(pToken->TValue,")");
	          pToken->TType = closeroundbracket;
	          break;
   case '[' : strcpy(pToken->TValue,"[");
	          pToken->TType = opensquarebracket;
	          break;
   case ']' : strcpy(pToken->TValue,"]");
	          pToken->TType = closesquarebracket;
	          break;
   case '=' : strcpy(pToken->TValue,"=");
	          pToken->TType = reloperator;
			  NextCh();
	          switch (CurrCh)
	           {
		        case '=' : strcat(pToken->TValue,"=");
			               break;
		        default  : PrevCh();
	           }
	          break;
   case '<' : strcpy(pToken->TValue,"<");  // relational operators 
	          pToken->TType = reloperator;   // <= or < or <> 
	          NextCh();
	          switch (CurrCh)
	           {
		        case '=' : strcat(pToken->TValue,"=");
			               break;
		        case '>' : strcat(pToken->TValue,">");
			               break;
		        default  : PrevCh();
	           }
	          break;

   case '>' : strcpy(pToken->TValue,">");   // relational operator
	          pToken->TType = reloperator;    // > or >=
	          NextCh();
	          switch (CurrCh)
	           {
		        case '=' : strcat(pToken->TValue,"=");
			               break;
		        default  : PrevCh();
	           }
	          break;
   /*	 ElectroDynamics Removed so as to pass ' as a normal SQL delimiter
   case '\'': strcpy(pToken->TValue,"");
	          while (lpLexBuf->CurrCh != '\r')
		       {
		        sprintf(TwoBuf,"%c",lpLexBuf->CurrCh);
		        strcat(pToken->TValue,TwoBuf);
		        NextCh(lpLexBuf);
		       }
	         PrevCh(lpLexBuf);
	         pToken->TType = stringliteral;
	         break;
   */
   case '\r': pToken->TType = delimiter;
	          strcpy(pToken->TValue,"CR");
	          break;
   case '*' :
   case '-' :
   case '+' :
   case '/' : 
              
	          sprintf(pToken->TValue,"%c",CurrCh);
	          pToken->TType = binaryoperator; 
              break;
   case '\'':
   case '"' :
   case ' ' :
   case ',' :
   case ';' :
   case '#' :
   case ':' :
   case '.' :
   case '@' :
   case '\\':
   case '_' :
	          
	          sprintf(pToken->TValue,"%c",CurrCh);
	          pToken->TType = delimiter;
	          break;
   
   case '0' :
   case '1' :
   case '2' :
   case '3' :
   case '4' :
   case '5' :
   case '6' :
   case '7' :            // Build an integer literal
   case '8' :
   case '9' : strcpy(pToken->TValue,"");
	          if (CurrCh == '-')
		      {
		       strcpy(pToken->TValue,"-");
		       NextCh();
		      }
	          while ((m_sNumeralSet.Find(CurrCh) != -1) ||
		             (m_sFloatSet.Find(CurrCh) != -1))
		       {
		        sprintf(TwoBuf,"%c",CurrCh);
		        strcat(pToken->TValue,TwoBuf);
		        NextCh();
		       }
	          PrevCh();
	          pToken->TType = integerliteral;
	          break;
   default : if ((m_sAlphaUpSet.Find(CurrCh) != -1) ||
		         (m_sAlphaLwSet.Find(CurrCh) != -1))
		        {
		         strcpy(pToken->TValue,"");
		         while ((m_sAlphaUpSet.Find(CurrCh) != -1) ||
			            (m_sAlphaLwSet.Find(CurrCh) != -1) ||
			            (m_sNumeralSet.Find(CurrCh) != -1))
		          {
		           sprintf(TwoBuf,"%c",CurrCh);
		           if (strlen(pToken->TValue) <
			          (sizeof(pToken->TValue) - 1))
		              strcat(pToken->TValue,TwoBuf);
		           NextCh();
					  //SkipBlanks(); 
		          }
				 pToken->TType = stringliteral;
		         PrevCh();
		        }
	         else
	            {
	             #ifndef _UNALLOWCHAR_  // If any chars don't match the prev
				 #ifndef _WINDOWS
	             printf("Unallowable char %c = %i\n",CurrCh,CurrCh);
				 #endif
	             #endif
		         SetLexErrNo(LAEInvalidCh);  // then set the error
	             }
	         break;
  }
}

/* ----------------------------------------------------------------------
   This function requests the token builder to build a token if no 
   previous errors have occurred.  If an error has occurred then an 
   end of file token and type is returned.
   ---------------------------------------------------------------------- */

void CLexical::GetToken(TOKEN * pToken)
{
 if (GetLexErrNo() == LAENoProblems)   // If no previous problems
    GetTok(pToken);                // get the next token
 else
   {
    strcpy(pToken->TValue,"ctrlZ");         // else signal End of File
    pToken->TType = eofT;
   }
}

/* ===================================================================== */


