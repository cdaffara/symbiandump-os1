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
#include "epos_cpospsyuidecomtest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYUidEComTest::CPosPSYUidEComTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYUidEComTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYUidEComTest* CPosPSYUidEComTest::NewL()
    {
    CPosPSYUidEComTest* self = new (ELeave) CPosPSYUidEComTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYUidEComTest::~CPosPSYUidEComTest()
    {
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYUidEComTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUidEComTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Uid-ECom Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYUidEComTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUidEComTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& /*aTestParameters*/)
    {   
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;

    TRAP(iTestStatus, CheckUidL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
    }

// ---------------------------------------------------------
// CPosPSYUidEComTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUidEComTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	iRequester->Cancel();

    CompleteTest(KErrCancel);
    }

// ---------------------------------------------------------
// CPosPSYUidEComTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUidEComTest::HandlePositionCompleteL(
	TInt aCompleteCode, 
	TTimeIntervalMicroSeconds /*aRequestTime*/)
    {
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		UnexpectedCompletionCode(aCompleteCode);
		}
	else
		{
		if (iPSYInfo.PSYBaseInfo().Uid() == iPosInfo.ModuleId())
			{
			_LIT(KInfo, "UID retrieved and it was correct.");
			AddTestResultL(KInfo, EInfoMessage);
			}
		else
			{
			_LIT(KError, "UID retrieved, incorrect: Correct UID = %d, Retrieved UID = %d.");
			TBuf<KUidEComTestBufSize> buf;
			buf.Format(KError, iPSYInfo.PSYBaseInfo().Uid().iUid, iPosInfo.ModuleId().iUid);
			AddTestResultL(buf, EErrorMessage);
			}
		}

	// Complete test
    CompleteTest(KErrNone);
	}

// ---------------------------------------------------------
// CPosPSYUidEComTest::CheckUidL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYUidEComTest::CheckUidL()
    {
	iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	iRequester->MakeRequest(iPosInfo);
    }

//  End of File
