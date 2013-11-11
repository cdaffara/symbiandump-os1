// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalTechnology
*/


#include <e32base.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KSubsystem, "BT");
_LIT8(KLogCmpt, LOG_COMPONENT_LOGGER);
#endif

/**
This constant is used to direct how to pad the logged text to ensure it
lines up, as tab characters are involved, lining up requires knowledge of
how many spaces a tab character is viewed as.
*/
#ifdef __FLOG_ACTIVE
const TUint KTabWidthInViewer = 4;
const TUint KMaxTabsLength = 1 + KMaxTagLength/KTabWidthInViewer;
#endif

NONSHARABLE_CLASS(TLogData)
	{
	public:
#ifdef __FLOG_ACTIVE
		TLogData();
		
		void SetLogTags(const TDesC8& aCmpt);

		TInt iAccessCount;

		RFileLogger iLogEngine;
		TBuf8<KMaxTagLength> iCurrentComponent;
		
		TBuf8<KMaxTabsLength> iInitTabPad;
		
		TUint iIndent;
#endif
	};


#ifdef __FLOG_ACTIVE
TLogData::TLogData()
	: iAccessCount(0), iCurrentComponent(KNullDesC8), iIndent(0)
	{}

void TLogData::SetLogTags(const TDesC8& aCmpt)
	{
	if (aCmpt != iCurrentComponent)
		{
		iCurrentComponent = aCmpt.Left(KMaxTagLength);
		TInt numPadChars = KMaxTagLength-iCurrentComponent.Length(); // must be >=0
		TInt padTabs = (numPadChars/KTabWidthInViewer) + ((numPadChars%KTabWidthInViewer > 0) ? 1 : 0);
		iInitTabPad.Zero();
		iInitTabPad.Fill('\t', padTabs);
		iLogEngine.SetLogTags(KSubsystem, iCurrentComponent);
		}
	}
#endif

#define GETLOG TLogData* __logger = static_cast<TLogData*>(Dll::Tls());



EXPORT_C /*static*/ TInt CBtLog::Connect()
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (!__logger)
		{
		CBtLog::Write(KLogCmpt, _L8("Opening new logger connection"));
		__logger = new TLogData();
		if (!__logger)
			{
			CBtLog::Write(KLogCmpt, _L8("Opening logger connection failed, no memory"));
			return KErrNoMemory;
			}
		
		__logger->iLogEngine.Connect();
		Dll::SetTls(__logger);
		}
	
	__logger->iAccessCount++;
	CBtLog::WriteFormat(KLogCmpt, _L8("Opening -- %d instances now open"), __logger->iAccessCount);
		
	return KErrNone;
#else
	return KErrNotSupported;
#endif
	}


EXPORT_C /*static*/ void CBtLog::Close()
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		TInt& count = __logger->iAccessCount;
		
		if (count)
			{
			count--;
			CBtLog::WriteFormat(KLogCmpt, _L8("Closing -- %d instance(s) left open"), count);
			if (!count)
				{
				__logger->iLogEngine.Close();
				delete __logger;
				Dll::SetTls(NULL);
				CBtLog::Write(KLogCmpt, _L8("Fully closed and deleted, now flogging statically."));
				}
			}
		else
			{
			CBtLog::Write(KLogCmpt, _L8("Not closing -- not opened"));
			}
		}
#endif
	}



