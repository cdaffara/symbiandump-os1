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

#include "tmultiptreventhandlingperf.h"
#include <hal.h>

_LIT(KPerformanceTimingChunk, "PerformanceTimingChunk");

// Number of iterations for eventset
const TInt KNumOfIterations = 100;

// Maximum descriptor length for results
const TInt KMaxDescLength = 528;

// Static variables used for synchronisation between threads
GLDEF_D TInt NumOfEventsAdded = 0;
GLDEF_D RMutex Mutex;
GLDEF_D RCondVar ConVar;

void MultiPtrPerfPanic(TInt aPanic)
	{
	User::Panic(_L("MultiPterEventHanldingPerformancePanic"), aPanic);
	}

// This function opens the chunk and adds the failure description followed by '*'
TInt CMultiPtrPerfTestControl::Failed(TPoint3D aExp3DPoint, TPoint3D aActual3DPoint, TInt aErrorCode, TInt aExpPtrNum, TInt aActPtrNum)
	{
	TInt ret = iChunk.OpenGlobal(KPerformanceTimingChunk, ETrue);
	if (ret != KErrNone)
		{
		return ret;
		}
	TUint8* desPtr = iChunk.Base() + iChunkOffset;
	TPtr8 ptrDes(desPtr, 0, KMaxDescLength);
	
	TBuf<80> buf;
	if (aExpPtrNum != aActPtrNum)
		{
		_LIT(KFailedPointerNum, "Failed Expected Pointer Num = %d Actual Pointer Num = %d*");
		buf.AppendFormat(KFailedPointerNum, aExpPtrNum, aActPtrNum);
		ptrDes.Append(buf);
		} 
	else if (aErrorCode != KErrNone)
		{
		_LIT(KFailedErrorCode, "Failed Errorcode = %d*");
		buf.AppendFormat(KFailedErrorCode, aErrorCode);
		ptrDes.Append(buf);
		}
	else
		{
		_LIT(KFailedWrongCoord, "Failed Coordinates Expected = [%d, %d, %d] Actual = [%d, %d, %d]*");
		buf.AppendFormat(KFailedWrongCoord, aExp3DPoint.iX, aExp3DPoint.iY, aExp3DPoint.iZ, aActual3DPoint.iX, aActual3DPoint.iY, aActual3DPoint.iZ);
		ptrDes.Append(buf);
		}
	
	iChunk.Close();
	return ret;
	}

// This function opens the chunk and adds a pointer event not supported message followed by '#'
TInt CMultiPtrPerfTestControl::PointerEventsNotSupported()
    {
    iPointerEventNotSupported = ETrue;
    TInt ret = iChunk.OpenGlobal(KPerformanceTimingChunk, ETrue);
        if (ret != KErrNone)
            {
            return ret;
            }
    TUint8* desPtr = iChunk.Base() + iChunkOffset;
    TPtr8 ptrDes(desPtr, 0, KMaxDescLength);
    TBuf<80> buf;
    _LIT(KPointerEventsNotSupported, "Test Skipped As Pointer Events Are Not Supported By This Configuration#");
    buf.AppendFormat(KPointerEventsNotSupported);
    ptrDes.Append(buf);
    iChunk.Close();
    return ret;
    }

// This function calulates the average latency, opens the chunk and appends
// the same in descriptor form. Then close the child thread and calls function 
// for creating new thread for events. 
// If all the events have been tested then stops the tests
// by calling active CActiveScheduler::Stop. 
TInt CMultiPtrPerfTestControl::CalculateLatencyAndStartThread()
	{
	TInt ret = KErrNone;
	switch (iPtrAppUi->TestCaseNum())
		{
		case 1:
		case 5:
		case 9:
		case 13:	
			iAverageLatency = iLatency/(4*KNumOfIterations);
			break;
		case 2:
		case 6:
		case 10:
		case 14:
			iAverageLatency = iLatency/(8*KNumOfIterations);
			break;
		case 3:
		case 7:
		case 11:
		case 15:
			iAverageLatency = iLatency/(16*KNumOfIterations);
			break;
		case 4:
		case 8:
		case 12:
		case 16:
			iAverageLatency = iLatency/(32*KNumOfIterations);
			break;
		default:
			MultiPtrPerfPanic(EMultiPtrPerfPanicWrongTest);
		}

	ret = iChunk.OpenGlobal(KPerformanceTimingChunk, ETrue);
	if (ret != KErrNone)
		{
		return ret;
		}
	TUint8* desPtr = iChunk.Base() + iChunkOffset;
	TPtr8 ptrDes(desPtr, iLatLenInLetters, KMaxDescLength);
	
	TBuf<66> buf;
	buf.Num(iAverageLatency);
	buf.Append(',');
	ptrDes.Append(buf);
	
	// reset the iLatency for next set of test
	iLatency = 0;	
	
	// For writing the next latency in chunk memory
	iLatLenInLetters = ptrDes.Length();
	
	iChunk.Close();
	
	// Before calling the thread Function close the current thread and then call the function
	// here when we kill the child thread it releases both mutex and condvar that it holds
	iPtrAppUi->ChildThread().Kill(KErrNone);
	iPtrAppUi->ChildThread().Close();
	
	if (iPtrAppUi->TestCaseNum() == 16)
		{
		// It it is the last test case then stop the active scheduler
		CActiveScheduler::Stop();
		return ret;
		}
	else
		{
		return iPtrAppUi->CreateEventSimulatingThreads();
		}
	}

