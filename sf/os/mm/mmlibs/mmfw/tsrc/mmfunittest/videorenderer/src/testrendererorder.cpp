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

#include "testrendererorder.h"
#include "videoframebuffer.h"
#include "testgceharness.h"

RTestRendererOOOStep* RTestRendererOOOStep::NewL(const TDesC& aStepName)
	{
	RTestRendererOOOStep* self = new (ELeave) RTestRendererOOOStep(aStepName);
	return self;
	}

RTestRendererOOOStep::RTestRendererOOOStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, ETrue)
	{
	}

void RTestRendererOOOStep::FsmL(TTestRendererEvents aEventCode)
	{
    switch (aEventCode)
        {
        case EStartTest:
        	iFsmState = EStateCreate;
            CreateRendererAndSurfaceL(2);
            break;
        case EBufferAvailable:
        	iBuffAvailCallback++;
        	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
        		{
    	    	// received the first callback, map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL(err == KErrNone);
    	    	}
    	    else if (iFsmState == EStateCreate && iBuffAvailCallback == 2)
            	{
	            iFsmState = EStateUpdate;

	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer which has a earlier presentation time than in the previous request
	        	TTimeIntervalMicroSeconds delay2(250000); // use quarter of a second delay
	        	TTime presentationTime2 = nowTime + delay2;
	        	GetNextBufferAndSubmitUpdateL(1, KRgbRed, presentationTime2);
            	}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 3 && iBuffDisplayCallback == 1)
        		{
	        	// wait for another buffer displayed
         		}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 3 && iBuffDisplayCallback == 2)
        		{
        		EndTest(EPass); 
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
        	if (iFsmState == EStateUpdate && iBuffDisplayCallback == 1 && iBufferId == 1)
        		{
        		// receive the first display callback
        		}
        	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 0)
        		{
        		if(iBuffAvailCallback == 3)
        			{
        			EndTest(EPass);
        			}
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
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererOOOStep::FsmL"), aEventCode);
        	EndTest(EFail);
        	break;
        }
	}

RTestRendererNonTimedOOOStep* RTestRendererNonTimedOOOStep::NewL(const TDesC& aStepName)
	{
	RTestRendererNonTimedOOOStep* self = new (ELeave) RTestRendererNonTimedOOOStep(aStepName);
	return self;
	}

RTestRendererNonTimedOOOStep::RTestRendererNonTimedOOOStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, EFalse)
	{
	}

void RTestRendererNonTimedOOOStep::FsmL(TTestRendererEvents aEventCode)
	{
    switch (aEventCode)
        {
        case EStartTest:
        	iFsmState = EStateCreate;
            CreateRendererAndSurfaceL(3);
            break;
        case EBufferAvailable:
        	iBuffAvailCallback++;
        	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime presentationTime1(0);
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay2(500000); // use half second delay
	        	TTime presentationTime2 = nowTime + delay2;
	            GetNextBufferAndSubmitUpdateL(1, KRgbBlue, presentationTime2);

	        	// send the third buffer which has a earlier presentation time than in the previous request
	        	TTimeIntervalMicroSeconds delay3(250000); // use quarter of a second delay
	        	TTime presentationTime3 = nowTime + delay3;
	            GetNextBufferAndSubmitUpdateL(2, KRgbRed, presentationTime3);
            	}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 1)
        		{
   	        	// buffer is available again after update
        		}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 2)
        		{
   	        	// buffer is available again after update
        		}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffDisplayCallback == 2)
        		{
   	        	// buffer is available again after update
        		}
        	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffDisplayCallback == 3)
        		{
        		EndTest(EPass);
        		}
        	else
        		{
        		// unexpected state, fail test
        		ERR_PRINTF2(_L("State %d was not expected when handling buffer available event."), iFsmState);
            	//EndTest(EFail);
        		}

        	break;
        case EBufferDisplayed:
        	iBuffDisplayCallback++;
        	if (iFsmState == EStateUpdate && iBuffDisplayCallback == 1 && iBufferId == 0)
        		{
        		// receive the first display callback, the buffer id is 0 (first buffer)
        		break;
        		}
        	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 2)
        		{
        		// receive the second display callback, the buffer id is 2 (third buffer)
        		break;
        		}
        	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 3 && iBufferId == 1)
        		{
        		// receive the third display callback, the buffer id is 1 (second buffer)
            	if (iBuffAvailCallback == 5)
            		{
            		EndTest(EPass);
            		}
        		}
         	else
        		{
        		// received unexpected event
        		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
        		//EndTest(EFail);
        		}
        	break;
        default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererNonTimedOOOStep::FsmL"), aEventCode);
        	//EndTest(EFail);
        	break;
        }
	}

