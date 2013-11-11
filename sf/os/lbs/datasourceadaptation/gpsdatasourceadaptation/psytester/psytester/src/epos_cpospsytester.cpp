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
#include "epos_cpospsytester.h"
#include "epos_cpospsycrcapabilitytest.h"
#include "epos_cpospsyparametertest.h"
#include "epos_cpospsymaxagetest.h"
#include "epos_cpospsypartialupdatetest.h"
#include "epos_cpospsymemorytest.h"
#include "epos_cpospsyuidecomtest.h"
#include "epos_cpospsycancelrequesttest.h"
#include "epos_cpospsylocationrequesttest.h"
#include "epos_cpospsyupdateintervaltest.h"
#include "epos_cpospsymonitorstatustest.h"
#include "epos_cpospsymultiplesessiontest.h"
#include "epos_cpospsymonitorthreadtest.h"
#include "epos_cpospsyclearpositiondatatest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYTester::CPosPSYTester() : 
	CActive(CActive::EPriorityStandard), iTestIndex(0)
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosPSYTester::ConstructL()
    {
    iLog = CPosPSYLog::NewL();
	
	// The CR capability test must be the first test. This test invokes an executable, which also accesses the log file,
	// which will not be open by the main test executable.
	AddTestL(CPosPSYCRCapabilityTest::NewL());
    AddTestL(CPosPSYParameterTest::NewL());
    AddTestL(CPosPSYMaxAgeTest::NewL());
    AddTestL(CPosPSYMemoryTest::NewL());
    AddTestL(CPosPSYPartialUpdateTest::NewL());
    AddTestL(CPosPSYUidEComTest::NewL());
    AddTestL(CPosPSYCancelRequestTest::NewL());
    AddTestL(CPosPSYLocationRequestTest::NewL());
    AddTestL(CPosPSYUpdateIntervalTest::NewL());
    AddTestL(CPosPSYMonitorStatusTest::NewL());
    AddTestL(CPosPSYClearPositionDataTest::NewL());
    AddTestL(CPosPSYMonitorThreadTest::NewL());
    AddTestL(CPosPSYMultipleSessionTest::NewL());
    }

// Two-phased constructor.
CPosPSYTester* CPosPSYTester::NewL()
    {
    CPosPSYTester* self = new (ELeave) CPosPSYTester;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYTester::~CPosPSYTester()
    {
    Cancel();
    delete iLog;
    iTestArray.ResetAndDestroy();
	iTestArray.Close();
    }

// ---------------------------------------------------------
// CPosPSYTester::StartTestsL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTester::StartTestsL(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {
    aStatus = KRequestPending;
    iHandlerStatus = &aStatus;
	iPSYTestParameters = aTestParameters;
    
    iPSYInfo = aPSYInfo;
 
	// Pass an additional paramater, aTestParameters to this method
    // aTestParameters contains the PSY Tester parameters
    // specified in the resource file.
    iLog->CreateLogL(iPSYInfo, aTestParameters);
    
    TBuf<KTestNameLength> name;
    iTestArray[iTestIndex]->GetName(name);
    iLog->MakeTestCaseHeaderL(name);
    
    // Close the Log file and start the CR Capability Test (which must be the first test added).
    iLog->CloseLog();
    iTestArray[iTestIndex]->StartTest(iPSYInfo, iStatus, iPSYTestParameters);
    // Reopen the log file
    iLog->OpenLog();

    SetActive();
    }

// ---------------------------------------------------------
// CPosPSYTester::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTester::RunL()
    {
    if (iStatus != KErrCancel)
        {
		if (iStatus != KErrNone)
			{
			_LIT(KError, "Test completed with error code %d.");
			TBuf<40> buf;
			buf.Format(KError, iStatus.Int());
			iTestArray[iTestIndex]->AddTestResultL(buf, EErrorMessage);
			}
        for (TInt i=0; i < iTestArray[iTestIndex]->TestResults()->NumberOfResults(); i++)
            {
            iLog->AppendResultL(*iTestArray[iTestIndex]->TestResults()->GetResult(i));
            }
        iTestArray[iTestIndex]->ClearResults();
        
        if (iTestArray.Count() > iTestIndex+1)
            {
			iTestIndex++;
            TBuf<KTestNameLength> name;
            iTestArray[iTestIndex]->GetName(name);
            iLog->MakeTestCaseHeaderL(name);
            iTestArray[iTestIndex]->StartTest(iPSYInfo, iStatus, iPSYTestParameters);
            SetActive();
            }
        else
            {
			iLog->WriteSummaryAndCloseL(iStatus.Int());
            iTestIndex = 0;
            User::RequestComplete(iHandlerStatus, KErrNone);
            }
        }
    else // check if this should be else if
        {
        iLog->WriteSummaryAndCloseL(iStatus.Int());
        iTestIndex = 0;
        User::RequestComplete(iHandlerStatus, iStatus.Int());
        }
    }

// ---------------------------------------------------------
// CPosPSYTester::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTester::DoCancel()
    {
    iTestArray[iTestIndex]->CancelTest();

    TInt err = KErrNone;
    
    TRAP(err, iLog->WriteSummaryAndCloseL(KErrCancel));
    iTestIndex = 0;
	if (iHandlerStatus)
		{
		User::RequestComplete(iHandlerStatus, KErrCancel);
		}
	}

// ---------------------------------------------------------
// CPosPSYTester::RunError
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYTester::RunError(TInt aErr)
    {
	if (iHandlerStatus)
		{
		User::RequestComplete(iHandlerStatus, aErr);
		}
    return KErrNone;
    }

// ---------------------------------------------------------
// CPosPSYTester::AddTestL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTester::AddTestL(
	CPosPSYTestBase* aTest)
    {
	CleanupStack::PushL(aTest);
	User::LeaveIfError(iTestArray.Append(aTest));
	CleanupStack::Pop(aTest);
    }

//  End of File
