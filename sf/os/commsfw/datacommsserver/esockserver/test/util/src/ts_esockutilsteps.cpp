// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32property.h>
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/testsuite.h>

// Test includes
#include "ts_esockutilsteps.h"
#include "esockloader.h"
#include "esocktestutils.h"
#include <comms-infras/es_availability.h>

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

void CTestStepESockUtil::Log(TRefByValue<const TDesC16> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow16 truncateOverflow;
	TBuf<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );

	// write to the console
	CTestStep::Log(_L("%S"), &buf);
	}

void CTestStepESockUtil::Log(TRefByValue<const TDesC8> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow8 truncateOverflow;
	TBuf8<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	TBuf<512> dispBuf;
	dispBuf.Copy(buf);

	// write to the console
	CTestStep::Log(_L("%S"), &dispBuf);
	}

CTestStepLoadESock::CTestStepLoadESock()
	{
	iTestStepName = _L("ESockUtilLoadESock");
	}

enum TVerdict CTestStepLoadESock::doTestStepL(void)
	{
	TVerdict verdict = EPass;

	TInt err(ESockLoader::LoadESock(this));
	if(err!=KErrNone)
		{
		verdict = EFail;
		Log(_L("LoadESock returned error %d"), err);
		}
	return verdict;
	}

CTestStepUnloadESock::CTestStepUnloadESock()
	{
	iTestStepName = _L("ESockUtilUnloadESock");
	}

enum TVerdict CTestStepUnloadESock::doTestStepL(void)
	{
	TVerdict verdict = EPass;

	TInt err(ESockLoader::UnloadESock(EGraceful, this));
	if(err!=KErrNone)
		{
		verdict = EFail;
		Log(_L("UnloadESock returned error %d"), err);
		}
	return verdict;
	}
	
CTestStepUnloadESockBase::CTestStepUnloadESockBase(TLeak aLeak) :
	iLeak(aLeak)
	{
	}

enum TVerdict CTestStepUnloadESockBase::doTestStepL(void)
	{
	TVerdict verdict = EPass;

	TInt err(ESockLoader::UnloadESock(EImmediate, this));

	verdict = EFail;
	switch (err)
		{
		case KErrNone:
			{
			if (iLeak == ELeaks)
				{
				INFO_PRINTF1(_L("We were expecting a leak and non occurred : check that the leak-detecion machnism is working."));
				verdict = EFail;	
				}
			else
				{
				verdict = EPass;	
				}
			break;
			}
		case KErrSessionClosed:
			{
			INFO_PRINTF2(_L("UnloadESock returned %d. ESock had still had unclosed sessions when exiting."), err);
			break;
			}
		case ESockLoader::KModuleLeaked:
			{
			INFO_PRINTF1(_L("A memory leak was detected on shutdown."));
			INFO_PRINTF1(_L("Expect leaks if a panic occurred - Check the epocwind.out."));
			if (iLeak == EMayLeak || iLeak == ELeaks)
				{
				verdict = EPass;
				}
#ifdef ALWAYS_PASS_UNLOAD
			verdict = EPass;
#endif
			break;		
			}
		default:
			{
			INFO_PRINTF2(_L("UnloadESock returned error %d"), err);
			}
		}

	// Explicitly reset pubsub keys used for controlling bearer availability during testing
	const TInt KMaxLikelyAvailabilityAccessPoints = 32;
	const TInt KDummyNifTestingPubSubUid = 0x10272F42;
	INFO_PRINTF1(_L("Resetting availability pubsub keys"));
	for(TInt i = 0; i < KMaxLikelyAvailabilityAccessPoints; i++)
		{
		// Harmless if the key doesn't exist - so we can ignore the return code
		RProperty::Set(TUid::Uid(KDummyNifTestingPubSubUid), i, ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}
	
	return verdict;
	}

CTestStepUnloadESockForced::CTestStepUnloadESockForced() :
	CTestStepUnloadESockBase(CTestStepUnloadESockBase::ENoLeak)
	{
	iTestStepName = _L("ESockUtilUnloadESockForced");
	}

	
CTestStepUnloadESockMayLeak::CTestStepUnloadESockMayLeak() :
	CTestStepUnloadESockBase(CTestStepUnloadESockBase::EMayLeak)
	{
	iTestStepName = _L("ESockUtilUnloadESock_MayLeak");
	}
	
CTestStepUnloadESockLeaks::CTestStepUnloadESockLeaks() :
	CTestStepUnloadESockBase(CTestStepUnloadESockBase::ELeaks)
	{
	iTestStepName = _L("ESockUtilUnloadESock_Leaks");
	}





CTestStepStopAllInterfaces::CTestStepStopAllInterfaces()
	{
	iTestStepName = _L("StopAllInterfaces");
	}

enum TVerdict CTestStepStopAllInterfaces::doTestStepL(void)
	{
	TVerdict verdict = EPass;

	TBuf<1000> errDesc;
	TInt err(ESockTestUtils::StopAllInterfaces(errDesc));
	if(err!=KErrNone)
		{
		verdict = EFail;
		Log(_L("StopAllInterfaces returned error %d:\n%S"), err, &errDesc);
		}
	return verdict;
	}
	
CTestStepDefaultToConfig1::CTestStepDefaultToConfig1()
	{
	iTestStepName = _L("DefaultToConfig1");
	}

enum TVerdict CTestStepDefaultToConfig1::doTestStepL(void)
	{
	ESockTestUtils::DefaultToConfig1L();
	return EPass;
	}
	
CTestStepAlwaysFail::CTestStepAlwaysFail()
	{
	iTestStepName = _L("AlwaysFail");
	}

enum TVerdict CTestStepAlwaysFail::doTestStepL(void)
	{
	return EFail;
	}