RTestRendererSkipStep* RTestRendererSkipStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererSkipStep* self = new (ELeave) RTestRendererSkipStep(aStepName, aTimed);
	return self;
	}

RTestRendererSkipStep::RTestRendererSkipStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

void RTestRendererSkipStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
            CreateRendererAndSurfaceL(3);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;

	    	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);
				
				TVideoFrameBuffer* buf1 = GetNextBufferL(0, KRgbGreen);
				TVideoFrameBuffer* buf2 = GetNextBufferL(1, KRgbBlue);
				TVideoFrameBuffer* buf3 = GetNextBufferL(2, KRgbRed);
				
	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay(1); // use 1 microsecond delay
	        	TTime presentationTime1 = nowTime + delay;
	        	TTime presentationTime2 = presentationTime1 + delay;
	        	TTime presentationTime3 = presentationTime2 + delay;
	            
	            INFO_PRINTF1(_L("Submit all buffers with iVideoRenderer->UpdateBuffer()"));
				iVideoRenderer->UpdateBuffer(buf1, presentationTime1);
				iVideoRenderer->UpdateBuffer(buf2, presentationTime2);
				iVideoRenderer->UpdateBuffer(buf3, presentationTime3);
            	}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 1 && iBuffSkipCallback == 1)
	    		{
   	        	// buffer is available again after update
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffDisplayCallback == 1 && iBuffSkipCallback == 1)
	    		{
   	        	// buffer is available again after update
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffDisplayCallback == 2 && iBuffSkipCallback == 1)
	    		{
	    		EndTest(EPass);
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
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 2)
	    		{
        		// received the 2nd display callback
       			EndTest(EPass);
        		}
	    	else
	    		{
	    		// received unexpected event
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
	    		EndTest(EFail);
	    		}
	    	break;
	    case EBufferSkipped:
	    	iBuffSkipCallback++;
	    	// expect 1 buffer skipped callback at fsm state EStateUpdate for bufferId 1
	    	if (iFsmState != EStateUpdate || iBuffSkipCallback != 1 || iBufferId != 1)
	    		{
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer skipped event."), iFsmState);
	        	EndTest(EFail);
	    		}
	    	break;
        default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererSkipStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}


RTestRendererOODStep* RTestRendererOODStep::NewL(const TDesC& aStepName, TBool aTimed)
	{
	RTestRendererOODStep* self = new (ELeave) RTestRendererOODStep(aStepName, aTimed);
	return self;
	}

RTestRendererOODStep::RTestRendererOODStep(const TDesC& aStepName, TBool aTimed) :
	RTestRendererStep(aStepName, aTimed)
	{
	}

void RTestRendererOODStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(3);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(250000); // use quarter of a second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer
	        	nowTime.UniversalTime();
	        	TTime presentationTime2 = nowTime - delay1; // use a out of date presentation time
	            GetNextBufferAndSubmitUpdateL(1, KRgbBlue, presentationTime2);
	        	
	        	// send the third buffer 
	        	TTimeIntervalMicroSeconds delay3(500000); // use half second delay
	        	TTime presentationTime3 = nowTime + delay3;
	        	GetNextBufferAndSubmitUpdateL(2, KRgbRed, presentationTime3);
            	}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffSkipCallback == 1)
	    		{
   	        	// buffer is available again after update
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffSkipCallback == 1)
	    		{
   	        	if(iBuffDisplayCallback == 2)
   	        		{
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
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 2)
	    		{
            	if (iBuffAvailCallback == 5)
            		{
            		EndTest(EPass);
            		}
        		}
	    	else
	    		{
	    		// received unexpected event
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
	    		EndTest(EFail);
	    		}
	    	break;
	    case EBufferSkipped:
	    	iBuffSkipCallback++;
	    	// expect buffer 0 and/or 1 skipped callback at fsm state EStateUpdate but never 2
	    	if (iFsmState != EStateUpdate || iBuffSkipCallback > 2 || iBufferId == 2)
	    		{
	        	EndTest(EFail);
	    		}
	    	break;
        default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererOODStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}

