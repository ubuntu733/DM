//=============================================================================
//
//   Copyright (c) 2000-2004, Carnegie Mellon University.  
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer. 
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//   This work was supported in part by funding from the Defense Advanced 
//   Research Projects Agency and the National Science Foundation of the 
//   United States of America, and the CMU Sphinx Speech Consortium.
//
//   THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
//   ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
//   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
//   NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//=============================================================================
 
//-----------------------------------------------------------------------------
// 
// UTILS.CPP - implementation of various functions useful throughout the 
//             components of the architecture
// 
// ----------------------------------------------------------------------------
// 
// BEFORE MAKING CHANGES TO THIS CODE, please read the appropriate 
// documentation, available in the Documentation folder. 
//
// ANY SIGNIFICANT CHANGES made should be reflected back in the documentation
// file(s)
//
// ANY CHANGES made (even small bug fixes, should be reflected in the history
// below, in reverse chronological order
// 
// HISTORY --------------------------------------------------------------------
//
//   [2006-01-24] (dbohus):  added support for constructing hashes from string
//                           descriptions and the other way around
//   [2005-02-08] (antoine): added the Sleep function that waits for a number
//                           milliseconds
//   [2004-12-24] (antoine): added a version of PartitionString with quotation.
//   [2004-04-01] (dbohus): corrected buffer overrun problem in FormatString
//   [2003-10-15] (dbohus): added ReplaceSubString for strings
//   [2003-10-08] (dbohus): added SplitOnFirst variant which takes into account
//                           quoted strings
//   [2003-04-23] (dbohus): added GetRandomIntID
//   [2002-06-26] (dbohus): Added a static buffer to be commonly used by string
//							 routines
//	 [2002-05-30] (agh):    Fixed PartitionString() so that it doesn't push
//                           empty strings into the resultant vector
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//	 [2002-04-28] (agh):    added functionality to Trim() functions and added 
//						     PartitionString()
//   [2001-12-29] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

//#include <windows.h>
#include "Utils.h"
#include <ctype.h>
// D: Static buffer commonly used by string routines
static char szBuffer[STRING_MAX];

//-----------------------------------------------------------------------------
// Functions for extending the string STL class with some desired but
// missing functionality
//-----------------------------------------------------------------------------
// D: implements the printf functionality in a string
string FormatString(const char *lpszFormat, ...) {
	va_list pArgs;
	va_start(pArgs, lpszFormat);

	// print into the buffer
	vsnprintf(szBuffer, STRING_MAX, lpszFormat, pArgs);
	
	// return a string object initialized from that buffer
	return((string)szBuffer);
}

// D: implements the + operator for strings
string operator + (const string sString1, const string sString2) {
	string sResult = sString1;
	sResult += sString2;
	return sResult;
}

// D: conversion from bool to string
string BoolToString(bool bBool) {
    return bBool?"true":"false";
}

// A: conversion from int to string
string IntToString(int iInt) {
    return FormatString("%d", iInt);
}

// D: conversion from float to string
string FloatToString(float fFloat) {
    return FormatString("%f", fFloat);
}

// A: tests if a string contains a number
bool IsANumber(string sStr) {
	size_t i = 0;

	// empty string -> not a number
	if (sStr.size() == 0)
		return false;

	// skip a potential +/- sign
	if ((sStr[0] == '-')||(sStr[0] == '+'))
		i++;

	// string just contains + or - -> not a number
	if (i == sStr.size())
		return false;

	// skip all digits
	while ((i < sStr.size()) && isdigit(sStr[i]))
		i++;

	// traversed the whole string -> number
	if (i == sStr.size())
		return true;
	// reached a decimal point
	else if (sStr[i] == '.') {
		i++;
		// skip digits after the decimal point
		while ((i < sStr.size()) && isdigit(sStr[i]))
			i++;
		// traversed the whole string -> number
		if (i == sStr.size())
			return true;
		// found something else than a digit -> not a number
		else 
			return false;
	} else
		// found something else than a digit or a decimal point -> not a number
		return false;
}

//自己实现_strypr
char *_strupr(char *str) {
  char *orign = str;
  for(; *str != '\0'; str++) {
    *str = toupper(*str);
  }
  return orign;
}
//自己实现_strlwr
char *_strlwr(char *str) {
  if (str==NULL) {
    return NULL;
  } else {
    char *p = str;
    while (*p != '\0') {
      if (*p>='A' && *p<='Z') {
        *p += 'a'-'A';
      }
      p++;
    }
    return str;
  }
}
// D: make a string uppercase
string ToUpperCase(string sString) {
	strcpy(szBuffer, sString.c_str());
	_strupr(szBuffer);
	return (string)szBuffer;
}

