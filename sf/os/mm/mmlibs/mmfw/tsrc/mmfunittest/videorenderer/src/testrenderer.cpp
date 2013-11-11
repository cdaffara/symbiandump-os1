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

#include "testrenderer.h"
#include "videoframebuffer.h"
#include "testgceharness.h"

RTestRendererStep* RTestRendererStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererStep* self = new (ELeave) RTestRendererStep(aStepName, aTimed);
	return self;
	}

RTestRendererStep::RTestRendererStep(const TDesC& aStepName, TBool aTimed) :
	iVideoRenderer(NULL),
	iTimed(aTimed)
	{
	iTestStepName = aStepName;

	// size for buffer created
	iSize.iWidth = 1024;
	iSize.iHeight = 768;
	iVideoFormat.iDataFormat = ERgbRawData;
	iVideoFormat.iRgbFormat = ERgb32bit888;
	}

void RTestRendererStep::MvroVideoBufferAvailable()
	{
	INFO_PRINTF1(_L("MvroVideoBufferAvailable() callback received"));
	TRAPD(err, FsmL(EBufferAvailable));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("FsmL(EBufferAvailable) failed with %d"), err);
		EndTest(EFail);
		}
	}

void RTestRendererStep::MvroBufferDisplayed(TInt aBufferId, const TTime& aTime)
	{
	INFO_PRINTF2(_L("MvroBufferDisplayed() callback received aBufferId=%d"), aBufferId);
	iBufferId = aBufferId;
	iDisplayedTime = aTime;
	TRAPD(err, FsmL(EBufferDisplayed));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("FsmL(EBufferDisplayed) failed with %d"), err);
		EndTest(EFail);
		}
	}

void RTestRendererStep::MvroBufferSkipped(TInt aBufferId)
	{
	INFO_PRINTF2(_L("MvroBufferSkipped() callback received aBufferId=%d"), aBufferId);
	iBufferId = aBufferId;
	TRAPD(err, FsmL(EBufferSkipped));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("FsmL(EBufferSkipped) failed with %d"), err);
		EndTest(EFail);
		}
	}

TVerdict RTestRendererStep::DoTestStepPreambleL()
	{
    // Install the Active Scheduler
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
	iActiveSchedulerStarted = EFalse;
		
    iBuffAvailCallback = 0;
    iBuffDisplayCallback = 0;
    iBuffSkipCallback = 0;

    // The following is required because of the panic tests.
    // When a thread panics the global pointer is not reset to NULL
    // and the following test fails because the pointer is hanging.
    // When testframework creates a thread to run the test step it also
    // defines a heap and the default process heap is not used. Therefore
    // when the thread dies the heap is no longer available.
	CTestGCEHarness::ResetGlobalPointer();
    
    // Initialise Window server objects
    InitWservL();

    return EPass;
	}

TVerdict RTestRendererStep::DoTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
    iActiveScheduler = NULL;

    // Destroy Window server objects
    DeInitWserv();

    return EPass;
	}

void RTestRendererStep::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        ERR_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), TSize(100,100));
    iWindow->SetVisible(ETrue);
    iWindow->SetRequiredDisplayMode(EColor16MA);
    iWindow->Activate();
    }

void RTestRendererStep::DeInitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    iWs.Flush();
    iWs.Close();
    }

TInt RTestRendererStep::SetBackgroundSurface(TSurfaceId& id)
	{
	TInt err = iWindow->SetBackgroundSurface(id);
	iWs.Finish();
	return err;
	}

void RTestRendererStep::RemoveBackgroundSurface()
	{
	iWindow->RemoveBackgroundSurface(ETrue);
	}

TVerdict RTestRendererStep::DoTestStepL()
	{
	__UHEAP_MARK;
	
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EStartTest));
    if (err == KErrNone && iTestStepResult == EPass)
        {
        // Start the scheduler - Done only once !
    	iActiveSchedulerStarted = ETrue;
        CActiveScheduler::Start();    
        }
    else if (err != KErrNone)
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