// This function is called by Cone whenever a event is outstanding
// Here we check if the sent event is same as the expected event in array.
// Then resumes the thread for next set of events. When all the events are completed
// calls CalculateLatencyAndStartThread
void CMultiPtrPerfTestControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
    ASSERT(!iPointerEventNotSupported);
	// The event that is received is (0,0,0) then it is the first event that the test code is sending
	const TAdvancedPointerEvent* advancedPointerEvent = aPointerEvent.AdvancedPointerEvent();
	
	if (iPtrAppUi->TestCaseNum() == 0)
		{
		if (advancedPointerEvent->PointerNumber() == 0 && advancedPointerEvent->Position3D() == TPoint3D())
			{
			iPtrAppUi->CreateEventSimulatingThreads();
			return;
			}
		else
			{
			return;	// ignore other events before starting the tests
			}
		}

	// Get the current time.
	// Subtract from the test start time.
	TTime currentTime;
	currentTime.UniversalTime();
	TInt64 curTime64 = currentTime.Int64();
	TInt timeSinceTestStart = curTime64 - iPtrAppUi->TestStartTime();
	
	// Get the event, get the time stored in Z corodinate
	// Then subtract the calculated time from Z coordinate
	iLatency += timeSinceTestStart - advancedPointerEvent->ProximityAndPressure(); 
	
	// Get the current event from buffer and compare it with got event
	TAdvancedPointerEvent expectedPtrEvent;
	if(0 == iEventBuffer.Remove(&expectedPtrEvent))
		{
		iPtrAppUi->Failed(TPoint3D(), TPoint3D(), KErrNotFound);
		return;
		}
	
	// Here Z corodinate is not checked.
	if (expectedPtrEvent.iPosition != advancedPointerEvent->iPosition)
		{
		iPtrAppUi->Failed(expectedPtrEvent.Position3D(), advancedPointerEvent->Position3D(), KErrNone);
		return;
		}
	
	if (expectedPtrEvent.PointerNumber() != advancedPointerEvent->PointerNumber())
		{
		iPtrAppUi->Failed(TPoint3D(), TPoint3D(), KErrNone, expectedPtrEvent.PointerNumber(), advancedPointerEvent->PointerNumber());
		return;
		}
	
	iPtrAppUi->DecrementEventCount();
	NumOfEventsAdded--;
	
	// Event count is zero when all the events have been tested
	if (iPtrAppUi->EventCount() == 0)
		{
		TInt ret = CalculateLatencyAndStartThread(); 
		if (ret != KErrNone)
			{
			iPtrAppUi->Failed(TPoint3D(), TPoint3D(), ret);
			return;
			}
		}
	else if (NumOfEventsAdded == 0) 
		{
		ConVar.Signal();
		}
	}

void CMultiPtrPerfTestControl::AddExpectedEvent(TAdvancedPointerEvent& aExpEvent)
	{
	iEventBuffer.Add(&aExpEvent);
	}

CMultiPtrPerfTestControl::~CMultiPtrPerfTestControl()
	{
	iChunk.Close();
	}

void CMultiPtrPerfTestControl::ConstructL()
	{
    iPointerEventNotSupported = EFalse;
	ConstructL(TPoint(), ControlEnv()->ScreenDevice()->SizeInPixels());
	}

void CMultiPtrPerfTestControl::ConstructL(TPoint aOrigin, TSize aSize)
	{
    iEventBuffer.SetLengthL(EEventBufferSize);
    
    iPtrAppUi = static_cast<CMultiPtrPerfTestAppUi*>(ControlEnv()->AppUi());
        
    CreateWindowL();
    Window().EnableAdvancedPointers();
    EnableDragEvents();
    SetExtent(aOrigin, aSize);
    ActivateL();
    
 	// Get the cmdline argument of this process in descriptor
 	// convert the descriptor to number and store it in iChunkOffset
 	TBuf<128> buf;
 	User::CommandLine(buf);
 	TLex lex(buf);
 	User::LeaveIfError(lex.Val(iChunkOffset));
	}

