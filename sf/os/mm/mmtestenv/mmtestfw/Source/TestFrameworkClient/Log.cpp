// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Client / server logging for Test Framework
// NOTE : does NOT include secure API changes in EKA2
// 
//


// Test system includes
#include <testframework.h>

// do not export if Unit Testing
#if defined (__TSU_TESTFRAMEWORK__)
#undef EXPORT_C
#define EXPORT_C
#endif

/**
 *
 * HTML font formatting strings
 *
 * @xxxx
 *
 */
_LIT(KResultPass, "<font size=4 color=00AF00>");
_LIT(KResultFail, "<font size=4 color=FF0000>");
_LIT(KResultInconclusive, "<font size=4 color=0000FF>");
_LIT(KResultUndefined, "<font size=4 color=FF00FF>");
_LIT(KResultEnd, "</font>");
_LIT(KResultNonHtml, "");

_LIT(KResultKnownFailure, "<font size=4 color=008080>"); //A new TVerdict for a known failed test

// logging macro used only for verdicts - not for external use
#define VER_PRINTF(r1, r2, r3)		LogExtra(__FILE8__, __LINE__, ESevrVer, _L("%S%S%S\n"), (r1), (r2), (r3)) 

/**
 *
 * Static constructor for CLog.
 *
 *
 * @return	"CLog*"
 *			The constructed CLog
 *
 * @xxxx
 *
 */
EXPORT_C CLog* CLog::NewL()
	{
	CLog* self = new(ELeave) CLog;
	self->Construct();
	return self;
	}

/**
 *
 * Static NewLC constructor for CLog.
 *
 *
 * @return	"CLog*"
 *			The constructed CLog
 *
 * @xxxx
 *
 */
EXPORT_C CLog* CLog::NewLC()
	{
	CLog* self = new(ELeave) CLog;
	CleanupStack::PushL(self);
	self->Construct();
	return self;
	}

/**
 *
 * Second-phase constructor for CLog.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::Construct()
	{
	iLogStatus = 0;		// no outputs enabled yet
	iSeverity = ESevrAll;
	//Do we need to put information about source file & #line?
	//Default is yes.
	iLogFlags = ELogPutSrcInfo | ELogHtmlMode;
	}

/**
 *
 * Destructor for CLog.
 *
 * @xxxx
 *
 */
EXPORT_C CLog::~CLog()
	{
	// tell server to close logs here
	iClientSession.Close();
	}

/**
 *
 * Open a new test log.
 *
 * @param	"const TDesC& aLogName"
 *			Log file or console name
 *
 * @param	"TInt aLogMode"
 *			Log modes (a bitmask of TTestFrameworkLogMode).
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::OpenLogFileL(const TDesC& aLogName, TInt aLogMode)
	{
	User::LeaveIfError(iClientSession.Connect());

	iClientSession.OpenLog(aLogName, aLogMode);
	// get and store the log status - this will determine whether we
	// e.g. do any formatting for file output
	iLogStatus = iClientSession.LogStatus();
	}

/**
 *
 * Open an existing test log.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::OpenLogFileL()
	{
	User::LeaveIfError(iClientSession.Connect());
	iLogStatus = iClientSession.LogStatus();
	}

/**
 *
 * Set put source info (i.e. traceable logging).
 *
 * @param	"TBool aPutSrcInfo"
 *			Put source info on or off.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::SetPutSrcInfo(TBool aPutSrcInfo)
	{
	if(aPutSrcInfo)
		iLogFlags |= ELogPutSrcInfo;
	else
		iLogFlags &= ~ELogPutSrcInfo;
	}
	
/**
 *
 * Set HTML logging mode.
 *
 * @param	"TBool aArg"
 *			HTML mode on or off.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::SetHtmlLogMode(TBool aArg)
	{
	if(aArg)
		iLogFlags |= ELogHtmlMode;
	else
		iLogFlags &= ~ELogHtmlMode;
	}
 
/**
 *
 * Get HTML logging mode.
 *
 * @return	"TBool"
 *			HTML mode on or off.
 *
 * @xxxx
 *
 */
EXPORT_C TBool CLog::HtmlLogMode() const
	{
	return ((iLogFlags & ELogHtmlMode) != 0);
	}


