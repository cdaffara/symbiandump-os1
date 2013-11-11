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

#include <c32comm.h>
#include <test/testexecutelog.h>
#include "te_c32utilsteps.h"

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

void CTestStepC32Util::Log(TRefByValue<const TDesC16> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow16 truncateOverflow;
	TBuf<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}

void CTestStepC32Util::Log(TRefByValue<const TDesC8> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow8 truncateOverflow;
	TBuf8<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}



CTestStepLoadC32::CTestStepLoadC32()
	{
	SetTestStepName(KTestStepLoadC32);
	}

/* virtual */ TVerdict CTestStepLoadC32::doTestStepL()
	{
	TVerdict verdict = EPass;

	TInt err(C32Loader::LoadC32(this));
	if(err!=KErrNone)
		{
		verdict = EFail;
		INFO_PRINTF2(_L("LoadC32 returned error %d"), err);
		}
	return verdict;
	}

CTestStepUnloadC32::CTestStepUnloadC32(CommsFW::TCFShutdownType aType, TLeak aLeak):
	iType(aType),
	iLeak(aLeak)
	{
	SetTestStepName(KTestStepUnloadC32);
	}

/* virtual */ TVerdict CTestStepUnloadC32::doTestStepL()
	{
	TInt err(C32Loader::UnloadC32(iType, this));

	switch (err)		
	{
		case KErrNone:
			{
			if (iLeak == ELeaks)
				{
				INFO_PRINTF1(_L("We were expecting a leak and non occurred : check that the leak-detecion mechanism is working."));
				SetTestStepResult(EFail);
				}
			break;
			}
		case KErrSessionClosed:
			{
			INFO_PRINTF2(_L("UnloadC32 returned %d. C32 had still had unclosed sessions when exiting."), err);
			SetTestStepResult(EFail);
			break;
			}
		case C32Loader::KModuleLeaked:
			{
			INFO_PRINTF1(_L("A memory leak was detected on shutdown."));
			INFO_PRINTF1(_L("Expect leaks if a panic occurred - Check the epocwind.out."));
			if (iLeak == ENoLeak)
				{										
				SetTestStepResult(EFail);
				}
			break;		
			}	
		default:
			{
			INFO_PRINTF2(_L("UnloadC32 returned error %d"), err);
			SetTestStepResult(EFail);
			}
	}
	
	return TestStepResult();
	}

CTestStepInjectMemLeakC32::CTestStepInjectMemLeakC32()
	{
	SetTestStepName(KTestStepInjectMemLeakC32);
	}

/* virtual */ TVerdict CTestStepInjectMemLeakC32::doTestStepL()
	{
	// inject memory leak to test memory leak detection mechanism working:
	
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KMemLeakToken, "--MemoryLeakToken--");	
	
	RCommServ commServ;
	
	TInt ret=commServ.Connect();
	TESTCHECKL(ret,KErrNone);
	
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	ret=iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
		
	TRequestStatus stat0;
	
	TBuf8<20> writeBuf;
	writeBuf.Append(KMemLeakToken);
	
	iSerialPortList[1].Write(stat0,writeBuf);
	
	User::WaitForRequest(stat0);
	TESTCHECKL(stat0.Int(), KErrNone);
				
	iSerialPortList[1].Close();
	
	commServ.Close();				
	
	return TestStepResult();
	}