// D: makes a string lowercase
string ToLowerCase(string sString) {
	strcpy(szBuffer, sString.c_str());
	_strlwr(szBuffer);
	return (string)szBuffer;
}

// A: trim specified characters (default spaces) from the string on the left
string TrimLeft(string sString, char * pToTrim) {
	unsigned int i = 0;
	while(i < sString.length()) 
	{
		char * c = pToTrim;
		while ( *c != '\0' && *c != sString[i] )
			c++;
		if ( *c == '\0' )
			break;
		i++;
	}
	string sResult = sString.substr(i, sString.length() - i);
	return sResult;
}

// A: trim specified characters (default spaces) from the string on the right
string TrimRight(string sString, char * pToTrim) {
	int i = sString.length()-1;
	while((i >= 0) )
	{
		char * c = pToTrim;
		while ( *c != '\0' && *c != sString[i] )
			c++;
		if ( *c == '\0' )
			break;
		i--;
	}
	string sResult = sString.substr(0, i+1);
	return sResult;
}

// A: trim specified characters (default space) from the string at both ends
string Trim(string sString, char * pToTrim) {
	return TrimLeft(TrimRight(sString, pToTrim), pToTrim);
}

// D: extracts the first line of a string, and returns it (the string is 
//    chopped)
string ExtractFirstLine(string& rString) {
	string sResult;
	SplitOnFirst(rString, "\n", sResult, rString);
	return sResult;
}

// A: splits the string in 2 parts, around and not including the first 
//    occurence of any of a set of specified characters. Returns true on success
bool SplitOnFirst(string sOriginal, char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart) {

    int iCharPos = sOriginal.find_first_of(pDividers);
    if(iCharPos == -1) {
		// if the character was not found
		rsFirstPart = sOriginal;
		rsSecondPart = "";
		return false;
	} else {
		// if the character was found
		rsFirstPart = sOriginal.substr(0, iCharPos);
		rsSecondPart = sOriginal.substr(iCharPos + 1, 
										sOriginal.length() - iCharPos);
		return true;			
	}
}

// D: function similar to SplitOnFirst. It takes as an extra argument a char
// that act as quote characters, and therefore any occurence of the dividers 
// within that is not considered
bool SplitOnFirst(string sOriginal, char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart, char cQuote) {

    int i = 0;
    bool bWithinQuotes = false;
    int l = sOriginal.length();
    while(i < l) {
        // if we/re within quotes, just skip over everything until 
        // a new quote character is met
        if(bWithinQuotes) {
            while((sOriginal[i] != cQuote) && (i < l)) 
                i++;
            // check that we didn't reach the end
            if(i == l) {
                rsFirstPart = sOriginal;
                rsSecondPart = "";
                return false;
            }
            // o/w increment i;
            i++;
            // and set ourselves out of quotes
            bWithinQuotes = false;
        } else if(sOriginal[i] == cQuote) {
            // o/w if we just saw a quote, put ourselves in quote mode
            bWithinQuotes = true;
            // and move on
            i++;
        } else if(strchr(pDividers, sOriginal[i]) != NULL) {
            // o/w if we hit on one of the dividers
            rsFirstPart = sOriginal.substr(0, i);
            rsSecondPart = sOriginal.substr(i + 1, sOriginal.length() - i);
            return true;
        } else {
            i++;
        }
    }

    // if we got out of the loop, it means we reached the end without returning, 
    // so then there are no dividers
    rsFirstPart = sOriginal;
    rsSecondPart = "";
    return false;
}

// A: splits the string in 2 parts, around and not including the last
//    occurence of any of a set of specified characters. Returns true on success
bool SplitOnLast(string sOriginal, char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart) {
    int iCharPos = sOriginal.find_last_of(pDividers);
	if(iCharPos == -1) {
		// if the character was not found
		rsFirstPart = "";
		rsSecondPart = sOriginal;
		return false;
	} else {
		// if the character was found
		rsFirstPart = sOriginal.substr(0, iCharPos);
		rsSecondPart = sOriginal.substr(iCharPos + 1, 
										sOriginal.length() - iCharPos);
		return true;			
	}
}