/**
 *
 * General logging function.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::Log(TRefByValue<const TDesC16> aFmt, ...)
	{
    VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow overflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf = KNullDesC;
	lineBuf.AppendFormatList(aFmt, aList, &overflow16);

	// write to the console
	if(iLogStatus & ELogConsoleFull)
		iClientSession.WriteLog(lineBuf, ELogToConsole);

	// write to log file
	WriteFormat(_L("%S\n"), &lineBuf);

	VA_END(aList); 
	}

/**
 *
 * General logging function with severity.
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...)
	{
    VA_LIST aList;
	VA_START(aList, aFmt);

	if(LogSeverity::IsActive(aSeverity, Severity()))
		{
		Log(aFmt, aList);
		}

	VA_END(aList); 
	}

/**
 *
 * General logging function.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"VA_LIST aList"
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::Log(TRefByValue<const TDesC16> aFmt, VA_LIST aList)
	{
	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	TIntegrationTestLog16Overflow overflow16;

	lineBuf = KNullDesC;
	lineBuf.AppendFormatList(aFmt, aList, &overflow16);

	// write to log file
	WriteFormat(_L("%S\n"),&lineBuf);

	// write to the console
	if(iLogStatus & ELogConsoleFull)
		iClientSession.WriteLog(lineBuf, ELogToConsole);

	}

/**
 *
 * Traceable logging function
 *
 * Should be used for macros only
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"VA_LIST aList"
 *			Variable print parameters
 *
 * @xxxx
 *
 */
#ifdef EXCLUDE_FOR_UNITTEST
EXPORT_C void CLog::LogExtra(const TText8* /*aFile*/, TInt /*aLine*/, TInt /*aSeverity*/,
							 TRefByValue<const TDesC16> /*aFmt*/, VA_LIST /*aList*/)
	{
	}
#else
EXPORT_C void CLog::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
							 TRefByValue<const TDesC16> aFmt, VA_LIST aList)
	{
	if(LogSeverity::IsActive(aSeverity, Severity()))
		{

		TIntegrationTestLog16Overflow overflow16;

		// decode formatted data for display on console
		TBuf<KMaxLogLineLength> lineBuf;

		lineBuf = KNullDesC;
		lineBuf.AppendFormatList(aFmt, aList, &overflow16);

		// write to the console
		if(iLogStatus & ELogConsoleFull)
			WriteLogConsole(lineBuf);

		// log severity
		TBuf<1> charSevr;
		switch(aSeverity)
			{
			case ESevrVer:
				charSevr = _L("V");
				break;
			case ESevrErr:
				charSevr = _L("E");
				break;
			case ESevrWarn:
				charSevr = _L("W");
				break;
			case ESevrInfo:
			default:
				charSevr = _L("I");
				break;
			}

		// Do we need to put information about source file & #line?
		if(iLogFlags & ELogPutSrcInfo)
			{		// Braces used to scope lifetime of TBuf objects
			TPtrC8 fileName8(aFile);
			TBuf<256> fileName;
			TParse printFileName;
			fileName.Copy(fileName8);  // TText8->TBuf16
			// We don't need full file name.
			printFileName.Set(fileName, NULL, NULL) ;
			fileName.Copy(printFileName.NameAndExt()) ;
			// write to log file
			WriteFormat(_L("%S\t%S\t%d\t%S\n"), &charSevr, &fileName, aLine, &lineBuf);
			}
		else
			{
			// write to log file
			WriteFormat(_L("%S\t%S\n"), &charSevr, &lineBuf);
			}
		}
	}
#endif // EXCLUDE_FOR_UNITTEST

/**
 *
 * Traceable logging function with variable param list.
 *
 * Should be used for macros only
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);
	LogExtra(aFile, aLine, aSeverity, aFmt, aList);
	VA_END(aList); 
	}


/**
 *
 * Write a test result highlighted to the log.
 *
 * @param	"TVerdict aVerdict"
 *			The test verdict.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::LogResult(TVerdict aVerdict, TRefByValue<const TDesC16> aFmt, ...)
	{
    VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow iOverflow16;

	// decode formatted data for display on console
	TBuf <KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, aList, &iOverflow16);

	// write to the console
	iClientSession.WriteLog(lineBuf, ELogToConsole);

	// write to log file
	if(iLogFlags & ELogHtmlMode)
		{ 
		switch(aVerdict)
			{
		case EPass:
			VER_PRINTF(&KResultPass(), &lineBuf, &KResultEnd());
			break;
		case EFail:
			VER_PRINTF(&KResultFail(), &lineBuf, &KResultEnd());
			break;
		case EInconclusive:
		case ETestSuiteError:
		case EAbort:
			VER_PRINTF(&KResultInconclusive(), &lineBuf, &KResultEnd());
			break;
		case EKnownFailure:	//A new TVerdict for a known failed test
			VER_PRINTF(&KResultKnownFailure(), &lineBuf, &KResultEnd());
			break;

		default: // undefined
			VER_PRINTF(&KResultUndefined(), &lineBuf, &KResultEnd());
			break;
			}
		}
	else
		{
			VER_PRINTF(&KResultNonHtml(), &lineBuf, &KResultNonHtml());
		}	
	VA_END(aList); 
	}

/**
 *
 * Make a readable string out of an EPOC error code.
 *
 * @param	"TInt aError"
 *			The error code
 *
 * @return	"TPtrC"
 *			The error code as a readable string
 *
 * @xxxx
 *
 */
