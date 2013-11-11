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

#ifndef __TE_SIMVOICE_TESTSTEP_BASE__
#define __TE_SIMVOICE_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include "SimTsy.h"

_LIT(KSimtsyName,"SIM");

const TInt KLocalTel=32;
const TInt KRemoteTel=32;

_LIT(KGoodNumber,"1789");

#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (TestStepResult() != EPass ){\
		INFO_PRINTF1(_L("Failed Checkpoint:"));\
		INFO_PRINTF1(msg);\
		}\
	}

class CSimVoiceTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void SetState(TInt aState);

protected:
	void GetGoodNumber(TDes& aNum) ;
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	TInt CallMeDear() ;
	TInt HangMeUpDear() ;
	void Print_RCall_TStatus(RCall::TStatus& aArg) ;
	void Print_RLine_TCapsFlags(RLine::TCapsFlags& aArg);
	void Print_TCapsEnum(RCall::TCaps& aArg) ;
	void PrintCallCaps(TUint32 aCaps) ;
	void TestGetCallCapabilitiesL(RMobileCall& aCall) ;
	void TestLineCapabilitiesL() ;
	void Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg) ;

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;

	TBuf8<KLocalTel> iLocalNumber;		//< Local telephone number
	TBuf8<KRemoteTel> iRemoteNumber;	//< Remote telephone number (modem)

	TRequestStatus iStatus;			//< Request status used by asynchronous functions
	TRequestStatus iStatus2;
	TRequestStatus iStatus3;
	TRequestStatus iStatus4;
	TRequestStatus iStatus5;
	TRequestStatus iStatus6;
	TRequestStatus iStatus7;

	RMobileLine iLine;
	RMobileCall iVoiceCall;
	RCall::TCallInfo iCallInfo;
	RPhone::TLineInfo iLineInfo;
	RPhone::TCaps iPhoneCaps;
	TInt iLineIndex;
	TInt iState;
	};

#endif // __TE_SIMVOICE_TESTSTEP_BASE__
