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
#include "epos_cpospsycancelrequesttest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCancelRequestTest::CPosPSYCancelRequestTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYCancelRequestTest::ConstructL()
    {
    BaseConstructL();
    iRequester = CPosPSYRequester::NewL(this);
    iMultiRequester = CPosPSYMultiRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYCancelRequestTest* CPosPSYCancelRequestTest::NewL()
    {
    CPosPSYCancelRequestTest* self = new (ELeave) CPosPSYCancelRequestTest;
    CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYCancelRequestTest::~CPosPSYCancelRequestTest()
    {
	delete iRequester;
	delete iMultiRequester;
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Cancel Request Test");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& /*aTestParameters*/)
    {   
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;

    TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	iMultiRequester->Cancel();
	iMultiRequester->ClosePositioner();

	CompleteTest();
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone && 
		aCompleteCode != KErrCancel)
		{
		UnexpectedCompletionCode(aCompleteCode, iRequestNumber);
		}

    TBuf<KBufSize> buf;
    if (iMultiRequester->GetNumberOfCanceledRequests() == 0)
        {
        if (aCompleteCode == KErrNone)
			{
			_LIT(KWarning, "Location request was completed before cancel request was completed. Status = %d.");
			buf.Format(KWarning, aCompleteCode);
			AddTestResultL(buf, EWarningMessage);
			}
		else
			{
			_LIT(KError, "Error occured before cancel request was completed. Status = %d.");
			buf.Format(KError, aCompleteCode);
			AddTestResultL(buf, EErrorMessage);
			}
        }
    else
        {
        if (aCompleteCode != KErrCancel)
            {
            _LIT(KError, "Cancel of request did not complete with KErrCancel. Status = %d.");
            buf.Format(KError, aCompleteCode);
            AddTestResultL(buf, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Cancel of request worked properly, request completed with KErrCancel.");
            buf.Format(KInfo, aCompleteCode);
            AddTestResultL(buf, EInfoMessage);
            }
		}

	iMultiRequester->ClosePositioner();
	CompleteTest(KErrNone);
	}

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::ExecuteTestL()
    {
    TestCancelWithoutPreceedingRequestL();
    TestCancelRequestL();
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::TestCancelRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::TestCancelRequestL()
    {
	iMultiRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	iMultiRequester->SetRequesterWillCancelL();
    iMultiRequester->MakeRequestsL();
    }

// ---------------------------------------------------------
// CPosPSYCancelRequestTest::TestCancelWithoutPreceedingRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCancelRequestTest::TestCancelWithoutPreceedingRequestL()
    {
    iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	iRequester->CancelRequest();
	iRequester->ClosePositioner();
    }

//  End of File
