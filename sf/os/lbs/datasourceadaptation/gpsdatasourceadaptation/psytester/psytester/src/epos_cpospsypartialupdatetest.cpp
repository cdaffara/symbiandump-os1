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
#include "epos_cpospsypartialupdatetest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYPartialUpdateTest::CPosPSYPartialUpdateTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYPartialUpdateTest::ConstructL()
    {
	BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYPartialUpdateTest* CPosPSYPartialUpdateTest::NewL()
    {
    CPosPSYPartialUpdateTest* self = new (ELeave) CPosPSYPartialUpdateTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYPartialUpdateTest::~CPosPSYPartialUpdateTest()
    {
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYPartialUpdateTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Partial Update Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYPartialUpdateTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;

	iTotalNumberOfRequests = aTestParameters.NumberOfPartialUpdateRequests();
	iNumberOfRequests = 0;
	iPartialUpdates = 0;

	TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
	}

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYPartialUpdateTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	iRequester->Cancel();

	CompleteTest();
	}

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYPartialUpdateTest::HandlePositionCompleteL(
	TInt aCompleteCode, TTimeIntervalMicroSeconds /*aRequestTime*/)
    {
	iNumberOfRequests++;

	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone && 
		aCompleteCode != KPositionPartialUpdate)
		{
		UnexpectedCompletionCode(aCompleteCode, iNumberOfRequests, EWarningMessage);
		}

	TBuf<KBufSize> buf;
	if (aCompleteCode == KPositionPartialUpdate && !IsFullPositionFix(iPosInfo))
		{
		// OK, partial
		iPartialUpdates++;

		// Log info remark
		_LIT(KPartOk, "Request %d: Received a correct partial update.");
		buf.Format(KPartOk(), iNumberOfRequests);
		AddTestResultL(buf, EInfoMessage);
		}
	else if (aCompleteCode == KPositionPartialUpdate && IsFullPositionFix(iPosInfo))
		{
		// Log error remark
		_LIT(KPartError, "Request %d: Received a partial update with full fix.");
		buf.Format(KPartError(), iNumberOfRequests);
		 AddTestResultL(buf, EErrorMessage);
		}
	else if (aCompleteCode == KErrNone && !IsFullPositionFix(iPosInfo))
		{
		// NOT OK, not full fix when reporting KErrNone
		// Log error remark
		_LIT(KNotFullFixError, "Request %d: Received status KErrNone and not a full fix.");
		buf.Format(KNotFullFixError(), iNumberOfRequests);
		AddTestResultL(buf, EErrorMessage);			
		}
	// Don't care about KErrNone and full fix

	if (iNumberOfRequests == iTotalNumberOfRequests)
		{
		// Log number of partial updates
		_LIT(KNrPartialUpdates, "Number of partial updates: %d out of %d requests.");
		TBuf<50> bufStack;
		bufStack.Format(KNrPartialUpdates(), iPartialUpdates, iNumberOfRequests);
		AddTestResultL(bufStack, EInfoMessage);

		CompleteTest(KErrNone);
		}
	else
		{
		iRequester->MakeRequest(iPosInfo);
		}
	}

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYPartialUpdateTest::ExecuteTestL()
    {
	if (iTotalNumberOfRequests <= 0)
		{
		_LIT(KNoTests, "No partial update tests performed because number of partial update location requests is 0 or less.");
		AddTestResultL(KNoTests, EInfoMessage);
		CompleteTest(KErrNone);
		}
	else
		{
		iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
		iRequester->SetPartialUpdateAllowed(ETrue);
		iRequester->MakeRequest(iPosInfo);
		}
	}

// ---------------------------------------------------------
// CPosPSYPartialUpdateTest::IsFullPositionFix
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYPartialUpdateTest::IsFullPositionFix(
	TPositionInfoBase& aPosInfo)
	{
	TPositionInfo& posInfo = static_cast<TPositionInfo&> (aPosInfo);
	TPosition pos;
	posInfo.GetPosition(pos);

	// Check if time is set
	// Check if longitude is a number
	// Check if latitude is a number
	if (TInt(pos.Time().Int64()) == 0 || 
		Math::IsNaN(TRealX(pos.Longitude())) ||
		Math::IsNaN(TRealX(pos.Latitude())))
		{
		return EFalse;
		}

	return ETrue;
	}

//  End of File