// A: partitions a string into tokens divided by any of a set of specified
//    characters.
vector<string> PartitionString(string sString, char * pDividers) {
	vector<string> saResult;
	string sTemp;
	while (sString != "" )
	{
		SplitOnFirst(sString, pDividers, sTemp, sString);
		if ( sTemp != "" )
			saResult.push_back(sTemp);
	}
	return saResult;
} 

// A: partitions a string into tokens divided by any of a set of specified
//    characters.
vector<string> PartitionString(string sString, char * pDividers, char cQuote) {
	vector<string> saResult;
	string sTemp;
	while (sString != "" )
	{
		SplitOnFirst(sString, pDividers, sTemp, sString, cQuote);
		if ( sTemp != "" )
			saResult.push_back(sTemp);
	}
	return saResult;
} 

// D: replaces all the occurences of a substring in a string with another
//    substring
string ReplaceSubString(string sSource, string sToReplace, string sReplacement) {
    // the resulting string
    string sResult = sSource;
    int pos = 0;
    while((pos = sResult.find(sToReplace, pos)) >= 0) {
        sResult.replace(pos, sToReplace.length(), sReplacement);
		pos += sReplacement.length();
    }
    return sResult;
}

// D: find the corresponding closing quote
unsigned int FindClosingQuoteChar(string sString, unsigned int iStartPos, 
								  char cOpenQuote, char cCloseQuote) {

	unsigned int iOpenBraces = 1;	
	unsigned int iPos = iStartPos;
	while((iOpenBraces > 0) && (iPos < sString.length())) {
		if(sString[iPos] == cOpenQuote) 
			iOpenBraces++;
		else if(sString[iPos] == cCloseQuote)
			iOpenBraces--;
		iPos++;
	}

	// finally return the position
	return iPos;
}

// D: convert a STRING2STRING hash of attribute values into a string 
//    representation								  
string S2SHashToString(STRING2STRING s2sHash, string sSeparator, 
                       string sEquals) {
    // store the string
    string sResult = "";
	if (!s2sHash.empty()) {
		// go through the mapping and find something that matches the focus
		STRING2STRING::iterator iPtr;
		for(iPtr = s2sHash.begin(); iPtr != s2sHash.end(); iPtr++) {
			if (iPtr != s2sHash.begin()) sResult += sSeparator;
			sResult += iPtr->first + sEquals + iPtr->second;
		}
	}
    // finally, return the string
    return sResult;
}

// D: convert a string list of attribute values into a STRING2STRING 
//    representation
STRING2STRING StringToS2SHash(string sString, string sSeparator, 
                              string sEquals) {
    // extract the pairs
    TStringVector vsPairs = 
        PartitionString(sString, (char *)sSeparator.c_str());
    // form the hash
    STRING2STRING s2s;
    for(unsigned int i = 0; i < vsPairs.size(); i++) {
        string sAttr, sValue;
        SplitOnFirst(vsPairs[i], (char *)sEquals.c_str(), sAttr, sValue);        
        s2s.insert(STRING2STRING::value_type(Trim(sAttr), Trim(sValue)));
    }
    // finally, return the constructed hash
    return s2s;
}

// D: add to a S2S hash from a string description
void AppendToS2S(STRING2STRING& rs2sInto, STRING2STRING& rs2sFrom) {
    STRING2STRING::iterator iPtr;
    for(iPtr = rs2sFrom.begin(); iPtr != rs2sFrom.end(); iPtr++) 
        rs2sInto.insert(STRING2STRING::value_type(iPtr->first, iPtr->second));
}


//-----------------------------------------------------------------------------
// Functions for constructing unique IDs
//-----------------------------------------------------------------------------

static int __ID = 0;

// D: creates a uniques string ID
string GetUniqueStringID() {
	// make sure we're not starting from 0 again
	__ID++;
	assert( __ID );

	return (FormatString("%d", __ID - 1));
}

// D: creates a unique integer ID
int GetUniqueIntID() {
	// make sure we're not starting from 0 again
	__ID++;
	assert( __ID );
	return ( __ID - 1 );
}

// D: creates a random integer ID
int GetRandomIntID() {
    return rand();
}


//-----------------------------------------------------------------------------
// Time related functions and variables
//-----------------------------------------------------------------------------

// D: variable that holds the counter frequency
/*static LARGE_INTEGER iHighResCounterFrequency;*/