RTestRendererSameTimeStep* RTestRendererSameTimeStep::NewL(const TDesC& aStepName)
	{
	RTestRendererSameTimeStep* self = new (ELeave) RTestRendererSameTimeStep(aStepName);
	return self;
	}

RTestRendererSameTimeStep::RTestRendererSameTimeStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, ETrue)
	{
	}

void RTestRendererSameTimeStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(3);
	        break;
	    case EBufferAvailable:
	    	CTestGCEHarness::SetBufferEventProcessingDelay(CTestGCEHarness::EEventAll, 1000000);
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half a second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer with same time as first buffer
	            GetNextBufferAndSubmitUpdateL(1, KRgbBlue, presentationTime1);
	        	
	        	// send the third buffer 
	        	TTimeIntervalMicroSeconds delay3(750000); // use more than half second delay
	        	TTime presentationTime3 = nowTime + delay3;
	        	GetNextBufferAndSubmitUpdateL(2, KRgbRed, presentationTime3);
            	}
            else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 1 && iBuffSkipCallback == 1)
            	{
            	// buffer is available again after update
            	}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5 && iBuffSkipCallback == 1)
	    		{
	    		if(iBuffDisplayCallback == 2)
	    			{
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
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 2)
	    		{
	    		// receive the second display callback, the buffer id is 2 (third buffer)
	        	if (iBuffAvailCallback == 5)
	        		{
	        		EndTest(EPass);
	        		}
        		}
	        	// otherwise, haven't received all callbacks yet, continue waiting
	    	else
	    		{
	    		// received unexpected event
	    		ERR_PRINTF2(_L("State %d was not expected when handling buffer displayed event."), iFsmState);
	    		EndTest(EFail);
	    		}
	    	break;
	    case EBufferSkipped:
	    	iBuffSkipCallback++;
	    	// expect buffers 0 and/or 1 skipped callback at fsm state EStateUpdate but never buffer 2
	    	if (iFsmState != EStateUpdate || iBuffSkipCallback > 2 || iBufferId == 2)
	    		{
	        	EndTest(EFail);
	    		}
	    	break;
        default:
			// unexpected event
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererSameTimeStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}


RTestRendererSameTimeNonTimedStep* RTestRendererSameTimeNonTimedStep::NewL(const TDesC& aStepName)
	{
	RTestRendererSameTimeNonTimedStep* self = new (ELeave) RTestRendererSameTimeNonTimedStep(aStepName);
	return self;
	}

RTestRendererSameTimeNonTimedStep::RTestRendererSameTimeNonTimedStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, EFalse)
	{
	}

void RTestRendererSameTimeNonTimedStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(3);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half a second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer with same time as first buffer
	            GetNextBufferAndSubmitUpdateL(1, KRgbBlue, presentationTime1);
	        	
	        	// send the third buffer 
	        	TTimeIntervalMicroSeconds delay3(750000); // use more than half second delay
	        	TTime presentationTime3 = nowTime + delay3;
	        	GetNextBufferAndSubmitUpdateL(2, KRgbRed, presentationTime3);
            	}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 1)
	    		{
	        	// buffer is available again after update
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 2)
	    		{
	    		// buffer is available again after update
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5)
	    		{
	    		if(iBuffDisplayCallback == 3)
	    			{
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
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 1)
	    		{
	    		// receive the second display callback, the buffer id is 2 (third buffer)
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 3 && iBufferId == 2)
	    		{
	        	if (iBuffAvailCallback == 5)
	        		{
	        		EndTest(EPass);
	        		}
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
	    	ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererSameTimeNonTimedStep::FsmL"), aEventCode);
        	EndTest(EFail);
	    	break;
	    }
	}


RTestRendererZeroTimeStep* RTestRendererZeroTimeStep::NewL(const TDesC& aStepName)
	{
	RTestRendererZeroTimeStep* self = new (ELeave) RTestRendererZeroTimeStep(aStepName);
	return self;
	}

