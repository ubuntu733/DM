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
// LOG.CPP    - implementation of logging support
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
//   [2004-03-15] (dbohus):  fixed bug in logging long strings to the screen
//   [2003-05-13] (dbohus):  changed InitializeLogging function to work with 
//                            the new configuration parameters
//   [2003-04-09] (dbohus, 
//                 antoine): added dialog task logging stream
//   [2002-12-05] (dbohus):  added support for colored logging
//   [2002-10-20] (dbohus):  changed initialize function so that streams are 
//                            added according to specified parameters
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-21] (dbohus):  added support for multiple logging streams
//   [2002-01-02] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

/*#include "DMInterfaces/DMInterface.h"*/
//#include "DMBridge.h"
#include "Log.h"

//-----------------------------------------------------------------------------
// D: Exit on fatal error flag
//-----------------------------------------------------------------------------

auto my_logger = spd::basic_logger_mt("basic_logger","/home/alex/c++/DM/logs/log.txt");
bool bExitOnFatalError; 

//-----------------------------------------------------------------------------
// Logging streams: definition and functions
//-----------------------------------------------------------------------------

string sAllLoggingStreams = (string)CORETHREAD_STREAM + ";" + 
							(string)REGISTRY_STREAM + ";" + 
							(string)DMCORE_STREAM + ";" +
							(string)EXPECTATIONAGENDA_STREAM + ";" +
							(string)TRAFFICMANAGER_STREAM + ";" +
							(string)TRAFFICMANAGERDUMP_STREAM + ";" + 
							(string)INPUTMANAGER_STREAM + ";" + 
							(string)OUTPUTMANAGER_STREAM + ";" + 
							(string)OUTPUTHISTORY_STREAM + ";" + 
							(string)STATEMANAGER_STREAM + ";" + 
                            (string)DTTMANAGER_STREAM + ";" + 
                            (string)GROUNDINGMANAGER_STREAM + ";" + 
                            (string)GROUNDINGMODELX_STREAM +";" +
                            (string)DIALOGTASK_STREAM + ";" +
                            (string)CONCEPT_STREAM + ";" + 
							(string)BELIEFUPDATING_STREAM;


// D: Add a logging stream
void AddLoggingStream(string sStreamName, string sColorMap, 
                      bool bDisplayed, bool bEnabled) {
	/*TLoggingStreamsHash::iterator iPtr;*/

	//if((iPtr = lshLogStreams.find(sStreamName)) == lshLogStreams.end()) {
		//// if the stream is not already in there
		//TLoggingStream lsLogStream;
		//lsLogStream.bDisplayed = bDisplayed;
		//lsLogStream.bEnabled = bEnabled;
        //if(sColorMap.empty()) {
            //// if no color map specified, use white
            //lsLogStream.iColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        //} else {           
            //// sColorMap should be RGBI
            //lsLogStream.iColor = 0;
            //if(sColorMap[0] == '1')
                //lsLogStream.iColor |= FOREGROUND_RED;
            //if(sColorMap[1] == '1')
                //lsLogStream.iColor |= FOREGROUND_GREEN;
            //if(sColorMap[2] == '1')
                //lsLogStream.iColor |= FOREGROUND_BLUE;
            //if(sColorMap[3] == '1')
                //lsLogStream.iColor |= FOREGROUND_INTENSITY;
        //}

		//lshLogStreams.insert(TLoggingStreamsHash::value_type(sStreamName, 
															 //lsLogStream));
	//} else {
		//// if the stream is already in there, just set its properties
		//iPtr->second.bDisplayed = bDisplayed;
		//iPtr->second.bEnabled = bEnabled;
        //int iColor;
        //if(!sColorMap.empty()) {
            //// sColorMap should be RGBI
            //iColor = 0;
            //if(sColorMap[0] == '1')
                //iColor |= FOREGROUND_RED;
            //if(sColorMap[1] == '1')
                //iColor |= FOREGROUND_GREEN;
            //if(sColorMap[2] == '1')
                //iColor |= FOREGROUND_BLUE;
            //if(sColorMap[3] == '1')
                //iColor |= FOREGROUND_INTENSITY;
            //iPtr->second.iColor = iColor;
        //}
    /*}*/
}

