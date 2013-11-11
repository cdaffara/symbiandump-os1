// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testrendererneg.h"
#include "videoframebuffer.h"
#include "resourcefilereader.h"
#include "testgceharness.h"

RTestRendererCreateSurfaceStep* RTestRendererCreateSurfaceStep::NewL(const TDesC& aStepName, 
																			TInt aExpectedError, 
																			TRgbFormat aRgbFormat, 
																			TSize aSize, 
																			TInt aNumBuffers)
	{
	RTestRendererCreateSurfaceStep* self = new (ELeave) RTestRendererCreateSurfaceStep(aStepName,
																					aExpectedError, 
																					aRgbFormat, 
																					aSize, 
																					aNumBuffers);
	return self;
	}

RTestRendererCreateSurfaceStep::RTestRendererCreateSurfaceStep(const TDesC& aStepName, 
																		TInt aExpectedError, 
																		TRgbFormat aRgbFormat, 
																		TSize aSize, 
																		TInt aNumBuffers) :
	RTestRendererStep(aStepName, EFalse),
	iExpectedError(aExpectedError), 
	iNumBuffers(aNumBuffers)
	{
	iVideoFormat.iRgbFormat = aRgbFormat;
	iSize = aSize;
	}

TVerdict RTestRendererCreateSurfaceStep::DoTestStepL()
	{
	__UHEAP_MARK;
	
	CTestGCEHarness::NewL(iNumBuffers);
	iVideoRenderer = CVideoRenderer::NewL(*this, iTimed);
	TRAPD(err, iVideoRenderer->CreateSurfaceL(iSize, iNumBuffers, iVideoFormat, iSurfaceId));
	INFO_PRINTF3(_L("Expected error code=%d, received error code=%d"), iExpectedError, err);
	TESTL(err == iExpectedError);

	delete iVideoRenderer;
	iVideoRenderer = NULL;

	// cleanup test harness
	CTestGCEHarness::Remove();
	    
	__UHEAP_MARKEND;

    return iTestStepResult;
	}


RTestRendererUpdateTwiceStep* RTestRendererUpdateTwiceStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererUpdateTwiceStep* self = new (ELeave) RTestRendererUpdateTwiceStep(aStepName, aTimed);
	return self;
	}

RTestRendererUpdateTwiceStep::RTestRendererUpdateTwiceStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

void RTestRendererUpdateTwiceStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	            iFsmState = EStateUpdate;

	            // received the first callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            if (err != KErrNone)
	            	{
		        	ERR_PRINTF2(_L("RWindow::SetBackgroundSurface() returned %d"), err);
                    iTestStepResult = EFail;
                    break;
	            	}

	            // send the first buffer
	        	TTime presentationTime(0);

	        	INFO_PRINTF1(_L("iVideoRenderer->NextBuffer()"));
	        	TVideoFrameBuffer* buffer;
	        	buffer = iVideoRenderer->NextBuffer();
	        	TESTL(buffer != NULL);
	        	TESTL(buffer->BufferId() == 0);
	        	
	        	TInt bufSize = buffer->Stride() * iSize.iHeight;
	        	Mem::Fill(buffer->Buffer(), bufSize, KRgbGreen.Internal());
	            INFO_PRINTF1(_L("iVideoRenderer->UpdateBuffer()"));
	        	iVideoRenderer->UpdateBuffer(buffer, presentationTime);

	        	// send the same buffer again
	        	TESTL(buffer != NULL);
	        	TESTL(buffer->BufferId() == 0);
	        	iVideoRenderer->UpdateBuffer(buffer, presentationTime);
	    		}
	    	else if (iFsmState == EStateUpdate)
	    		{
	        	// buffer is available again after update
	    	    if (iBuffAvailCallback == 2 && iBuffDisplayCallback == 1)
	    	    	{
	    	    	// all available callback and display callback have been received, complete test with pass
	            	EndTest(EPass);
	    	    	}
	    		}
	    	else
	    		{
	    		// unexpected state, fail test
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	        	EndTest(EFail);
	    		}
	
	    	break;
	    case EBufferDisplayed:
	    	iBuffDisplayCallback++;
	    	if (iFsmState == EStateUpdate && iBuffDisplayCallback == 1 && iBufferId == 0)
	    		{
	    		// receive the first display callback, the buffer id is 0 (first buffer)
	        	if (iBuffAvailCallback == 2)
	        		{
	        		// received all bufAvailable callback after buffer displayed, test complete with pass
	        		EndTest(EPass);
	        		}
	        	// otherwise, haven't received all bufAvailable callback yet, continue waiting
	    		}
	    	else
	    		{
	    		// received unexpected event
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
	    		EndTest(EFail);
	    		}
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererUpdateTwiceStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}