EXPORT_C /*static*/ void CBtLog::Write(const TDesC8& IF_FLOGGING(aCmpt), const TDesC8& IF_FLOGGING(aText))
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.Write(aText);
		}
	else
		{
		RFileLogger::Write(KSubsystem, aCmpt, aText);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::WriteFormat(const TDesC8& IF_FLOGGING(aCmpt), TRefByValue<const TDesC8> IF_FLOGGING(aFmt), ...)
	{
#ifdef __FLOG_ACTIVE
	VA_LIST list;
	VA_START(list, aFmt);

	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.WriteFormat(aFmt, list);
		}
	else
		{
		RFileLogger::WriteFormat(KSubsystem, aCmpt, aFmt, list);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::WriteFormat(const TDesC8& IF_FLOGGING(aCmpt), TRefByValue<const TDesC8> IF_FLOGGING(aFmt), VA_LIST& IF_FLOGGING(aList))
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.WriteFormat(aFmt, aList);
		}
	else
		{
		RFileLogger::WriteFormat(KSubsystem, aCmpt, aFmt, aList);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::Write(const TDesC8& IF_FLOGGING(aCmpt), const TDesC16& IF_FLOGGING(aText))
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.Write(aText);
		}
	else
		{
		RFileLogger::WriteFormat(KSubsystem, aCmpt, aText);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::WriteFormat(const TDesC8& IF_FLOGGING(aCmpt), TRefByValue<const TDesC16> IF_FLOGGING(aFmt), ...)
	{
#ifdef __FLOG_ACTIVE
	VA_LIST list;
	VA_START(list, aFmt);

	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.WriteFormat(aFmt, list);
		}
	else
		{
		RFileLogger::WriteFormat(KSubsystem, aCmpt, aFmt, list);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::WriteFormat(const TDesC8& IF_FLOGGING(aCmpt), TRefByValue<const TDesC16> IF_FLOGGING(aFmt), VA_LIST& IF_FLOGGING(aList))
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.WriteFormat(aFmt, aList);
		}
	else
		{
		RFileLogger::WriteFormat(KSubsystem, aCmpt, aFmt, aList);
		}
#endif
	}


EXPORT_C /*static*/ void CBtLog::HexDump(const TDesC8& IF_FLOGGING(aCmpt), const TText* IF_FLOGGING(aHeader), const TText* IF_FLOGGING(aMargin), const TUint8* IF_FLOGGING(aPtr), TInt IF_FLOGGING(aLen))
	{
#ifdef __FLOG_ACTIVE
	GETLOG;
	
	if (__logger)
		{
		__logger->SetLogTags(aCmpt);
		__logger->iLogEngine.HexDump(aHeader, aMargin, aPtr, aLen);
		}
	else
		{
		RFileLogger::HexDump(KSubsystem, aCmpt, TPtrC8(aPtr, aLen), KNullDesC8);
		}
#endif
	}







/**
Leave (if error) verbosely- log name of file and line number just before 
leaving.
@param aFile The file we're leaving from.
@param aLine The line number we're leaving from.
@param aReason The leave code.
*/
EXPORT_C void VerboseLeaveIfErrorL(const TDesC8& IF_FLOGGING(aCpt), 
						  char* IF_FLOGGING(aFile), 
						  TInt IF_FLOGGING(aLine), 
						  TInt IF_FLOGGING(aReason))
	{
#ifdef __FLOG_ACTIVE
	// only leave if non-zero value
	if ( aReason >= KErrNone )
		{
		return;
		}

	_LIT8(KLeavePrefix, "LEAVE: ");

	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);

	TBuf8<256> buf;
	buf.Append(KLeavePrefix);
	buf.AppendFormat(_L8("aReason = %d [file %S, line %d]"), aReason, &fileName, 
		aLine);
	CBtLog::Write(aCpt, buf);

	// finally
	User::Leave(aReason);
#endif
	}

/**
Leave verbosely- log name of file and line number just before
leaving.
@param aFile The file we're leaving from.
@param aLine The line number we're leaving from.
@param aReason The leave code.
*/
EXPORT_C void VerboseLeaveL(const TDesC8& IF_FLOGGING(aCpt),
						  char* IF_FLOGGING(aFile),
						  TInt IF_FLOGGING(aLine),
						  TInt IF_FLOGGING(aReason))
	{
#ifdef __FLOG_ACTIVE
	
	_LIT8(KLeavePrefix, "LEAVE: ");

	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);

	TBuf8<256> buf;
	buf.Append(KLeavePrefix);
	buf.AppendFormat(_L8("aReason = %d [file %S, line %d]"), aReason, &fileName,
		aLine);
	CBtLog::Write(aCpt, buf);

	// finally
	User::Leave(aReason);