// D: Delete a logging stream
void DeleteLoggingStream(string sStreamName) {
	TLoggingStreamsHash::iterator iPtr;

	// if the stream is not defined, return
	if((iPtr = lshLogStreams.find(sStreamName)) == lshLogStreams.end()) 
		return;

	// otherwise delete it, then return
	lshLogStreams.erase(iPtr);
}

// D: Enable a logging stream
void EnableLoggingStream(string sStreamName) {
	TLoggingStreamsHash::iterator iPtr;

	// if the stream is not defined, return
	if((iPtr = lshLogStreams.find(sStreamName)) == lshLogStreams.end()) 
		return;

	// otherwise set it to enabled, and return
	iPtr->second.bEnabled = true;
}

// D: Disable a logging stream
void DisableLoggingStream(string sStreamName) {
	TLoggingStreamsHash::iterator iPtr;

	// if the stream is not defined, return
	if((iPtr = lshLogStreams.find(sStreamName)) == lshLogStreams.end()) 
		return;

	// otherwise set it to disabled, and return
	iPtr->second.bEnabled = false;
}

// D: Enable all logging streams
void EnableAllLoggingStreams() {
	TLoggingStreamsHash::iterator iPtr;
	
	// go through all the streams and enable them all
	for(iPtr = lshLogStreams.begin(); iPtr != lshLogStreams.end(); iPtr++) 
		iPtr->second.bEnabled = true;
}

// D: Disable all logging streams
void DisableAllLoggingStreams() {
	TLoggingStreamsHash::iterator iPtr;
	
	// go through all the streams and enable them all
	for(iPtr = lshLogStreams.begin(); iPtr != lshLogStreams.end(); iPtr++) 
		iPtr->second.bEnabled = false;
}


//-----------------------------------------------------------------------------
// Logging defines, levels, function etc
//-----------------------------------------------------------------------------

// D: the log function (works like a printf)
int Log(string sLoggingStream, const char* lpszFormat, ... ) {
  va_list pArgs;
  int returnCode = 0;

  // check if the logging stream exists
  //TLoggingStreamsHash::iterator iPtr;
  // if the stream is not defined, return
  //if((iPtr = lshLogStreams.find(sLoggingStream)) == lshLogStreams.end()) {
    //Log(WARNING_STREAM, "Attempt to log on undefined logging stream: " + 
      //sLoggingStream);
    //return 0;
  //}

  // get the current time
  //_int64 liTimestamp = GetCurrentAbsoluteTimestamp();

  // put it on the screen, if the stream is to be displayed
  va_start(pArgs, lpszFormat);
  //if(iPtr->second.bDisplayed) {
        // construct the string
  //char lpszBuffer1[STRING_MAX];
  char lpszBuffer[STRING_MAX];

 /** snprintf(lpszBuffer1, STRING_MAX-1, "[%s@%s (%d)] ", sLoggingStream.c_str(), 
                 TimestampToString(liTimestamp).c_str(), 
                 GetSessionTimestamp(liTimestamp));
                 **/
  vsnprintf(lpszBuffer, STRING_MAX-1, lpszFormat, pArgs);
  //strncat(lpszBuffer1, lpszBuffer2, STRING_MAX-2-strlen(lpszBuffer1));
  strncat(lpszBuffer, "\n", STRING_MAX-1-strlen(lpszBuffer));
        // display it
        //SetConsoleTextAttribute(hStdOutput, (WORD)lshLogStreams[sLoggingStream].iColor);
        //DWORD cWritten;
        //WriteFile(hStdOutput, lpszBuffer1, lstrlen(lpszBuffer1), &cWritten, NULL);
  if (sLoggingStream == "WAR"){
    my_logger->warn(static_cast<string>(lpszBuffer));
  } else if (sLoggingStream == "ERR") {
    my_logger->error(static_cast<string>(lpszBuffer));
  } else if (sLoggingStream == "FATAL_ERROR"){
    my_logger->error(static_cast<string>(lpszBuffer));
  }else {
    string output = sLoggingStream + "|" + static_cast<string>(lpszBuffer);
    my_logger->info(output);
  }
   //my_logger->info(static_cast<string>(lpszBuffer1));

  // also send it to the file if logging is initialized
  // and the stream is to enabled for logging
  /*if(fileLog && iPtr->second.bEnabled) {*/

    //// then do the printf into the file
    //fprintf(fileLog, "[%s@%s (%d)] ", sLoggingStream.c_str(), 
                 //TimestampToString(liTimestamp).c_str(), 
                 //GetSessionTimestamp(liTimestamp));
    //returnCode = vfprintf(fileLog, lpszFormat, pArgs);
    //fprintf(fileLog, "\n");
    /*fflush(fileLog);*/
  //}

  return returnCode;
}