// This function gets the current time and subtracts it from the time when the whole test was started.
// Simulates the event and suspends the thread if aWaitAfterEachEvent is ETrue
// If aWaitAfterEachEvent is EFalse then it suspends the thread after each set of event i,e after 4, 8, 16 and 32 events 
void SimulatePointerEvents(TInt aCount, CMultiPtrPerfTestAppUi* appUi, TBool aMultiPtrEvent = EFalse, TBool aWaitAfterEachEvent = ETrue)
	{
	TRawEvent rawEvent;
	TPoint ptrPos;
	TTime currentTime;
	TInt64 testStartTime = appUi->TestStartTime();
	TInt ptrNum = 0;
	TInt ptrMax = 0;
	// HAL as already been tested at the start of these tests. So no need to test once again
	HAL::Get(HALData::EPointerNumberOfPointers, ptrMax);
	
	appUi->SetNumberOfEvents((KNumOfIterations*aCount)-1);

	// For testing time taken for each event and for each set of events we make use of RMutex and RConvar.
	// RMutex is used not to preempt this thread until events has been added
	// RConVar is used to hold this thread until the events have been tested
	for (TInt loop = 0; loop < KNumOfIterations; loop++)
		{
		ptrNum = 0;
		ptrPos.iX = ptrPos.iY = 0;
		Mutex.Wait();
		for (TInt count = 0; count < aCount/2; count++, ptrNum++)
			{
			if (ptrNum >= ptrMax)
				{
				ptrNum = 0;
				}			
			ptrPos.iX += 2;
			ptrPos.iY += 1;
			
			TWsEvent event;
			event.InitAdvancedPointerEvent(TPointerEvent::EButton1Down, 0, TPoint3D(ptrPos.iX, ptrPos.iY, 0), (aMultiPtrEvent ? ptrNum : 0));
			appUi->AddExpectedEvent(*event.Pointer());
			currentTime.UniversalTime();
			TInt64 curTime64 = currentTime.Int64();
			TInt timeSinceTestStart = curTime64 - testStartTime;
			rawEvent.Set(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, timeSinceTestStart, (aMultiPtrEvent ? ptrNum : 0));
			UserSvr::AddEvent(rawEvent);
			NumOfEventsAdded++;
			if (aWaitAfterEachEvent)
				{
				while(NumOfEventsAdded)
					ConVar.Wait(Mutex);
				}
			
			event.InitAdvancedPointerEvent(TPointerEvent::EButton1Up, 0, TPoint3D(ptrPos.iX, ptrPos.iY, 0), (aMultiPtrEvent ? ptrNum : 0));
			appUi->AddExpectedEvent(*event.Pointer());
			currentTime.UniversalTime();
			curTime64 = currentTime.Int64();
			timeSinceTestStart = curTime64 - testStartTime;
			rawEvent.Set(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, timeSinceTestStart, (aMultiPtrEvent ? ptrNum : 0));
			UserSvr::AddEvent(rawEvent);
			NumOfEventsAdded++;
			if (aWaitAfterEachEvent)
				{
				while(NumOfEventsAdded)
					ConVar.Wait(Mutex);
				}
			}
		if (!aWaitAfterEachEvent)
			{
			while(NumOfEventsAdded)
				ConVar.Wait(Mutex);
			}
		Mutex.Signal();
		}
	}

TInt EventSimulatingThreadStartFunc(TAny* aAny)
	{
	CMultiPtrPerfTestAppUi* appUi = static_cast<CMultiPtrPerfTestAppUi*>(aAny);

	switch(appUi->TestCaseNum()++)
		{
		case 0:
			SimulatePointerEvents(4, appUi);
			break;
		case 1:
			SimulatePointerEvents(8, appUi);
			break;
		case 2:
			SimulatePointerEvents(16, appUi);
			break;
		case 3:
			SimulatePointerEvents(32, appUi);
			break;
		case 4:
			SimulatePointerEvents(4, appUi, ETrue);
			break;
		case 5:
			SimulatePointerEvents(8, appUi, ETrue);
			break;
		case 6:
			SimulatePointerEvents(16, appUi, ETrue);
			break;
		case 7:
			SimulatePointerEvents(32, appUi, ETrue);
			break;
		case 8:
			SimulatePointerEvents(4, appUi, EFalse, EFalse);
			break;
		case 9:
			SimulatePointerEvents(8, appUi, EFalse, EFalse);
			break;
		case 10:
			SimulatePointerEvents(16, appUi, EFalse, EFalse);
			break;
		case 11:
			SimulatePointerEvents(32, appUi, EFalse, EFalse);
			break;
		case 12:
			SimulatePointerEvents(4, appUi, ETrue, EFalse);
			break;
		case 13:
			SimulatePointerEvents(8, appUi, ETrue, EFalse);
			break;
		case 14:
			SimulatePointerEvents(16, appUi, ETrue, EFalse);
			break;
		case 15:
			SimulatePointerEvents(32, appUi, ETrue, EFalse);
			break;		
		default:
			break;
		}
	return KErrNone;
	}