RTestRendererReleaseTwiceStep* RTestRendererReleaseTwiceStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererReleaseTwiceStep* self = new (ELeave) RTestRendererReleaseTwiceStep(aStepName, aTimed);
	return self;
	}

RTestRendererReleaseTwiceStep::RTestRendererReleaseTwiceStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

TVerdict RTestRendererReleaseTwiceStep::DoTestStepL()
	{
	__UHEAP_MARK;
	
    // Call the state handler from IDLE state, don't need to start active scheduler
    TRAPD(err, FsmL(EStartTest));
    if (err != KErrNone)
    	{
        ERR_PRINTF2(_L("FsmL(EStartTest) leave with %d"), err);
    	iTestStepResult = EFail;
    	}
    
	delete iVideoRenderer;
	iVideoRenderer = NULL;

	// cleanup test harness
	CTestGCEHarness::Remove();
	    
	__UHEAP_MARKEND;

    return iTestStepResult;
	}

void RTestRendererReleaseTwiceStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	        	// buffer is available after surface create
	            iFsmState = EStateReleaseBuffer;

	        	INFO_PRINTF1(_L("iVideoRenderer->NextBuffer()"));
	        	iBuffer = iVideoRenderer->NextBuffer();
	        	TESTL(iBuffer != NULL);
	        	TESTL(iBuffer->BufferId() == 0);
	        	
	            INFO_PRINTF1(_L("iVideoRenderer->ReleaseBuffer()"));
	        	iVideoRenderer->ReleaseBuffer(iBuffer);
	    		}
	    	else if (iFsmState == EStateReleaseBuffer && iBuffAvailCallback == 2)
	    		{
	        	// buffer is available again after update
	        	// check that buffer still point to the same buffer
	        	TESTL(iBuffer != NULL);
	        	TESTL(iBuffer->BufferId() == 0);

	        	// call release buffer again, expect no error and no extra callback
	            INFO_PRINTF1(_L("iVideoRenderer->ReleaseBuffer()"));
	        	iVideoRenderer->ReleaseBuffer(iBuffer);
	    		}
	    	else
	    		{
	    		// unexpected state, fail test
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
                iTestStepResult = EFail;
	    		}
	
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererReleaseTwiceStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}


RTestRendererDestroySurfaceStep* RTestRendererDestroySurfaceStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererDestroySurfaceStep* self = new (ELeave) RTestRendererDestroySurfaceStep(aStepName, aTimed);
	return self;
	}

RTestRendererDestroySurfaceStep::RTestRendererDestroySurfaceStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererReleaseTwiceStep(aStepName, aTimed)
	{
	}

void RTestRendererDestroySurfaceStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	        	// buffer is available after surface create
	            iFsmState = EStateReleaseBuffer;
	
	        	INFO_PRINTF1(_L("iVideoRenderer->NextBuffer()"));
	        	iBuffer = iVideoRenderer->NextBuffer();
	        	TESTL(iBuffer != NULL);
	        	TESTL(iBuffer->BufferId() == 0);

	        	INFO_PRINTF1(_L("iVideoRenderer->DestroySurface()"));
	        	iVideoRenderer->DestroySurface(iSurfaceId);

	        	// check that iBuffer is not null
	        	TESTL(iBuffer != NULL);

	        	// submit update, expect no callback and no error
	            TTime presentationTime(0);
	            INFO_PRINTF1(_L("iVideoRenderer->UpdateBuffer()"));
	        	iVideoRenderer->UpdateBuffer(iBuffer, presentationTime);

	        	// release buffer, expect no callback and no error
	            INFO_PRINTF1(_L("iVideoRenderer->ReleaseBuffer()"));
	        	iVideoRenderer->ReleaseBuffer(iBuffer);

	        	// try to get the next buffer after surface destroyed, expect null
	        	INFO_PRINTF1(_L("iVideoRenderer->NextBuffer()"));
	        	iBuffer = iVideoRenderer->NextBuffer();
	        	TESTL(iBuffer == NULL);
	    		}
	    	else
	    		{
	    		// unexpected state, fail test
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	            iTestStepResult = EFail;
	    		}
	
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererDestroySurfaceStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}


