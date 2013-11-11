// Flogger.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32base.h>
#include <fshell/clogger.h>
#include "cliserv.h"

// This file is shared between building shims for traditional flogger and comms debug util
// Bits specific to one or the other are guarded with  __CDU__

#ifdef __CDU__

#define __FLOG_SUPPRESS_REL_WARNING
#define __FLOG_ACTIVE
#include <comms-infras/commsdebugutility.h>
#define iPtr ((RClogger*)iLoggerBody)
#define Connected() (iPtr && iPtr->Handle())
#define DoClose() { if (iPtr) { iPtr->Close(); delete iPtr; iLoggerBody = NULL; } }

#else

#include <flogger.h>
#define iPtr (*((RClogger**)&iFormatter))
#define Connected() (iPtr && iPtr->Handle())
#define DoClose() { if (iPtr) { iPtr->Close(); delete iPtr; } }

#endif

#define iClogger (*iPtr)
#define FlogPtr(clogPtr) ((RFloggerClogger*)clogPtr)

// declares RClogger* clogger
#define CheckConnected(aDir, aPath)                            \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	GetTls(clogger, ETrue, &aDir, &aPath, ETrue, NULL);        \
	if (!clogger) return

#define CheckConnected8(aDir, aPath)                           \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	GetTls(clogger, ETrue, &aDir, &aPath, EFalse, NULL);       \
	if (!clogger) return

/*
#define GetTlsOrConnect(aTag)                                  \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	TInt err;                                                  \
	TBool isHandle = GetTls(clogger, ETrue, &aTag, &err)
*/

#define GetTlsOrNull()                                         \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
TBool isHandle = ::GetTls(clogger, EFalse, &KNullDesC, &KNullDesC, ETrue, NULL)

#define KDontAllocateWhileLogging 0x20000000

static TBool GetTls(RClogger*& aClogger, TBool aAutoConnect, const TAny* aTag, const TAny* aSubTag, TBool aWide, TInt* aError);

// Need these because flogger has a single-argument overload that doesn't have a format applied over it, and clogger doesn't
_LIT8(KEsc8, "%S");
_LIT(KEsc, "%S");

#define iBody (*(RCloggerBody*)&iPimpl)

#ifdef _DEBUG
// Close the session after every call in debug builds, to avoid heap-check and handle-check failures
#define STATICCLOSE() RFloggerClogger::FloggerStaticClose()
#else
#define STATICCLOSE()
#endif

class RFloggerClogger : public RClogger
	{
public:
	TUint& Flags()
		{
		return iFlags;
		}

	TInt FloggerConnect(const TDesC& aDir, const TDesC& aName)
		{
		TAny* arg1 = NULL;
		TInt err = Reserved(0xF10C10, arg1, NULL); // Does the connect
		if (!err)
			{
			err = DoSetTags(aDir, aName);
			}
		return err;
		}

	TInt FloggerConnect(const TDesC8& aDir, const TDesC8& aName)
		{
		TAny* arg1 = NULL;
		TInt err = Reserved(0xF10C10, arg1, NULL); // Does the connect
		if (!err)
			{
			err = DoSetTags8(aDir, aName);
			}
		return err;
		}

	TInt DoSetTags(const TDesC& aDir, const TDesC& aName)
		{
		//TPtrC name(aName);
		//_LIT(KDotTxt, ".txt");
		//_LIT(KDotLog, ".log");
		//if (name.Right(KDotTxt().Length()) == KDotTxt) name.Set(name.Left(name.Length()-KDotTxt().Length()));
		//else if (name.Right(KDotLog().Length()) == KDotLog) name.Set(name.Left(name.Length()-KDotLog().Length()));
		TPckg<TUint32> enabledPkg(iEnabled);
		TInt result = SendReceive(ESetTag, TIpcArgs(&aDir, &aName, &enabledPkg));
		if (result >= 0)
			{
			iSequence = result;
			return KErrNone;
			}
		else
			{
			return result;
			}
		}

	TInt DoSetTags8(const TDesC8& aDir, const TDesC8& aName)
		{
		//TPtrC8 name(aName);
		//_LIT8(KDotTxt, ".txt");
		//_LIT8(KDotLog, ".log");
		//if (name.Right(KDotTxt().Length()) == KDotTxt) name.Set(name.Left(name.Length()-KDotTxt().Length()));
		//else if (name.Right(KDotLog().Length()) == KDotLog) name.Set(name.Left(name.Length()-KDotLog().Length()));
		TPckg<TUint32> enabledPkg(iEnabled);
		TInt result = SendReceive(ESetTag8, TIpcArgs(&aDir, &aName, &enabledPkg));
		if (result >= 0)
			{
			iSequence = result;
			return KErrNone;
			}
		else
			{
			return result;
			}
		}

	static void FloggerStaticClose()
		{
		GetTlsOrNull();
		if (clogger)
			{
			clogger->Close();
			if (!isHandle)
				{
				delete clogger;
				}
			Dll::SetTls(NULL);
			Dll::FreeTls();
			}
		}

	void HexDump16(const TDesC16& aHeader, const TDesC8& aData)
		{
		SendReceive(2002, TIpcArgs(&aHeader, &aData, User::NTickCount(), RClogger::EAllEnabled));
		}
	};