TInt CMultiPtrPerfTestAppUi::CreateEventSimulatingThreads()
	{
	// Depending upon the iTestCase number create thread and simulate events in that thread function
	TInt ret = KErrNone;
	_LIT(KSimulateEventsThread, "Events simulating thread");
	ret = iThread.Create(KSimulateEventsThread, EventSimulatingThreadStartFunc, KDefaultStackSize, 0x4000, 0x4000, this, EOwnerThread);
	if (ret == KErrNone)
		{
		iThread.Resume();
		}
	return ret;
	}

void CMultiPtrPerfTestAppUi::AddExpectedEvent(TAdvancedPointerEvent& aExpEvent)
	{
	iControl->AddExpectedEvent(aExpEvent);
	}

void CMultiPtrPerfTestAppUi::Failed(TPoint3D aExp3DPoint, TPoint3D aActual3DPoint, TInt aErrorCode, TInt aExpPtrNum, TInt aActPtrNum)
	{
	// Write error description in the memory of shared chunk 
	// so that the main performance test just stops by saying that it failed
	// and then stop the active scheduler.
	iControl->Failed(aExp3DPoint, aActual3DPoint, aErrorCode, aExpPtrNum, aActPtrNum);
	CActiveScheduler::Stop();
	}

void CMultiPtrPerfTestAppUi::PointerEventsNotSupported()
    {
    // Write a pointer events not supported message in the memory of shared chunk 
    // so that the main performance test just skips the pointer test
    iControl->PointerEventsNotSupported();
    }

CMultiPtrPerfTestAppUi::CMultiPtrPerfTestAppUi()
	{
	}

CMultiPtrPerfTestAppUi::~CMultiPtrPerfTestAppUi()
	{
	RemoveFromStack(iControl);
	delete iControl;
	}

void CMultiPtrPerfTestAppUi::ConstructL()
	{
	CCoeAppUi::ConstructL();
	
	TTime testStartTime;
	testStartTime.UniversalTime();
	iTestStartTime = testStartTime.Int64();
	
	iControl = new (ELeave) CMultiPtrPerfTestControl();
	iControl->ConstructL();
	AddToStackL(iControl);
	}

void ConstructControlEnvironmentL(CCoeEnv* aCoe)
	{
 	aCoe->ConstructL();
 	CMultiPtrPerfTestAppUi* appUi=new(ELeave) CMultiPtrPerfTestAppUi();
 	CleanupStack::PushL(appUi);			// If it leaves after this then there is no way of deleting the appui
 	aCoe->SetAppUi(appUi);				// So pushed it on to cleanup stack
 	aCoe->WsSession().SetAutoFlush(ETrue);
 	appUi->ConstructL();
 	User::LeaveIfError(Mutex.CreateLocal(EOwnerProcess));
 	User::LeaveIfError(ConVar.CreateLocal(EOwnerProcess));
 	CleanupStack::Pop(appUi);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CCoeEnv* coe = new CCoeEnv;
	if (!coe)
		{
		return KErrNoMemory; 
		}
	TRAPD(err, ConstructControlEnvironmentL(coe));
	if (err != KErrNone)
		{
		delete coe;
		}
	else	// If KErrNone then no need to delte coe as it is taken care by ConEnv
		{
        // Check whether the configuration supports pointer events.
        // If it dosn't support pointer events then the pointer cursor area will be empty,
        // in this case skip the test and inform the test framework that the test has been skipped
        TRect pointerCursorArea = coe->WsSession().PointerCursorArea();
        if(pointerCursorArea.IsEmpty())
            {
            RDebug::Printf("PointerCursorArea is Empty");
            CMultiPtrPerfTestAppUi* appUi = static_cast<CMultiPtrPerfTestAppUi*>(coe->AppUi());
            appUi->PointerEventsNotSupported();
            }
        else
            {
            // First event which starts the test from HandlePointerEventL
            TRawEvent rawEvent;
            rawEvent.Set(TRawEvent::EButton1Down, 0, 0, 0, 0);
            UserSvr::AddEvent(rawEvent);
            coe->ExecuteD();
            }
		Mutex.Close();
		ConVar.Close();
	 	}
	
	__UHEAP_MARKEND;
	return err;
	}
