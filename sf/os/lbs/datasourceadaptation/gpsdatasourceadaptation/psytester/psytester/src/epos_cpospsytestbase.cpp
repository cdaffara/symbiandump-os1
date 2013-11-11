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
#include "epos_cpospsytestbase.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYTestBase::CPosPSYTestBase()
    {
    }

void CPosPSYTestBase::BaseConstructL()
    {
    iTestResults = CPosPSYResults::NewL();
    }

// Destructor
CPosPSYTestBase::~CPosPSYTestBase()
    {
    delete iTestResults;
    }

// ---------------------------------------------------------
// CPosPSYTestBase::CompleteTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::CompleteTest()
    {
	CompleteTest(iTestStatus);
	}

// ---------------------------------------------------------
// CPosPSYTestBase::CompleteTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::CompleteTest(TInt aCompleteCode)
    {
	if (iRequester)
		{
		iRequester->ClosePositioner();
		}
	if (iMultiRequester)
		{
		iMultiRequester->ClosePositioner();
		}

	if (iRequestStatus)
		{
		User::RequestComplete(iRequestStatus, aCompleteCode);
		}
	}

// ---------------------------------------------------------
// CPosPSYTestBase::UnexpectedCompletionCode
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::UnexpectedCompletionCode(TInt aCompleteCode)
    {
	UnexpectedCompletionCode(aCompleteCode, 0, EWarningMessage);
	}

// ---------------------------------------------------------
// CPosPSYTestBase::UnexpectedCompletionCode
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::UnexpectedCompletionCode(
	TInt aCompleteCode, 
	TInt aRequestNumber,
	TInt aMessageType)
    {
	TBuf<100> buf;
	if (aRequestNumber == 0)
		{
		_LIT(KText, "Location request from PSY returned with error code %d.");
		buf.Format(KText, aCompleteCode);
		}
	else 
		{
		_LIT(KText, "Location request %d from PSY returned with error code %d.");
		buf.Format(KText, aRequestNumber, aCompleteCode);
		}
	TInt err = KErrNone;
	TRAP(err, AddTestResultL(buf, aMessageType));
	}

// ---------------------------------------------------------
// CPosPSYTestBase::ClearResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::ClearResults()
    {
    iTestResults->ClearResults();
    }

// ---------------------------------------------------------
// CPosPSYTestBase::TestResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosPSYResults* CPosPSYTestBase::TestResults()
    {
    return iTestResults;
    }

// ---------------------------------------------------------
// CPosPSYTestBase::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::HandlePositionCompleteL(
	TInt /*aCompleteCode*/,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	}

// ---------------------------------------------------------
// CPosPSYTestBase::HandleRequestError
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::HandleRequestError(TInt aErrorCode)
    {
	if (iRequestStatus) 
		{
		UnexpectedCompletionCode(aErrorCode, 0, EErrorMessage);

		iTestStatus = KErrCancel;
		CompleteTest(aErrorCode);
		}
	}

// ---------------------------------------------------------
// CPosPSYTestBase::AddTestPSYResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::AddTestPSYResultL(
    const CPosPSYResult& aTestResult)
    {
    iTestResults->AddResultL(aTestResult);
    }

// ---------------------------------------------------------
// CPosPSYTestBase::AddTestResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestBase::AddTestResultL(
	const TDesC& aMessage, 
    const TInt aResultType)
    {
    CPosPSYResult* result = CPosPSYResult::NewL(aMessage, aResultType);
    CleanupStack::PushL(result);
    AddTestPSYResultL(*result);
    CleanupStack::Pop(result);
	}

//  End of File
