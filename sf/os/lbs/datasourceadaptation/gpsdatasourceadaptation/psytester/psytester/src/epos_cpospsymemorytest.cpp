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
#include "epos_cpospsymemorytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMemoryTest::CPosPSYMemoryTest()
    {
	} 

// EPOC default constructor can leave.
void CPosPSYMemoryTest::ConstructL()
    {
	BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYMemoryTest* CPosPSYMemoryTest::NewL()
    {
    CPosPSYMemoryTest* self = new (ELeave) CPosPSYMemoryTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMemoryTest::~CPosPSYMemoryTest()
    {
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYMemoryTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Memory Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYMemoryTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iLocalUsedHeap = 0;

	iNumberOfRequests = aTestParameters.NumberOfMemoryRequests();
	iErrorMemoryPercentLevel = aTestParameters.HeapSizePercentageLevel();

	TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}
	}

// ---------------------------------------------------------
// CPosPSYMemoryTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	iRequester->Cancel();

	CompleteTest();
	}

// ---------------------------------------------------------
// CPosPSYMemoryTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	if (iRequestNumber == 1) 
		{
		TInt tmp;
		GetMemoryFootPrint(iHeapAlloc, iHeapUsed, tmp, tmp, iStackSize);

		iSizeAfterStart = iHeapUsed - iBeforeHeapUsed;
		iHeapMin = iHeapUsed;
		iHeapMax = iHeapUsed;
		}

	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		TInt tmp;
		TInt heapBefore;
		TInt heapAfter;
		GetMemoryFootPrint(tmp, heapBefore, iHeapMin, iHeapMax, tmp);
		UnexpectedCompletionCode(aCompleteCode, iRequestNumber);
		GetMemoryFootPrint(tmp, heapAfter, iHeapMin, iHeapMax, tmp);
		iLocalUsedHeap += heapAfter - heapBefore;
		}

	if (iRequestNumber < iNumberOfRequests)
		{
		GetMemoryFootPrint(iHeapAlloc, iHeapUsed, iHeapMin, iHeapMax, iStackSize);

		iRequester->MakeRequest(iPosInfo);
		iRequestNumber++;
		}
	else 
		{
		GetMemoryFootPrint(iHeapAlloc, iHeapUsed, iHeapMin, iHeapMax, iStackSize);

		iHeapMax -= iLocalUsedHeap;
		iHeapMin -= iBeforeHeapUsed;
		iHeapMax -= iBeforeHeapUsed;

		// Log things
		TBuf<256> buf;
		_LIT(KStack, "Stack size: %d bytes.");
		buf.Format(KStack, iStackSize);
		AddTestResultL(buf, EInfoMessage);
		_LIT(KHeap, "Made %d location requests: Min heap size: %d bytes, Max heap size: %d bytes.");
		buf.Format(KHeap(), iNumberOfRequests, iHeapMin, iHeapMax);
		AddTestResultL(buf, EInfoMessage);

		TReal result;
		TInt err = Math::Round(result, TReal(((TReal32(iHeapMax) / TReal32(iSizeAfterStart)) - 1) * 100), 0);
		TInt16 percentIncr;
		TInt err2 = Math::Int(percentIncr, result);
		if (err != KErrNone || err2 != KErrNone) 
			{
			CompleteTest(KErrGeneral);
			return;	
			}
		// Check if more than "iErrorMemoryPercentLevel" was allocated on 
		// heap during requests
		if (percentIncr > iErrorMemoryPercentLevel)
			{
			_LIT(KHeapErr, "Making %d location requests causes max heapsize to increase %d%% of initial size. Must not be above %d%%.");
			buf.Format(KHeapErr(), iNumberOfRequests, percentIncr, 
				iErrorMemoryPercentLevel);
			AddTestResultL(buf, EErrorMessage);
			}

		CompleteTest(KErrNone);
		}
	}

// ---------------------------------------------------------
// CPosPSYMemoryTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::ExecuteTestL()
    {
	if (iNumberOfRequests <= 0)
		{
		_LIT(KNoMemoryTest, "No memory tests performed because number of memory location requests is 0 or less.");
		AddTestResultL(KNoMemoryTest, EInfoMessage);
		CompleteTest(KErrNone);
		}
	else
		{
		TInt tmp;
		GetMemoryFootPrint(tmp, iBeforeHeapUsed, tmp, tmp, tmp);
		iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());

		iRequester->MakeRequest(iPosInfo);
		iRequestNumber = 1;
		}
    }

// ---------------------------------------------------------
// CPosPSYMemoryTest::GetMemoryFootPrint
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMemoryTest::GetMemoryFootPrint(
    TInt& aHeapSize, 
	TInt& aUsedHeapSize, 
    TInt& aHeapMin, 
	TInt& aHeapMax, 
    TInt& aStackSize)
    {
	TInt tmp;
	RHeap& heap = User::Heap();
	RThread thread;
	TThreadStackInfo info;
	thread.StackInfo(info);
	aStackSize = info.iBase - info.iLimit;
	
    aHeapSize = heap.Size();
	aUsedHeapSize = aHeapSize - (heap.Available(tmp));

	if (aUsedHeapSize < aHeapMin)
		{
		aHeapMin = aUsedHeapSize;
		}
	if (aUsedHeapSize > aHeapMax)
		{
		aHeapMax = aUsedHeapSize;
		}
    }

//  End of File
