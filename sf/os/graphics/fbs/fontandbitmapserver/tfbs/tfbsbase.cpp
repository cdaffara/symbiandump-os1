// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tfbsbase.h"
#include "fbsmessage.h"

CTFbsBase::CTFbsBase(CTestStep* aStep, TBool aRunWithLowMemory):
	CTGraphicsBase(aStep), 
	iTestStep(*aStep), 
	iLastTestCase(EFalse),
	iRunWithLowMemory(aRunWithLowMemory)
	{
	}

CTFbsBase::~CTFbsBase()
	{
	}

/** Run the passed the test case. Classes that inherit from CTFbsBase should override this
and choose which test case to run depending on the passed test case number. Test cases are 
run once normally, 1..n times with out of memory (OOM) testing switched on for the FBServ 
heap and 1..n times with OOM testing switched on for the current heap.
  
@param aCurTestCase The number of the test case to run
 */
void CTFbsBase::RunTestCaseL(TInt aCurTestCase)
	{
	if(iLastTestCase)
		{
		TestComplete();
		return;
		}
		
	INFO_PRINTF2(_L("***** Starting test case %i *****"), aCurTestCase);
	
	// Run the test normally first
	iCurrentRunIsLowMemory = EFalse;
	TRAPD(err, RunFbsTestL(aCurTestCase));
	if(KErrNone != err)
		{
		iTestStep.SetTestStepResult(EFail);
		}

#ifndef _DEBUG
	if(iRunWithLowMemory)
		{
		iRunWithLowMemory = EFalse;
		INFO_PRINTF1(_L("WARNING: Can't run out of memory tests under a release build. OOM tests set to run in ini file so turning OOM tests off."));
		}
#endif

	// Run the test in out of memory conditions, checking both the FBServ heap and the
	// current thread's heap for memory leaks
	if(iRunWithLowMemory)
		{
		iCurrentRunIsLowMemory = ETrue;
		RFbsSession* fbsSession = RFbsSession::GetSession();
		ASSERT(fbsSession);

		INFO_PRINTF2(_L("***** Running Out Of Memory Tests on test case %i (FBSERV heap) *****"), aCurTestCase);
		
		// Save the current state of test step results
		TVerdict currentTestStepResult = iTestStep.TestStepResult();
		
		// Create 1000 pixel wide bitmap to prevent allocation of scanline buffer
		// during testings, to allow for memory leak testing
		const TSize KSizeInPixels 		= TSize(1000,1);
		const TDisplayMode KDisplayMode	= EColor64K;
		CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bmp);
		User::LeaveIfError(bmp->Create(KSizeInPixels, KDisplayMode));
		CleanupStack::PopAndDestroy(bmp);

		// Run the test with heap checking for the FbServ heap
		for(TInt failAfter=1; failAfter < 1000; ++failAfter)
			{
			INFO_PRINTF2(_L("***** Set fail after %i allocs (FBSERV heap) *****"), failAfter);

			// Count cells so we can know if any leaked
			TInt cellsStart = fbsSession->SendCommand(EFbsMessHeapCount, RFbsSession::EHeapFailTypeHeapMemory);
			fbsSession->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeHeapMemory, failAfter);

			// Run test case (implemented by sub class)
			TRAPD(err, RunFbsTestL(aCurTestCase));

			fbsSession->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeHeapMemory);
			TInt cellsEnd = fbsSession->SendCommand(EFbsMessHeapCount, RFbsSession::EHeapFailTypeHeapMemory);
			if(cellsStart < cellsEnd)
				{
				// leaked.
				TInt leakedCells = cellsEnd - cellsStart;
				ERR_PRINTF3(_L("***** On loop number %i we leaked %i cells (FBSERV heap) *****"), failAfter, leakedCells);
				currentTestStepResult = EFail;
				}
		
			// Check to see if any failures reported within test case run
			if(KErrNone == err)
				{
				INFO_PRINTF3(_L("***** Test case %i completed successfully after %d iterations (FBSERV heap) *****"), aCurTestCase, failAfter);
				break;
				}
			}
		
		// Run the test with heap checking for the current thread's heap
		TInt tryCount = 0;
		INFO_PRINTF2(_L("***** Running Out Of Memory Tests on test case %i, current thread's heap (current heap) *****"), aCurTestCase);
		
		FOREVER
			{					
			TInt err = KErrNone;
			
			// count cells so we can know how many we leaked
			TInt cellsStart = User::CountAllocCells();
			++tryCount;
			INFO_PRINTF2(_L("***** Set fail after %d allocs (current heap) *****"), tryCount);
			
			__UHEAP_FAILNEXT(tryCount);
			__UHEAP_MARK;											
			
			TRAP(err, RunFbsTestL(aCurTestCase));
	
			TBool finishedCorrectly = EFalse;
			if ((err == KErrNone))
				{
				// claims to have finished correctly, and we're not failing every alloc
				finishedCorrectly = iStep->CheckForHeapFailNext();
				}		
			__UHEAP_RESET;
			TInt cellsEnd = User::CountAllocCells();
			if (cellsStart < cellsEnd)
				{
				// leaked.
				TInt leakedCells = cellsEnd - cellsStart;
				ERR_PRINTF3(_L("***** On loop number %d we leaked %d cells (current heap). About to cause panic."),tryCount,leakedCells);				
				}
			__UHEAP_MARKEND;
			
			// check to see if we finished all OOM testing successfully
			if ((err == KErrNone) && finishedCorrectly)
				{				
				INFO_PRINTF3(_L("***** Test case %i completed successfully after %d iterations (current heap) *****"), aCurTestCase, tryCount);
				break;
				}
			
			if (tryCount == 999)
				{
				ERR_PRINTF1(_L("***** OOM testing stopped after 999 iterations (current heap)"));
				break;				
				}
			}
		
		// Restore test step result and ignore any test failures the out of memory tests produce
		iTestStep.SetTestStepResult(currentTestStepResult);
		}		
	}