RTestRendererUpdateAndDestroyStep* RTestRendererUpdateAndDestroyStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererUpdateAndDestroyStep* self = new (ELeave) RTestRendererUpdateAndDestroyStep(aStepName, aTimed);
	return self;
	}

RTestRendererUpdateAndDestroyStep::RTestRendererUpdateAndDestroyStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

TVerdict RTestRendererUpdateAndDestroyStep::DoTestStepPreambleL()
	{
	TVerdict result = RTestRendererStep::DoTestStepPreambleL();
	
	TCallBack callback(TimerCallbackFunc, this);
	iCallbackTimer = CCallBackTimer::NewL(callback);

	return result;
	}

TVerdict RTestRendererUpdateAndDestroyStep::DoTestStepPostambleL()
	{
	delete iCallbackTimer;
	return RTestRendererStep::DoTestStepPostambleL();
	}

TInt RTestRendererUpdateAndDestroyStep::TimerCallbackFunc(TAny *aPtr)
	{
	RTestRendererDelayStep* teststep = static_cast<RTestRendererDelayStep*>(aPtr);
	TRAP_IGNORE(teststep->FsmL(ETimerExpired)); // ETimerExpired state does not leave

	return KErrNone;
	}

void RTestRendererUpdateAndDestroyStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	            iFsmState = EStateUpdate;

    	    	// received callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL(err == KErrNone);

	            // send the buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);
	            
	            // destroy surface
	        	INFO_PRINTF1(_L("iVideoRenderer->DestroySurface()"));
	        	iVideoRenderer->DestroySurface(iSurfaceId);
	        	
	        	// wait for a second for ensure that timed mode is ok, expect nothing
	        	TTimeIntervalMicroSeconds32 oneSecond(1000000);
	            iCallbackTimer->After(oneSecond);
            	}
            else
            	{
            	// unexpected state, test failed
            	ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
                EndTest(EFail);
	    		}
	    	break;
	    case ETimerExpired:
	    	// timer expired, stop active scheduler, heap check marco will check for any leak
	    	CActiveScheduler::Stop();
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererUpdateAndDestroyStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}


RTestRendererUpdateAndDeleteStep* RTestRendererUpdateAndDeleteStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererUpdateAndDeleteStep* self = new (ELeave) RTestRendererUpdateAndDeleteStep(aStepName, aTimed);
	return self;
	}

RTestRendererUpdateAndDeleteStep::RTestRendererUpdateAndDeleteStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererReleaseTwiceStep(aStepName, aTimed)
	{
	}

void RTestRendererUpdateAndDeleteStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	            iFsmState = EStateUpdate;
	
		    	// received callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL(err == KErrNone);
	
	            // send the buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);
	            
	            // delete renderer
	        	INFO_PRINTF1(_L("delete iVideoRenderer"));
	        	delete iVideoRenderer;
	        	iVideoRenderer = 0;

	        	// wait for a second for ensure that timed mode is ok, expect nothing
	        	TTimeIntervalMicroSeconds32 oneSecond(1000000);
	        	User::After(oneSecond);
	    		}
	        else
	        	{
	        	// unexpected state, test failed
	        	ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	            iTestStepResult = EFail;
	    		}
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererUpdateAndDeleteStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}


RTestRendererUpdateInvalidStep* RTestRendererUpdateInvalidStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererUpdateInvalidStep* self = new (ELeave) RTestRendererUpdateInvalidStep(aStepName, aTimed);
	return self;
	}

RTestRendererUpdateInvalidStep::RTestRendererUpdateInvalidStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererReleaseTwiceStep(aStepName, aTimed)
	{
	}

void RTestRendererUpdateInvalidStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	            iFsmState = EStateUpdate;
	
		    	// received callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL(err == KErrNone);

	            // get a buffer
	        	TVideoFrameBuffer* buffer;
	        	buffer = iVideoRenderer->NextBuffer();
	        	TESTL(buffer != NULL);
	        	TESTL(buffer->Format() == iVideoFormat);
	        	TESTL(buffer->Stride() == iSize.iWidth *  4); // 4 bite per pixel for ERgb32bit888
	        	TESTL(buffer->BufferId() == 0);
	        	TESTL(buffer->Buffer() != NULL);

	            // create a local buffer with same parameters
	        	TVideoFrameBuffer localBuffer(buffer->Format(), 
	        			buffer->Stride(),
	        			buffer->BufferId(), 
	        			buffer->Chunk(),
	        			0);
	        	
	        	TInt bufSize = buffer->Stride() * iSize.iHeight;
	        	Mem::Fill(buffer->Buffer(), bufSize, KRgbGreen.Internal());
	        	TTime time(0);
	            INFO_PRINTF1(_L("iVideoRenderer->UpdateBuffer()"));
	        	iVideoRenderer->UpdateBuffer(&localBuffer, time);
	    		}
	        else
	        	{
	        	// unexpected state, test failed
	        	ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	            iTestStepResult = EFail;
	    		}
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererUpdateInvalidStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}

RTestRendererCreateTwiceStep* RTestRendererCreateTwiceStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererCreateTwiceStep* self = new (ELeave) RTestRendererCreateTwiceStep(aStepName, aTimed);
	return self;
	}

RTestRendererCreateTwiceStep::RTestRendererCreateTwiceStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererReleaseTwiceStep(aStepName, aTimed)
	{
	}

void RTestRendererCreateTwiceStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	    		// Create surface again
	    		TRAPD(err, CreateSurfaceL(1));
	    		INFO_PRINTF3(_L("Expected error code=%d, received error code=%d"), KErrInUse, err);
	    		TESTL(err == KErrInUse);
	    		}
	        else
	        	{
	        	// unexpected state, test failed
	        	ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	            iTestStepResult = EFail;
	    		}
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererCreateTwiceStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}

RTestRendererDestroyInvalidStep* RTestRendererDestroyInvalidStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererDestroyInvalidStep* self = new (ELeave) RTestRendererDestroyInvalidStep(aStepName, aTimed);
	return self;
	}

RTestRendererDestroyInvalidStep::RTestRendererDestroyInvalidStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererReleaseTwiceStep(aStepName, aTimed)
	{
	}

void RTestRendererDestroyInvalidStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	    		// create a surface id that is not the same as iSurfaceId
	    		TSurfaceId invalidId = iSurfaceId;
	    		invalidId.iInternal[0] += 1;
	    		invalidId.iInternal[1] += 1;
	    		invalidId.iInternal[2] += 1;
	    		invalidId.iInternal[3] += 1;
	    		
	    		// Destory surface with invalid id
	    		iVideoRenderer->DestroySurface(invalidId);

	    		// Destory surface twice, the second call is ignored
	    		iVideoRenderer->DestroySurface(iSurfaceId);
	    		iVideoRenderer->DestroySurface(iSurfaceId);

	    		// Destory surface with invalid id after the surface is destroyed
	    		iVideoRenderer->DestroySurface(invalidId);
	    		}
	        else
	        	{
	        	// unexpected state, test failed
	        	ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
	            iTestStepResult = EFail;
	    		}
	    	break;
	    default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererDestroyInvalidStep::FsmL"), aEventCode);
	    	iTestStepResult = EFail;
	    	break;
	    }
	}

RTestResourceFileReaderStep* RTestResourceFileReaderStep::NewL(const TDesC& aStepName, const TDesC& aFileName, TInt aExpectedFormatErr, TInt aExpectedTimerErr, TInt64 aExpectedDefaultDelay, TInt64 aExpectedDelay)
	{
	RTestResourceFileReaderStep* self = new (ELeave) RTestResourceFileReaderStep(aStepName, aFileName, aExpectedFormatErr, aExpectedTimerErr, aExpectedDefaultDelay, aExpectedDelay);
	return self;	
	}

