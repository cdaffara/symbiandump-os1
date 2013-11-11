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
#include "epos_cpospsymultiplesessiontest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMultipleSessionTest::CPosPSYMultipleSessionTest() : 
	iNrOfInstances(0),
	iNrOfCancelingInstances(0),
	iNrOfStressInstances(0),
	iTimeToFixThreshold(1)
    {
    }

// EPOC default constructor can leave.
void CPosPSYMultipleSessionTest::ConstructL()
    {
    BaseConstructL();
    iMultiRequester = CPosPSYMultiRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYMultipleSessionTest* CPosPSYMultipleSessionTest::NewL()
    {
    CPosPSYMultipleSessionTest* self = new (ELeave) CPosPSYMultipleSessionTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMultipleSessionTest::~CPosPSYMultipleSessionTest()
    {
    iPositionArray.ResetAndDestroy();
    iPositionArray.Close();
    iTimeToFixArray.Reset();
    iTimeToFixArray.Close();
    iStatusArray.Reset();
    iStatusArray.Close();

	delete iMultiRequester;
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Multiple Session Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {   
	aStatus = KRequestPending;
	iRequestStatus = &aStatus;
	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iTestState = EMultipleSessions;

	iNrOfInstances = aTestParameters.NumberOfMultipleSessionTestInstances();
	iNrOfCancelingInstances = aTestParameters.NumberOfMultipleSessionCancelTestInstances();
	iNrOfStressInstances = aTestParameters.NumberOfStressTestInstances();
	iTimeToFixThreshold = aTestParameters.TimeToFixThreshold();

    TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::CancelTest()
    {   
	iTestStatus = KErrCancel;
	iMultiRequester->Cancel();

	CompleteTest();
	}

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::HandlePositionCompleteL(
	TInt /*aCompleteCode*/,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	GetResultL();
	
	if (iTestState == EMultipleSessions) 
		{
		CheckBundlingQueuingL();
		iTestState = EMultipleSessionsCancel;
		}
	else if (iTestState == EMultipleSessionsCancel)		
		{
		CheckCanceledL();
		iTestState = EStressTest;
		}
	else // EStressTest 
		{
		iTestState = EAllTested;
		}
	
	CheckAndClearResultsL();
	iMultiRequester->ClosePositioner();
	ExecuteTestL();
	}

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::ExecuteTestL()
    {
	if (iTestStatus == KErrCancel)
		{
		CancelTest();
		return;
		}

	if (iTestState == EMultipleSessions)
		{
		_LIT(KInfoMult, "=== Testing multiple sessions ===");
 		AddTestResultL(KInfoMult, EDisplayMessage);
		iNumberOfSessions = iNrOfInstances;
		TestMultipleSessionsL();
		}
	else if (iTestState == EMultipleSessionsCancel)
		{
		TBuf<KMessageBufSize> buf;
		_LIT(KInfoCancel, "=== Testing multiple sessions with cancel, canceling %d of %d requests ===");
		buf.Format(KInfoCancel, iNrOfCancelingInstances, iNrOfInstances);
		AddTestResultL(buf, EDisplayMessage);
		iNumberOfSessions = iNrOfInstances;
		CancelTestL();
		}
	else if (iTestState == EStressTest)
		{
		_LIT(KInfoStress, "=== Stress testing multiple sessions ===");
		AddTestResultL(KInfoStress, EDisplayMessage);
		iNumberOfSessions = iNrOfStressInstances;
		TestMultipleSessionsL();
		}
	else // EAllTested
		{
		CompleteTest(KErrNone);
		}
	}

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::TestMultipleSessionsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::TestMultipleSessionsL()
    {
	iMultiRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid(), iNumberOfSessions);
    iMultiRequester->MakeRequestsL();
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CancelTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultipleSessionTest::CancelTestL()
    {
	iMultiRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid(), iNumberOfSessions);
    
	for (TInt i = 0; i < iNrOfCancelingInstances; i++)
		{
		iMultiRequester->SetRequesterWillCancelL(i, ETrue);	
		}

    iMultiRequester->MakeRequestsL();
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckTimeToFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYMultipleSessionTest::CheckTimeToFixL()
    {
    TInt toLongTimeNr = 0;
    TBuf<KMessageBufSize> buf;
    for (TInt i = 0; i < iTimeToFixArray.Count(); i++)
        {
        if (iTimeToFixArray[i] > iPSYInfo.TimeToFix())
            {
            toLongTimeNr++;
            }
        }

	_LIT(KInfo, "%d of %d TimeToFix values are greater than specified value.");
    buf.Format(KInfo, toLongTimeNr, iTimeToFixArray.Count());
    
	if (toLongTimeNr > (iTimeToFixArray.Count() / iTimeToFixThreshold))
        {   
        AddTestResultL(buf, EWarningMessage);
        }
    else
        {
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckStatusesL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::CheckStatusesL()
    {
    TBuf<KMessageBufSize> buf;
    TInt nrOfCancel = 0;
    TInt nrOfKErrNone = 0;
    TInt nrOfOthers = 0;
	for (TInt i = 0; i < iStatusArray.Count(); i++)
        {
        if (iStatusArray[i].Int() == KErrCancel)
            {
            nrOfCancel++;
            }
        else if (iStatusArray[i].Int() == KErrNone)
            {
            nrOfKErrNone++;
            }
        else
            {
            nrOfOthers++;
			}
        }
    if (nrOfCancel > 0)
        {
        _LIT(KInfo, "%d of %d requests returned with KErrCancel.");
        buf.Format(KInfo, nrOfCancel, iStatusArray.Count());
        AddTestResultL(buf, EInfoMessage);
        }
    if (nrOfKErrNone > 0)
        {
        _LIT(KInfo, "%d of %d requests returned with KErrNone.");
        buf.Format(KInfo, nrOfKErrNone, iStatusArray.Count());
        AddTestResultL(buf, EInfoMessage);
        }
    if (nrOfOthers > 0)
        {
        _LIT(KWarning, "%d of %d requests returned did not return with either KErrCancel or KErrNone.");
        buf.Format(KWarning, nrOfOthers, iStatusArray.Count());
        AddTestResultL(buf, EWarningMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckBundlingQueuingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::CheckBundlingQueuingL()
    {
    TBool bundlingUsed = EFalse;
    for (TInt i = 0; i < iPositionArray.Count()-1; i++)
        {
        TPosition pos1;
        TPosition pos2;
        iPositionArray[i]->GetPosition(pos1);
        iPositionArray[i+1]->GetPosition(pos2);

        if (pos1.Time() == pos2.Time() &&
			pos1.Time().Int64() != 0)
            {
            bundlingUsed = ETrue;
            }
        }

    if (bundlingUsed)
        {   
        _LIT(KInfo, "PSY probably uses Request Bundling.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "PSY probably uses Request Queuing.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckUidL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::CheckUidL()
    {
    TInt nrOfWrongUidNr = 0;
    TBuf<KMessageBufSize> buf;
    for (TInt i = 0; i < iPositionArray.Count(); i++)
        {
		if (iPositionArray[i]->ModuleId().iUid != iPSYInfo.PSYBaseInfo().Uid().iUid)
            {
            nrOfWrongUidNr++;
            }
        }
    if (nrOfWrongUidNr > 0)
        {   
        _LIT(KError, "%d of %d requests returned wrong UID number.");
        buf.Format(KError, nrOfWrongUidNr, iPositionArray.Count());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "All requests returned correct UID number.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckAndClearResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::CheckAndClearResultsL()
    {
    CheckTimeToFixL();
    CheckUidL();
    CheckStatusesL();
    
    iPositionArray.ResetAndDestroy();
    iTimeToFixArray.Reset();
    iStatusArray.Reset();
    }

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::GetResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::GetResultL()
	{
	for (TInt i = 0; i < iNumberOfSessions; i++)
		{
		TPositionInfo* info = new (ELeave) TPositionInfo;
		CleanupStack::PushL(info);
		TInt status(KErrGeneral);
		TTimeIntervalMicroSeconds requestTime;
    
		iMultiRequester->GetResult(status, requestTime, *info, i);
		
		// Check for unexpected completion codes
		if (status != KErrNone && status != KErrCancel && 
			status < 0)
			{
			CleanupStack::PopAndDestroy(info);
			UnexpectedCompletionCode(status);
			}
		else
			{
			User::LeaveIfError(iTimeToFixArray.Append(requestTime));
			User::LeaveIfError(iPositionArray.Append(info));
			CleanupStack::Pop(info);
			}
		
		User::LeaveIfError(iStatusArray.Append(status));
		}
	}

// ---------------------------------------------------------
// CPosPSYMultipleSessionTest::CheckCanceledL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYMultipleSessionTest::CheckCanceledL()
	{
	TInt nrOfCanceledInstances = iMultiRequester->GetNumberOfCanceledRequests();
		
	if (nrOfCanceledInstances != iNrOfCancelingInstances)
		{
		_LIT(KWarning, "%d of %d requests was completed before cancel was done.");
		TBuf<KMessageBufSize> buf;
		buf.Format(KWarning, iNrOfCancelingInstances - nrOfCanceledInstances, iNrOfCancelingInstances);
		AddTestResultL(buf, EWarningMessage);
		}
	}

//  End of File
