// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "epos_cpospsycrcapabilitytest.h"
#include <f32file.h>
#include <bautils.h>

// Constant Definition
_LIT(KCRTesterExe, "PSYCRTester.exe");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCRCapabilityTest::CPosPSYCRCapabilityTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYCRCapabilityTest::ConstructL()
    {
    BaseConstructL();
    }

CPosPSYCRCapabilityTest* CPosPSYCRCapabilityTest::NewL()
    {
    CPosPSYCRCapabilityTest* self = new (ELeave) CPosPSYCRCapabilityTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYCRCapabilityTest::~CPosPSYCRCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCRCapabilityTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "PSY Central Repository Capability Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCRCapabilityTest::StartTest(
    const TPosPSYInfo&  aPSYInfo,
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& /* aTestParameters */)
    {
    iPSYInfo = aPSYInfo;
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;
	iTestStatus = KErrNone;
        
    TRAP(iTestStatus, ExecuteTestL());
    if (iTestStatus == KErrNone)
    {
    	CompleteTest();	
    }
	}

// ---------------------------------------------------------
// CPosPSYParameterTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCRCapabilityTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	}

// ---------------------------------------------------------
// CPosPSYParameterTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCRCapabilityTest::ExecuteTestL()
    {
    	// Retrieve the UID of the PSY
    	TPosPSYBaseInfo basePSYInfo = iPSYInfo.PSYBaseInfo();
    	TInt psyUID = basePSYInfo.Uid().iUid;
    	    	    	  	
    	TBuf<10> arg;
    	arg.Num(psyUID);
    	
    	RProcess crTestProc;

        TFileName crTesterExe;
        crTesterExe.Append(KCRTesterExe);

    	TInt error = crTestProc.Create(crTesterExe, arg);
    	if(error == KErrNone)
    	{
	    	crTestProc.Resume();
	    	
	    	TRequestStatus status;
	    	crTestProc.Rendezvous(status);
	    	
	    	User::WaitForRequest(status);	
    	}
    }


//  End of File