RTestResourceFileReaderStep::RTestResourceFileReaderStep(const TDesC& aStepName, const TDesC& aFileName, TInt aExpectedFormatErr, TInt aExpectedTimerErr, TInt64 aExpectedDefaultDelay, TInt64 aExpectedDelay)
	{
	iTestStepName = aStepName;
	iFileName = aFileName;
	iExpectedFormatErr = aExpectedFormatErr;
	iExpectedTimerErr = aExpectedTimerErr;
	iExpectedDefaultDelay = aExpectedDefaultDelay;
	iExpectedMaxDelay = aExpectedDelay;
	}

TVerdict RTestResourceFileReaderStep::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check Resource file reader"));
	
	__UHEAP_MARK;
	
	RArray<TUncompressedVideoFormat> supportedFormat;
	CleanupClosePushL(supportedFormat);
	CResourceFileReader* reader = CResourceFileReader::NewLC(iFileName);
	TRAPD(err, reader->ReadSupportedFormatL(supportedFormat));
	if (err != iExpectedFormatErr)
		{
		iTestStepResult = EFail;
		ERR_PRINTF3(_L("Expected error code=%d, received error code=%d"), iExpectedFormatErr, err);
		}
	TInt64 defaultDelay;
	TInt64 maxDelay;
	TRAP(err, reader->ReadTimerInfoL(defaultDelay, maxDelay));
	if (err != iExpectedTimerErr)
		{
		iTestStepResult = EFail;
		ERR_PRINTF3(_L("Expected error code=%d, received error code=%d"), iExpectedTimerErr, err);
		}
	else if (defaultDelay != iExpectedDefaultDelay || maxDelay != iExpectedMaxDelay)
		{
		iTestStepResult = EFail;
		ERR_PRINTF3(_L("Expected default delay=%d, received default delay=%d"), iExpectedDefaultDelay, defaultDelay);
		ERR_PRINTF3(_L("Expected max delay=%d, received max delay=%d"), iExpectedMaxDelay, maxDelay);
		}
		
	CleanupStack::PopAndDestroy(2, &supportedFormat);
	
	__UHEAP_MARKEND;

	return iTestStepResult;
	}


RTestRendererDelayStep* RTestRendererDelayStep::NewL(const TDesC& aStepName)
	{
	RTestRendererDelayStep* self = new (ELeave) RTestRendererDelayStep(aStepName);
	return self;
	}

RTestRendererDelayStep::RTestRendererDelayStep(const TDesC& aStepName) :
	RTestRendererUpdateAndDestroyStep(aStepName, ETrue)
	{
	}

void RTestRendererDelayStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(1);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
	    		{
	            iFsmState = EStateUpdate;

	            // send the buffer without set background surface
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);
	            
	            iCallbackTimer->AtUTC(presentationTime1);
            	}
	    	else if (iFsmState == EStateUpdate)
	    		{
	        	// buffer is available again after update
	    	    if (iBuffAvailCallback == 2 && iBuffSkipCallback == 1)
	    	    	{
	    	    	// all callbacks have been received, complete test with pass
	            	EndTest(EPass);
	    	    	}
	    		}
	    	else
	    		{
	    		// unexpected state, fail test
	        	EndTest(EFail);
	    		}
	
	    	break;
	    case ETimerExpired:
	    	{
	    	// received the first callback, map surface to display
			INFO_PRINTF1(_L("Timer expired event receibed"));

	        TInt err = SetBackgroundSurface(iSurfaceId);
	        TESTL(err == KErrNone);

	        if (iBuffAvailCallback == 2 && iBuffSkipCallback == 1)
    	    	{
    	    	// all callbacks have been received, complete test with pass
            	EndTest(EPass);
    	    	}
	    	break;
	    	}
	    case EBufferSkipped:
	    	iBuffSkipCallback++;
	    	// expect 1 buffer skipped callback at fsm state EStateUpdate for bufferId 0
	    	if (iFsmState != EStateUpdate || iBuffSkipCallback != 1 || iBufferId != 0)
	    		{
	        	EndTest(EFail);
	    		}
	    	break;
	    default:
	    	// unexpected state
	    	EndTest(EFail);
	    	break;
	    }
	}
