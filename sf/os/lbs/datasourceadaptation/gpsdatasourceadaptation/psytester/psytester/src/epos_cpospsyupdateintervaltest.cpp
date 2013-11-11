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
#include "epos_cpospsyupdateintervaltest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYUpdateIntervalTest::CPosPSYUpdateIntervalTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYUpdateIntervalTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYUpdateIntervalTest* CPosPSYUpdateIntervalTest::NewL()
    {
    CPosPSYUpdateIntervalTest* self = new (ELeave) CPosPSYUpdateIntervalTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYUpdateIntervalTest::~CPosPSYUpdateIntervalTest()
    {
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYUpdateIntervalTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUpdateIntervalTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Update Interval Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYUpdateIntervalTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUpdateIntervalTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {   
    aStatus = KRequestPending;
	iNrOfUpdateIntervalRequests = aTestParameters.NumberOfLocationRequests();
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
// CPosPSYUpdateIntervalTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUpdateIntervalTest::CancelTest()
    {   
	iTestStatus = KErrCancel;
	iRequester->Cancel();

	CompleteTest();
    }

// ---------------------------------------------------------
// CPosPSYUpdateIntervalTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUpdateIntervalTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		UnexpectedCompletionCode(aCompleteCode);
		}
	
	if (iRequestNumber < iNrOfUpdateIntervalRequests)
        {
        iRequestNumber++;
		iRequester->MakeRequest(iPosInfo);
        }
	else 
		{
		iRequester->StopTracking();
		iRequestNumber = 0;
		CompleteTest(KErrNone);
		}
	}

// ---------------------------------------------------------
// CPosPSYUpdateIntervalTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUpdateIntervalTest::ExecuteTestL()
    {
	TBool trackingSupported = EFalse;
	iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	
	trackingSupported = iRequester->TrackingOverridden();
    
	if (trackingSupported)
		{
		_LIT(KInfo, "Update Interval (Tracking) supported.");
		AddTestResultL(KInfo, EInfoMessage);
		TRAPD(err, iRequester->StartTrackingL(KTrackingTime));
		
		if (err == KErrNotSupported)
			{
			_LIT(KNotSupported, "PSY supports tracking but does not override StartTracking method.");
			AddTestResultL(KNotSupported, EErrorMessage);
			}
		else if (err != KErrNone)
			{
			_LIT(KLeave, "StartTracking method leaved with error code %d.");
			TBuf<KBufSize> buf;
			buf.Format(KLeave, err);
			AddTestResultL(buf, EErrorMessage);
			}

		iRequester->MakeRequest(iPosInfo);
		}
	else
		{
		_LIT(KInfo, "Update Interval (Tracking) not supported.");
		AddTestResultL(KInfo, EInfoMessage);
		CompleteTest(KErrNone);
		}
    }

//  End of File