static TBool GetTls(RClogger*& aClogger, TBool aAutoConnect, const TAny* aTag, const TAny* aSubTag, TBool aWide, TInt* aError)
	{
	TAny* tls = Dll::Tls();
	if ((TUint)tls & 1)
		{
		// Then it's a handle
		aClogger->SetHandle((TInt)((TUint)tls >> 1));
		return ETrue;
		}
	else if (tls)
		{
		// Then it's a pointer
		aClogger = (RClogger*)tls;
		return EFalse;
		}
	else if (aAutoConnect)
		{
		//TInt err = aClogger->Connect(aTag ? *aTag : KNullDesC);
		TInt err;
		if (aWide)
			{
			err = FlogPtr(aClogger)->FloggerConnect(*(const TDesC*)aTag, *(const TDesC*)aSubTag);
			}
		else
			{
			err = FlogPtr(aClogger)->FloggerConnect(*(const TDesC8*)aTag, *(const TDesC8*)aSubTag);
			}
		
		if (err)
			{
			if (aError) *aError = err;
			aClogger = NULL;
			return ETrue;
			}
		if ((aClogger->Handle() & 0x80000000) != 0)
			{
			__ASSERT_DEBUG(EFalse, User::Panic(_L("HandleTooBig"), 0));
			err = KErrOverflow;
			}

		if (!err)
			{
			err = Dll::SetTls((TAny*) ((aClogger->Handle() << 1) | 1));
			}

		if (err)
			{
			aClogger->Close();
			if (aError) *aError = err;
			aClogger = NULL;
			}
		return ETrue;
		}
	else
		{
		aClogger = NULL;
		return ETrue;
		}
	}

EXPORT_C RFileLogger::RFileLogger()
	{
	Mem::FillZ(this, sizeof(RFileLogger));
	}

EXPORT_C RFileLogger::~RFileLogger()
	{
	}

EXPORT_C TVersion RFileLogger::Version() const
	{
	return TVersion(1, 0, 20);
	}

EXPORT_C TInt RFileLogger::Connect()
	{
	if (Connected())
		{
		return KErrAlreadyExists;
		}

	#ifdef __CDU__
		iLoggerBody = (RFileLoggerBody*)new RClogger();
		if (!iLoggerBody) return KErrNoMemory;
	#else
		iPtr = new RClogger();
		if (!iPtr) return KErrNoMemory;
	#endif

	TInt err = iClogger.Connect();
	/*
	#ifdef _DEBUG
		if (!err)
			{
			// Don't cache client buffers in debug mode (so we will pass UDEB heap check macros)
			FlogPtr(iPtr)->Flags() &= ~RClogger::ECacheClientBuffers;
			}
	#endif
	*/

	#if !defined(__CDU__) && defined(_DEBUG)
		if (err == KErrNone)
			{
			SetHandle(iClogger.Handle()); // We do this so that when Close() is called, it cleans up the clogger interface. And we do that because we can't override Close(), and if the session doesn't get closed somehow, we'll fail CONE's handle check in debug builds
			}
	#endif
	if (!err)
		{
		iClogger.SetLogBehaviour(0 /*RClogger::EMonitorTagState*/); // Don't set EUseHeapBuffer because then we won't be thread-safe
		}
	else
		{
		DoClose();
		}
	return err;
	}

EXPORT_C void RFileLogger::SetDateAndTime(TBool /*aUseDate*/, TBool /*aUseTime*/)
	{
	// Has no real meaning in clogger
	}

EXPORT_C void RFileLogger::CreateLog(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/)
	{
	TInt err;
	if (Connected())
		{
		err = FlogPtr(&iClogger)->DoSetTags(aDir, aName);
		}
	else
		{
		err = KErrNotFound;
		}
	#ifdef __CDU__
		if (err) {} //Like CDU, ignore errors in legacy methods. This construction shuts up RVCT
	#else
		 iLastError = err;
	#endif

	}

EXPORT_C void RFileLogger::CloseLog()
	{
	// Has no meaning in clogger terms
	}

EXPORT_C void RFileLogger::Write(const TDesC16& aText)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.Log(RClogger::EAllEnabled, KEsc, &aText);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	VA_LIST args;
	VA_START(args, aFmt);
	iClogger.LogList(RClogger::EAllEnabled, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.LogList(RClogger::EAllEnabled, aFmt, aList);
	}

EXPORT_C void RFileLogger::Write(const TDesC8& aText)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.Log(RClogger::EAllEnabled, KEsc8, &aText);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	VA_LIST args;
	VA_START(args, aFmt);
	iClogger.LogList(RClogger::EAllEnabled, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RFileLogger::WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.LogList(RClogger::EAllEnabled, aFmt, aList);
	}