void RTestRendererStep::FsmL(TTestRendererEvents aEventCode)
	{
    switch (aEventCode)
        {
        case EStartTest:
        	{
        	iFsmState = EStateCreate;
        	TInt numBuffers = 1;
        	
        	INFO_PRINTF2(_L("CVideoRenderer::NewL() Timed=%d"), iTimed);
        	CTestGCEHarness::NewL(numBuffers);
            iVideoRenderer = CVideoRenderer::NewL(*this, iTimed);
            
            INFO_PRINTF1(_L("iVideoRenderer->GetSupportedFormatsL()"));
            RArray<TUncompressedVideoFormat> expectedArray;
            CleanupClosePushL(expectedArray);
            TUncompressedVideoFormat format;
            format.iDataFormat = ERgbRawData;
    		format.iRgbFormat = ERgb16bit444;
    		expectedArray.AppendL(format); // append EVideoRendererPixelFormatXRGB_4444
    		format.iDataFormat = ERgbRawData;
    		format.iRgbFormat = ERgb16bit565;
    		expectedArray.AppendL(format); // append EVideoRendererPixelFormatRGB_565
    		format.iDataFormat = ERgbRawData;
    		format.iRgbFormat = ERgb32bit888;
    		expectedArray.AppendL(format); // append EVideoRendererPixelFormatXRGB_8888:

    		RArray<TUncompressedVideoFormat> array;
            CleanupClosePushL(array);
        	iVideoRenderer->GetSupportedFormatsL(array);
        	TInt expectedCount = expectedArray.Count();
        	TESTL(array.Count() == expectedCount);
        	for (TInt i = 0; i < expectedCount; i++)
        		{
        		TESTL(expectedArray[i] == array[i]);
        		}

        	CleanupStack::PopAndDestroy(2, &expectedArray);
        	CreateSurfaceL(numBuffers);
        	
            break;
        	}
        case EBufferAvailable:
        	iBuffAvailCallback++;
        	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
        		{
	            iFsmState = EStateUpdate;

	        	// map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            if (err != KErrNone)
	            	{
		        	ERR_PRINTF2(_L("RWindow::SetBackgroundSurface() returned %d"), err);
                    EndTest(EFail);
                    break;
	            	}

	        	TTime presentationTime;
	        	presentationTime.UniversalTime();
	        	TTimeIntervalMicroSeconds microsec(500000); // use half a second delay
	        	presentationTime += microsec;
	        	GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime);
        		}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 2)
        		{
	        	iFsmState = EStateReleaseBuffer;

	        	// buffer is available again after update;
        	    // try to get next buffer
        	    TVideoFrameBuffer* buffer;
	            INFO_PRINTF1(_L("iVideoRenderer->NextBuffer() after update"));
	        	buffer = iVideoRenderer->NextBuffer();
	        	TESTL(buffer != NULL);
	            INFO_PRINTF1(_L("iVideoRenderer->ReleaseBuffer()"));
	        	iVideoRenderer->ReleaseBuffer(buffer);
        		}
        	else if (iFsmState == EStateReleaseBuffer)
        		{
        		if (iBuffDisplayCallback == 1)
        			{
            		// both release buffer callback and display callback are received, test complete with pass
                	EndTest(EPass);
        			}
        		// else continue waiting for more callback
        		}
        	else
        		{
        		ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
            	EndTest(EFail);
        		}

        	break;
        case EBufferDisplayed:
        	iBuffDisplayCallback++;
        	if ((iFsmState != EStateUpdate && iFsmState != EStateReleaseBuffer) ||
        		iBufferId != 0)
        		{
        		// not in a valid state
        		ERR_PRINTF2(_L("State %d was not expected when handing buffer displayed event."), iFsmState);
            	EndTest(EFail);
        		}
        	if (iFsmState == EStateReleaseBuffer && iBuffAvailCallback == 3)
        		{
        		// both release buffer callback and display callback are received, test complete with pass
        		EndTest(EPass);
        		}
        	// otherwise, haven't received release callback yet, continue waiting
        	break;
        default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererStep::FsmL"), aEventCode);
        	EndTest(EFail);
        	break;
        }
	}

void RTestRendererStep::EndTest(TVerdict aVerdict)
	{
    iTestStepResult = aVerdict;
    if (iActiveSchedulerStarted)
    	{
    	CActiveScheduler::Stop();
    	}
	}

void RTestRendererStep::CreateRendererAndSurfaceL(TInt aNumBuffers)
	{
	INFO_PRINTF2(_L("CVideoRenderer::NewL() Timed=%d"), iTimed);
	CTestGCEHarness::NewL(aNumBuffers);
	iVideoRenderer = CVideoRenderer::NewL(*this, iTimed);
	CreateSurfaceL(aNumBuffers);
	}

void RTestRendererStep::CreateSurfaceL(TInt aNumBuffers)
	{
	INFO_PRINTF1(_L("iVideoRenderer->CreateSurfaceL()"));
	iVideoRenderer->CreateSurfaceL(iSize, aNumBuffers, iVideoFormat, iSurfaceId);
	}

void RTestRendererStep::GetNextBufferAndSubmitUpdateL(TInt aExpectedBufId, TRgb aColor, const TTime& aPresentationTime)
	{
    TVideoFrameBuffer* buffer = GetNextBufferL(aExpectedBufId, aColor);
    INFO_PRINTF1(_L("iVideoRenderer->UpdateBuffer()"));
	iVideoRenderer->UpdateBuffer(buffer, aPresentationTime);
	}