RTestRendererZeroTimeStep::RTestRendererZeroTimeStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, ETrue)
	{
	}

void RTestRendererZeroTimeStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(2);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && iBuffAvailCallback == 1)
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 2)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay1(500000); // use half second delay
	        	TTime presentationTime1 = nowTime + delay1;
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer which has a earlier presentation time than in the previous request
	        	TTime presentationTime2(0);
	        	GetNextBufferAndSubmitUpdateL(1, KRgbRed, presentationTime2);
            	}
	    	else if (iFsmState == EStateUpdate && iBuffAvailCallback == 3)
	    		{
	    		if(iBuffDisplayCallback == 2)
	    			{
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
	    	if (iFsmState == EStateUpdate && iBuffDisplayCallback == 1 && iBufferId == 1)
	    		{
	    		// receive the first display callback, the buffer id is 1 (second buffer)
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 0)
	    		{
	    		// receive the second display callback, the buffer id is 0 (first buffer)
	        	if (iBuffAvailCallback == 3)
	        		{
	        		EndTest(EPass);
	        		}
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
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererZeroTimeStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}

RTestRendererZeroTimeNonTimedStep* RTestRendererZeroTimeNonTimedStep::NewL(const TDesC& aStepName)
	{
	RTestRendererZeroTimeNonTimedStep* self = new (ELeave) RTestRendererZeroTimeNonTimedStep(aStepName);
	return self;
	}

RTestRendererZeroTimeNonTimedStep::RTestRendererZeroTimeNonTimedStep(const TDesC& aStepName) :
	RTestRendererStep(aStepName, EFalse)
	{
	}

void RTestRendererZeroTimeNonTimedStep::FsmL(TTestRendererEvents aEventCode)
	{
	switch (aEventCode)
	    {
	    case EStartTest:
	    	iFsmState = EStateCreate;
	        CreateRendererAndSurfaceL(3);
	        break;
	    case EBufferAvailable:
	    	iBuffAvailCallback++;
	    	if (iFsmState == EStateCreate && (iBuffAvailCallback == 1 || iBuffAvailCallback == 2))
        		{
        		break;
        		}
            else if (iFsmState == EStateCreate && iBuffAvailCallback == 3)
            	{
	            iFsmState = EStateUpdate;

	            // map surface to display
	            TInt err = SetBackgroundSurface(iSurfaceId);
	            TESTL (err == KErrNone);

	            // send the first buffer
	        	TTime presentationTime1(0);
	            GetNextBufferAndSubmitUpdateL(0, KRgbGreen, presentationTime1);

	        	// send the second buffer
	        	TTime nowTime;
	        	nowTime.UniversalTime();
	        	TTimeIntervalMicroSeconds delay2(500000); // use half second delay
	        	TTime presentationTime2 = nowTime + delay2;
	            GetNextBufferAndSubmitUpdateL(1, KRgbBlue, presentationTime2);

	        	// send the third buffer with zero time
	            GetNextBufferAndSubmitUpdateL(2, KRgbRed, presentationTime1);
            	}
            else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 1)
	    		{
	        	// buffer is available again after update
	    		}
            else if (iFsmState == EStateUpdate && iBuffAvailCallback == 4 && iBuffDisplayCallback == 2)
	    		{
	        	// buffer is available again after update
	    		}
            else if (iFsmState == EStateUpdate && iBuffAvailCallback == 5)
	    		{
	    		if(iBuffDisplayCallback == 3)
	    			{
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
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 2 && iBufferId == 2)
	    		{
	    		// receive the second display callback, the buffer id is 2 (third buffer)
	    		break;
	    		}
	    	else if (iFsmState == EStateUpdate && iBuffDisplayCallback == 3 && iBufferId == 1)
	    		{
	    		// receive the second display callback, the buffer id is 1 (first buffer)
	        	if (iBuffAvailCallback == 5)
	        		{
	        		EndTest(EPass);
	        		}
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
			ERR_PRINTF2(_L("Unexpected event code %d in RTestRendererZeroTimeNonTimedStep::FsmL"), aEventCode);
	    	EndTest(EFail);
	    	break;
	    }
	}
