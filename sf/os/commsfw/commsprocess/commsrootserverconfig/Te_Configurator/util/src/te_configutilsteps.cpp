// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_configutilsteps.h"
#include <test/testexecutelog.h>

class TTruncateOverflow16 : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&)
		{
		}
	};

class TTruncateOverflow8 : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8&)
		{
		}
	};

void CTestStepConfigUtil::Log(TRefByValue<const TDesC16> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow16 truncateOverflow;
	TBuf<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}

void CTestStepConfigUtil::Log(TRefByValue<const TDesC8> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow8 truncateOverflow;
	TBuf8<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}



CTestStepLoadConfig::CTestStepLoadConfig()
	{
	SetTestStepName(KTestStepLoadConfig);
	}

/* virtual */ TVerdict CTestStepLoadConfig::doTestStepL()
	{
	TVerdict verdict = EPass;

	TInt err(ConfigLoader::LoadConfig(this));
	if(err!=KErrNone)
		{
		verdict = EFail;
		INFO_PRINTF2(_L("LoadConfig returned error %d"), err);
		}
	return verdict;
	}

CTestStepUnLoadConfig::CTestStepUnLoadConfig(CommsFW::TCFShutdownType aType):iType(aType)
	{
	SetTestStepName(KTestStepUnLoadConfig);
	}

/* virtual */ TVerdict CTestStepUnLoadConfig::doTestStepL()
	{
	TVerdict verdict = EPass;

	TInt err(ConfigLoader::UnLoadConfig(iType, this));
	if(err!=KErrNone)
		{
		verdict = EFail;
		if(KErrSessionClosed==err)
			{
			INFO_PRINTF2(_L("UnLoadConfig returned %d. C32 had still had unclosed sessions when exiting."), err);
			}
		else
			{
			INFO_PRINTF2(_L("UnLoadConfig returned error %d"), err);
			}

		}
	return verdict;
	}