// D: another variant of the log function, just marshalls info
//    towards the basic one
int Log(string sLoggingStream, string sMessage) {
	return Log(sLoggingStream, sMessage.c_str());
}

//-----------------------------------------------------------------------------
// Logging initialization and closing functions
//-----------------------------------------------------------------------------
// D: logging initialization
void InitializeLogging(string sALogFolder, string sALogPrefix, string sALogFilename, 
                       string sLoggedStreamsList, string sDisplayedStreamsList,
                       string sAExitOnFatalError) {

    // Set the exit on fatal error flag                           
  /*  bExitOnFatalError = (sAExitOnFatalError == "true");*/

	//// Add the basic logging streams
	//AddLoggingStream(WARNING_STREAM, "1001");
    //AddLoggingStream(ERROR_STREAM, "1001");
	//AddLoggingStream(FATALERROR_STREAM, "1001");

    //// Add all the default streams, but don't enable them yet
    //vector<string> vsStreams = 
		//PartitionString(sAllLoggingStreams, ";");
    //for(unsigned int i=0; i < vsStreams.size(); i++) {
        //string sName, sColorMap;
        //SplitOnFirst(Trim(vsStreams[i]), ":", sName, sColorMap);
        //AddLoggingStream(sName, sColorMap, false, false);
    //}

	//// Enable the streams from the description
    //vector<string> vsEnabledStreams = 
        //PartitionString(sLoggedStreamsList, ";");
    //for(unsigned int i=0; i < vsEnabledStreams.size(); i++) {
        //string sName, sColorMap;
        //SplitOnFirst(Trim(vsEnabledStreams[i]), ":", sName, sColorMap);
        //AddLoggingStream(sName, sColorMap, false, true);
    //}

    //// And display the ones that are in the displayed list
    //vector<string> vsDisplayedStreams = 
        //PartitionString(sDisplayedStreamsList, ";");
    //for(unsigned int i=0; i < vsDisplayedStreams.size(); i++) {
        //string sName, sColorMap;
        //SplitOnFirst(Trim(vsDisplayedStreams[i]), ":", sName, sColorMap);
        //AddLoggingStream(sName, sColorMap, true, true);
    //}


	//// try to open the log file
	//sLogFolder = sALogFolder + "\\";
	//sLogFilename = sALogPrefix + sALogFilename;
	//string sLogPath = sLogFolder + sLogFilename;
	//fileLog=fopen(sLogPath.c_str(),"w");

	//// if it's not possible in the location indicated, try in the current 
	//// folder
    //if(!fileLog) {
        //Warning(FormatString("ERROR CREATING LOGFILE %s (%s)\n", sLogPath.c_str(), 
            //strerror(errno)));
        //fileLog=fopen(("./" + sLogFilename).c_str(),"w");
    /*}*/
}

// D: close logging
void TerminateLogging() {
 /* if(fileLog) */
		/*fclose(fileLog);*/
}


//-----------------------------------------------------------------------------
// Error handling functions 
//   these functions are basically stubs for the Log function
//-----------------------------------------------------------------------------

// D: Warning
void __Warning(const char* lpszWarning, char* lpszFile, int iLine) {
  Log(WARNING_STREAM, "%s <file %s, line %d>.", lpszWarning, lpszFile, iLine);
}

// D: NonFatal error. 
void __Error(const char* lpszError, char* lpszFile, int iLine) {
  Log(ERROR_STREAM, "%s <file %s, line %d>.", lpszError, lpszFile, iLine);
}

// D: Fatal error. Quit after dealing with it
#pragma warning (disable:4127)
void __FatalError(const char* lpszError, char* lpszFile, int iLine) {
 // Log(FATALERROR_STREAM, "%s <file %s, line %d>.", lpszError, lpszFile, iLine);
  //  if(!bExitOnFatalError) {
   //     while(1);   
   // }
  //exit(1);
}
#pragma warning (default:4127)