#endif
	}
/**
Panic verbosely- log name of file and line number just before panicking.
@param aFile The file that's panicking.
@param aLine The line number that's panicking.
@param aReason The panic code.
@param aPanicName The text of the panic code.
@param aPanicCategory The panic category.
*/
EXPORT_C void VerbosePanic(const TDesC8& IF_FLOGGING(aCpt), 
				  char* IF_FLOGGING(aFile), 
				  TInt IF_FLOGGING(aLine), 
				  TInt IF_FLOGGING(aPanicCode), 
				  TText8* IF_FLOGGING(aPanicName),
				  const TDesC& IF_FLOGGING(aPanicCategory))
	{
#ifdef __FLOG_ACTIVE
	_LIT8(KPanicPrefix, "PANIC: code ");
	
	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);

	TBuf8<256> buf;
	buf.Append(KPanicPrefix);
	buf.AppendFormat(_L8("%d = %s [file %S, line %d]"), 
		aPanicCode, 
		aPanicName, 
		&fileName, 
		aLine);
	CBtLog::Write(aCpt, buf);

	// finally
	User::Panic(aPanicCategory, aPanicCode);
#endif
	}

/**
Panic the given message verbosely- log name of file and line number just
before panicking.
@param aMsg Message to panic.
@param aFile The file that's panicking.
@param aLine The line number that's panicking.
@param aReason The panic code.
@param aPanicName The text of the panic code.
@param aPanicCategory The panic category.
*/
EXPORT_C void VerboseMsgPanic(const TDesC8& IF_FLOGGING(aCpt),
								char* IF_FLOGGING(aFile),
								TInt  IF_FLOGGING(aLine),
								const RMessage2& aMsg,
								const TDesC& aCat,
								TInt  aPanicCode)
	{
#ifdef __FLOG_ACTIVE
	_LIT8(KPanicPrefix, "PANICKING CLIENT: code ");

	TPtrC8 fullFileName((const TUint8*)aFile);
	TPtrC8 fileName(fullFileName.Ptr()+fullFileName.LocateReverse('\\')+1);

	TBuf8<256> buf;
	buf.Append(KPanicPrefix);
	buf.AppendFormat(_L8("%d [file %S, line %d]"),
		aPanicCode,
		&fileName,
		aLine);
	CBtLog::Write(aCpt, buf);
#endif
	// finally
	aMsg.Panic(aCat, aPanicCode);
	}

#ifdef __FLOG_ACTIVE
_LIT8(KInstrumentIn, "%S%S>>%S this = [0x%08x]");
_LIT8(KInstrumentOut, "%S%S<<%S");
_LIT8(KInstrumentStaticIn, "%S%S>>%S static");
_LIT8(KInstrumentStaticOut, "%S%S<<%S");
#define BT_LOGGER_INDENT_CHAR ' '

TUint TLoggerBase::PreIncrementIndent()
	{
	GETLOG;
	return (__logger ? ++(__logger->iIndent) : 0);
	}
	
TUint TLoggerBase::PostDecrementIndent()
	{
	GETLOG;
	return (__logger && (__logger->iIndent > 0) ? (__logger->iIndent)-- : 0);
	}

TUint TLoggerBase::Indent()
	{
	GETLOG;
	return (__logger ? (__logger->iIndent) : 0);
	}
#endif

#ifdef __FLOG_ACTIVE
EXPORT_C TFunctionLogger::TFunctionLogger(const TDesC8& aCpt, const TDesC8& aString, TAny* aThis)
	{
	GETLOG;
	iCpt.Set(aCpt);
	iString.Set(aString);
	if (PreIncrementIndent() < KMaxIndent)
		{
		iIndentBuf.Fill(BT_LOGGER_INDENT_CHAR, Indent());
		}
		
	TPtrC8 initPad(KNullDesC8);
	if(__logger)
		{
		__logger->SetLogTags(iCpt);
		initPad.Set(__logger->iInitTabPad);
		}
	CBtLog::WriteFormat(iCpt, KInstrumentIn, &initPad, &iIndentBuf, &iString, aThis);
	}