EXPORT_C TPtrC CLog::EpocErrorToText(TInt aError)
	{
	switch (aError)
		{
	case KErrNone:
		return _L("KErrNone");
	case KErrNotFound:
		return _L("KErrNotFound");
	case KErrGeneral:
		return _L("KErrGeneral");
	case KErrCancel:
		return _L("KErrCancel");
	case KErrNoMemory:
		return _L("KErrNoMemory");
	case KErrNotSupported:
		return _L("KErrNotSupported");
	case KErrArgument:
		return _L("KErrArgument");
	case KErrTotalLossOfPrecision:
		return _L("KErrTotalLossOfPrecision");
	case KErrBadHandle:
		return _L("KErrBadHandle");
	case KErrOverflow:
		return _L("KErrOverflow");
	case KErrUnderflow:
		return _L("KErrUnderflow");
	case KErrAlreadyExists:
		return _L("KErrAlreadyExists");
	case KErrPathNotFound:
		return _L("KErrPathNotFound");
	case KErrDied:
		return _L("KErrDied");
	case KErrInUse:
		return _L("KErrInUse");
	case KErrServerTerminated:
		return _L("KErrServerTerminated");
	case KErrServerBusy:
		return _L("KErrServerBusy");
	case KErrCompletion:
		return _L("KErrCompletion");
	case KErrNotReady:
		return _L("KErrNotReady");
	case KErrUnknown:
		return _L("KErrUnknown");
	case KErrCorrupt:
		return _L("KErrCorrupt");
	case KErrAccessDenied:
		return _L("KErrAccessDenied");
	case KErrLocked:
		return _L("KErrLocked");
	case KErrWrite:
		return _L("KErrWrite");
	case KErrDisMounted:
		return _L("KErrDisMounted");
	case KErrEof:
		return _L("KErrEof");
	case KErrDiskFull:
		return _L("KErrDiskFull");
	case KErrBadDriver:
		return _L("KErrBadDriver");
	case KErrBadName:
		return _L("KErrBadName");
	case KErrCommsLineFail:
		return _L("KErrCommsLineFail");
	case KErrCommsFrame:
		return _L("KErrCommsFrame");
	case KErrCommsOverrun:
		return _L("KErrCommsOverrun");
	case KErrCommsParity:
		return _L("KErrCommsParity");
	case KErrTimedOut:
		return _L("KErrTimedOut");
	case KErrCouldNotConnect:
		return _L("KErrCouldNotConnect");
	case KErrCouldNotDisconnect:
		return _L("KErrCouldNotDisconnect");
	case KErrDisconnected:
		return _L("KErrDisconnected");
	case KErrBadLibraryEntryPoint:
		return _L("KErrBadLibraryEntryPoint");
	case KErrBadDescriptor:
		return _L("KErrBadDescriptor");
	case KErrAbort:
		return _L("KErrAbort");
	case KErrTooBig:
		return _L("KErrTooBig");
	default:
		return _L("Unknown");
		} // end switch
	}


/**
 *
 * Make a readable string out of a test verdict.
 *
 * @param	"TVerdict aTestVerdict"
 *			The test verdict
 *
 * @return	"TPtrC"
 *			The test verdict as a readable string
 *
 * @xxxx
 *
 */
EXPORT_C TPtrC CLog::TestResultText(TVerdict aTestVerdict)
	{
	switch (aTestVerdict)
		{
	case EPass:
		return _L("PASS");
	case EFail:
		return _L("FAIL");
	case EInconclusive:
		return _L("INCONCLUSIVE");
	case ETestSuiteError:
		return _L("TEST_SUITE_ERROR");
	case EAbort:
		return _L("ABORT");
	case EKnownFailure: //A new TVerdict for a known failed test
		return _L("KNOWN_Failure"); //using lower case in 'Failure' can remove the confusion on 'savres2html.bat' making test summary
	default:
		return _L("undefined");

		}
	}

/**
 *
 * Write blank lines to the log.
 *
 * @param	"TInt number"
 *			Number of lines to write
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::LogBlankLine(TInt aNumber)
	{
	for (TInt i = 0; i < aNumber; i++)
		Log(_L(" "));
	}

/**
 *
 * Close the log.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::CloseLogFile()
	{
	// add the htm end
	WriteFormat(_L("</end>"));
	
	iClientSession.CloseLog();
	}

/**
 *
 * Set log severity.
 *
 * @param	"TInt aSeverity"
 *			The required severity
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::SetSeverity(TInt aSeverity)
{
	iSeverity = aSeverity;
}

/**
 *
 * Get log severity.
 *
 * @return	"TInt"
 *			The current severity
 *
 * @xxxx
 *
 */
