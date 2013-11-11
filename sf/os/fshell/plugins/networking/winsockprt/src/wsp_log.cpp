// wsp_log.cpp
// 
// Copyright (c) 2004 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "wsp_log.h"

#ifdef WSP_LOGGING

#include <comms-infras/commsdebugutility.h>

_LIT8(KSubSystem, "fshell");
_LIT8(KComponent, "winsockprt");  


RFileLogger& Logger()
	{
	return *(static_cast<RFileLogger*>(Dll::Tls()));
	}

TInt WspLog::Open()
	{
	RFileLogger* logger = new RFileLogger();
	if (logger == NULL)
		{
		return KErrNoMemory;
		}

	TInt err = logger->Connect();
	if (err == KErrNone)
		{
		err = logger->SetLogTags(KSubSystem(), KComponent());
		if (err == KErrNone)
			{
			Dll::SetTls(logger);
			}
		}

	return err;
	}

void WspLog::Close()
	{
	RFileLogger* logger = &(Logger());
	logger->Close();
	delete logger;
	Dll::SetTls(NULL);
	}

void WspLog::Write(const TDesC& aDes)
	{
	Logger().Write(aDes);
	}

void WspLog::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Logger().WriteFormat(aFmt, list);
	}

void WspLog::Printf(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Logger().WriteFormat(aFmt, list);
	}

void WspLog::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
	{
	Logger().HexDump(aHeader, aMargin, aPtr, aLen);
	}

void WspLog::WriteRawInbound(const TDesC8& aDes)
	{
	Logger().Write(_L("In-bound data:"));
	Logger().HexDump(aDes);
	}

void WspLog::WriteRawOutbound(const TDesC8& aDes)
	{
	Logger().Write(_L("Out-bound data:"));
	Logger().HexDump(aDes);
	}


#endif // _DEBUG
