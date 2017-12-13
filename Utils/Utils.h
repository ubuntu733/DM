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
// UTILS.H   - definition of various functions useful throughout the 
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
//   [2004-12-24] (antoine): Added a version of PartitionString with quotation.
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

#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__

// avoid warning messages which occur
// when including the some of the headers (at least this is the case with 
// VC++ 6.0
#pragma warning (disable:4786)
#pragma warning (disable:4710)
#pragma warning (disable:4702)
#pragma warning (disable:4514)
#pragma warning (push, 1)

// D: Other includes for generic headers
#include <stdio.h>
#include <sys/malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <errno.h>
// D: now include the standard template library headers
#include <map>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <queue>
#include <stack>
#include <iostream>

#pragma warning (pop)

using namespace std;

//-----------------------------------------------------------------------------
// Various defines used throughout the system
//-----------------------------------------------------------------------------
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define STRING_MAX 65535

// D: defines for common data types derived from STL
typedef vector <int> TIntVector;
typedef vector <string> TStringVector;
typedef map <string, string> STRING2STRING;
typedef map <string, float> STRING2FLOAT;
typedef map <string, STRING2FLOAT> STRING2STRING2FLOAT;


// D: defines for OAAS and GALAXYS (these defines are used to specify 
//    strings that appear only under an OAA or a GALAXY configuration (i.e. 
//    a common use is in the dialog task specification for specifying prompts)
/*#ifdef OAA*/
	//#define OAAS(STRING) STRING
	//#define GALS(STRING) 
//#endif

//#ifdef GALAXY
	//#define OAAS(STRING) 
	//#define GALS(STRING) STRING
//#endif
#define OAAS(STRING) STRING
#define GALS(STRING) STRING

//-----------------------------------------------------------------------------
// Functions for extending the string STL class with missing functionality
//-----------------------------------------------------------------------------
// D: implements the printf functionality in a string
string FormatString(const char *lpszFormat, ...);

// D: implements the + operator for strings
string operator + (const string sString1, const string sString2);

// D: conversion from bool to string
string BoolToString(bool bBool);

// A: conversion from int to string
string IntToString(int iInt);

// D: conversion from float to string
string FloatToString(float fFloat);

// A: tests if a string contains a number
bool IsANumber(string sStr);

// D: make a string uppercase
string ToUpperCase(string sString);

// D: make a string lowercase
string ToLowerCase(string sString);

// D: trim specified characters (default spaces) from the string on the left
string TrimLeft(string sString, char * pToTrim = " \n\t");

// D: trim specified characters (default spaces) from the string on the right
string TrimRight(string sString, char * pToTrim = " \n\t");

// D: trim specified characters (default space) from the string at both ends
string Trim(string sString, char * pToTrim = " \n\t");

// D: extracts the first line of a string, and returns it (the string is 
//    chopped)
string ExtractFirstLine(string& rString);

// D: splits the string in 2 parts, around and not including the first 
// occurence of any of a set of specified characters. Returns true on success
bool SplitOnFirst(string sOriginal, char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart);

// D: splits the string in 2 parts, around and not including the first 
// occurence of any of a set of specified characters, also taking into account
// a quote character. Returns true on success
bool SplitOnFirst(string sOriginal, char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart, char cQuote);

// D: splits the string in 2 parts, around and not including the last
// occurence of any of a set of specified characters. Returns true on success
bool SplitOnLast(string sOriginal, char* pDividers, 
				 string& rsFirstPart, string& rsSecondPart);

// D: partitions a string into tokens divided by any of a set of specified
//    characters.
vector<string> PartitionString(string sString, char * pDividers);

// D: partitions a string into tokens divided by any of a set of specified
//    characters. This takes into account a quote character.
vector<string> PartitionString(string sString, char * pDividers, char cQuote);

// D: replaces a substring ocurring in a string with another string
string ReplaceSubString(string sSource, string sToReplace, string sReplacement=string());

// D: find the corresponding closing quote
unsigned int FindClosingQuoteChar(string sString, unsigned int iStartPos, 
								  char cOpenQuote, char cCloseQuote);
								  
// D: convert a STRING2STRING hash of attribute values into a string 
//    representation								  
string S2SHashToString(STRING2STRING s2sHash, 
                       string sSeparator = ", ", string sEquals = " = ");

// D: convert a string list of attribute values into a STRING2STRING 
//    representation
STRING2STRING StringToS2SHash(string sString, string sSeparator = ",", 
                              string sEquals = "=");
                              
// D: add to a S2S hash from a string description
void AppendToS2S(STRING2STRING& rs2sInto, STRING2STRING& rs2sFrom);

//-----------------------------------------------------------------------------
// Functions for constructing unique IDs
//-----------------------------------------------------------------------------

// D: creates a uniques string ID
string GetUniqueStringID();

// D: creates a unique integer ID
int GetUniqueIntID();

// D: creates a random integer ID
int GetRandomIntID();

//-----------------------------------------------------------------------------
// Time related functions 
//-----------------------------------------------------------------------------

// D: initializes the high resolution timer used for logging
void InitializeHighResolutionTimer();

// D: returns the time of day as a _timeb structure
timeb GetTime();

// A: returns the current value of the internal counter
long long GetCurrentAbsoluteTimestamp();

// A: sets the session's initial absolute timestamp
void SetSessionStartTimestamp(long long liTimestamp);

// A: gets the session's initial absolute timestamp
long long GetSessionStartTimestamp();

// A: converts an absolute timestamp into a session timestamp
//    (by substracting the session start timestamp)
long GetSessionTimestamp(long long liTimestamp);

// A: returns the current session timestamp
long GetSessionTimestamp();

// A: converts an absolute timestamp into a _timeb structure
timeb TimestampToTime(long long liTimestamp);

// A: converts an absolute timestamp into a string
string TimestampToString(long long liTimestamp);

// D: transforms a time (as a _timeb structure) into a string
string TimeToString(timeb time);

// D: returns the time of day as a string. Used mainly for logging purposes
string GetTimeAsString();

// A: pauses for a specified number of milliseconds
void Sleep(int iDelay);

#endif // __UTILS_H__