EXPORT_C TInt CLog::Severity() const
{
	return iSeverity;
}

/**
 *
 * Get log status from this logger's client session.
 *
 * @return	"TInt"
 *			The current log status (a bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 *
 */
EXPORT_C TInt CLog::LogStatus()
	{
	return iClientSession.LogStatus();
	}


/**
 *
 * Write formatted output to the log.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::WriteFormat(TRefByValue<const TDesC16> aFmt, ...)
	{

	// file / port logging not enabled
	if (!(iLogStatus & ELogToFile))
		if (!(iLogStatus & ELogToPort))
			return;

    VA_LIST aList;
	VA_START(aList, aFmt);

	TIntegrationTestLog16Overflow overflow16;

	TUint16* dataBufPtr = (TUint16*)(iDataBuf.Ptr());
	TPtr16 lineBuf(dataBufPtr, 0, KMaxLogLineLength);
	lineBuf.Fill('\0', KMaxLogLineLength);

	// get the current time and date
	TTime now;
	now.HomeTime();
	TDateTime dateTime = now.DateTime() ;

	// add the current time and date 
	lineBuf.Format(_L("%02d/%02d/%04d\t%02d:%02d:%02d:%03d\t"),
		dateTime.Day()+1,
		dateTime.Month()+1,
		dateTime.Year(),
		dateTime.Hour(),
		dateTime.Minute(),
		dateTime.Second(),
		(dateTime.MicroSecond() / 1000)); 

	// followed by the formatted data
	lineBuf.AppendFormatList(aFmt, aList, &overflow16);
	VA_END(aList); 

	// send one message - the server will write to both if enabled
	TInt theStatus = 0;
	if (iLogStatus & ELogToFile)
		theStatus |= ELogToFile;
	if (iLogStatus & ELogToPort)
		theStatus |= ELogToPort;

	iClientSession.WriteLog(lineBuf, theStatus);
	}

/**
 *
 * Write output to the console with date/time stamp
 *
 * @param	"const TDesC& aBuf"
 *			The output.
 *
 * @xxxx
 *
 */
EXPORT_C void CLog::WriteLogConsole(const TDesC& aBuf)
	{

	// console logging not enabled
	if (!(iLogStatus & ELogConsoleFull))
		return;

	// decode formatted data for display on console
	TUint16* dataBufPtr = (TUint16*)(iDataBuf.Ptr());
	TPtr16 lineBuf(dataBufPtr, 0, KMaxLogLineLength);
	lineBuf.Fill('\0', KMaxLogLineLength);

	// get the current time and date
	TTime now;
	now.HomeTime();
	TDateTime dateTime = now.DateTime() ;

	// add the current time and date 
	lineBuf.Format(_L("%02d/%02d/%04d\t%02d:%02d:%02d:%03d\t%S"),
		dateTime.Day()+1,
		dateTime.Month()+1,
		dateTime.Year(),
		dateTime.Hour(),
		dateTime.Minute(),
		dateTime.Second(),
		(dateTime.MicroSecond() / 1000),
		&aBuf); 

	iClientSession.WriteLog(lineBuf, ELogToConsole);
	}

/**
 *
 * This function is used to avoid a panic if format text overflows 
 * the internal buffer.
 *
 * @param "TDes16&"
 *		  the overflowing string
 *
 * @xxxx
 *
 */
void TIntegrationTestLog16Overflow::Overflow(TDes16& aDes)
	{
	aDes = _L("*** ERROR : line too long, cannot log ***");
	}

/**
 *
 * Check a severity value is valid.
 *
 * @param "TInt aSev"
 *		  The value to check.
 *
 * @return "TBool"
 *			Valid (ETrue) or not (EFalse).
 *
 * @xxxx
 *
 */
EXPORT_C TBool LogSeverity::IsValid(TInt aSev)
	{ 
	return ((aSev & ~(ESevrAll)) == 0);
	}

/**
 *
 * Check a severity value is active
 *
 * @param "TInt aThisSev"
 *		  The value to check.
 *
 * @param "TInt aGlobalSev"
 *		  The value to check against (e.g. global severity value of a log).
 *
 * @return "TBool"
 *			Active (ETrue) or not (EFalse).
 *
 * @xxxx
 *
 */
EXPORT_C TBool LogSeverity::IsActive(TInt aThisSev, TInt aGlobalSev)
	{ 
	return ((aThisSev & aGlobalSev) != 0);
	}
