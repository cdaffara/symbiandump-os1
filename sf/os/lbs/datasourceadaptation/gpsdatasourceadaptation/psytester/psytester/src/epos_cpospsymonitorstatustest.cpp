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
#include "epos_cpospsymonitorstatustest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMonitorStatusTest::CPosPSYMonitorStatusTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYMonitorStatusTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYMonitorStatusTest* CPosPSYMonitorStatusTest::NewL()
    {
    CPosPSYMonitorStatusTest* self = new (ELeave) CPosPSYMonitorStatusTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMonitorStatusTest::~CPosPSYMonitorStatusTest()
    {
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYMonitorStatusTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorStatusTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Monitor Status Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYMonitorStatusTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorStatusTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {   
	aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iRequestNumber = 0;
	iNumberOfRequests = aTestParameters.NumberOfLocationRequests();

    TRAP(iTestStatus, CheckStatusL());
    if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
	}

// ---------------------------------------------------------
// CPosPSYMonitorStatusTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorStatusTest::CancelTest()
    {   
	iTestStatus = KErrCancel;
	iRequester->Cancel();

	CompleteTest();
    }

// ---------------------------------------------------------
// CPosPSYMonitorStatusTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorStatusTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		UnexpectedCompletionCode(aCompleteCode, iRequestNumber);
		}
	
	if (iRequestNumber <= iNumberOfRequests)
        {
        iRequester->MakeRequest(iPosInfo);
		iRequestNumber++;
		}
	else
		{
		iRequester->ClosePositioner();

		TInt changes = 0;
		TBool statusReported = EFalse;
		TPositionModuleStatus moduleStatus;
		iRequester->StatusChanges(changes, statusReported, moduleStatus);

		if (!statusReported)
			{
			_LIT(KInfo, "Status updates seems not to be supported.");
			AddTestResultL(KInfo, EInfoMessage);
			}
		else
			{
			if (!(changes & EInitialisingDone))
				{
				_LIT(KWarning, "Module Status Initialising never reported.");
				AddTestResultL(KWarning, EWarningMessage);
				}
			if (!((changes & EReadyDone) || (changes & EActiveDone)))
				{
				_LIT(KError, "Module Status Ready or Active never reported.");
				AddTestResultL(KError, EErrorMessage);
				}
			// Can never happen as implementation of CPositionerExtension is as it is
			if ((changes & EDisabledDone)) 
				{
				_LIT(KError, "Module Status Disabled should not be reported by PSY.");
				AddTestResultL(KError, EErrorMessage);
				}
			if (moduleStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityUnknown || 
				moduleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceInactive)
				{
				_LIT(KError, "Data Quality Status Unknown and/or Device Status Inactive not reported when closing PSY.");
				AddTestResultL(KError, EErrorMessage);
				}
			}

		iRequester->ClearStatusHistory();		
		CompleteTest(KErrNone);
		}
	}

// ---------------------------------------------------------
// CPosPSYMonitorStatusTest::CheckStatusL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorStatusTest::CheckStatusL()
    {
    iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid(), ETrue);
    iRequester->MakeRequest(iPosInfo);
	iRequestNumber = 1;
    }

//  End of File
