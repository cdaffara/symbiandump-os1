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

#if (!defined TE_C32UTILSTEPS_H)
#define TE_C32UTILSTEPS_H

#include <test/testexecutestepbase.h>
#include "te_c32utilserver.h"
#include "c32loader.h"
#include <cfshared.h>

#define NUMBER_OF_PORTS (2) //< number of ports for this machine

#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

class CEasyTestStep : public CTestStep
/**
This class makes things easier, the test steps doesnt have to
explicitely call SetTestStepResult, they just return the result.
*/
	{
public:
	virtual TVerdict doTestStepPreambleL()
		{
		MergeVerdict(easyTestStepPreambleL());
		return TestStepResult();
		}
	virtual TVerdict doTestStepL()
		{
		MergeVerdict(easyTestStepL());
		return TestStepResult();
		}
	virtual TVerdict doTestStepPostambleL()
		{
		MergeVerdict(easyTestStepPostambleL());
		return TestStepResult();
		}

	virtual TVerdict easyTestStepPreambleL()
		{ return EPass; }

	virtual TVerdict easyTestStepL() = 0;
	virtual TVerdict easyTestStepPostambleL()
		{ return EPass; }

	void MergeVerdict(TVerdict aVerdict)
		{
		if(TestStepResult() == EPass)
			{
			SetTestStepResult(aVerdict);
			}
		}
	};

class CTestStepC32Util : public CTestStep, public MHarnessLogAdaptor
	{
public:
	void Log( TRefByValue<const TDesC16> format, ... );
	void Log( TRefByValue<const TDesC8> format, ... );
	};

class CTestStepLoadC32 : public CTestStepC32Util
	{
public:
	CTestStepLoadC32();
	virtual TVerdict doTestStepL();
	};

class CTestStepUnloadC32 : public CTestStepC32Util
	{
public:
    enum TLeak {ENoLeak, EMayLeak, ELeaks};
	CTestStepUnloadC32(CommsFW::TCFShutdownType aType, TLeak aLeak = ENoLeak);
	virtual TVerdict doTestStepL();
private:
	CommsFW::TCFShutdownType iType;
	TLeak iLeak;
	};
	
class CTestStepInjectMemLeakC32 : public CTestStepC32Util
	{
public:
	CTestStepInjectMemLeakC32();
	virtual TVerdict doTestStepL();
private:
    RComm iSerialPortList[NUMBER_OF_PORTS];  //< the Comm ports used here		
	};	

_LIT(KTestStepLoadC32,"TE_C32UtilLoadC32");
_LIT(KTestStepUnloadC32,"TE_C32UtilUnloadC32");
_LIT(KTestStepUnloadC32Forced,"TE_C32UtilUnloadC32Forced");
_LIT(KTestStepUnloadC32_Leaks,"TE_C32UtilUnloadC32_Leaks");
_LIT(KTestStepInjectMemLeakC32,"TE_C32UtilInjectMemLeakC32");

#endif // TE_C32UTILSTEPS_H


