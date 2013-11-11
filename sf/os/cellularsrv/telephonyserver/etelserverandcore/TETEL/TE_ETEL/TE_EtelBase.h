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

/**
 @file
 @internalComponent 
*/


#ifndef TE_ETELSUITE_H
#define TE_ETELSUITE_H


#include "ETEL.H"
#include "f32file.h"
#include "c32comm.h"

#include <e32base.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

#include "../DSTD/DDEF.H"
#include "../DSTD/DBDEF.H"
#include "../DSTD/DACQDEF.H"
#include "../DSTDNC/DNCDEF.H"
#include "../DSTDNC/didendef.h"
#include "../DSTDNC/dbaddef.h"

#define DSTDNC_NEWCALL_FULLNAME				_L("DummyNc::PotsNc::")

const TInt KDummyStringSize=30;
typedef TBuf<KDummyStringSize> TDummyString;
typedef TBuf8<KDummyStringSize> TDummyString8;
const TUint KDefaultHeapSize=0x4000;
const TUint KETelThreadShutdownGuardPeriod=500000;
const TInt KOOMTestNumberStart=239;	// The test count number at which OOM tests start and KErrNoMemory responses will be tolerated.



//
// Test macros
//
_LIT(KTestCheckFailText, "Failed: Got %d while expecting %d.");
_LIT(KTestCheckConditionFailText, "Failed: Condition was false.");
_LIT(KTestCheckStringFailText, "Failed: Got \"%S\" while expecting \"%S\".");

#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKSTRL(a, b) \
    { \
    TBuf<128>  bufferA; \
    TBuf<128>  bufferB; \
    \
    bufferA.Copy(a); \
    bufferB.Copy(b); \
    \
    if (bufferA != bufferB) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &bufferA, &bufferB); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    TBuf<128>  bufferA; \
    TBuf<128>  bufferB; \
    \
    bufferA.Copy(a); \
    bufferB.Copy(b); \
    \
    if (bufferA != bufferB) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &bufferA, &bufferB); \
        SetTestStepResult(EFail); \
        } \
    }


class CEtelTestStep : public CTestStep
{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	void ServerClose(RTelServer& aServer);

protected:
	RCommServ iCommServer;
	RTelServer iTelServer;

private:
	CActiveScheduler*  iScheduler;
};


class RTestPhone : public RPhone
	{
public:
	RTestPhone();

	void TestNarrowSetAndGet(TRequestStatus& aReqStatus, const TDesC8& aSet, TDes8& aGet) const;
	void TestUnicodeSetAndNarrowGet(TRequestStatus& aReqStatus, TDes8& aGet, const TDesC16& aSet) const;
	void TestUnicodeSetAndGet(TRequestStatus& aReqStatus, const TDesC16& aSet, TDes16& aGet) const;
	void TestNarrowSetAndUnicodeGet(TRequestStatus& aReqStatus, const TDesC8& aSet, TDes16& aGet) const;
	void TestDoubleUnicodeGet(TRequestStatus& aReqStatus, TDes16& aGet1, TDes16& aGet2) const;
	
	TInt TestCustomIPC(const TInt aCustomIpc);

	};


#endif
