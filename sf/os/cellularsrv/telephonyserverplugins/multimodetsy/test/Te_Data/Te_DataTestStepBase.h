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

#ifndef __TE_DATA_TESTSTEP_BASE__
#define __TE_DATA_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>

//Test Checking Macros
#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EXPR(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}

const TInt KLocalTel=32;
const TInt KRemoteTel=32;

/**
 * Test class that enables Te_Data tests.
 * It's a base class for all test steps.
 */

class CTestDataStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	CTestDataStep();
	static void CleanUp(TAny *aPtr) ;

	enum EPhoneNumbers { EGood, ELocal, ERemote, ESca} ;

protected:
	//Utils
	void GetPhoneNumberL(TDes& aNum, EPhoneNumbers aPhoneType) ;
	void WaitWithTimeoutL(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;

	void Print_RCall_TStatus(RCall::TStatus& aArg) ;
	void Print_TCapsEnum(RCall::TCaps& aArg) ;
	void TestLineCapabilitiesL() ;
	void Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg) ;
	void TestGetCallCapabilitiesL(RCall& aCall) ;
	void TestLineCapabilitiesL(RMobileLine& aLine) ;
	//>returns remote modem number
	void GetRemoteNumber(TDes& aNum){ GetPhoneNumberL(aNum, ERemote); } 
	//>returns local number
	void GetLocalNumber(TDes& aNum){ GetPhoneNumberL(aNum, ELocal); } 
	// TCleanupItem operator for objects of this class
	operator TCleanupItem() ;

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
	RMobileCall iDataCall;			  //< Voice call object
	RCall::TCallInfo iCallInfo;
	RPhone::TLineInfo iLineInfo;
	RPhone::TCaps iPhoneCaps;		  //< Phone capabalities
	TInt iLineIndex;
	};

#endif // __TE_DATA_TESTSTEP_BASE__
