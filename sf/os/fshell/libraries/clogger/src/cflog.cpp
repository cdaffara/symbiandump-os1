// cflog.cpp
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

#include <cflog.h>

#include <comms-infras/commsdebugutility.h>

#define iFlog reinterpret_cast<RFileLogger*>(iCFLog)
//class CCFLog


CCFLogIf::CCFLogIf()
	{
	}

void CCFLogIf::ConstructL()
	{
//	iCFLog = reintepret_cast<CCFLog*>(new(ELeave) RFileLogger);
	}

CCFLogIf::~CCFLogIf()
	{
//	iFlog->Close();
//	delete iFlog;
	}

EXPORT_C CCFLogIf* CCFLogIf::NewL()
	{
/*
	CCFLogIf* self = new(ELeave) CCFLogIf;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
*/
	return NULL;
	}

EXPORT_C TInt CCFLogIf::Delete(CCFLogIf* aCFLogIf)
	{
	return 0;
	}
	
EXPORT_C void CCFLogIf::Panic(TCFLogPanic aPanic)
	{
	_LIT(tcflogger, "TCFLogger-Clogger");
	User::Panic(tcflogger, aPanic);	
	}

EXPORT_C CCFLogIf* CCFLogIf::Context()
	{
	return NULL;
	}

EXPORT_C void CCFLogIf::SetContext()
	{
	}


EXPORT_C void CCFLogIf::Connect()
	{
	}

EXPORT_C void Close()
	{
	}

EXPORT_C void CCFLogIf::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText)
	{
	RFileLogger::Write(aSubsystem, aComponent, aText);
	}

EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST args;
	VA_START(args, aFmt);

	WriteFormat(aSubsystem, aComponent, aFmt, args);
	VA_END(args);
	}

EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
	{
	RFileLogger::Write(aSubsystem, aComponent, aFmt, aList);
	}

EXPORT_C void CCFLogIf::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
	{
	RFileLogger::Write(aSubsystem, aComponent, aText);
	}

EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST args;
	VA_START(args, aFmt);

	WriteFormat(aSubsystem, aComponent, aFmt, args);
	VA_END(args);
	}

EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	RFileLogger::Write(aSubsystem, aComponent, aFmt, aList);
	}

EXPORT_C void CCFLogIf::WriteBinary(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData)
	{
	RFileLogger::WriteBinary(aSubsystem, aComponent, aData);
	}

EXPORT_C void CCFLogIf::HexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
	{
	RFileLogger::HexDump(aSubsystem, aComponent, aData);
	}