TVideoFrameBuffer* RTestRendererStep::GetNextBufferL(TInt aExpectedBufId, TRgb aColor)
	{
	INFO_PRINTF1(_L("iVideoRenderer->NextBuffer()"));
	TVideoFrameBuffer* buffer;
	buffer = iVideoRenderer->NextBuffer();
	TESTL(buffer != NULL);
	TESTL(buffer->Format() == iVideoFormat);
	TESTL(buffer->Stride() == iSize.iWidth *  4); // 4 bite per pixel for ERgb32bit888
	TESTL(buffer->BufferId() == aExpectedBufId);
	TESTL(buffer->Buffer() != NULL);
	
	TInt bufSize = buffer->Stride() * iSize.iHeight;
	Mem::Fill(buffer->Buffer(), bufSize, aColor.Internal());
	return buffer;
	}

RTestRendererReplaceStep* RTestRendererReplaceStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererReplaceStep* self = new (ELeave) RTestRendererReplaceStep(aStepName, aTimed);
	return self;
	}

RTestRendererReplaceStep::RTestRendererReplaceStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

void RTestRendererReplaceStep::FsmL(TTestRendererEvents aEventCode)
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
	            TESTL(err == KErrNone);

	            // send the buffer
	            TTime presentationTime(0);
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime);
	    		}
	    	else if (iFsmState == EStateUpdate)
	    		{
	    		if (iBuffDisplayCallback == 1 && iBuffAvailCallback == 2)
	    			{
	    			iFsmState = EStateReplaceSurface;
	    			ReplaceSurfaceL();
	    			}
	    		}
	    	else if (iFsmState == EStateReplaceSurface && iBuffAvailCallback == 1)
	    		{
    	    	// received the first callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            if (err != KErrNone)
	            	{
		        	ERR_PRINTF2(_L("RWindow::SetBackgroundSurface() returned %d"), err);
                    EndTest(EFail);
	            	}
	    		}
	    	else if (iFsmState == EStateReplaceSurface && iBuffAvailCallback == 2)
            	{
	            iFsmState = EStateUpdateAfterReplace;

	            // send the buffer
	            
	            TTime presentationTime1;
	            presentationTime1.UniversalTime();
	            TTimeIntervalMicroSeconds delay(500);
	            TTime presentationTime2 = presentationTime1 + delay;
	            
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);
	            GetNextBufferAndSubmitUpdateL(1, KRgbGreen, presentationTime2);
            	}
	    	else if (iFsmState == EStateUpdateAfterReplace)
	    		{
	    		if (iBuffDisplayCallback == 2 && iBuffAvailCallback == 3)
	    			{
	    			EndTest(EPass);
	    			}
	    		// else continue waiting for more callback
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
	    		// receive display callback
	        	if (iBuffAvailCallback == 2)
	        		{
	    			iFsmState = EStateReplaceSurface;
	    			ReplaceSurfaceL();
	        		}
	    		}
	    	else if (iFsmState == EStateUpdateAfterReplace && iBuffDisplayCallback == 1 && iBufferId == 0)
	    		{
	    		// receive display callback
	    		}
	    	else if (iFsmState == EStateUpdateAfterReplace && iBuffDisplayCallback == 2 && iBufferId == 1)
	    		{
	        	if (iBuffAvailCallback == 3)
	        		{
	    			EndTest(EPass);
	        		}
	        	// else haven't received all callback, continue waiting
	    		}
	    	else
	    		{
	    		// not in a valid state
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
	        	EndTest(EFail);
	    		}
	    	break;
	    case EBufferSkipped:
	    	ERR_PRINTF1(_L("Frame unexpectedly skipped"));
        	EndTest(EFail);
	    	break;
	    }
	}

void RTestRendererReplaceStep::ReplaceSurfaceL()
	{
	// reset the number of callback
	iBuffDisplayCallback = 0;
	iBuffAvailCallback = 0;
	
	RemoveBackgroundSurface();
	
	// received all expected callback for update
	// destroy surface and create new one for testing replace surface
	INFO_PRINTF1(_L("iVideoRenderer->DestroySurface()"));
	iVideoRenderer->DestroySurface(iSurfaceId);
	TInt buffers = 2;
	CTestGCEHarness::ResetBuffersL(buffers);
	CreateSurfaceL(buffers);
	}

/**
Standard static NewL() taking a callback function
*/
CCallBackTimer* CCallBackTimer::NewL(TCallBack aCallBack, TPriority aPriority)
	{
	CCallBackTimer* self = new(ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Private c'tor
*/
CCallBackTimer::CCallBackTimer(TCallBack aCallBack, TPriority aPriority) :
	CTimer(aPriority), iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

/*
Callback on timer complete
*/
void CCallBackTimer::RunL()
	{
	iCallBack.CallBack();
	}