EXPORT_C void RFileLogger::HexDump(const TText* aHeader, const TText* /*aMargin*/, const TUint8* aPtr, TInt aLen)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	TPtrC16 header;
	if (aHeader)
		{
		header.Set(aHeader, User::StringLength(aHeader));
		}
	const TPtrC8 data(aPtr, aLen);
	FlogPtr(iPtr)->HexDump16(header, data);
	}

EXPORT_C TInt RFileLogger::LastError() const
	{
	#ifdef __CDU__
		return KErrNone;
	#else
		return iLastError;
	#endif
	}

EXPORT_C TBool RFileLogger::LogValid() const
	{
	return ETrue;
	}

// The static functions
EXPORT_C void RFileLogger::Write(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, const TDesC16& aText)
	{
	CheckConnected(aDir, aName);
	clogger->Log(KEsc, &aText);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC16> aFmt,...)
	{
	CheckConnected(aDir, aName);
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aFmt, args);
	VA_END(args);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
	{
	CheckConnected(aDir, aName);
	clogger->LogList(aFmt, aList);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::Write(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, const TDesC8& aText)
	{
	CheckConnected(aDir, aName);
	clogger->Log(KEsc8, &aText);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC8> aFmt,...)
	{
	CheckConnected(aDir, aName);
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aFmt, args);
	VA_END(args);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	CheckConnected(aDir, aName);
	clogger->LogList(aFmt, aList);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::HexDump(const TDesC& aDir, const TDesC& aName, TFileLoggingMode /*aMode*/, const TText* aHeader, const TText* /*aMargin*/, const TUint8* aPtr, TInt aLen)
	{
	CheckConnected(aDir, aName);
	const TPtrC16 header(aHeader);
	const TPtrC8 data(aPtr, aLen);

	FlogPtr(clogger)->HexDump16(header, data);
	}

// Boring stuff

#ifndef __CDU__

TLogFormatter::TLogFormatter() {}
TLogFile::TLogFile() {}
void TLogFormatter8Overflow::Overflow(TDes8& /*aDes*/) {}
void TLogFormatter16Overflow::Overflow(TDes16& /*aDes*/) {}
EXPORT_C TInt FLogger::Run(FLogger::TSignal& /*aSignal*/)
	{
	return KErrNotSupported;
	}

#endif

#ifdef __CDU__

// Some functions new in CDU

EXPORT_C void RFileLogger::Close()
	{
	DoClose();
	}

EXPORT_C void ClientRunStubOrdinal1()
	{
	}

EXPORT_C TInt RFileLogger::ClearLog()
	{
	if (!Connected()) return KErrNone; // Be compatible with CDU and flogger in terms of not crashing
	return iClogger.Rotate(); // Close enough
	}

EXPORT_C TInt RFileLogger::SetLogTags(const TDesC8& aSubsystem, const TDesC8& aComponent)
	{
	return FlogPtr(iPtr)->DoSetTags8(aSubsystem, aComponent);
	}

EXPORT_C void RFileLogger::__DbgShutDownServer()
	{
	// Why bother supporting this?
	}

EXPORT_C void RFileLogger::__DbgSetHeapFailure(TInt /*aFailAfter*/)
	{
	// See above...
	}

EXPORT_C void RFileLogger::WriteBinary(const TDesC8& aData)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.Log(KEsc8, &aData);
	}

EXPORT_C void RFileLogger::HexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData, const TDesC8& aHeader)
	{
	CheckConnected8(aSubsystem, aComponent);

	clogger->HexDump(aHeader, aData);
	}

EXPORT_C void RFileLogger::HexDump(const TDesC8& aData, const TDesC8& aHeader)
	{
	if (!Connected()) return; // Be compatible with CDU and flogger in terms of not crashing
	iClogger.HexDump(aHeader, aData);
	}

EXPORT_C TInt RFileLogger::Handle() const
	{
	return iClogger.Handle();
	}

EXPORT_C TInt RFileLogger::Share() 
	{
	return iClogger.ShareAuto();
	}

EXPORT_C void RFileLogger::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
	{
	CheckConnected8(aSubsystem, aComponent);
	clogger->Log(KEsc8, &aText);
	}

EXPORT_C void RFileLogger::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC& aText)
	{
	CheckConnected8(aSubsystem, aComponent);
	clogger->Log(KEsc, &aText);
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, ...)
	{
	CheckConnected8(aSubsystem, aComponent);
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aFmt, args);
	VA_END(args);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, ...)
	{
	CheckConnected8(aSubsystem, aComponent);
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aFmt, args);
	VA_END(args);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	CheckConnected8(aSubsystem, aComponent);
	clogger->LogList(aFmt, aList);

	STATICCLOSE();
	}

EXPORT_C void RFileLogger::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, const TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
	{
	CheckConnected8(aSubsystem, aComponent);
	clogger->LogList(aFmt, aList);

	STATICCLOSE();
	}

#endif
