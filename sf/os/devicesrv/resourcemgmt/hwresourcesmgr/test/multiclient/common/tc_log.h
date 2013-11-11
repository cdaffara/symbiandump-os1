/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


 
#ifndef TC_LOG_H
#define TC_LOG_H

//Includes

#include "tc_config.h"

// This class implements writing results to log files
class Log : public CBase
{
public:

	// this enumeration defines priorities of printed lines.
    enum Priority
    {

        ZERO = 0,
        PASS,              // priority of passed line
        NOTIFY,            // priority of notification
		SCHEDULER,         // priority of scheduler line
		MONITOR,           // priority of monitor
		WARNING,           // priority of warning line
        FAIL,              // priority of failed line
        COMMENT,           // priority of comment line
        SUMMARY,           // priority of summary line
		SESSION_MARK,      // priority of session line
		BEGIN_TESTCASE,    // priority of beginning test case line
		END_TESTCASE,      // priority of ending test case line
    };


	Log();
	virtual ~Log();

	void OpenL(const TDesC &aFileName, TBool aBatch, TBool aEchoOn, TBool aTimestamp, Priority aPriorityFilter);


	// print EPOC descriptors
	virtual void Printf(Priority priority, TRefByValue<const TDesC> aFmt, ...);

	// some TSY wrappers use this function jlof 13-12-2001
	virtual void Printf(TRefByValue<const TDesC>, ...);

	// write descriptor
	virtual void Write(Priority priority, const TDesC& aStr);

	// print C -strings
	virtual void Printf(Priority priority, const char *fmt, ...);

	// prints summary report
	virtual void SummaryReport(const TDesC& aTestCaseName);

	// used to implement pause stuff..
	virtual void  RequestKeyPress();
	virtual TBool ReceiveKeyPress();
	virtual void  WaitKeyPress();

	// multiple choice
	virtual TInt MultipleChoice(TInt aMin, TInt aMax);

	// set console title
	virtual void SetTitle(const TDesC& aTitle);

private:

    CConsoleBase*  iConsole;
	TRequestStatus iConsoleKeyRequest;

	// if true, dont ask anything from user
	TBool iBatch;

	// if true write timestamps to log
	TBool iWriteTimestamp;

	// log message priority filter
    Priority iPriorityFilter;

	// TBuf for log functions
	TBuf<MAX_LEN*2> iStr;

	// output to file
	TBuf8<MAX_LEN*2> iOut8;

	// log output buffer
	TBuf<MAX_LEN*2> iLog_bufout; 

	// TUint8 buffer
	TUint8 iUint8[MAX_LEN*2];

    // current time to log
	TTime iTime;


	// file pointer for log file
	void* iFile; // using void* insteand of file to avoid including stdio.h

	// helper class used for counting some statistics
	class Counter
	{
		const TDesC& iName;
		TInt         iCount;
		TInt         iTotal;
	public:
		Counter(const TDesC& aName):iName(aName),iCount(0),iTotal(0)
		{
		}
		void operator++(int)
		{
			iCount++;
			iTotal++;
		}
		void Summary(Log& aLog)
		{
			TBuf<MAX_LEN> buf;
			buf.AppendFormat(_L("%S: %d"), &iName, iCount);
			aLog.Write(Log::SUMMARY, buf);
			iCount = 0;
		}
		void Total(Log& aLog)
		{
			TBuf<MAX_LEN> buf;
			buf.AppendFormat(_L("%S: %d"), &iName, iTotal);
			aLog.Write(Log::SUMMARY, buf);
			iCount = 0;
			iTotal = 0;
		}
	};
	friend class Log::Counter; // counter should be friend of log..

	// line counters...
	Counter iPassed;
	Counter iFailed;
	Counter iWarnings;
	Counter iComments;
};




#endif // TC_LOG_H