EXPORT_C TFunctionLogger::~TFunctionLogger()
	{
	GETLOG;
	if (Indent() < KMaxIndent)
		{
		iIndentBuf.Fill(BT_LOGGER_INDENT_CHAR, PostDecrementIndent());
		}
	else
		{
		PostDecrementIndent();
		}
		
	TPtrC8 initPad(KNullDesC8);
	if(__logger)
		{
		__logger->SetLogTags(iCpt);
		initPad.Set(__logger->iInitTabPad);
		}
	CBtLog::WriteFormat(iCpt, KInstrumentOut, &initPad, &iIndentBuf, &iString);
	}

EXPORT_C TStaticFunctionLogger::TStaticFunctionLogger(const TDesC8& aCpt, const TDesC8& aString)
	{
	GETLOG;
	iCpt.Set(aCpt);
	iString.Set(aString);
	if (PreIncrementIndent() < KMaxIndent)
		{
		iIndentBuf.Fill(BT_LOGGER_INDENT_CHAR, Indent());
		}
		
	TPtrC8 initPad(KNullDesC8);
	if(__logger)
		{
		__logger->SetLogTags(iCpt);
		initPad.Set(__logger->iInitTabPad);
		}
	CBtLog::WriteFormat(iCpt, KInstrumentStaticIn, &initPad, &iIndentBuf, &iString);
	}

EXPORT_C TStaticFunctionLogger::~TStaticFunctionLogger()
	{
	GETLOG;
	if (Indent() < KMaxIndent)
		{
		iIndentBuf.Fill(BT_LOGGER_INDENT_CHAR, PostDecrementIndent());
		}
	else
		{
		PostDecrementIndent();
		}
		
	TPtrC8 initPad(KNullDesC8);
	if(__logger)
		{
		__logger->SetLogTags(iCpt);
		initPad.Set(__logger->iInitTabPad);
		}
	CBtLog::WriteFormat(iCpt, KInstrumentStaticOut, &initPad, &iIndentBuf, &iString);
	}
#else
#ifdef __UREL_RDEBUG_PRINT
#include <e32debug.h>
EXPORT_C TFunctionLogger::TFunctionLogger(const TDesC8& aCpt, const TDesC8& aString, TAny* aThis)
	{
	iCpt.Set(aCpt);
	iString.Set(aString);
	TBuf8<1024> buf;
	buf.Format(_L8("%S >>%S this = [0x%08x]"), &iCpt, &iString, aThis);
	
	RDebug::Print(buf.Expand());
	}

EXPORT_C TFunctionLogger::~TFunctionLogger()
	{
	TBuf8<1024> buf;
	buf.Format(_L8("%S <<%S"), &iCpt, &iString);
	RDebug::Print(buf.Expand());
	}

EXPORT_C TStaticFunctionLogger::TStaticFunctionLogger(const TDesC8& aCpt, const TDesC8& aString)
	{
	iCpt.Set(aCpt);
	iString.Set(aString);
	TBuf8<1024> buf;
	buf.Format(_L8("%S >>%S static"), &iCpt, &iString);
	
	RDebug::Print(buf.Expand());
	}

EXPORT_C TStaticFunctionLogger::~TStaticFunctionLogger()
	{
	TBuf8<1024> buf;
	buf.Format(_L8("%S <<%S"), &iCpt, &iString);
	RDebug::Print(buf.Expand());
	}
#else // NO DEBUG
EXPORT_C TFunctionLogger::TFunctionLogger(const TDesC8& /*aCpt*/, const TDesC8& /*aString*/, TAny* /*aThis*/)
	{
	}

EXPORT_C TFunctionLogger::~TFunctionLogger()
	{
	}

EXPORT_C TStaticFunctionLogger::TStaticFunctionLogger(const TDesC8& /*aCpt*/, const TDesC8& /*aString*/)
	{
	}

EXPORT_C TStaticFunctionLogger::~TStaticFunctionLogger()
	{
	}
#endif
#endif