//// D: variable that holds the start-up counter value
//static LARGE_INTEGER iStartUpCounterValue;

//// D: variable that holds the system start-up and session start counter value
//static long long iStartUpTimestamp;
//static long long iSessionStartTimestamp;

//// D: variable that holds the start-up clock
//static _timeb tStartUpTime;

//// D: initializes the high resolution timer
//void InitializeHighResolutionTimer() {
    //// obtain the counter frequency (ticks per second)
    //QueryPerformanceFrequency(&iHighResCounterFrequency);

    //// obtain the current time (starting time) (actually sit in a loop
    //// until the current time changes so that we can avoid as much as 
    //// possible granularity effects
    //_timeb tTimeNow, tLastTime;
    //_ftime(&tTimeNow);
    //do {
        //tLastTime = tTimeNow;
        //_ftime(&tTimeNow);
    //} while(tTimeNow.millitm == tLastTime.millitm);
    //tStartUpTime = tTimeNow;

    //// and obtain the current initial counter time
    //QueryPerformanceCounter(&iStartUpCounterValue);

	//// converts it into milliseconds
	//iStartUpTimestamp = 1000*iStartUpCounterValue.QuadPart/
        //iHighResCounterFrequency.QuadPart;
//}

//// D: returns the time of day as a _timeb structure
//_timeb GetTime() {

    //// finally, return the updated time
	//return TimestampToTime(GetCurrentAbsoluteTimestamp());
//}

//// A: transforms a string into a time (as a _timeb structure)
//_int64 GetCurrentAbsoluteTimestamp() {

    //// now compute obtain the current value for the high resolution timer
    //LARGE_INTEGER iCurrentCounterValue;
    //QueryPerformanceCounter(&iCurrentCounterValue);

    //// compute the number of milliseconds elapsed
    //_int64 liTimestamp = 1000*iCurrentCounterValue.QuadPart/
        //iHighResCounterFrequency.QuadPart;

	//return liTimestamp;
//}

//// A: sets the session's initial absolute timestamp
//void SetSessionStartTimestamp(_int64 liTimestamp) {
	//iSessionStartTimestamp = liTimestamp;
//}

//// A: gets the session's initial absolute timestamp
//_int64 GetSessionStartTimestamp() {
	//return iSessionStartTimestamp;
//}

//// A: transforms a string into a time (as a _timeb structure)
//long GetSessionTimestamp(_int64 liTimestamp) {

	//long lTimestamp = (long)(liTimestamp-iSessionStartTimestamp);

	//return lTimestamp;
//}

//// A: transforms a string into a time (as a _timeb structure)
//long GetSessionTimestamp() {

	//return GetSessionTimestamp(GetCurrentAbsoluteTimestamp());
//}

//// A: converts an absolute timestamp into a _timeb structure
//_timeb TimestampToTime(_int64 liTimestamp) {
    //_timeb theTime;

	//_int64 liMillisecElapsed = liTimestamp-iStartUpTimestamp;

    //// now add those milliseconds to the time structure
    //theTime = tStartUpTime;
    //unsigned short ms = theTime.millitm;
    //theTime.millitm = (unsigned short)((ms + liMillisecElapsed) % 1000);
    //// and the seconds
    //theTime.time = (time_t)(theTime.time + 
        //(long int)((ms + liMillisecElapsed)/1000));

    //// finally, return the updated time
	//return(theTime);	
//}

//// D: transforms a time (as a _timeb structure) into a string
//string TimeToString(_timeb time) {
	//tm* tmTime;
	//string sResult;
	//// convert to local timezone
	//tmTime = localtime(&time.time);

	//// and format it as a string and return it
    //return FormatString("%02d:%02d:%02d.%03d", tmTime->tm_hour, 
		 //tmTime->tm_min, tmTime->tm_sec, time.millitm);
//}

//// A: converts an absolute timestamp into a string
//string TimestampToString(_int64 liTimestamp) {
	//return TimeToString(TimestampToTime(liTimestamp));
//}

//// D: returns the time as a long int. Used mainly for logging purposes
//string GetTimeAsString() {
	//return TimeToString(GetTime());
//}

//// A: pauses for a specified number of milliseconds
//void Sleep( int iDelay )
//{
   //int iGoal;
   //iGoal = iDelay + (int)clock();
   //while (iGoal > clock());
/*}*/