/** Helper method for extracting a TRgb colour from the passed buffer given a pixel
offset in to the buffer and a display mode.

@param aBuffer A buffer to extract the colour from.
@param aPixelOffset A pixel offset to use in to the buffer.
@param aDispMode The display mode to use when converting the pixel colour to TRgb.
 */
TRgb CTFbsBase::ExtractRgb(TUint8* aBuffer, TInt aPixelOffset, TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 3));
		if (byte & (1 << (aPixelOffset & 7)))
			return KRgbWhite;
		return KRgbBlack;
		}
	case EGray4:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 2));
		byte >>= ((aPixelOffset & 3) << 1);
		return TRgb::Gray4(byte & 3);
		}
	case EGray16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::Gray16(byte & 0xf);
		}
	case EGray256:
		return TRgb::Gray256(*(aBuffer + aPixelOffset));
	case EColor16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::Color16(byte & 0xf);
		}
	case EColor256:
		return TRgb::Color256(*(aBuffer + aPixelOffset));
	case EColor4K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::Color4K(doubleByte & 0xfff);
		}
	case EColor64K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::Color64K(doubleByte);
		}
	case EColor16M:
		{
		aBuffer += aPixelOffset * 3;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer << 16;
		return TRgb::Color16M(value);
		}
	case ERgb:
		return *(((TRgb*)aBuffer) + aPixelOffset);
	case EColor16MU:
		{
		return TRgb::Color16MU(*(((TUint32*)aBuffer) + aPixelOffset));
		}
	case EColor16MA:
		{
		return TRgb::Color16MA(*(((TUint32*)aBuffer) + aPixelOffset));
		}	
	case EColor16MAP:
		{
		return TRgb::_Color16MAP(*(((TUint32*)aBuffer) + aPixelOffset));
		}	
	default:
		break;
		};
	return KRgbBlack;
	}

/** Function to be used by classes that inherit from CTFbsBase. SetLastTestCase()
should be called after the last test case for a class is called to signal that 
testing has finished for that class.
 */
void CTFbsBase::SetLastTestCase()
	{
	iLastTestCase = ETrue;
	}
