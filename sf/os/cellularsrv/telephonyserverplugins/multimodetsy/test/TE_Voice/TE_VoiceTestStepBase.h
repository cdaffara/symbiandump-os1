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
// @file
// Voice API Test base header file
// 
//

#ifndef _TE_VOICETESTSTEPBASE_H_
#define _TE_VOICETESTSTEPBASE_H_

#include <etel.h>
#include <etelmm.h>

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

//
// Test macros
//
#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EXPR(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}

// Taken from old TS_Voice
#define TEST_START(p1)	\
	{\
	INFO_PRINTF2(_L("======Start test %S"), p1);\
	}
	
#define TEST_END()	\
	{\
	INFO_PRINTF1(_L("======End test =====")); \
	}
	
// Test macros taken from TestExecute base class
//
// Test macros
//
#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &(a),&(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &(a),&(b)); \
        SetTestStepResult(EFail); \
        } \
    }

const TInt KLocalTel=32;
const TInt KRemoteTel=32;

_LIT(KServerName,"TE_Voice");
_LIT(KMmtsyName, "MM");
_LIT(KPhoneName,  "GsmPhone1");
_LIT(KGoodNumber,"1789");

/**
 * Test class that enables GSM Voice  tests.
 * It's a base class for all test steps.
 * CTestVoice inherits from the CBase class.
 */
class CTestVoiceStep : public CTestStep
	{
public:
	~CTestVoiceStep();
	CTestVoiceStep();
	void SetPhone(RMobilePhone aPhone)
		{iPhone = aPhone ;}
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

protected:
	//Utils
	void GetGoodNumber(TDes& aNum) ;
	void PrintCallCaps(TUint32 aCaps);
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	void Print_RCall_TStatus(RCall::TStatus& aArg);
	void Print_TCapsEnum(RCall::TCaps& aArg);
	void Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg) ;
	void TestLineCapabilitiesL() ;
	//Call tests
	void TestGetCallCapabilitiesL(RMobileCall& aCall);

protected:
	TBuf8<KLocalTel> iLocalNumber;		//< Local telephone number
	TBuf8<KRemoteTel> iRemoteNumber;	//< Remote telephone number (modem)
	TRequestStatus iStatus;				//< Request status used by asynchronous functions
	TRequestStatus iStatus2;
	TRequestStatus iStatus3;
	TRequestStatus iStatus4;
	TRequestStatus iStatus5;
	TRequestStatus iStatus6;
	TRequestStatus iStatus7;

	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobileLine iLine;				  //< Telephone line
	RMobileCall iVoiceCall;			  //< Voice call object
	RCall::TCallInfo iCallInfo;
	RPhone::TLineInfo iLineInfo;
	RPhone::TCaps iPhoneCaps;		  //< Phone capabalities
	TInt iLineIndex;
	
private:
	TProcessPriority  iOrgProcessPriority;
	TThreadPriority  iOrgThreadPriority;
	};

#endif // _TE_VOICETESTSTEPBASE_H_

