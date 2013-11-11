// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Multi pointer and Z-coordinate test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __INIPARSER_H__
#define __INIPARSER_H__
#endif // __INIPARSER_H__

#include "PARSEINIDATA.H"
#include "tmultiptrevent.h"
#include <e32cmn.h>
#include <hal.h>

//#define FAILLOG 1			// Uncomment this code to get detailed log
#define TESTMULTIPOINTER 1	// Comment this line to get detailed log without testing

_LIT(KMultiPtrPluginDll, "multiptrclick.dll");

const TInt KErrDescSize = 128;
const TInt KMaxQueueSize = 32;	// Queue size of wserv. In any test we cannot test more than MaxQueueSize events. 

LOCAL_C void DestroyAnim(TAny* aAnim)
	{
	(static_cast<RMultiPointerAnim*>(aAnim))->Destroy();
	}


//CTMultiPtrEventBuffer
CTMultiPtrEventBuffer::CTMultiPtrEventBuffer(RWsSession *aWs, CTMultiPtrEventTest *aTest, CTMultiPtrEventClient* aClient) 
	: CTEvent(aWs), iTest(aTest), iClient(aClient)
	{
	}

void CTMultiPtrEventBuffer::ConstructL()
	{
	CTEventBase::Construct();
	iEventBuffer.SetLengthL(EEventBufferSize);
	iMovePtsBuffer.SetLengthL(EMovePtsBuffer);
	}

void CTMultiPtrEventBuffer::AddExpectedEvent(TWsEvent &aEvent)
	{
	iEventBuffer.Add(&aEvent);
	}

void CTMultiPtrEventBuffer::AddExpectedMovePtEvent(TPoint &aMovePt)
	{
	iMovePtsBuffer.Add(&aMovePt);
	}

TInt CTMultiPtrEventBuffer::EventsRemaining()
	{
	return iEventBuffer.Count();
	}

void CTMultiPtrEventBuffer::GetMoveBufferAndCompareL()
	{
	TBuf8<EMovePtsBuffer*sizeof(TPoint)> buf;
	iClient->ParentWin()->BaseWin()->RetrievePointerMoveBuffer(buf);
	
	TUint bufferSize = buf.Length();
	TUint numLoop = bufferSize/sizeof(TPoint);
	
	if (iMovePtsBuffer.Count() != numLoop)
		{
#if defined(FAILLOG)
		TLogMessageText logText;
		_LIT(KPointerNumber, "Actual number of move/drag buffer points from Wserv = %d Expected number of move/drag buffer points = %d ");
		logText.Format(KPointerNumber, numLoop, iMovePtsBuffer.Count());
		iTest->LOG_MESSAGE(logText);
#endif
		TestL(EFalse);
		}
	
	TPoint wsPoint;
	TPoint expPoint;
	TUint8* BasePtr = const_cast<TUint8*>(buf.Ptr());
	for (TUint count = 0; count < numLoop; count++)
		{
		wsPoint = *(reinterpret_cast<TPoint*>(BasePtr));
		iMovePtsBuffer.Remove(&expPoint);
		if (wsPoint != expPoint)
			{
#if defined(FAILLOG)
			TLogMessageText logText;
			_LIT(KMoveBufferPoint, "Actual move/drag buffer point from Wserv = ([%d], [%d]) Expected move/drag buffer point = ([%d], [%d])");
			logText.Format(KMoveBufferPoint, wsPoint.iX, wsPoint.iY, expPoint.iX, expPoint.iY);
			iTest->LOG_MESSAGE(logText);
#endif
			TestL(EFalse);
			}
		BasePtr = BasePtr + sizeof(TPoint);
		}
	}


#define TESTXL(expr1, oper, expr2, event, expected) \
    TestL(expr1 oper expr2, expr1, expr2, #oper, #expr1 ## #oper ## #expr2, \
            event, expected, __FILE__, __LINE__);

// Main function which gets the event, checks with the event in buffer
// Then calls function NextSetOfEventsL for running other tests of a particualar test case
void CTMultiPtrEventBuffer::doRunL()
	{
	// Get the event from wserv
	TWsEvent wsEvent;
	iWs->GetEvent(wsEvent);

	TWsEvent expectedEvent;
	TInt wsType=wsEvent.Type();
	if (wsEvent.Type()==EEventNull)
		{
		TESTXL(wsEvent.Handle(), ==, 0, &wsEvent, NULL);
		goto End;
		}
	// This code is for running successive anim test code 
	if (wsEvent.Type()==EEventPointer && wsEvent.Pointer()->iType==TPointerEvent::ESwitchOn)
		goto End;
	// if this is called accidentally
	if (iEventBuffer.Count()==0 && wsType==EEventFocusGained)
		goto End;

	iEventBuffer.Remove(&expectedEvent);
	iEventCount++;

	if (wsEvent.Type() == EEventPointerBufferReady)
		{
		GetMoveBufferAndCompareL();
#if defined(FAILLOG)
		TLogMessageText logText;
		_LIT(KEventHandle, "Actual Window Handle from Wserv = %d Expected Window Handle = %d ");
		logText.Format(KEventHandle, wsEvent.Handle(), expectedEvent.Handle());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)
		TESTXL(wsEvent.Handle(), ==, expectedEvent.Handle(), &wsEvent, &expectedEvent);
#endif
		goto End;
		}

	// Test wsEvent and expected event have same handle, position and type.
#if defined(FAILLOG)
	{
	TLogMessageText logText;
	_LIT(KEventCountCheck, "Checking event number = %d");
	logText.Format(KEventCountCheck, iEventCount);
	iTest->LOG_MESSAGE(logText);
	_LIT(KEventType, "Actual Event type from Wserv = %d Expected Event Type = %d ");
	logText.Format(KEventType, wsEvent.Type(), expectedEvent.Type());
	iTest->LOG_MESSAGE(logText);
	}
#endif

#if defined(TESTMULTIPOINTER)
	TESTXL(wsEvent.Type(), ==, expectedEvent.Type(), &wsEvent, &expectedEvent);
#endif
	// Test pointer number for Enter/Exit events
	if (wsType == EEventPointerEnter || wsType == EEventPointerExit)
		{
#if defined(FAILLOG)
		TLogMessageText logText;
		_LIT(KPointerNumber, "Actual PointerNumber for Enter/Exit event from Wserv = %d Expected PointerNumber for Enter/Exit event = %d ");
		logText.Format(KPointerNumber, *wsEvent.Int(), *expectedEvent.Int());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)
		TESTXL(*wsEvent.Int(), ==, *expectedEvent.Int(), &wsEvent, &expectedEvent);
#endif
		}

	// Test only if it is a Pointer event
	if (wsType==EEventPointer)
		{
		TAdvancedPointerEvent* expectedPointerEvent=expectedEvent.Pointer();
		TAdvancedPointerEvent* actualPointerEvent=wsEvent.Pointer();
#if defined(FAILLOG)
		TLogMessageText logText;
		_LIT(KEventHandle, "Actual Window Handle from Wserv = %d Expected Window Handle = %d ");
		logText.Format(KEventHandle, wsEvent.Handle(), expectedEvent.Handle());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)
		TESTXL(wsEvent.Handle(), ==, expectedEvent.Handle(), &wsEvent, &expectedEvent);
#endif
#if defined(FAILLOG)
		_LIT(KPointerType, "Actual PointerType from Wserv = %d Expected PointerType = %d ");
		logText.Format(KPointerType, actualPointerEvent->iType, expectedPointerEvent->iType);
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)
		TESTXL(actualPointerEvent->iType, ==, expectedPointerEvent->iType, &wsEvent, &expectedEvent);
#endif
#if defined(FAILLOG)
		_LIT(KPointerPosition, "Actual PointerPosition from Wserv = (%d, %d) Expected PointerPosition = (%d, %d) ");
		logText.Format(KPointerPosition, actualPointerEvent->iPosition.iX, actualPointerEvent->iPosition.iY, expectedPointerEvent->iPosition.iX, expectedPointerEvent->iPosition.iY);
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)		
		TESTXL(actualPointerEvent->iPosition, ==, expectedPointerEvent->iPosition, &wsEvent, &expectedEvent);
#endif		
#if defined(FAILLOG)
		_LIT(KPointerNumber, "Actual PointerNumber from Wserv = %d Expected PointerNumber = %d ");
		logText.Format(KPointerNumber, actualPointerEvent->PointerNumber(), expectedPointerEvent->PointerNumber());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)		
		TESTXL(actualPointerEvent->PointerNumber(), ==, expectedPointerEvent->PointerNumber(), &wsEvent, &expectedEvent);
#endif
#if defined(FAILLOG)
		_LIT(KPointerProximity, "Actual PointerProximity from Wserv = %d Expected PointerProximity = %d ");
		logText.Format(KPointerProximity, actualPointerEvent->Proximity(), expectedPointerEvent->Proximity());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)		
		TESTXL(actualPointerEvent->Proximity(), ==, expectedPointerEvent->Proximity(), &wsEvent, &expectedEvent);
#endif		
#if defined(FAILLOG)
		_LIT(KPointerPressure, "Actual PointerPressure from Wserv = %d Expected PointerPressure = %d ");
		logText.Format(KPointerPressure, actualPointerEvent->Pressure(), expectedPointerEvent->Pressure());
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)		
		TESTXL(actualPointerEvent->Pressure(), ==, expectedPointerEvent->Pressure(), &wsEvent, &expectedEvent);
#endif
#if defined(FAILLOG)
		_LIT(KPointerModifier, "Actual Modifier value from Wserv = 0x%08x Expected Modfier value = 0x%08x ");
		logText.Format(KPointerModifier, actualPointerEvent->iModifiers, expectedPointerEvent->iModifiers);
		iTest->LOG_MESSAGE(logText);
#endif
#if defined(TESTMULTIPOINTER)		
		TESTXL((actualPointerEvent->iModifiers&expectedPointerEvent->iModifiers), ==, expectedPointerEvent->iModifiers, &wsEvent, &expectedEvent);
#endif		
		}
End:
	// Call NextSetOfEvents() only for primary client
	if (iEventBuffer.Count()==0 && !iClient->IsSecondaryClient())	
		{
		iTest->NextSetOfEventsL();		// Cannot Leave
		}
	
	// If this is for secondary client and active scheduler(nested loop) has started, this code stops that scheduler
	if (iEventBuffer.Count()==0 && iClient->IsSecondaryClient() && iNestedLoopStarted)
		{
		iNestedLoopStarted = EFalse;
		CActiveScheduler::Stop();
		}
	}

void CTMultiPtrEventBuffer::TestL(TInt aTest)
	{
	if (!aTest)
		{
		iTest->Failed();
		User::Leave(ETestFailed);
		}
	}

static void DumpEvent(const char *aName, const TWsEvent *aEvent)
    {
    // aEvent may be NULL - if it isn't NULL, we print it's data.
    if (aEvent)
        {
        RDebug::Printf("%s:", aName);
        RDebug::Printf("handle = %u", aEvent->Handle());
        RDebug::Printf("type() = %d", aEvent->Type());
        if (aEvent->Type() == EEventPointer)
            {
            TAdvancedPointerEvent *pointer = aEvent->Pointer();
            RDebug::Printf("pointer type = %d, position = (%d, %d)", 
                    pointer->iType, pointer->iPosition.iX, pointer->iPosition.iY);
            RDebug::Printf("PointerNumber() = %d, proximity=%d, pressure=%d", 
                    pointer->PointerNumber(), pointer->Proximity(), pointer->Pressure());
            RDebug::Printf("modifiers = 0x%08x", pointer->iModifiers);
            }
        else
            {
            RDebug::Printf("Not a pointer event, event type");
            }
        }
    else
        {
        // aEvent is NULL, just let us know it is. 
        RDebug::Printf("%s: is NULL which means not applicapable");
        }
    }

void CTMultiPtrEventBuffer::TestL(TInt aTest, TInt aVal1, TInt aVal2, const char *aOper, const char *aTestStr, 
        const TWsEvent *aEvent, const TWsEvent *aExpected, const char *aFile, TUint aLine)
    {
    if (!aTest)
        {
        RDebug::Printf("Expression %d %s %d [%s] (=%d) failed [call from %s:%d]", aVal1, aOper, aVal2, aTestStr, aTest, aFile, aLine);
        DumpEvent("actual", aEvent);
        DumpEvent("expected", aExpected);
        iTest->Failed();
        User::Leave(ETestFailed);
        }
    }


void CTMultiPtrEventBuffer::TestL(TInt aTest, TPoint aVal1, TPoint aVal2, const char *oper, const char *aTestStr, 
        const TWsEvent *aEvent, const TWsEvent *aExpected, const char *aFile, TUint aLine)
    {
    if (!aTest)
        {
        RDebug::Printf("Expression (%d, %d) %s (%d, %d) [%s] (=%d) failed [call from %s:%d]", 
                aVal1.iX, aVal1.iY, oper, aVal2.iX, aVal2.iY, aTestStr, aTest, aFile, aLine);
        DumpEvent("actual", aEvent);
        DumpEvent("expected", aExpected);
        iTest->Failed();
        User::Leave(ETestFailed);
        }
    }

//CTMultiPtrEventClient
CTMultiPtrEventClient::CTMultiPtrEventClient(CTMultiPtrEventTest *aTest, TBool aSecondaryClient) 
	: iTest(aTest), iSecondaryClient(aSecondaryClient)
	{
	}

CTMultiPtrEventClient::~CTMultiPtrEventClient()
	{
	if (iWinAutoFocus1 || iWinAutoFocus2 || iGroupWinAutoFocus2 || iGroupWinAutoFocus1)
		{
		DeleteGroupBlankWin();
		}
	delete iChildWin;
	delete iParentWin;
	}

void CTMultiPtrEventClient::ConstructL()
	{
	CTClient::ConstructL();
	
	// Create a group window and assign it iGroup
	iGroup = new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();
	
	iParentWinSize = iScreen->SizeInPixels();
	iParentWinPos = TPoint();
	
	// Change the parent window size and position for secondary client
	if (iSecondaryClient)
		{
		iParentWinSize.iWidth /= 2; 
		iParentWinPos.iX = iParentWinSize.iWidth;
		}
	
	// Create window/s for this client so that events can come to this client
	iParentWin=new(ELeave) CTBlankWindow();
	iParentWin->ConstructL(*iGroup);
	iParentWin->SetExt(iParentWinPos, iParentWinSize);
	iParentWin->SetColor(KRgbRed);
	if (iTest->TestBase()->iState != 12)
		{
		iParentWin->BaseWin()->EnableAdvancedPointers();
		}
	iParentWin->Activate();
	
	// Don't create child window for secodary client
	if (!iSecondaryClient)
		{
		iChildWinSize = TSize(iParentWinSize.iWidth/2, iParentWinSize.iHeight);
		iChildWinPos = TPoint(iParentWinSize.iWidth/2,0);
		
		iChildWin=new(ELeave) CTBlankWindow();
		iChildWin->ConstructL(*iGroup);
		iChildWin->SetExt(iChildWinPos, iChildWinSize);
		iChildWin->SetColor(KRgbRed);
		iChildWin->BaseWin()->EnableAdvancedPointers();
		iChildWin->Activate();
		}
	}

void CTMultiPtrEventClient::CalculatePtrPosAndSet3Ddata(TWsEvent& aEvent, TPointerEvent::TType aType, TPoint aPos, TUint aModifiers, TInt aZ, TUint8 aPointerNumber, TUint aHandle)
	{
	// If in future you need to change parent position then put this in corresponding if statements
	aEvent.Pointer()->iParentPosition = aPos;
	
	// This is for Auto foreground test
	if (iTest->TestBase()->iState == 11)
		{
		if (TRect(iWinPosAutoFocus1, iWinSizeAutoFocus1).Contains(aPos))
			{
			aPos -= iWinPosAutoFocus1;
			aEvent.SetHandle((TUint)iWinAutoFocus1);
			}
		else if (TRect(iWinPosAutoFocus2, iWinSizeAutoFocus2).Contains(aPos))
			{
			aPos -= iWinPosAutoFocus2;
			aEvent.SetHandle((TUint)iWinAutoFocus2);
			}
		goto SETOTHERDATA;
		}

	// Here simulated pointer position is w.r.t to 0,0
	// Actual pointer position is w.r.t windowPosition
	if (iSecondaryClient)
		{
		if (TRect(iParentWinPos, TSize(iParentWinSize.iWidth, iParentWinSize.iHeight)).Contains(aPos))
			{
			// Since for secondary client parent window is at half of screensize
			aPos -= iParentWinPos;
			aEvent.SetHandle((aHandle == 0) ? (TUint)iParentWin : aHandle);
			}
		}
	else
		{
		// If sent position is on blankWindow then
		if (TRect(iParentWinPos, TSize(iParentWinSize.iWidth/2, iParentWinSize.iHeight)).Contains(aPos))
			{
			aEvent.SetHandle((aHandle == 0) ? (TUint)iParentWin : aHandle);
			}
		else // else the pointer event occured on child window
			{
			// if aPos < 0 then position has been generated in test code and should not be adjusted
			if (aPos.iX >= 0)
				{
				aPos -= iChildWinPos;	
				}
			aEvent.SetHandle((aHandle == 0) ? (TUint)iChildWin : aHandle);
			}
		}
	
SETOTHERDATA:	
	aEvent.Pointer()->iType = aType;
	aEvent.Pointer()->iPosition = aPos;
	if(iExpectNonAdvancedPointerEvents)
		{
		aEvent.Pointer()->iModifiers = aModifiers;		
		}
	else
		{
		aEvent.Pointer()->iModifiers = aModifiers|EModifierAdvancedPointerEvent;
		aEvent.SetPointerNumber(aPointerNumber);
		aEvent.SetPointerZ(aZ);
		}
	}

void CTMultiPtrEventClient::AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TUint8 aPointerNumber, TUint aHandle)
	{
	AddExpectedPointerEvent(aType, aPos, 0, 0, aPointerNumber, aHandle);
	}

void CTMultiPtrEventClient::AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle)
	{
	TWsEvent ptrEvent;
	ptrEvent.SetType(EEventPointer);
	if (aType != TPointerEvent::ESwitchOn)
		{
		CalculatePtrPosAndSet3Ddata(ptrEvent, aType, aPos, aModifier, aZ, aPointerNumber, aHandle);
		}
	static_cast<CTMultiPtrEventBuffer*>(iEventHandler)->AddExpectedEvent(ptrEvent);
	}

void CTMultiPtrEventClient::AddExpectedWsEvent(TEventCode aType, TInt aPointerNumber, TUint aHandle)
	{
	TWsEvent ptrEvent;
	ptrEvent.SetType(aType);
	if (aType == EEventPointerEnter || aType == EEventPointerExit)
		{
		*ptrEvent.Int() = aPointerNumber;
		}
	if (aType == EEventPointerBufferReady)
		{
		ptrEvent.SetHandle(aHandle);
		}
	static_cast<CTMultiPtrEventBuffer*>(iEventHandler)->AddExpectedEvent(ptrEvent);
	}

void CTMultiPtrEventClient::AddExpectedMovePoint(TPoint aPos)
	{
	static_cast<CTMultiPtrEventBuffer*>(iEventHandler)->AddExpectedMovePtEvent(aPos);
	}

// Create and store CTMultiPtrEventBuffer in iEventHandler
void CTMultiPtrEventClient::ConstructEventHandlerL()
	{
	CTMultiPtrEventBuffer* eventBufferAndHandler = new(ELeave) CTMultiPtrEventBuffer(&iWs, iTest, this);
	eventBufferAndHandler->ConstructL();
	iEventHandler = eventBufferAndHandler;
	}

// Create groupwindow and blank window for AutoFocus tests
void CTMultiPtrEventClient::ConstructGroupBlankWinL()
	{
	iWinPosAutoFocus1 = TPoint(10,10);
	iWinSizeAutoFocus1 = TSize(40,40);
	
	iWinPosAutoFocus2 = TPoint(50,50);
	iWinSizeAutoFocus2 = TSize(80,80);
	
	iGroupWinAutoFocus1 = new(ELeave) CTWindowGroup(this);
	iGroupWinAutoFocus1->ConstructL();
	iGroupWinAutoFocus1->GroupWin()->AutoForeground(ETrue);
	iGroupWinAutoFocus1->GroupWin()->EnableFocusChangeEvents();
	
	iGroupWinAutoFocus2 = new(ELeave) CTWindowGroup(this);
	iGroupWinAutoFocus2->ConstructL();
	iGroupWinAutoFocus2->GroupWin()->AutoForeground(ETrue);
	iGroupWinAutoFocus2->GroupWin()->EnableFocusChangeEvents();
	
	iWinAutoFocus1=new(ELeave) CTBlankWindow();
	iWinAutoFocus1->ConstructL(*iGroupWinAutoFocus1);
	iWinAutoFocus1->SetExt(iWinPosAutoFocus1, iWinSizeAutoFocus1);
	iWinAutoFocus1->SetColor(KRgbDarkYellow);
	iWinAutoFocus1->BaseWin()->EnableAdvancedPointers();
	iWinAutoFocus1->Activate();
	
	iWinAutoFocus2=new(ELeave) CTBlankWindow();
	iWinAutoFocus2->ConstructL(*iGroupWinAutoFocus2);
	iWinAutoFocus2->SetExt(iWinPosAutoFocus2, iWinSizeAutoFocus2);
	iWinAutoFocus2->SetColor(KRgbDarkCyan);
	iWinAutoFocus2->BaseWin()->EnableAdvancedPointers();
	iWinAutoFocus2->Activate();
	}

void CTMultiPtrEventClient::DeleteGroupBlankWin()
	{
	delete iWinAutoFocus2;
	iWinAutoFocus2 = NULL;
	delete iGroupWinAutoFocus2;
	iGroupWinAutoFocus2 = NULL;
	delete iWinAutoFocus1;
	iWinAutoFocus1 = NULL;
	delete iGroupWinAutoFocus1;
	iGroupWinAutoFocus1 = NULL;
	}

TBool CTMultiPtrEventClient::CheckOrdinalPosition(TInt aAutoFocusWin)
	{
	if (aAutoFocusWin == 1)
		{
		return iWinAutoFocus1->BaseWin()->OrdinalPosition() == 0;
		}
	else
		{
		return iWinAutoFocus2->BaseWin()->OrdinalPosition() == 0;
		}
	}

//CTMultiPtrEventTest
CTMultiPtrEventTest::CTMultiPtrEventTest(CTestStep* aStep)
	: CTWsGraphicsBase(aStep)
	{
	}

CTMultiPtrEventTest::~CTMultiPtrEventTest()
	{
	((CTMultiPtrEventTestStep*)iStep)->CloseTMSGraphicsStep();
	delete iMultiPtrEventClient;
	DestroyAnimDllAndAnim();	
	if (iPtrPluginLoaded)
		{
		iClick.Unload();
		}
	iClick.Close();
	if (iHeap)
		{
		iHeap->Close();
		iChunk.Close();
		}
	DeleteGroupWin();
	}

void CTMultiPtrEventTest::ConstructL()
	{
	// Here check if the HAL configuration are correct if not then finish the test case
	TInt ret = HAL::Get(HALData::EPointerMaxPointers, iMaxDevPointers);
	if (ret != KErrNone || iMaxDevPointers < 2 || iMaxDevPointers > 8)
		{
		TLogMessageText logText1;
		_LIT(KWrongHALConfig, "HAL configuration are incorrect. \n");
		logText1.Append(KWrongHALConfig);
		LOG_MESSAGE(logText1);
		TEST(EFalse);
		User::Leave(ret);
		}
	
	// Get the data by using HAL api
  	User::LeaveIfError(HAL::Get(HALData::EPointerMaxPointers,iMaxDevPointers));
  	User::LeaveIfError(HAL::Get(HALData::EPointerNumberOfPointers,iMaxUiPointers));
  	User::LeaveIfError(HAL::Get(HALData::EPointer3DMaxPressure, iMaxPressure));
  	User::LeaveIfError(HAL::Get(HALData::EPointer3DPressureStep, iPressureStep));
  	User::LeaveIfError(HAL::Get(HALData::EPointer3DMaxProximity, iMaxProximity));
  	User::LeaveIfError(HAL::Get(HALData::EPointer3DProximityStep, iProximityStep));
	
    _LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
    CIniData* iniData = CIniData::NewL(KWsIniFile, ' ');
    _LIT( KWsiniYShifting, "YSHIFTING");
    iniData->FindVar(KWsiniYShifting, iYOffset);
    delete iniData;
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);
	iPhysicalScreenSize = TheClient->iScreen->SizeInPixels();
	
	TheGc->Activate(*BaseWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(12));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(BaseWin->Win()->Size()));
	TheGc->Deactivate();
	
	TheGc->Activate(*TestWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(4));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(TestWin->Win()->Size()));
	TheGc->Deactivate();
	
	TheClient->iWs.Flush();
	}

// Create raw event setting type, X, Y, Z and pointernumber for rawevent
// use UserSvr for simulating the event
void CTMultiPtrEventTest::SimulatePointerEvent(TRawEvent::TType aType, TInt aX, TInt aY, TUint8 aPointerNumber)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType, aX, aY+iYOffset, aPointerNumber);
	UserSvr::AddEvent(rawEvent);
	}

void CTMultiPtrEventTest::SimulatePointerEvent(TRawEvent::TType aType, TInt aX, TInt aY, TInt aZ, TUint8 aPointerNumber)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType, aX, aY+iYOffset, aZ, aPointerNumber);
	UserSvr::AddEvent(rawEvent);
	}

// Simulate multipointer down and up event
void CTMultiPtrEventTest::SimulatePointerDownUp(TInt aX, TInt aY, TInt aZ, TUint8 aPointerNumber)
	{
	SimulatePointerEvent(TRawEvent::EButton1Down, aX, aY, aZ, aPointerNumber);
	SimulatePointerEvent(TRawEvent::EButton1Up, aX, aY, aZ, aPointerNumber);
	}

void CTMultiPtrEventTest::AddExpectedKey(TInt aEventCode, TInt aScanCode, TInt aCode/*=0*/, TInt aRepeats/*=0*/, TUint aModifiers/*=0*/)
	{
	TPckgBuf<TWsEvent> evtPkg;
	TWsEvent& event=evtPkg();
	if (aEventCode==EEventKey && aCode==0)
		aCode=aScanCode;
	event.SetType(aEventCode);
	event.SetHandle((TUint)iMultiPtrEventClient->iGroup);
	event.Key()->iCode=aCode;
	event.Key()->iScanCode=aScanCode;
	event.Key()->iModifiers=aModifiers;
	event.Key()->iRepeats=aRepeats;
	iMultiPtrEventClient->EventBuffer()->AddExpectedEvent(event);
	}

void CTMultiPtrEventTest::AddExpectedKeyDownUp(TInt aScanCode, TInt aCode/*=0*/, TInt aRepeats/*=0*/, TUint aModifiers/*=0*/)
	{
	__ASSERT_DEBUG(aScanCode<'a' || aScanCode>'z',AutoPanic(EAutoPanicScanCapital));
	AddExpectedKey(EEventKeyDown,aScanCode,0,aRepeats,aModifiers);
	AddExpectedKey(EEventKey,aScanCode,aCode,aRepeats,aModifiers);
	AddExpectedKey(EEventKeyUp,aScanCode,0,aRepeats,aModifiers);
	}

void CTMultiPtrEventTest::AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TUint8 aPointerNumber, TUint aHandle)
	{
	AddExpectedPointerEvent(aType, aPos, 0, 0, aPointerNumber, aHandle);
	}

void CTMultiPtrEventTest::AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle, TBool aSecondaryClient)
	{
	if (aSecondaryClient)
		{
		iSecMultiPtrEventClient->AddExpectedPointerEvent(aType, aPos, aZ, aModifier, aPointerNumber, aHandle);
		}
	else
		{
		iMultiPtrEventClient->AddExpectedPointerEvent(aType, aPos, aZ, aModifier, aPointerNumber, aHandle);
		}
	}

void CTMultiPtrEventTest::SetExpectNonAdvancedPointerEvents(TBool aSecondaryClient)
	{
	if (aSecondaryClient)
		{
		iSecMultiPtrEventClient->SetExpectNonAdvancedPointerEvents();
		}
	else
		{
		iMultiPtrEventClient->SetExpectNonAdvancedPointerEvents();
		}
	}

void CTMultiPtrEventTest::ClearExpectNonAdvancedPointerEvents(TBool aSecondaryClient)
	{
	if (aSecondaryClient)
		{
		iSecMultiPtrEventClient->ClearExpectNonAdvancedPointerEvents();
		}
	else
		{
		iMultiPtrEventClient->ClearExpectNonAdvancedPointerEvents();
		}
	}

void CTMultiPtrEventTest::AddExpectedPointerDownUp(TPoint aPos, TInt aZ, TUint8 aPointerNumber)
	{
	AddExpectedPointerEvent(TPointerEvent::EButton1Down, aPos, aZ, 0, aPointerNumber);
	AddExpectedPointerEvent(TPointerEvent::EButton1Up, aPos, aZ, 0, aPointerNumber);
	}

void CTMultiPtrEventTest::AddExpectedWsEvent(TEventCode aType, TBool aSecondaryClient, TInt aPointerNumber, TUint aHandle)
	{
	if (aSecondaryClient)
		{
		iSecMultiPtrEventClient->AddExpectedWsEvent(aType);
		}
	else
		{
		iMultiPtrEventClient->AddExpectedWsEvent(aType, aPointerNumber, aHandle);
		}
	}

void CTMultiPtrEventTest::TestDragForMultiPointer(TPoint aPtrPos, TInt aPrValue, TUint8 aPointerNumber)
	{
	SimulatePointerEvent(TRawEvent::EButton1Down, aPtrPos.iX, aPtrPos.iY, aPrValue, aPointerNumber);
	SimulatePointerEvent(TRawEvent::EPointerMove, aPtrPos.iX+2, aPtrPos.iY+1, aPrValue, aPointerNumber);
	SimulatePointerEvent(TRawEvent::EButton1Up, aPtrPos.iX+2, aPtrPos.iY+1, 0, aPointerNumber);
	AddExpectedPointerEvent(TPointerEvent::EButton1Down, aPtrPos, aPrValue, 0, aPointerNumber);
	aPtrPos.iX += 2; aPtrPos.iY += 1;
	AddExpectedPointerEvent(TPointerEvent::EDrag, aPtrPos, aPrValue, 0, aPointerNumber);
	AddExpectedPointerEvent(TPointerEvent::EButton1Up, aPtrPos, 0, 0, aPointerNumber);
	}

void CTMultiPtrEventTest::SimulateAndAddLoneUpEvents(TInt aNumSimulation, TInt aNumAddition, TPoint aPos)
	{
	for (TInt count = 0; count < aNumSimulation; count++)
		{
		SimulatePointerEvent(TRawEvent::EButton1Up, aPos.iX, aPos.iY, 0, 0);
		}
	for (TInt count = 0; count < aNumAddition; count++)
		{
		AddExpectedPointerEvent(TPointerEvent::EButton1Up, aPos, 0, 0, 0);
		}
	}

void CTMultiPtrEventTest::TestFilteringForMultiPointer(TBool aFiltering)
	{
	TPoint ptrPos;
	TInt ptrNum;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
	
	for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
		SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
		if(!aFiltering)
			{
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, ptrNum);
			}
		AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
		ptrPos.iX += xInc/2;
		ptrPos.iY += yInc/2;
		}
	
	ptrPos.SetXY(0,0);
	TPoint interPt = ptrPos;
	for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX+2, interPt.iY+1, 0, ptrNum);
		SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX+4, interPt.iY+2, 0, ptrNum);
		SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX+6, interPt.iY+3, 0, ptrNum);
		if(!aFiltering)
			{
			interPt.iX += 6; interPt.iY += 3;
			AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, ptrNum);
			}
		ptrPos.iX += xInc/2;
		ptrPos.iY += yInc/2;
		interPt = ptrPos; 
		}

	ptrPos.SetXY(0,0);
	interPt = ptrPos;
	for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		interPt.iX += 6;
		interPt.iY += 3;
		SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, ptrNum);
		ptrPos.iX += xInc/2;
		ptrPos.iY += yInc/2;
		interPt = ptrPos; 
		}
	}

void CTMultiPtrEventTest::TestGrabbingForMultiPointer(TBool aGrabbing)
	{
	TPoint ptrPos;
	TInt ptrNum;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
	
	for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
		ptrPos.iX += xInc/3;	// Ensure all Button1Down events are on iParentWin
		ptrPos.iY += yInc/3;
		}
	
	ptrPos.SetXY(0,0);
	ptrPos.iX += iPhysicalScreenSize.iWidth/2;		// Ensure all Drag and Button1Up events are on iChildWin
	ptrPos.iY += iPhysicalScreenSize.iHeight/2;
	for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
		if(aGrabbing)
			{
			// CalculatePtrPosAndSet3Ddata will subtract iChildWinPos from ptrPos if pointer event occured on child window. Thus we add iChildWinPos here.
			AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos+iMultiPtrEventClient->ChildWin()->Position(), ptrNum, (TUint)iMultiPtrEventClient->ParentWin());	
			}
		else
			{
			AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos, ptrNum);
			}
		ptrPos.iX += xInc/3;
		ptrPos.iY += yInc/3; 
		}

	ptrPos.SetXY(0,0);
	ptrPos.iX += iPhysicalScreenSize.iWidth/2;
	ptrPos.iY += iPhysicalScreenSize.iHeight/2;
	for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
		if(aGrabbing)
			{
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos+iMultiPtrEventClient->ChildWin()->Position(), ptrNum, (TUint)iMultiPtrEventClient->ParentWin());
			}
		else
			{
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
			}
		ptrPos.iX += xInc/3;
		ptrPos.iY += yInc/3;
		}
	}

void CTMultiPtrEventTest::TestCapturingForMultiPointer(TBool aAllWinGroups)
	{
	TPoint ptrPos;
	TInt ptrNum = 0;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
	
	// These are some typical values which are used only for testing purposes.
	// 20mm to screen
	// 50mm to screen
	// 4N of pressure
	// 2N of pressure
	SetThresholdValues(-20,-50,4000,2000);
	
	iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerMoveEvents, 0);
	
	if(!aAllWinGroups)
		{
		for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
			{
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, -30, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), -30, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, -10, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EEnterCloseProximity, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), -10, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), 0, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+=2, ptrPos.iY+=2, 5000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), 5000, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX-=2, ptrPos.iY-=2, 1000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EExitHighPressure, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), 1000, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+=2, ptrPos.iY+=2, 1000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), 1000, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), 0, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX-=2, ptrPos.iY-=2, -60, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos-iMultiPtrEventClient->ChildWin()->Position(), -60, 0, ptrNum, (TInt)iMultiPtrEventClient->ChildWin());
			ptrPos.iX += xInc/3;	// Ensure all Pointer events are on iParentWin
			ptrPos.iY += yInc/3;
			}		
		}
	else
		{
		ptrPos.iX = iPhysicalScreenSize.iWidth/2 + xInc/3;
		ptrPos.iY = 0;
		 		
		for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
			{
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, -10, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EEnterCloseProximity, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), -10, 0, ptrNum, (TInt)iWinForCapturing);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 0, 0, ptrNum, (TInt)iWinForCapturing);		
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+=2, ptrPos.iY+=2, 5000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 5000, 0, ptrNum, (TInt)iWinForCapturing);			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX-=2, ptrPos.iY-=2, 1000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EExitHighPressure, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 1000, 0, ptrNum, (TInt)iWinForCapturing);
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+=2, ptrPos.iY+=2, 1000, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EDrag, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 1000, 0, ptrNum, (TInt)iWinForCapturing);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 0, 0, ptrNum, (TInt)iWinForCapturing);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, TPoint(ptrPos.iX+iPhysicalScreenSize.iWidth/2, ptrPos.iY), 0, 0, ptrNum, (TInt)iWinForCapturing);
			ptrPos.iX += xInc/3;	
			ptrPos.iY += yInc/3;
			}
		}
	}

void CTMultiPtrEventTest::SetAutoFlush()
	{
	iTheClientFlush = TheClient->iWs.SetAutoFlush(ETrue);
	iClientFlush = iMultiPtrEventClient->iWs.SetAutoFlush(ETrue);
	}

void CTMultiPtrEventTest::ResetFlushStopAS()
	{
	if (iActiveSchedulerStarted)
		{
		iActiveSchedulerStarted = EFalse;
		CActiveScheduler::Stop();
		}
	TheClient->iWs.SetAutoFlush(iTheClientFlush);
	iMultiPtrEventClient->iWs.SetAutoFlush(iClientFlush);
	}

// Common function for all test cases
void CTMultiPtrEventTest::RunTestsL()
	{
	// Create CTMultiPtrEventClient for each test case and call NextSetOfEventsL()
	iMultiPtrEventClient = new(ELeave) CTMultiPtrEventClient(this);
	iMultiPtrEventClient->SetScreenNumber(iTest->iScreenNumber);
	iMultiPtrEventClient->ConstructL();
	
	iEventSet=0;

	// Create secondary client for test case number 3
	if (iTest->iState == 3)
		{
		iSecMultiPtrEventClient = new(ELeave) CTMultiPtrEventClient(this, 1);
		iSecMultiPtrEventClient->SetScreenNumber(iTest->iScreenNumber);
		iSecMultiPtrEventClient->ConstructL(); 
		}
	
	TRAPD(err, NextSetOfEventsL());
	if (err != KErrNone)
		{
		iFailed = ETrue;
		}

	// Start nested activescheduler if it is not failed for the first attempt
	if (!iFailed)
		{
		iActiveSchedulerStarted = ETrue;
		CActiveScheduler::Start();
		}

	//
	TInt eventsRem = iMultiPtrEventClient->EventBuffer()->EventsRemaining();
	if (eventsRem > 0)
		{
		iFailed=ETrue;
		}

	// Delete secondary client for test case number 3
	if (iTest->iState == 3)
		{
		delete iSecMultiPtrEventClient;
		iSecMultiPtrEventClient = NULL;
		}
	
	delete iMultiPtrEventClient;
	iMultiPtrEventClient = NULL;
	}

// Common function for all tests
// Important point to remember is that this function should not leave when running tests
void CTMultiPtrEventTest::NextSetOfEventsL()
	{
	// For each test case calls its respective tests
		
	((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (iTest->iState)
		{
	case 0:
/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0036
*/
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0036"));
		MultiPointerEvents();
		break;
	case 1:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0014"));
		PurgingMultiPointerEvents();
		break;
	case 2:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0013"));
		MultiPointerEventsForAnimsL();
		break;
	case 3:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0011"));
		GetRemainingEventsFromSecondaryClient();	// Does nothing for first time
		MultiClientMultiPointerEvents();
		break;
	case 4:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0027"));
		MultiPointerEventsForPointerClickL();
		break;
	case 5:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0028"));
		MultiPointerEventsInDiffScreenModes();
		break;
	case 6:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0025"));
		FilteringMultiPointerEvents();
		break;
	case 7:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0021"));
		GrabbingMultiPointerEvents();
		break;
	case 8:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0031"));
		VirtualKeyBoardForMultiplePointers();
		break;
	case 9:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0032"));
		DoubleClickForMultiplePointers();
		break;
	case 10:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0015"));
		MultiPointerOutOfRangeEvents();
		break;
	case 11:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0026"));
		AutoForeGroundForMultiplePointersL();
		break;
	case 12:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0029"));
		EmulationRulesForMultiplePointers();
		break;
	case 13:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0034"));
		CloseProximityAndHighPressureEventsL();
		break;
	case 14:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0017"));
		OutOfBoundValueTesting();
		break;	
	case 15:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0019"));
		CapturingMultiPointerEvents();
		break;
	case 16:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0035"));
		NegativeTestsForProximityPressureEventsL();
		break;
	case 17:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-ADVANCEDPOINTER-0034"));
		PointerRepeatEventForMultiplePointers();
		break;
	// Any new test case should be added here.
	// because some of the above tests use testcase number for certain checks
	default:
		((CTMultiPtrEventTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		AutoPanic(EAutoPanicWrongTest);
		}
	((CTMultiPtrEventTestStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0033
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677
@SYMTestPriority		High
@SYMTestCaseDesc		To check Wserv sends Pointer Repeat Events for all pointers. 
@SYMTestStatus			Implemented

@SYMTestActions			Simulate events being passed to the client by implementing each discrete test 
						over 4 seperate test cases. 
						In the first test case simulate EButton1Down and add event to a TWsEvent buffer. 
						In the second test case call RequestPointerRepeatEvent on the base window passing 
						in a time interval,	a rectangle (the 'hitbox') and pointer number. 
						Add EButtonRepeat to TWsEvent buffer.  
						In the third test case simulate EPointerMove event within the hitbox, then simulate 
						an elapsed period of time longer that specified interval. Add only a EButtonRepeat
						to TWsEvent buffer.
						In the final test case simulate EButton1Up and add event to TWsEvent buffer.
						
						Repeat steps for the second discrete test, however, in the third test case simulate 
						EPointerMove event outside the hitbox. In addition add EDrag to TWsEvent buffer 
						instead of EButtonRepeat. 
						
						A third discrete test will be executed over two test cases and will test
						CancelPointerRepeatEventRequest API.
						
@SYMTestExpectedResults The received events must match the events added to the buffer except where EPointerMove
 						events should be filtered out. In addition, EButtonRepeat will be generated by wserv
 						after calling RequestPointerRepeatEvent and time interval specified has elapsed.
						
*/
void CTMultiPtrEventTest::PointerRepeatEventForMultiplePointers()
	{
	// Test PointerRepeatEvent for Multiple Pointers 
	TLogMessageText logText;
	_LIT(KSet, "PointerRepeatEventForMultiplePointers SetOfEvents: %d of 10");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	TPoint ptrPos;
	TInt ptrNum;
	// Ensure all pointers are on iParentWin
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers/3;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers/3;
		
	switch(iEventSet++)
		{
		// Test cases 0 - 3 represent first discrete test for PointerRepeatEvents
		case 0:
			// Simulate EButton1Down events only. This is in order to simulate the event being passed to the client. 
			// If the event remained in the window queue the PointerRepeatEvent request would be cancelled when PointerRepeatPurgeCheck is performed.
			INFO_PRINTF1(_L("Simulate EButton1Down event for all pointers."));
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);			
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}	
			break;
		case 1:
			// Simulate client calling RequestPointerRepeatEvent in response to EButton1Down event being within a 'hitbox'.
			INFO_PRINTF1(_L("Simulate PointerRepeatEvent request for all pointers."));
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(iPhysicalScreenSize.iWidth/2,iPhysicalScreenSize.iHeight)),ptrNum);				
				User::After(200000);	
				AddExpectedPointerEvent(TPointerEvent::EButtonRepeat, ptrPos, ptrNum);			
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			break;
		case 2:
			// Simulate client calling RequestPointerRepeatEvent in response to EButtonRepeat and pointer remaining within the 'hitbox'.
			INFO_PRINTF1(_L("Simulate additional PointerRepeatEvent request for all pointers."));
			// New coordinates will be used for a simulated EPointerMove. This event will be filtered out, however, EButtonRepeat will occur at new coordinates
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(iPhysicalScreenSize.iWidth/2,iPhysicalScreenSize.iHeight)),ptrNum);				
				// EPointerMove event is filtered out by TWsPointer::PointerRepeatPurgeCheck
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				User::After(200000);
				AddExpectedPointerEvent(TPointerEvent::EButtonRepeat, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			break;		
		case 3:
			INFO_PRINTF1(_L("Simulate EButton1Up for all pointers."));
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			break; 			
		// Test cases 4 - 7 represent second second discrete test for PointerRepeatEvents	
		case 4:
			// Simulate EButton1Down events only. This is in order to simulate the event being passed to the client. 
			// If the event remained in the window queue the PointerRepeatEvent request would be cancelled when PointerRepeatPurgeCheck is performed.
			INFO_PRINTF1(_L("Simulate EButton1Down event for all pointers.")); 
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);			
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}	
			break;
		case 5:
			// Simulate client calling RequestPointerRepeatEvent in response to EButton1Down event being within a 'hitbox'.
			INFO_PRINTF1(_L("Call RequestPointerRepeatEvent for all pointers."));
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(iPhysicalScreenSize.iWidth/3,iPhysicalScreenSize.iHeight)),ptrNum); // hitbox size reduced 				
				User::After(200000);	
				AddExpectedPointerEvent(TPointerEvent::EButtonRepeat, ptrPos, ptrNum);			
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			break;
		case 6:
			// Simulate client calling RequestPointerRepeatEvent in response to EButtonRepeat and event remaining within the 'hitbox' rect.
			// EPointerMove will take pointer out of the hitbox so should not be filtered. PointerRepeatEvent should be cancelled.
			INFO_PRINTF1(_L("Simulate a PointerMove event for all pointers to a coordinate outside the hitbox."));
			// New coordinates will be used for a simulated EPointerMove outisde the hitbox. 
			ptrPos.iX = iPhysicalScreenSize.iWidth/3 + xInc;
			ptrPos.iY = 0;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(iPhysicalScreenSize.iWidth/3,iPhysicalScreenSize.iHeight)),ptrNum);				
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				User::After(200000);
				AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos, ptrNum);
				}
			break;		
		case 7:
			INFO_PRINTF1(_L("Simulate EButton1Up for all pointers."));
			ptrNum = 0;
			ptrPos.iX = iPhysicalScreenSize.iWidth/3 + xInc;
			ptrPos.iY = 0;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
				}
			break;
		// Test cases 8 and 9 represent third discrete test for PointerRepeatEvents
		case 8:
			// Simulate EButton1Down events only. This is in order to simulate the event being passed to the client. 
			// If the event remained in the window queue the PointerRepeatEvent request would be cancelled when PointerRepeatPurgeCheck is performed.
			INFO_PRINTF1(_L("Simulate EButton1Down event for all pointers.")); 
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);			
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}	
			break;	
		case 9:
			// No simulated events. This test case is to explicity call RWindowBase::CancelPointerRepeatEventRequest(const TUint8 aPointerNumber)
			INFO_PRINTF1(_L("Call RequestPointerRepeatEvent and CancelPointerRepeatEventRequest for all pointers."));
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(iPhysicalScreenSize.iWidth/3,iPhysicalScreenSize.iHeight)),ptrNum); 				
				iMultiPtrEventClient->ParentWin()->BaseWin()->CancelPointerRepeatEventRequest(ptrNum); 
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			INFO_PRINTF1(_L("Simulate EButton1Up for all pointers."));
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
				ptrPos.iX+=xInc;
				ptrPos.iY+=yInc;
				}
			break;	
		default:
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

void CTMultiPtrEventTest::TestErrorCodeL(TInt aExpErrCode, TInt aActualErrCode)
	{
	if (aExpErrCode != aActualErrCode)
		{
		TLogMessageText logText;
		_LIT(KError, "Expected Error code = %d Actual Actual Error code %d");
		logText.AppendFormat(KError, aExpErrCode, aActualErrCode);
		LOG_MESSAGE(logText);
		User::Leave(ETestFailed);
		}
	}

void CTMultiPtrEventTest::NegativeTestingOfProximityPressureAPIsL()
	{
	// First send -10 for EnterCloseProximity and -5 for ExitCLoseProximity, it should return KErrArgument
	// Then send 20 and 10 which should return KErrNone
	// Then sent 10 and 20 which should return KErrArgument
	TInt retVal;
	retVal = iMultiPtrEventClient->iWs.SetCloseProximityThresholds(-10, -5);
	TestErrorCodeL(KErrArgument, retVal);
	
	retVal = iMultiPtrEventClient->iWs.SetCloseProximityThresholds(20, 10);
	TestErrorCodeL(KErrNone, retVal);
	
	retVal = iMultiPtrEventClient->iWs.SetCloseProximityThresholds(10, 20);
	TestErrorCodeL(KErrArgument, retVal);
	
	// Repeat the above for EnterHighPressure and ExitHighPressure with these values
	// -10 and -5 it should return KErrArgument
	// -5 and -10 it should return KErrNone
	retVal = iMultiPtrEventClient->iWs.SetHighPressureThresholds(-10, -5);
	TestErrorCodeL(KErrArgument, retVal);
	
	retVal = iMultiPtrEventClient->iWs.SetHighPressureThresholds(-5, -10);
	TestErrorCodeL(KErrNone, retVal);
	}

void CTMultiPtrEventTest::SimulateProximityAndPressureEvents(TInt aCloseProxEnterValue, TInt aCloseProxExitValue, TInt aHighPresEnterValue, TInt aHighPresExitValue)
	{
	TPoint ptrPos(10,10);
	// Simulate EPointerMove with proximity value > EnterCloseProximity threshold 
	for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, aCloseProxEnterValue, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EEnterCloseProximity, ptrPos, aCloseProxEnterValue, 0, ptrNum);
		}
	
	// Simulate EPointerMove with proximity value < ExitCloseProximity threshold
	for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, aCloseProxExitValue, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, aCloseProxExitValue, 0, ptrNum);
		}
	
	// Simulate EBUtton1Down with presure value > EnterHighPressure threshold
	for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, aHighPresEnterValue, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, aHighPresEnterValue, 0, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos, aHighPresEnterValue, 0, ptrNum);
		}
	
	// Simulate EPointerMove with presure value < ExitHighPressure threshold
	for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, aHighPresExitValue, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EExitHighPressure, ptrPos, aHighPresExitValue, 0, ptrNum);
		}
	
	// Finally simulate Ebutton1Up with proximity value < ExitCloseProximity
	for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
		{
		SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, aCloseProxExitValue, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, aCloseProxExitValue, 0, ptrNum);
		AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, aCloseProxExitValue, 0, ptrNum);
		}
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0035
@SYMPREQ				PREQ1226
@SYMREQ					REQ10887, REQ10886
@SYMTestPriority		High
@SYMTestCaseDesc		Negative tests for close Proximity and high Pressure events. 
@SYMTestStatus			Implemented

@SYMTestActions			Set CloseProximity and HighPressure thresholds with positive and negative values.
  						Simulate Enter/ExitCloseProximity and Enter/ExitHighPressure events.
@SYMTestExpectedResults The received events must match the simulated pointer events
*/
void CTMultiPtrEventTest::NegativeTestsForProximityPressureEventsL()
	{
	TLogMessageText logText;
	_LIT(KSet, "Negative tests for pressure and proximity events: %d of 3");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		NegativeTestingOfProximityPressureAPIsL();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	switch(iEventSet++)
		{
		case 0:
			// This is to check close proximity threshold API can be given +ve values
			// and high pressure threshold API can begiven -ve values
			iMultiPtrEventClient->iWs.SetCloseProximityThresholds(50, 20);
			iMultiPtrEventClient->iWs.SetHighPressureThresholds(-20, -50);
			SimulateProximityAndPressureEvents(60, 10, -10, -60);
			break;
		case 1:
			// Repeat the above with +ve values
			iMultiPtrEventClient->iWs.SetCloseProximityThresholds(50, 20);
			iMultiPtrEventClient->iWs.SetHighPressureThresholds(50, 20);
			SimulateProximityAndPressureEvents(60, 10, 60, 10);
			break;
		case 2:
			// Repeat the above -ve values
			iMultiPtrEventClient->iWs.SetCloseProximityThresholds(-20, -50);
			iMultiPtrEventClient->iWs.SetHighPressureThresholds(-20, -50);
			SimulateProximityAndPressureEvents(-10, -60, -10, -60);
			break;
		default:
			SetThresholdValues(KMaxTInt, KMinTInt, KMaxTInt, KMinTInt);
			ResetFlushStopAS();
			break;
		}

	LogEventsAdded();
	}


void CTMultiPtrEventTest::CreateGroupWinForCapturingL()
	{
	iGroupWinForCapturing = new(ELeave) CTWindowGroup(iMultiPtrEventClient);
	iGroupWinForCapturing->ConstructL();
	iGroupWinForCapturing->GroupWin()->AutoForeground(ETrue);
	iGroupWinForCapturing->GroupWin()->EnableFocusChangeEvents();
				
	iWinForCapturing = new(ELeave) CTBlankWindow();
	iWinForCapturing->ConstructL(*iGroupWinForCapturing);
	iWinForCapturing->SetExt(TPoint(), TSize(iPhysicalScreenSize.iWidth/2, iPhysicalScreenSize.iHeight));
	iWinForCapturing->SetColor(KRgbBlue);
	iWinForCapturing->BaseWin()->EnableAdvancedPointers();
	iWinForCapturing->Activate();
	}

void CTMultiPtrEventTest::DeleteGroupWinForCapturing()
	{
	if (iWinForCapturing)
		{
		delete iWinForCapturing;
		iWinForCapturing = NULL;
		}
	if (iGroupWinForCapturing)
		{
		delete iGroupWinForCapturing;
		iGroupWinForCapturing = NULL;
		}
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0019
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9683,
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Capturing in case of multi pointer events.
@SYMTestStatus			Implemented

@SYMTestActions			Create a window with group window as its parent. 
						Create another window with same parent and partially overlapping the first one. 
						SetPointerCapture for second window with TCaptureEnabled flag. 
						Simulate TRawEvent::EButton1Down from each pointer the device supports
						
						Repeat the above with TCaptureFlagAllGroups flag for SetPointerCapture 
						and creating windows from different window group
   						
@SYMTestExpectedResults Even if events were simulated on first window events are delivered to 
						second window as it is capturing. 
						With TCaptureFlagAllGroups flag set	events are delivered to window which is
						capturing even if events were simulated on a window from different group
						
*/
void CTMultiPtrEventTest::CapturingMultiPointerEvents()
	{
	// Test Capturing of multi pointer events
	// 1. Test Wserv supports Capturing in case of multi-pointer events
	// 2. Test pointer events are delivered correctly when Capturing is disabled.
	TLogMessageText logText;
	_LIT(KSet, "CapturingMultiPtrEvents SetOfEvents: %d of 3");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos;
	TInt ptrNum;
		
	switch(iEventSet++)
		{
		case 0:
			INFO_PRINTF1(_L("Ensure pointer state is Out of Range"));
			AddExpectedWsEvent(EEventFocusGained);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, ptrNum);
				}
			break;			
		case 1:
			INFO_PRINTF1(_L("Check Wserv supports Capturing in case of multi-pointer events, within window group"));
			iMultiPtrEventClient->ChildWin()->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			TestCapturingForMultiPointer(EFalse);
			break;	
		case 2:
			INFO_PRINTF1(_L("Check Wserv supports Capturing in case of multi-pointer events, from different window groups"));
			TRAPD(err, CreateGroupWinForCapturingL());	// it cannot leave so trap and fail
			if (err != KErrNone)
				{
				Failed();
				}			
			AddExpectedWsEvent(EEventFocusLost);		// First group window losing focus because of second group creation
			AddExpectedWsEvent(EEventFocusGained);		// Second group window creation
			
			iWinForCapturing->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			TestCapturingForMultiPointer(ETrue);
			break;
		default:
			DeleteGroupWinForCapturing();
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0017
@SYMPREQ				PREQ1226
@SYMREQ					Negative Testing
@SYMTestPriority		High
@SYMTestCaseDesc		Out of bound value testing
@SYMTestStatus			Implemented

@SYMTestActions			Simulate raw events of TRawEvent with pointer number greater than EPointerMaxPointers
						 						
@SYMTestExpectedResults The received events must match the events added to the buffer and check that out of bound events are ignored.
 
*/
void CTMultiPtrEventTest::OutOfBoundValueTesting()
	{
	// Test Wserv ignores pointer events which have a pointer number greater than EPointerMaxPointers. 	 
	TLogMessageText logText;
	_LIT(KSet, "OutOfBoundValueTesting SetOfEvents: %d of 1");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos;
	TInt ptrNum;
	TInt ptrNumOutOfBounds;
			
	switch(iEventSet++)
		{
		case 0:
			INFO_PRINTF1(_L("Checks Wserv ignores pointer events which have a pointer number greater than EPointerMaxPointers."));
			ptrPos.SetXY(0,0);
			ptrNum = iMaxDevPointers-1;				// Max value for ptrNum
			ptrNumOutOfBounds = iMaxDevPointers;	// Invalid ptrNum as pointer numbers are zero indexed
			
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNumOutOfBounds);
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+10, ptrPos.iY+10, 0, ptrNum);
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX+10, ptrPos.iY+10, 0, ptrNumOutOfBounds);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX+10, ptrPos.iY+10, 0, ptrNum);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX+10, ptrPos.iY+10, 0, ptrNumOutOfBounds);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, ptrNum);
			ptrPos.iX += 10; ptrPos.iY += 10;
			AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, ptrNum);
			break;	 
		default:
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

void CTMultiPtrEventTest::SetThresholdValues(TInt aEnterCloseProx, TInt aExitCloseProx, TInt aEnterHighPres, TInt aExitHighPres)
	{
	iEnterCloseProximityThreshold = aEnterCloseProx;
	iExitCloseProximityThreshold = aExitCloseProx;
	iEnterHighPressureThreshold = aEnterHighPres;
	iExitHighPressureThreshold  = aExitHighPres;
	
	iMultiPtrEventClient->iWs.SetCloseProximityThresholds(iEnterCloseProximityThreshold,iExitCloseProximityThreshold);
	iMultiPtrEventClient->iWs.SetHighPressureThresholds(iEnterHighPressureThreshold,iExitHighPressureThreshold);
	}

void CTMultiPtrEventTest::TestCloseProximityHighPressureAPIsL()
	{
	// These are some typical values which are used only for testing purposes.
	// 20mm to screen
	// 50mm to screen
	// 4N of pressure
	// 2N of pressure
	SetThresholdValues(-20,-50,4000,2000);
	
	if (iMultiPtrEventClient->iWs.GetEnterCloseProximityThreshold() != iEnterCloseProximityThreshold)
		{
		TLogMessageText logText;
		_LIT(KError, "Expected EnterCloseProximityThreshold = %d Actual EnterCloseProximityThreshold %d");
		logText.AppendFormat(KError, iEnterCloseProximityThreshold, iMultiPtrEventClient->iWs.GetEnterCloseProximityThreshold());
		LOG_MESSAGE(logText);
		User::Leave(ETestFailed);
		}
	
	if (iMultiPtrEventClient->iWs.GetExitCloseProximityThreshold() != iExitCloseProximityThreshold)
		{
		TLogMessageText logText;
		_LIT(KError, "Expected ExitCloseProximityThreshold = %d Actual ExitCloseProximityThreshold %d");
		logText.AppendFormat(KError, iExitCloseProximityThreshold, iMultiPtrEventClient->iWs.GetExitCloseProximityThreshold());
		LOG_MESSAGE(logText);		
		User::Leave(ETestFailed);
		}
	
	if (iMultiPtrEventClient->iWs.GetEnterHighPressureThreshold() != iEnterHighPressureThreshold)
		{
		TLogMessageText logText;
		_LIT(KError, "Expected EnterHighPressureThreshold = %d Actual EnterHighPressureThreshold %d");
		logText.AppendFormat(KError, iEnterHighPressureThreshold, iMultiPtrEventClient->iWs.GetEnterHighPressureThreshold());
		LOG_MESSAGE(logText);
		User::Leave(ETestFailed);
		}
	
	if (iMultiPtrEventClient->iWs.GetExitHighPressureThreshold() != iExitHighPressureThreshold)
		{
		TLogMessageText logText;
		_LIT(KError, "Expected ExitHighPressureThreshold = %d Actual ExitHighPressureThreshold %d");
		logText.AppendFormat(KError, iExitHighPressureThreshold, iMultiPtrEventClient->iWs.GetExitHighPressureThreshold());
		LOG_MESSAGE(logText);
		User::Leave(ETestFailed);
		}
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0034
@SYMPREQ				PREQ1226
@SYMREQ					REQ10887,REQ10886
@SYMTestPriority		High
@SYMTestCaseDesc		Tests close Proximity and high Pressure event types. 
@SYMTestStatus			Implemented

@SYMTestActions			Set CloseProximity and HighPressure thresholds and simulate events with different Z values  
@SYMTestExpectedResults The received events must match the simulated pointer events
*/
void CTMultiPtrEventTest::CloseProximityAndHighPressureEventsL()
	{
	TLogMessageText logText;
	_LIT(KSet, "Close Proximity and High Pressure events: %d of 11");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		TestCloseProximityHighPressureAPIsL();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos(10,10);
	TInt proxValue;
	TInt prValue;
	TInt tempCount; 
	switch(iEventSet++)
		{
		case 0:
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			// One whole cycle of events
			// Simulate out-of-range from all pointers and don't expect out-of-range events
			// since all the pointers are already in same state
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}
			
			// Simulate moves with proximity value < EEnterCloseProximity
			proxValue = -30;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				}

			// Simulate moves with proximity value > EEnterCloseProximity
			proxValue = -10;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate EButton1Down with pressure value < EEnterHighPressure
			prValue = 1000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate moves with pressure value > EEnterHighPressure
			prValue = 5000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate moves with pressure value < EExitHighPressure
			prValue = 1000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EExitHighPressure, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value > EEnterCloseProximity
			proxValue = -10;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate moves with proximity value < EExitCloseProximity
			proxValue = -60;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 1:
			// Simulate events to meet all scenarios 
			// Simulate EButton1Down with pressure value < EEnterHighPressure
			prValue = 1000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value < EExitCloseProximity
			proxValue = -60;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate EButton1Down with pressure value > EEnterHighPressure
			prValue = 5000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value < EExitCloseProximity
			proxValue = -60;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 2:
			// Simulate moves with proximity value > EEnterCloseProximity
			proxValue = -10;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate EButton1Down with pressure value > EEnterHighPressure
			prValue = 5000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value > EExitCloseProximity
			proxValue = -40;		// When you coming up, EEnterCloseProximity is of no use
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate out-of-range events
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 3:
			// From out-of-range state to EButton1Down
			// Simulate EButton1Down with pressure value < EEnterHighPressure
			prValue = 1000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value > EExitCloseProximity
			proxValue = -40;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate out-of-range events
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate EButton1Down with pressure value > EEnterHighPressure
			prValue = 5000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EEnterHighPressure, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value > EExitCloseProximity
			proxValue = -40;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate out-of-range events
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 4:
			// Simulate EButton1Dowm with pressure value < EEnterHighPressure
			prValue = 1000;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, ptrNum);
				}
			
			// Simulate EButton1Up with proximity value < EExitCloseProximity
			proxValue = -60;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EExitCloseProximity, ptrPos, proxValue, 0, ptrNum);
				}
			
			// Simulate out-of-range from Up/OutOfCloseProximity
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 5:
			// Purging of proximity and pressure events
			// Simulate EnterCloseProximity and ExitCloseProximity events which will be purged
			// when you send Button1Up events for KMaxQueueSize
			proxValue = -10;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 4); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				}
			proxValue = -60;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 4); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				}
			for (TInt count = 0; count < KMaxQueueSize; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, 0);
				}
			break;
		case 6:
			// Simulate EnterCloseProximity for 4 pointers. 
			// As we cannot create lone ExitCloseProximity events, so create just moves from next 4 pointers
			// Simulate out-of-range for these events on thier respective pointers
			// Then simulate EButton1Up events which will purge EnterCloseProximity and move events
			// which checks that lone EnterCloseProximity and lone move events will be purged
			proxValue = -10;
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			proxValue = -60;
			for (TInt ptrNum = iMaxDevPointers/2; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			for (TInt count = 0; count < KMaxQueueSize-iMaxDevPointers; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, 0);
				}
			break;
		case 7:
			// Bring all other pointers to Up state, becasue of previous test
			// Then simulate out-of-range twice for all the pointers
			// Then simulate Button1Up which will cause purging of successive out-of-range events
			proxValue = -60;
			tempCount = 0;
			for (TInt ptrNum = 1; ptrNum < iMaxDevPointers; ptrNum++)
				{
				tempCount++;
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, ptrNum);
				}
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				tempCount++;
				// Wserv does nothing if current state is outofrange and if receives again EOutOfRange event which is practically not possible
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			for (TInt count = 0; count < KMaxQueueSize-tempCount; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, proxValue, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, proxValue, 0, 0);
				}
			break;
		case 8:
			// Simulate EButton1Down with pressure value > EnterHIghPressure threshold
			// which will cause EButton1Down and EnterHighPressure to be filled in queue
			// Simulate ExitHighPressure and EButton1Up events for 2 pointers
			// Then simulate EButton1Up events from pointer0 so that all of the above events are purged
			prValue = 5000;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 2); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				}
			prValue = 1000;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 2); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				// Here these events are purged becasue their correpsonding  down event would have been purged
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);	
				}
			// All the above events get purged by this
			for (TInt count = 0; count < KMaxQueueSize; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
				}
			break;
		case 9:
			// Simulate EButton1Down for 2 pointers, which will generate Button1Down and EnterHighPressure
			// Simulate Ebutton1Up events for MaxQueueSize from last pointer so that all the above events are purged  
			prValue = 5000;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrNum < 2); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				}
			
			// This will cause above down and enterhighpressure events to be purged
			// When pointer is in outofrange state and when EButton1Up event is sent Wserv sends EnterCloseProximity 
			// EnterCloseProximity cannot be purged unless the next event is EExitCloseProximity or EOutOfRange
			for (TInt count = 0; count < KMaxQueueSize; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, iMaxDevPointers-1);
				AddExpectedPointerEvent((count ? TPointerEvent::EButton1Up : TPointerEvent::EEnterCloseProximity), ptrPos, 0, 0, iMaxDevPointers-1);
				}

			// As the first 2 pointers are in down state, and if we send the move events with prValue = 1000
			// This will generate the exithigh pressure events, which will be ignored
			// This will also generate the Ebutton1Up event which will also be ignored
			prValue = 1000;
			for (TInt ptrNum = 0; (ptrNum < iMaxDevPointers/2  && ptrNum < 2); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				}
			break;
		case 10:
			// This is to just bring all the pointers to out-of-range state
			// to make all pointers in valid state for other test to run
			// Wserv processes EOutOfRange events only when its current state is not in outofrange. 
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers && ptrNum < 2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, ptrNum);
				}
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, iMaxDevPointers-1);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, iMaxDevPointers-1);
			break;
		default:
			SetThresholdValues(KMaxTInt, KMinTInt, KMaxTInt, KMinTInt);
			ResetFlushStopAS();
			break;
		}

	LogEventsAdded();
	}

void CTMultiPtrEventTest::CreateAnimDllAndAnimL()
	{
	iAnimDll = new(ELeave) RAnimDll(iMultiPtrEventClient->iWs);
	User::LeaveIfError(iAnimDll->Load(KAnimDLLName));
	iAnim = RMultiPointerAnim::NewL(iMultiPtrEventClient->ParentWin()->BaseWin(), iAnimDll);
	}

void CTMultiPtrEventTest::DestroyAnimDllAndAnim()
	{
	if (iAnim)
		{
		iAnim->Destroy();
		iAnim = NULL;
		}
	if (iAnimDll)
		{
		iAnimDll->Destroy();
		iAnimDll = NULL;
		}
	}

void CTMultiPtrEventTest::CreateGroupWinL()
	{
	iGroupWinForEmul = new(ELeave) CTWindowGroup(iMultiPtrEventClient);
	iGroupWinForEmul->ConstructL();
	iGroupWinForEmul->GroupWin()->AutoForeground(ETrue);
	iGroupWinForEmul->GroupWin()->EnableFocusChangeEvents();

	iWinForEmul = new(ELeave) CTBlankWindow();
	iWinForEmul->ConstructL(*iGroupWinForEmul);
	iWinForEmul->SetExt(TPoint(50,50), TSize(80,80));
	iWinForEmul->SetColor(KRgbDarkYellow);
	iWinForEmul->Activate();	
	}

void CTMultiPtrEventTest::DeleteGroupWin()
	{
	if (iWinForEmul)
		{
		delete iWinForEmul;
		iWinForEmul = NULL;
		}
	if (iGroupWinForEmul)
		{
		delete iGroupWinForEmul;
		iGroupWinForEmul = NULL;
		}
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0029
@SYMPREQ				PREQ1226
@SYMREQ					REQ9688
@SYMTestPriority		High
@SYMTestCaseDesc		Tests the emulation rules for Apps, Anims, Autofocus and Buffered delivery
@SYMTestStatus			Implemented

@SYMTestActions			Simulate event to make a pointer primary. EnableAdvancedPointers API is not called. And then simulate events from other pointers  
@SYMTestExpectedResults The events from other pointer are not received
*/
void CTMultiPtrEventTest::EmulationRulesForMultiplePointers()
	{
	TLogMessageText logText;
	_LIT(KSet, "Emulation rules set of events: %d of 15");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetExpectNonAdvancedPointerEvents();
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	TPoint ptrPos(10,10);
	TInt proxValue = -iProximityStep;
	TInt prValue = iPressureStep;
	TInt ptrNum = 0;
	switch(iEventSet++)
		{
		case 0:
			// This event is for simulated/primary pointer
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Parent window has not called EnableAdvancedPointers API
			// So even if we simulate moves and out-of-range these are not delivered to client
			for (ptrNum = 1; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}
			break;
		case 1:
			// Since the primary pointer is 0, simulate EButton1Down for making pointer 1 as primary 
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate move or Ebutton1Down from pointer0 they are NOT sent back to client
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 2:
			// Simulate EButton1Down from pointer0 to make it primary
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, 0);
			
			// Move from pointer1
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			
			// Take the zero pointer out-of-range, this will not result in Pointer 1 becoming primary
			// and out-of-range event is sent to client
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, 0);
			
			// Send moves from pointer1, but these are not sent to the Client.
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 3:
			// Simulate EButton1Down from pointer0 to make it primary
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate EButton1Down from pointer1, which is not sent to client 
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Drag events are not sent becasue pointer1 has NOT become primary
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 1);
			
			// Simulate up from pointer1, still not primary, so event is not sent
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			
			// Simulate out-of-range from pointer1, still not primary, so event is not sent
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			// Now moves from out-of-range to move state 
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 4:
			// pointer0 sends move (becomes primary)
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// pointer0 sends down
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// pointer1 sends down, doesn't become primary
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);

			// pointer0 sends up, stays primary
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);

			// pointer2, pointer3 and pointer4 sends move with proximity -5, -10, -15
			for (ptrNum = 2; ptrNum < iMaxDevPointers; ptrNum++, proxValue -= iProximityStep)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				}
			
			// pointer0 sends out-of-range, loses primary pointer, But NO events are sent
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Send moves for pointer2, pointer3 and pointer4.
			// None of the evnst are sent
			ptrPos.SetXY(0,0);
			for (ptrNum = 2; ptrNum < iMaxDevPointers; ptrNum++)
				{
				ptrPos.iX += 10;
				ptrPos.iY += 5;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				}
			
			// Drag from pointer1, NOT sent, since it is not primary
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, 1);

			// Send out-of-range from all pointers
			for (ptrNum = 2; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}
			
			// As the pointer1 is down, this will not become primary even if EButton1Up is called
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			
			// And now sending out-of-range will also not send the event to client as it is not primary
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			break;
		case 5:
			// Test cases 5 - 7 represent one discrete test for PointerRepeatEvents
			// Case 5 simulates EButton1Down events only. This is to ensure the event is delivered to the client. 
			// If the event remained in the window queue the PointerRepeatEvent request would be cancelled when PointerRepeatPurgeCheck is performed.
			ptrPos.SetXY(0,0);
			
			// pointer0 sends down (become primary)
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// pointer1 sends down, doesn't become primary
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			
			// pointer4 sends down
			if (4 < iMaxDevPointers)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 4);
				}
			break;
		case 6:
			ptrPos.SetXY(0,0);
			// Simulate client calling RequestPointerRepeatEvent in response to EButton1Down event being within a 'hitbox'.
			// Legacy API called which does not specify a pointer number. EButtonRepeat should be generated for primary pointer only.
			iMultiPtrEventClient->ParentWin()->BaseWin()->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect(TPoint(0,0), TSize(20,20)));				
			User::After(200000);	
			AddExpectedPointerEvent(TPointerEvent::EButtonRepeat, ptrPos, TAdvancedPointerEvent::EDefaultPointerNumber);			
			break;
		case 7:
			// pointer0 sends up 
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// pointer2 sends out-of-range
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Send up and out-of-range for pointer1 and 4
			SimulatePointerEvent(TRawEvent::EButton1Up, 0, 0, 0, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			if (4 < iMaxDevPointers)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, 0, 0, 0, 4);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 4);
				}
			break;
		case 8:
			// Autofocus does not work for other than primary pointers
			// Create group window which will send focus lost and gained
			TRAPD(err, CreateGroupWinL());	// it cannot leave so trap and fail
			if (err != KErrNone)
				{
				Failed();
				}
			AddExpectedWsEvent(EEventFocusLost);
			AddExpectedWsEvent(EEventFocusGained);
			
			// Simulate Epointer down which will send focuslost, focusgained and focusgroupchanged events 
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedWsEvent(EEventFocusLost); 
			AddExpectedWsEvent(EEventFocusGained);
			AddExpectedWsEvent(EEventFocusGroupChanged);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Now simulate EButton1Down from first or second pointer on second group
			// This should not reflect the autofocus
			ptrPos.iX = 60;
			ptrPos.iY = 60;
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			if (2 < iMaxDevPointers)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 2);
				}
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			if (2 < iMaxDevPointers)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 2);
				}
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 9:
			// First thing to do in this case is to delete the group window created
			DeleteGroupWin();
			
			TRAP(err, CreateAnimDllAndAnimL());	// it cannot leave so trap and fail
			if (err != KErrNone)
				{
				Failed();
				}

			// This is the first event which makes first pointer as primary
			// When the last argument is EFalse, it means that the event is not consumed by Anim
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointerMove, ptrPos, 0, 0, 0, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			
			// Now simulate moves from other pointers and these events are not sent to anim
			for (ptrNum = 1; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}

			// This is to reset the primary pointer 
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointer3DOutOfRange, ptrPos, 0, 0, 0, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
	
			// Now simulate EButton1Down, which will make it as primary
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Down, ptrPos, 0, 0, 1, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			
			// Simulate move or Ebutton1Down from any other prointer they are not sent back to client
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Up, ptrPos, 0, 0, 1, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointer3DOutOfRange, ptrPos, 0, 0, 1, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);

			// Set the primary pointer to Zero			
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Down, ptrPos, 0, 0, 0, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			
			// Move from second pointer
			// As EnableMultiPointer is not called and this is not a primary pointer so this event
			// will cause the pointer 1 into move state
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			
			// Take the zero pointer out-of-range 
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Up, ptrPos, 0, 0, 0, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			
			iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointer3DOutOfRange, ptrPos, 0, 0, 0, EFalse);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			
			// These moves will not be sent
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);		

			TestAndDisplayAnimError();
			
			_LIT(KEventsChecked, "Events checked at server side = %d");
			logText.Format(KEventsChecked, 10);
			LOG_MESSAGE(logText);
			break;
		case 10:
			DestroyAnimDllAndAnim();
			
			// Test for PointerMove buffers
			// AllocPointerMoveBuffer, EnablePointerMoveBuffer and then simulate few moves
			// Add these moves to another points buffer for testing
			// Simulate moves from other pointers, these should not be delivered to clients.
			iMultiPtrEventClient->ParentWin()->BaseWin()->AllocPointerMoveBuffer(10, 0);
			iMultiPtrEventClient->ParentWin()->BaseWin()->EnablePointerMoveBuffer();
			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedWsEvent(EEventPointerBufferReady, EFalse, TAdvancedPointerEvent::EDefaultPointerNumber, (TUint)iMultiPtrEventClient->ParentWin());
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);   // If this position is above some other window then we need to consider its window postion also
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);
			
			// These events should not be added to buffer
			for (ptrNum = 1; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += 10;
				ptrPos.iY += 10;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}
			break;
		case 11:
			// Repeat the above for drag events
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedWsEvent(EEventPointerBufferReady, EFalse, TAdvancedPointerEvent::EDefaultPointerNumber, (TUint)iMultiPtrEventClient->ParentWin());
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);
			ptrPos.iX += 10;
			ptrPos.iY += 10;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, 0);
			iMultiPtrEventClient->AddExpectedMovePoint(ptrPos);
			
			// These events should not be added to buffer
			for (ptrNum = 1; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += 10;
				ptrPos.iY += 10;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				}
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 12:
			iMultiPtrEventClient->ParentWin()->BaseWin()->FreePointerMoveBuffer();
			iMultiPtrEventClient->ParentWin()->BaseWin()->DisablePointerMoveBuffer();
			
			// Pointer Cursor Position test 
			// This event is for simulating primary pointer
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			// Here I am not adding the event because this event is deleted by another move of same pointer
			if (ptrPos != iMultiPtrEventClient->iWs.PointerCursorPosition())
				{
				TEST(EFalse);
				Failed();
				}
			
			iMultiPtrEventClient->iWs.SetPointerCursorPosition(TPoint(20,20));
			if (TPoint(20,20) != iMultiPtrEventClient->iWs.PointerCursorPosition())
				{
				TEST(EFalse);
				Failed();
				}
			
			// Now simulate moves from other pointer, this should not have any effect on pointer cursor position
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);

			// Now move the primary pointer, and check the pointer cursor position which should be same as position used for move
			ptrPos.iX = 30;
			ptrPos.iY = 30;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			if (ptrPos != iMultiPtrEventClient->iWs.PointerCursorPosition())
				{
				TEST(EFalse);
				Failed();
				}
			
			// Send out-of-range event which will not lead to pointer 1 becoming primary
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Now check the pointer cursor position, which should be same
			// as primary even if there is other pointer in up state, but that is not primary
			if (ptrPos != iMultiPtrEventClient->iWs.PointerCursorPosition())
				{
				TEST(EFalse);
				Failed();
				}
			
			// This is to bring the pointer1 to out-of-range state
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			// then send move event and then again check the pointer cursor position
			ptrPos.iX = 20;
			ptrPos.iY = 20;
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			if (ptrPos != iMultiPtrEventClient->iWs.PointerCursorPosition())
				{
				TEST(EFalse);
				Failed();
				}
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			break;
		case 13:
			{
			// Pointer Cursor Area test-1
			iOrigPtrCursorArea = iMultiPtrEventClient->iWs.PointerCursorArea();
			TRect ptrCurArea(0,0,40,40);
			iMultiPtrEventClient->iWs.SetPointerCursorArea(ptrCurArea);
			
			// Simulate move from pointer0 in PointerCursorArea
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate move from other pointer, This should have no effect
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);

			// Repeat the above with moves outside pointer cursor area 
			ptrPos.iX = 50;
			ptrPos.iY = 50;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 0);
			TPoint pointInCurArea(39, 39);
			AddExpectedPointerEvent(TPointerEvent::EMove, pointInCurArea, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate move from other pointer, This should have no effect			
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
						
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, pointInCurArea, proxValue, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			}
			break;
		case 14:
			{
			// Pointer Cursor Area test-2
			// repeat the above test with button1 down
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate move from other pointer, This should have no effect
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Repeat the above with moves outside pointer cursor area 
			ptrPos.iX = 50;
			ptrPos.iY = 50;
			TPoint pointInCurArea(39, 39);
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, pointInCurArea, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			// Simulate move from other pointer, This should have no effect
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, 1);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 1);
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 1);
			
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, pointInCurArea, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			
			SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EOutOfRange, pointInCurArea, 0, 0, TAdvancedPointerEvent::EDefaultPointerNumber);
			}
			break;
		default:
			ClearExpectNonAdvancedPointerEvents();
			iMultiPtrEventClient->iWs.SetPointerCursorArea(iOrigPtrCursorArea);
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0026
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677, REQ9683
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Autofocus for multiple pointers
@SYMTestStatus			Implemented

@SYMTestActions			Create 2 group window and respective client windows for it. And simulate EButton1Down on each window consecutively 
@SYMTestExpectedResults The received events must match the simulated pointer events
*/
void CTMultiPtrEventTest::AutoForeGroundForMultiplePointersL()
	{
	TLogMessageText logText;
	_LIT(KSet, "Multiple pointer Auto foreground events: %d of 2");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);			// This if for the main groupwindow
		iMultiPtrEventClient->ConstructGroupBlankWinL();
		
		// These are for child group window creation
		AddExpectedWsEvent(EEventFocusLost);			// This if for the main groupwindow losing focus
		AddExpectedWsEvent(EEventFocusGained);			// First group window of this test creation
		
		AddExpectedWsEvent(EEventFocusLost);			// First group window losing focus because of second group creation
		AddExpectedWsEvent(EEventFocusGained);			// Second group window creation
		AddExpectedWsEvent(EEventFocusGroupChanged);	// First group window's Enable Focus changed flag is set
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	TPoint win1PtrPos(20,20);
	TPoint win2PtrPos(60,60);
	TPoint ptrPos;
	TInt autoFocusWin;
	switch(iEventSet++)
		{
		case 0:
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				if ((ptrNum & 1) == 0)
					{
					ptrPos = win1PtrPos;
					autoFocusWin = 1;
					}
				else
					{
					ptrPos = win2PtrPos;
					autoFocusWin = 0;
					}
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedWsEvent(EEventFocusLost);				// Second group window losing focus 
				AddExpectedWsEvent(EEventFocusGained);				// First group window gaining focus
				if (ptrNum == 0)
					{												// First group window has already received EEventFocusGroupChanged
					AddExpectedWsEvent(EEventFocusGroupChanged);	// Second group window's Enable Focus changed flag is set
					}
				// In the queue only once EEventFocusGroupChanged is sent
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				
				// These events are to clear out the pointer state
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, ptrNum);
				
				if (!iMultiPtrEventClient->CheckOrdinalPosition(autoFocusWin))
					{
					TEST(EFalse);
					Failed();
					}
				}
			break;
		case 1:
			for (TInt ptrNum = iMaxDevPointers/2; ptrNum < iMaxDevPointers; ptrNum++)
				{
				if ((ptrNum & 1) == 0)
					{
					ptrPos = win1PtrPos;
					autoFocusWin = 1;
					}
				else
					{
					ptrPos = win2PtrPos;
					autoFocusWin = 0;
					}
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedWsEvent(EEventFocusLost);				// Second group window losing focus 
				AddExpectedWsEvent(EEventFocusGained);				// First group window gaining focus
				if (ptrNum == iMaxDevPointers/2) // first loop
					{												
					// When ever a group window changes focus, For all the group/client windows which have 
					// registered for receiving group window changes recive EEventFocusGroupChanged
					// EEventFocusGroupChanged is sent only once in one queue
					AddExpectedWsEvent(EEventFocusGroupChanged);
					AddExpectedWsEvent(EEventFocusGroupChanged);
					}
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);

				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				
				// These events are to clear out the pointer state
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, 0, 0, ptrNum);
				if (!iMultiPtrEventClient->CheckOrdinalPosition(autoFocusWin))
					{
					TEST(EFalse);
					Failed();
					}
				}
			break;
		default:
			iMultiPtrEventClient->DeleteGroupBlankWin();
			ResetFlushStopAS();
			break;
		}
	
	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0015
@SYMPREQ				PREQ1226
@SYMREQ					REQ9684
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports out-of-range and pointer Enter/Exit events.
@SYMTestStatus			Implemented

@SYMTestActions			Simulate moves and EPointer3DOutOfRange from different pointers
@SYMTestExpectedResults The received events must match the simulated pointer events
*/
void CTMultiPtrEventTest::MultiPointerOutOfRangeEvents()
	{
	TLogMessageText logText;
	_LIT(KSet, "OutOfRange & Enter/Exit events: %d of 4");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	TPoint ptrPos(10,10);
	TInt proxValue = -iProximityStep; 
	switch(iEventSet++)
		{
		case 0:
			// Simulate moves and EPointer3DOutOfRange from all pointers
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				// Here I am not enabling the enter/exit events filter so not getting those 
				}
			break;
		case 1:
			// Repeat the above by enabling the enter/exit events
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterEnterExit, 0);
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum = ptrNum+2)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedWsEvent(EEventPointerEnter, EFalse, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedWsEvent(EEventPointerExit, EFalse, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			break;
		case 2:
			// Check the moves from one window to other
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum = ptrNum+2)
				{
				ptrPos.iX = 10;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedWsEvent(EEventPointerEnter, EFalse, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				
				ptrPos.iX = (iMultiPtrEventClient->ParentWin()->Size().iWidth)/2 + 10;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedWsEvent(EEventPointerExit, EFalse, ptrNum); // move and enter of child window is filtered out
				}
			break;
		case 3:
			// Since all the pointers are now on child and when we change the pointer filter
			// wserv sends enter event to all pointers
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterEnterExit, 0);
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum = ptrNum+2)
				{
				AddExpectedWsEvent(EEventPointerEnter, EFalse, ptrNum);
				}
			for (TInt ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum = ptrNum+2)
				{
				ptrPos.iX = ptrPos.iY =10;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedWsEvent(EEventPointerExit, EFalse, ptrNum);
				AddExpectedWsEvent(EEventPointerEnter, EFalse, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, 0, 0, 0, ptrNum);
				AddExpectedWsEvent(EEventPointerExit, EFalse, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EOutOfRange, ptrPos, proxValue, 0, ptrNum);
				}
			break;			
		default:
			ResetFlushStopAS();
			break;
		}
	
	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0032
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Double click events for multi pointer events.
@SYMTestStatus			Implemented

@SYMTestActions			Simulate double click events from different pointers 			
@SYMTestExpectedResults The received events must match the simulated pointer events
*/
void CTMultiPtrEventTest::DoubleClickForMultiplePointers()
	{
	// Logic involved, double click settings are global no need to test them
	// Check that double click events are sent by wserv for all pointers
	TLogMessageText logText;
	_LIT(KSet, "Double click settings and events: %d of 1");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		AddExpectedWsEvent(EEventFocusGained);
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	TPoint ptrPos(10,10);
	switch(iEventSet++)
		{
		case 0:
			for (TInt ptrNum = 0; ptrNum < Min(iMaxDevPointers, 7); ptrNum++)
				{
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, EModifierDoubleClick, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				}
			break;
		default:
			ResetFlushStopAS();
			break;
		}
	
	LogEventsAdded();
	}


/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0031
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Virtual keyboard for multi pointer events.
@SYMTestStatus			Implemented

@SYMTestActions			Create virtual keyboard by AddKeyRect() to a window and simulate pointer events from all pointers			
@SYMTestExpectedResults The received events must match the simulated Key events
*/
void CTMultiPtrEventTest::VirtualKeyBoardForMultiplePointers()
	{
	TLogMessageText logText;
	_LIT(KSet, "VirtualKeyboard SetOfEvents: %d of 8");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);

	// Create virtual keyboard for 8 keys on parent window
	// give a 5 pixel spacing at start/end and in between keys
	const TInt noVirtualKeys = iMaxDevPointers;
	const TInt keySpacing = 5;
	const TInt keyWidth = ((iPhysicalScreenSize.iWidth/2) - ((noVirtualKeys+1)*keySpacing))/noVirtualKeys;
	const TInt keyHeight = keyWidth/2; 
	TInt startX = keySpacing;
	const TInt baseChar ='A';
	
	if (!iEventSet)
		{
		SetAutoFlush();
		for (TInt count=0; count < noVirtualKeys; count++)
			{
			iMultiPtrEventClient->ParentWin()->BaseWin()->AddKeyRect(TRect(startX, keySpacing, startX+keyWidth, keySpacing+keyHeight), baseChar+count,EFalse);
			startX+=(keyWidth+keySpacing);
			}
		AddExpectedWsEvent(EEventFocusGained);
		}
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	TPoint ptrPos(keySpacing+(keyWidth/2), keySpacing+(keyHeight/2));

	TInt count = 0;
	if (iEventSet < iMaxDevPointers)
		{
		for (; ptrPos.iX < (iPhysicalScreenSize.iWidth/2)-5; ptrPos.iX += (keySpacing+keyWidth), count++)
			{
			SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, iEventSet);
			AddExpectedKeyDownUp(baseChar+count);
			}
		}
	else
		{
		ResetFlushStopAS();
		}
	iEventSet++;

	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0021
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9683,
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Grabbing in case of multi pointer events.
@SYMTestStatus			Implemented

@SYMTestActions			Simulate EButton1Down events on iParentWin. Simulate Drag and Button1Up events on iChildWin.
 						SetPointerGrab is called on iParentWin for first test and disabled for the second. 
@SYMTestExpectedResults The Drag and Button1Up events contain a handle to iParentWin when Grabbing active. 
						The Drag and Button1Up events contain a handle to iChildWin when Grabbing disabled.
*/
void CTMultiPtrEventTest::GrabbingMultiPointerEvents()
	{
	// Test Grabbing of multi pointer events
	// 1. Test Wserv supports Grabbing in case of multi-pointer events
	// 2. Test pointer events are delivered correctly when Grabbing is disabled.
	TLogMessageText logText;
	_LIT(KSet, "GrabbingMultiPtrEvents SetOfEvents: %d of 2");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		}
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos;
		
	switch(iEventSet++)
		{
		case 0:
			INFO_PRINTF1(_L("Check Wserv supports Grabbing in case of multi-pointer events"));
			// Check pointer events are delivered to the window where the EButton1Down event occured, 
			// even if subsequent events are simulated outside this window.
			AddExpectedWsEvent(EEventFocusGained);
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			iMultiPtrEventClient->ParentWin()->BaseWin()->SetPointerGrab(ETrue);
			TestGrabbingForMultiPointer(ETrue);
			break;	
		case 1:
			INFO_PRINTF1(_L("Check pointer events are delivered correctly when Grabbing is disabled."));
			// Check pointer events are delivered to the window on which they occured.					
			iMultiPtrEventClient->ParentWin()->BaseWin()->SetPointerGrab(EFalse);
			TestGrabbingForMultiPointer(EFalse);
			break;	
		default:
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0025
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9683,
@SYMTestPriority		High
@SYMTestCaseDesc		Checks Wserv supports Filtering in case of multi pointer events.
@SYMTestStatus			Implemented

@SYMTestActions			Simulate Move and Drag events. Add only Button1Up and Button1Down to TWsEvent buffer.
   						Disable Filtering and repeat test with the addition of Move and Drag events to TWsEvent buffer. 
@SYMTestExpectedResults The received events must match the expected simulated raw events
						The received events must match the simulated raw events
*/
void CTMultiPtrEventTest::FilteringMultiPointerEvents()
	{
	// Test Filtering of multi pointer events
	// 1. Test pointer move and drag events are filtered correctly by Wserv
	// 2. Test pointer events are delivered correctly when Filtered is disabled.
	TLogMessageText logText;
	_LIT(KSet, "FilteringMultiPtrEvents SetOfEvents: %d of 2");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	if (!iEventSet)
		{
		SetAutoFlush();
		}
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos;
		
	switch(iEventSet++)
		{
		case 0:
			INFO_PRINTF1(_L("Check pointer move and drag events are filtered with multi-pointer events"));
			// Check pointer move and drag events are filtered by Wserv
			AddExpectedWsEvent(EEventFocusGained);
			
			// Ensure default values are set i.e. Drag and Move events are filtered out and not delivered to the client.
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag|EPointerFilterMove,
																		EPointerFilterDrag|EPointerFilterMove);
			TestFilteringForMultiPointer(ETrue);
			break;	
		case 1:
			INFO_PRINTF1(_L("Check pointer move and drag events are not filtered, with multi-pointer events, when filtering is disabled"));
			// Check move and drag events are not filtered by Wserv when filtering is disabled					

			// Enable Drag and Move events
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag|EPointerFilterMove, 0);
			TestFilteringForMultiPointer(EFalse);
			break;				
		default:
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

void CTMultiPtrEventTest::SetDefaultScreenMode()
	{
	iMultiPtrEventClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetAppScreenMode(0);
	iMultiPtrEventClient->iScreen->SetScreenMode(0);
	iMultiPtrEventClient->iScreen->SetCurrentRotations(0, CFbsBitGc::EGraphicsOrientationNormal);
	}

TPoint PhysicalToLogical(TPoint aPhysicalPt, TPoint aOrigin, TSize aSize)
	{
	return PhysicalToLogical(aPhysicalPt-aOrigin, aSize);
	}

TPoint CTMultiPtrEventTest::GetPointerPostionOnRotatedMode(TPoint aPointerPos, CFbsBitGc::TGraphicsOrientation aRotation)
	{
	if (aRotation == CFbsBitGc::EGraphicsOrientationNormal)
		{
		return aPointerPos;
		}
	else if (aRotation == CFbsBitGc::EGraphicsOrientationRotated90)
		{
		// here width is actually height in rotated mode
		return TPoint(iPhysicalScreenSize.iWidth - aPointerPos.iY - 1, aPointerPos.iX);
		}
	else if (aRotation == CFbsBitGc::EGraphicsOrientationRotated180)
		{
		return TPoint(iPhysicalScreenSize.iWidth - aPointerPos.iX - 1, iPhysicalScreenSize.iHeight - aPointerPos.iY - 1);
		}
	else if (aRotation == CFbsBitGc::EGraphicsOrientationRotated270)
		{
		// here width is actually height in rotated mode
		return TPoint(aPointerPos.iY, iPhysicalScreenSize.iHeight - aPointerPos.iX -1);
		}
	return TPoint(0,0);
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0028
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677, REQ9683
@SYMTestPriority		High 
@SYMTestCaseDesc		To check Wserv sends multi pointer events in Different screen modes
@SYMTestStatus			Implemented

@SYMTestActions			Simulate multi pointer events in Different screenmodes
@SYMTestExpectedResults The received events must match the simulated raw events  
*/
void CTMultiPtrEventTest::MultiPointerEventsInDiffScreenModes()
	{
	// Logic inloved, For a particular screen mode check pointer down/up, moves and drags
	// for different pointers and for different co-ordinates.
	// For each screen mode check these events on diffferent rotation that screen mode supports
	// During the last switch case increment the screenmode
	TLogMessageText logText;
	_LIT(KSet, "Multi pointer events in screenmode(%d): %d of 2");
	logText.AppendFormat(KSet, iCurrentScreenMode+1, iEventSet);
	LOG_MESSAGE(logText);
	
	TInt numScrMode = iMultiPtrEventClient->iScreen->NumScreenModes();
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	
	if (!iEventSet && !iChangeScreenMode)
		{
		SetAutoFlush();
		iChangeScreenMode = ETrue;
		AddExpectedWsEvent(EEventFocusGained);
		}
	else
		{
		if (iChangeScreenMode)
			{
			if (iCurrentScreenMode >= numScrMode)
				{
EndFun:
				SetDefaultScreenMode();
				ResetFlushStopAS();
				return;
				}
			}
		}

	if (iChangeScreenMode)
		{
		iMultiPtrEventClient->iScreen->SetAppScreenMode(iCurrentScreenMode);
		TheClient->iScreen->SetAppScreenMode(iCurrentScreenMode);
		iMultiPtrEventClient->iScreen->SetScreenMode(iCurrentScreenMode);
		iChangeScreenMode = EFalse;
		}
	
	CArrayFixFlat<TInt>* rotations = NULL;
	TRAPD(err, 
			rotations = new(ELeave) CArrayFixFlat<TInt>(1);
			iMultiPtrEventClient->iScreen->GetRotationsList(iCurrentScreenMode, rotations));
	if (err != KErrNone)
		{
		if (iEventSet)
			{
			goto EndFun;
			}
		else
			{
			return;
			}
		}

#if !defined(__WINS__)	
	TPoint ptrRotatedPos;
	TPoint ptrNormalPos;
#else
	TPoint ptrPhyPos;
#endif
	TPoint ptrLogPos;	
	
	TInt ptrNum = 0;
	TInt proxValue = 0;
	// Since screenmode positioning and screenmode scaling is not supported in NGA wserv2
	// but it is supported in Non NGA. But for NGA it will return (0,0) as origin and (1,1) as scale.
	TPoint screenOrigin(iMultiPtrEventClient->iScreen->GetDefaultScreenModeOrigin());
	TPoint scaledScreenOrigin(iMultiPtrEventClient->iScreen->GetCurrentScreenModeScaledOrigin());
	TSize scale(iMultiPtrEventClient->iScreen->GetCurrentScreenModeScale());
	
	switch(iEventSet++)
		{
		case 0:
			// Simulate Pointer down/up for different rotations of a screenmode
			for (TInt rotCou = 0; rotCou < rotations->Count(); rotCou++)
				{
				CFbsBitGc::TGraphicsOrientation newOrientation = static_cast<CFbsBitGc::TGraphicsOrientation>(rotations[0][rotCou]);
				iMultiPtrEventClient->iScreen->SetCurrentRotations(iCurrentScreenMode, newOrientation);
				
#if !defined(__WINS__)
				ptrNormalPos = screenOrigin;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				SimulatePointerDownUp(ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);				
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				
				ptrNormalPos.iX += 10;
				ptrNormalPos.iY += 5;
				ptrNum = iMaxDevPointers/2;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				SimulatePointerDownUp(ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);				
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				
				ptrNormalPos.iX += 5;
				ptrNormalPos.iY += 10;
				ptrNum = iMaxDevPointers-1;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				SimulatePointerDownUp(ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);				
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrNum = 0;
#else
				// This is not needed for NGA, becasue NGA wserv does not support positioning and scaling 
				// but for early access we are deliverying for NonNGA so we need it. 
				ptrPhyPos = screenOrigin;
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				SimulatePointerDownUp(ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrPhyPos.iX += 10;
				ptrPhyPos.iY += 5;
				ptrNum = iMaxDevPointers/2;
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				SimulatePointerDownUp(ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrPhyPos.iX += 5;
				ptrPhyPos.iY += 10;
				ptrNum = iMaxDevPointers-1;
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				SimulatePointerDownUp(ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrNum = 0;
#endif
				}
			break;
		case 1:
			// Simulate moves for different rotations of a screenmode with different Proximity
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove,0);
			for (TInt rotCou = 0; rotCou < rotations->Count(); rotCou++)
				{
				CFbsBitGc::TGraphicsOrientation newOrientation = static_cast<CFbsBitGc::TGraphicsOrientation>(rotations[0][rotCou]);
				iMultiPtrEventClient->iScreen->SetCurrentRotations(iCurrentScreenMode, newOrientation);
				
#if !defined(__WINS__)				
				ptrNormalPos = screenOrigin;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				proxValue = -iProximityStep;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrRotatedPos.iX, ptrRotatedPos.iY, proxValue, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);			
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrLogPos, proxValue, 0, ptrNum);
				SimulatePointerDownUp(ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				
				ptrNormalPos.iX += 10;
				ptrNormalPos.iY += 5;
				ptrNum = iMaxDevPointers/2;
				if (proxValue-iProximityStep > iMaxProximity)
					{
					proxValue -= iProximityStep;
					}
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrRotatedPos.iX, ptrRotatedPos.iY, proxValue, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);								
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrLogPos, proxValue, 0, ptrNum);
				SimulatePointerDownUp(ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrNum = 0;
#else
				ptrPhyPos = screenOrigin;
				proxValue = -iProximityStep;
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPhyPos.iX, ptrPhyPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrLogPos, proxValue, 0, ptrNum);
				SimulatePointerDownUp(ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				
				ptrPhyPos.iX += 10;
				ptrPhyPos.iY += 5;
				ptrNum = iMaxDevPointers/2;
				if (proxValue-iProximityStep > iMaxProximity)
					{
					proxValue -= iProximityStep;
					}
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPhyPos.iX, ptrPhyPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrLogPos, proxValue, 0, ptrNum);
				SimulatePointerDownUp(ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrLogPos, 0, ptrNum);
				ptrNum = 0;
#endif
				}
			break;
		case 2:
			{
			// Simulate drags for different rotations of a screenmode
#if !defined(__WINS__)
			TPoint intNormalPt;
			TPoint intRotatedPt;
#else
			TPoint interPhyPt;
			TPoint interLogPt;
#endif			
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag,0);
			for (TInt rotCou = 0; rotCou < rotations->Count(); rotCou++)
				{
				CFbsBitGc::TGraphicsOrientation newOrientation = static_cast<CFbsBitGc::TGraphicsOrientation>(rotations[0][rotCou]);
				iMultiPtrEventClient->iScreen->SetCurrentRotations(iCurrentScreenMode, newOrientation);
#if !defined(__WINS__)				
				ptrNormalPos = screenOrigin;
				intNormalPt.iX = ptrNormalPos.iX+4;
				intNormalPt.iY = ptrNormalPos.iY+5;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				intRotatedPt = GetPointerPostionOnRotatedMode(intNormalPt, newOrientation);				
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, intRotatedPt.iX, intRotatedPt.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, intRotatedPt.iX, intRotatedPt.iY, 0, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);				
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrLogPos, 0, 0, ptrNum);
				TPoint intLogPt = PhysicalToLogical(intNormalPt, screenOrigin, scale);				
				AddExpectedPointerEvent(TPointerEvent::EDrag, intLogPt, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, intLogPt, 0, 0, ptrNum);
				
				ptrNormalPos.iX += 10;
				ptrNormalPos.iY += 5;
				ptrNum = iMaxDevPointers-1;
				intNormalPt.iX = ptrNormalPos.iX+5;
				intNormalPt.iY = ptrNormalPos.iY+4;
				ptrRotatedPos = GetPointerPostionOnRotatedMode(ptrNormalPos, newOrientation);
				intRotatedPt = GetPointerPostionOnRotatedMode(intNormalPt, newOrientation);
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrRotatedPos.iX, ptrRotatedPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, intRotatedPt.iX, intRotatedPt.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, intRotatedPt.iX, intRotatedPt.iY, 0, ptrNum);
				ptrLogPos = PhysicalToLogical(ptrNormalPos, screenOrigin, scale);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrLogPos, 0, 0, ptrNum);
				intLogPt = PhysicalToLogical(intNormalPt, screenOrigin, scale);
				AddExpectedPointerEvent(TPointerEvent::EDrag, intLogPt, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, intLogPt, 0, 0, ptrNum);

				ptrNum = 0;
#else
				ptrPhyPos = screenOrigin;
				interPhyPt.iX = ptrPhyPos.iX+4;
				interPhyPt.iY = ptrPhyPos.iY+5;
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				interLogPt = PhysicalToLogical(interPhyPt, screenOrigin, scale);
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, interPhyPt.iX, interPhyPt.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, interPhyPt.iX, interPhyPt.iY, 0, ptrNum);
				interLogPt = PhysicalToLogical(interPhyPt, screenOrigin, scale);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrLogPos, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interLogPt, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interLogPt, 0, 0, ptrNum);
				
				ptrPhyPos.iX += 10;
				ptrPhyPos.iY += 5;
				ptrNum = iMaxDevPointers-1;
				interPhyPt.iX = ptrPhyPos.iX+5;
				interPhyPt.iY = ptrPhyPos.iY+4;
				
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPhyPos.iX, ptrPhyPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, interPhyPt.iX, interPhyPt.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, interPhyPt.iX, interPhyPt.iY, 0, ptrNum);
				
				ptrLogPos = PhysicalToLogical(ptrPhyPos, screenOrigin, scale);
				interLogPt = PhysicalToLogical(interPhyPt, screenOrigin, scale);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrLogPos, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interLogPt, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interLogPt, 0, 0, ptrNum);
				ptrNum = 0;
#endif
				}
			}
			iChangeScreenMode = ETrue;
			iEventSet = 0;
			iCurrentScreenMode++;
			break;
		default:
			break;
		}
	
	delete rotations;
	
	LogEventsAdded();
	}

/* 
 Checks if pointer click plugin has an error when testing its events
 If so then it gets the error deescription and displays the same
 */
void CTMultiPtrEventTest::TestAndDisplayPtrClickError()
	{
	TPckgBuf<TInt> intPkg;
	TInt retErr = iClick.CommandReply(EMultiPtrClickEventError, intPkg);
	if (retErr != KErrNone)
		{
		_LIT(KError, "Failed in Pointer click plugin");
		LOG_MESSAGE(KError);
		// Error is ignored here, becasue this function should not leave
		TRAPD(errMem, iErrDesc = iHeap->AllocL(KErrDescSize));
		if (errMem == KErrNone)
			{
			intPkg() = reinterpret_cast<TUint8*>(iErrDesc) - iChunk.Base() ;
			
			// get the error desccription, print it out and stop the tests
			TInt retVal = iClick.CommandReply(EADllMultiPtrEventErrorDesc, intPkg);
			if (retVal > KErrNone)
				{
				TPtr8 memPtr2(reinterpret_cast<TUint8*>(iErrDesc), retVal, retVal);
				HBufC16* buf16 = NULL;
				TRAPD(err, buf16 = HBufC16::NewL(retVal));		// Error is ignored here
				if (err == KErrNone)
					{
					buf16->Des().Copy(memPtr2);
					LOG_MESSAGE(buf16->Des());
					delete buf16;
					}
				}
			iHeap->Free(iErrDesc);
			}
		Failed();
		}
	iClick.CommandReply(EMultiPtrClickEventReset, intPkg);
	}

void CTMultiPtrEventTest::AddExpectedMultiPtrClickEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle)
	{
	TPckgBuf<TWsEvent> evtPkg;
	TWsEvent& event=evtPkg();
	event.SetType(EEventPointer);
	iMultiPtrEventClient->CalculatePtrPosAndSet3Ddata(event, aType, aPos, aModifier, aZ, aPointerNumber, aHandle);
	
	// change the parent position here as they r with respect to screen corodinates.
	event.Pointer()->iParentPosition = aPos;
	
	iClick.CommandReply(EMultiPtrClickEventAdd, evtPkg);
	AddExpectedMultiPtrClickOtherEvent(aType, aPos, aZ, aModifier, aPointerNumber, aHandle);
	}

void CTMultiPtrEventTest::AddExpectedMultiPtrClickOtherEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle)
	{
	TWsEvent event;
	event.SetType(EEventPointer);
	iMultiPtrEventClient->CalculatePtrPosAndSet3Ddata(event, aType, aPos, aModifier, aZ, aPointerNumber, aHandle);

	TPointerEventData otherEvent;
	otherEvent.iClientHandle = event.Handle();
	otherEvent.iCurrentPos = aPos;
	otherEvent.iPointerEvent = *event.Pointer();
	
	TPckgBuf<TPointerEventData> evtPkg(otherEvent);
	iClick.CommandReply(EMultiPtrOtherEventAdd, evtPkg);
	}

/* 
 Function of creating 
 1. Pointer click plugin
 2. RChunk for transfering error description from pointer click plugin to current test
 3. RHeap for allocating memory in RChunk
*/
void CTMultiPtrEventTest::CreatePointerClickPluginL()
	{
	// tranfer this to another function
	iClick=RSoundPlugIn(TheClient->iWs);
	User::LeaveIfError(iClick.Construct(TUid::Uid(MULTIPTR_CLICK_THIRD_UID)));
	TBool ptrPluginLoadable = EFalse;
	iClick.IsLoaded(ptrPluginLoadable);
	if (ptrPluginLoadable)
		{
		User::LeaveIfError(iClick.Load(KMultiPtrPluginDll));
		iPtrPluginLoaded = ETrue;
		}
	iClick.SetPenClick(ETrue);
	iHeap = UserHeap::ChunkHeap(&KMultiPtrClickChunk, 128, 256, 10);
	if (iHeap == NULL)
		{
		User::LeaveNoMemory();
		}
	User::LeaveIfError(iChunk.OpenGlobal(KMultiPtrClickChunk, ETrue));
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0027
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677, REQ9683
@SYMTestPriority		High 
@SYMTestCaseDesc		To check Wserv sends multi pointer events to PointerClickPlugin
@SYMTestStatus			Implemented

@SYMTestActions			Simulate multi pointer events for PointerClickPlugin
@SYMTestExpectedResults The received events must match the simulated raw events  
*/
void CTMultiPtrEventTest::MultiPointerEventsForPointerClickL()
	{
	TLogMessageText logText;
	_LIT(KSet, "Multi pointer events for PointerClickPlugin: %d of 4");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	if (!iEventSet)
		{
		SetAutoFlush();
		}
	
	TPoint ptrPos;
	TInt ptrNum = 0;
	TInt prValue = 0;
	TInt proxValue = 0;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
 	
	switch(iEventSet++)
		{
		case 0:
			// Creates RSoundPlugin, loads the plugin DLL, create RChunk and RHeap
			CreatePointerClickPluginL();
			AddExpectedWsEvent(EEventFocusGained);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayPtrClickError();
			break;
		case 1:
			// Simulate moves from different pointer with different pressure
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove,0);
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterMove,0);
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				AddExpectedMultiPtrClickEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Down,ptrPos,0,0,ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Up,ptrPos,0,0,ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayPtrClickError();
			break;
		case 2:
			// Simulate moves from different pointer with different proximity
			ptrPos.SetXY(0,0);
			proxValue = -iProximityStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				AddExpectedMultiPtrClickEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Down,ptrPos,0,0,ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Up,ptrPos,0,0,ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayPtrClickError();
			break;
		case 3:
			{
			// Simulate drags from all pointers
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag,0);
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterDrag,0);
			ptrPos.SetXY(0,0);
			TPoint interPt;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				interPt.iX = ptrPos.iX+1;
				interPt.iY = ptrPos.iY+2;
				AddExpectedMultiPtrClickEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				AddExpectedMultiPtrClickEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayPtrClickError();
			}
			break;
		default:
			iClick.Unload();
			iClick.Close();
			iPtrPluginLoaded = EFalse;
			iHeap->Close();
			iHeap = NULL;
			iChunk.Close();
			ResetFlushStopAS();
			break;
		}

	LogEventsAdded();
	}

void CTMultiPtrEventTest::GetRemainingEventsFromSecondaryClient()
	{
	if (iSecMultiPtrEventClient->EventBuffer()->EventsRemaining() != 0)
		{
		iSecMultiPtrEventClient->EventBuffer()->SetNestedLoopState(ETrue);
		CActiveScheduler::Start();
		}	
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0011
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9674,
@SYMTestPriority		High 
@SYMTestCaseDesc		To check Wserv sends Multi Pointer events to correct clients
@SYMTestStatus			Implemented

@SYMTestActions			Simulate multi pointer events on windows from different client.
@SYMTestExpectedResults The received events must match the simulated raw events 
*/
void CTMultiPtrEventTest::MultiClientMultiPointerEvents()
	{
	// Logic for this test code
	// Create a secondary client when running this test
	// Simulate events for both main client and secondary simulataneously
	// Get events from DoRunL() of event buffer class
	// Before running next set of events, get all the events from secondary client
	// If test fails on any client make sure both active scheduler are stopped
	TLogMessageText logText;
	_LIT(KSet, "Multi pointer events for Multi client: %d of 5");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	iSecMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	if (!iEventSet)
		{
		SetAutoFlush();
		iSecClientFlush = iSecMultiPtrEventClient->iWs.SetAutoFlush(ETrue);
		}
	
	TPoint ptrPos;
	TInt ptrNum = 0;
	TInt prValue = 0;
	TInt proxValue = 0;
	TInt xInc = (iPhysicalScreenSize.iWidth/2)/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
	TPoint secPt(iPhysicalScreenSize.iWidth/2,0);
	TInt ptrSecNum = iMaxDevPointers-1;
 	
	switch(iEventSet++)
		{
		case 0:
			ptrPos.SetXY(0,0);
			
			// These events are generated when primary client is created and its group 
			// window constructed and then when button down for primary client is called
			AddExpectedWsEvent(EEventFocusGained);
			AddExpectedWsEvent(EEventFocusLost);
			AddExpectedWsEvent(EEventFocusGained);
			
			// Simulate pointer down/up events for primary client from 0,1,3... pointers and 6,5,4... pointers for secondary client
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
							
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			
			ptrPos.iX = iPhysicalScreenSize.iWidth/2;
			ptrPos.iY = 0;
			
			// These events are generated when secondary cleint is created and then when
			// button down for primary client is called, then when SetOrdinalPosition is 
			// called on secondary cleint's group window
			AddExpectedWsEvent(EEventFocusGained, ETrue);
			AddExpectedWsEvent(EEventFocusLost, ETrue);
			iSecMultiPtrEventClient->iGroup->GroupWin()->SetOrdinalPosition(0, 1);
			AddExpectedWsEvent(EEventFocusGained, ETrue);

			AddExpectedWsEvent(EEventFocusLost);
			
			for (ptrNum = iMaxDevPointers/2; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
							
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			break;
		case 1:
			// Simulate move events from primary and secondary client simultaneously with pressure
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			iSecMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
				
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				
				SimulatePointerEvent(TRawEvent::EPointerMove, secPt.iX, secPt.iY, prValue, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, secPt, prValue, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}
			break;
		case 2:
			// Simulate move events from primary and secondary client simultaneously with proximity
			ptrPos.SetXY(0,0);
			proxValue = -iProximityStep;
			
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				
				SimulatePointerEvent(TRawEvent::EPointerMove, secPt.iX, secPt.iY, proxValue, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, secPt, proxValue, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}
			break;
		case 3:
			{
			// Simulate drag events on different clients window one after the other 
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			iSecMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			
			ptrPos.SetXY(0,0);
			secPt.iX = iPhysicalScreenSize.iWidth/2;
			secPt.iY = 0; 
			ptrSecNum = iMaxDevPointers-1;
			TPoint interPt;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				interPt.iX = ptrPos.iX+2;
				interPt.iY = ptrPos.iY+1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				
				SimulatePointerEvent(TRawEvent::EButton1Down, secPt.iX, secPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, secPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				interPt.iX = secPt.iX+2;
				interPt.iY = secPt.iY+1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}
			}
			break;
		case 4:
			{
			// Simulate drag events on different clients, but interleaved each other 
			ptrPos.SetXY(0,0);
			secPt.iX = iPhysicalScreenSize.iWidth/2;
			secPt.iY = 0; 
			ptrSecNum = iMaxDevPointers-1;
			TPoint interPt;
			
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				
				SimulatePointerEvent(TRawEvent::EButton1Down, secPt.iX, secPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, secPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}
			
			ptrPos.SetXY(0,0);
			secPt.iX = iPhysicalScreenSize.iWidth/2;
			secPt.iY = 0;
			ptrSecNum = iMaxDevPointers-1;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				interPt.iX = ptrPos.iX+2;
				interPt.iY = ptrPos.iY+1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				

				interPt.iX = secPt.iX+2;
				interPt.iY = secPt.iY+1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}

			ptrPos.SetXY(0,0);
			secPt.iX = iPhysicalScreenSize.iWidth/2;
			secPt.iY = 0;
			ptrSecNum = iMaxDevPointers-1;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers/2 && ptrSecNum >= iMaxDevPointers/2); ptrNum++, ptrSecNum--)
				{
				interPt.iX = ptrPos.iX+2;
				interPt.iY = ptrPos.iY+1;
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				
				interPt.iX = secPt.iX+2;
				interPt.iY = secPt.iY+1;
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrSecNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrSecNum, (TUint)iSecMultiPtrEventClient->ParentWin(), ETrue);
				secPt.iX += xInc;
				secPt.iY += yInc;
				}
			}
			break;
		default:
			ResetFlushStopAS();
			iSecMultiPtrEventClient->iWs.SetAutoFlush(iSecClientFlush);
			break;
		}
	
	_LIT(KEventsAdded, "Events added to Primary client's buffer = %d");
	logText.Format(KEventsAdded, iMultiPtrEventClient->EventBuffer()->EventsRemaining());
	LOG_MESSAGE(logText);
	_LIT(KEventsAdded1, "Events added to Secondary client's buffer = %d");
	logText.Format(KEventsAdded1, iSecMultiPtrEventClient->EventBuffer()->EventsRemaining());
	LOG_MESSAGE(logText);
	}


// RMultiPointerAnim
RMultiPointerAnim* RMultiPointerAnim::NewL(RWindowBase* aWin, RAnimDll* aDll)
	{
	RMultiPointerAnim* self=new(ELeave) RMultiPointerAnim(aDll);
	CleanupStack::PushL(TCleanupItem(DestroyAnim,self));
	User::LeaveIfError(self->Construct(*aWin, EAnimTypeMultiPointer, TPtrC8()));
	CleanupStack::Pop(self);
	return self;
	}

TInt RMultiPointerAnim::AddExpectedAnimPointerEvent(TRawEvent::TType aType, TPoint aPos, TInt aZ, TUint /*aModifier*/, TUint8 aPointerNumber, TBool aEat /*= ETrue*/)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType, aPos.iX, aPos.iY, aZ, aPointerNumber);
	TAnimRawEvent animEvent;
	animEvent.iRawEvent = rawEvent;
	animEvent.iEat = aEat;
	TPckgBuf<TAnimRawEvent> param;
	param()=animEvent;
	return CommandReply(EADllAddExpectedMultiPtrEvent, param);
	}

void CTMultiPtrEventTest::TestAndDisplayAnimError()
	{
	TInt err = iAnim->CommandReply(EADllMultiPtrEventError);
	if (err != KErrNone)
		{
		_LIT(KError, "Failed in Anim dll");
		LOG_MESSAGE(KError);
		TBuf<255> errDesc;
		errDesc.SetMax();
		TIpcArgs ipcArgs;
		ipcArgs.Set(KIpcSlot,&errDesc);
		// get the error desccription, print it out and stop the tests
		TInt retVal = iAnim->CommandReply(EADllMultiPtrEventErrorDesc, KNullDesC8, ipcArgs);
		if (retVal == KErrNone)
			{
			LOG_MESSAGE(errDesc);
			}
		Failed();
		}
	iAnim->CommandReply(EADllMultiPtrEventReset);
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0013
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9683,
@SYMTestPriority		High 
@SYMTestCaseDesc		To check Wserv sends Multi Pointer events to Anims
@SYMTestStatus			Implemented

@SYMTestActions			Simulate multi pointer events for anims
@SYMTestExpectedResults The received events must match the simulated raw events 
*/
void CTMultiPtrEventTest::MultiPointerEventsForAnimsL()
	{
	// The logic behind these tests is
	// Create an anim, which creates animation at server side(which swicthes raw event handling on)
	// Add the required events to the animtaion at server side buffer
	// Simulate those events and check simulated events are same as required at server side
	// If any test fail, error description is got from server and displayed in the log file
	TLogMessageText logText;
	_LIT(KSet, "Multi pointer events for Anims: %d of 3");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	_LIT(KEventsChecked, "Events checked at server side = %d");
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	if (!iEventSet)
		{
		SetAutoFlush();
		iAnimDll = new(ELeave) RAnimDll(iMultiPtrEventClient->iWs);
		User::LeaveIfError(iAnimDll->Load(KAnimDLLName));
		}
	
	TPoint ptrPos;
	TInt ptrNum = 0;
	TInt prValue = 0;
	TInt proxValue = 0;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
 	
	switch(iEventSet++)
		{
		case 0:
			// Simulate downup events for all pointers
			iAnim = RMultiPointerAnim::NewL(iMultiPtrEventClient->ParentWin()->BaseWin(), iAnimDll);
			AddExpectedWsEvent(EEventFocusGained);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Down,ptrPos,0,0,ptrNum);
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Up,ptrPos,0,0,ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayAnimError();
			logText.Format(KEventsChecked, iMaxDevPointers*2);
			LOG_MESSAGE(logText);
			break;
		case 1:
			// Simulate moves from different pointer with different pressure
			// Here there is no need for events to be exaclty on the anim window. 
			// becasue when the animtaions ask for events it will be delivered all the events
			// irrespective of where they occured.
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointerMove, ptrPos, prValue, 0, ptrNum);
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Down,ptrPos,0,0,ptrNum);
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Up,ptrPos,0,0,ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayAnimError();
			
			// This is for just to add run the next test
			SimulatePointerEvent(TRawEvent::EPointerSwitchOn, 0, 0, 0, 0);
			
			logText.Format(KEventsChecked, iMaxDevPointers*3);
			LOG_MESSAGE(logText);
			break;
		case 2:
			// Simulate moves from different pointer with different proximity
			ptrPos.SetXY(0,0);
			proxValue = -iProximityStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EPointerMove, ptrPos, proxValue, 0, ptrNum);
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Down,ptrPos,0,0,ptrNum);
				iAnim->AddExpectedAnimPointerEvent(TRawEvent::EButton1Up,ptrPos,0,0,ptrNum);
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			TestAndDisplayAnimError();
			
			// This is for just to add run the next test
			SimulatePointerEvent(TRawEvent::EPointerSwitchOn, 0, 0, 0, 0);
			
			logText.Format(KEventsChecked, iMaxDevPointers*3);
			LOG_MESSAGE(logText);
			break;
		default:
			DestroyAnimDllAndAnim();
			ResetFlushStopAS();
			break;
		}

	LogEventsAdded();
	}


/**
@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0014
@SYMPREQ				PREQ1226
@SYMREQ					REQ9677,REQ9683,
@SYMTestPriority		High 
@SYMTestCaseDesc		To check Wserv purges multi pointer events with standard rules
@SYMTestStatus			Implemented

@SYMTestActions			Simulate raw events to be purged  
@SYMTestExpectedResults The purged events should not be delivered to client 
*/
void CTMultiPtrEventTest::PurgingMultiPointerEvents()
	{
	// Test purging of multi pointer events
	// 1. Test down/up pairs are purged
	// 2. Test moves events are purged
	// 3. Test drag events are purged
	// 4. Test lone down events are purged
	// 5. And finally test lone up events are also purged 
	TLogMessageText logText;
	_LIT(KSet, "Purging of multi pointer events: %d of 6");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);
	
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);
	TInt xInc = iPhysicalScreenSize.iWidth/KMaxQueueSize;
	TInt yInc = iPhysicalScreenSize.iHeight/KMaxQueueSize;
	TPoint ptrPos(xInc,yInc);
	TInt ptrNum = 0;
	TInt prValue = 0;
	TInt proxValue = 0;
	
	if (!iEventSet)
		{
		SetAutoFlush();
		}
	
	// Point to note is that Kernel has an event queue size of 40
	// So make sure that you don't send more than 40 events
	switch(iEventSet++)
		{
		case 0:
			// Simulate pointer down/up events for all pointers
			// Simulate many up events from any pointer which will purge down/up events
			AddExpectedWsEvent(EEventFocusGained);
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 4); ptrNum++)
				{
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				}
			SimulateAndAddLoneUpEvents(KMaxQueueSize, KMaxQueueSize-1, ptrPos);
			break;
		case 1:
			// Repeat the same for Move events
			// Simulate moves for all pointers with different pressure and proximity value
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			prValue = iPressureStep;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && prValue < iMaxPressure && ptrNum < 4); ptrNum++,prValue+=iPressureStep)
				{
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				}
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			proxValue = -iProximityStep;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && proxValue > iMaxProximity && ptrNum < 4); ptrNum++,proxValue-=iProximityStep)
				{
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				}
			SimulateAndAddLoneUpEvents(KMaxQueueSize, KMaxQueueSize, ptrPos);
			break;
		case 2:
			{
			// Repeat the same for drag events
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 2); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			TPoint interPt = ptrPos;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 2); ptrNum++)
				{	
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			interPt = ptrPos;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 2); ptrNum++)
				{
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
			SimulateAndAddLoneUpEvents(KMaxQueueSize, KMaxQueueSize, ptrPos);
			}
			break;
		case 3:
			// Repeat the same for lone down events
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 3); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			SimulateAndAddLoneUpEvents(KMaxQueueSize+1, KMaxQueueSize, ptrPos);

			// Before this we had to send dummy 7 EButton1Up events from 1 to 7 pointers so that iLastUnmatchedDown1
			// variable to be reset and that consecutive events was not sent to client. But now EPointer3DOutOfRange
			// resets all the variables.
			for (ptrNum = 1; (ptrNum < iMaxDevPointers && ptrNum < 3); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointer3DOutOfRange, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				}
			break;
		case 4:
			// Finally for even lone up events, check that when you send events more then the queuesize
			// the events are not sent back and they are ignored in wserv
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; (ptrNum < iMaxDevPointers && ptrNum < 4); ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (TInt count = 0; count < KMaxQueueSize; count++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
				// For all up events more then KMaxQueueSize, wserv igoners then as the queue if full
				if (count < (KMaxQueueSize - iMaxDevPointers))
					{
					AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
					}
				}
			break;
		case 5:
			// Check PurgePointerEvents works with multi pointer events
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			iMultiPtrEventClient->iWs.PurgePointerEvents();
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
			break;
		default:
			ResetFlushStopAS();
			break;
		}
	LogEventsAdded();
	}

void CTMultiPtrEventTest::MultiPointerEvents()
	{
	TLogMessageText logText;
	_LIT(KSet, "MultiPtrEvents SetOfEvents: %d of 9");
	logText.AppendFormat(KSet, iEventSet);
	LOG_MESSAGE(logText);

	if (!iEventSet)
		{
		SetAutoFlush();
		}
	iMultiPtrEventClient->EventBuffer()->SetEventCount(0);

	TPoint ptrPos;
	TInt ptrNum = 0;
	TInt prValue = 0;
	TInt proxValue = 0;
	TInt xInc = iPhysicalScreenSize.iWidth/iMaxDevPointers;
	TInt yInc = iPhysicalScreenSize.iHeight/iMaxDevPointers;
	
	switch(iEventSet++)
		{
		case 0:
			/**
			@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0001
			@SYMPREQ				PREQ1226
			@SYMREQ					REQ9674,REQ9687,
			@SYMTestPriority		High 
			@SYMTestCaseDesc		Checks Wserv sends multi touch pointer events received by it
			@SYMTestStatus			Implemented

			@SYMTestActions			Simulate raw events from different pointers 
			@SYMTestExpectedResults The received events must match the simulated raw events
			*/
			INFO_PRINTF1(_L("Simulate pointer events from all pointers"));
			INFO_PRINTF1(_L("Test pattern is PointerDown0/PointerUp0, PointerDown1/PointerUp1, PointerDown2/PointerUp2..."));
			// Test pattern is down0/up0, down1/up1, down2/up2, down3/up3....
			AddExpectedWsEvent(EEventFocusGained);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			break;
		case 1:
			/**
			@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0003
			@SYMPREQ				PREQ1226
			@SYMREQ					REQ9674,REQ9687,
			@SYMTestPriority		High 
			@SYMTestCaseDesc		To check Wserv sends simultaneous multi touch pointer events 
			@SYMTestStatus			Implemented

			@SYMTestActions			Simulate simultaneous raw events from different pointers 
			@SYMTestExpectedResults The received events must match the simulated raw events
			*/
			INFO_PRINTF1(_L("Test pattern PointerDown0/PointerDown1/PointerDown2/... PointerUp0/PointerUp1/PointerUp2..."));
			// Test pattern is pointer0Down/pointer1Down/pointer2Down/pointer3Down.... pointer0Up/pointer1Up/pointer2Up/pointer3Up.... 
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}

			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;				
				}
			break;
		case 2:
			/**
			@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0005
			@SYMPREQ				PREQ1226
			@SYMREQ					REQ9676
			@SYMTestPriority		High 
			@SYMTestCaseDesc		To check Wserv sends the multi touch pointer events received 
									by it along with the Pressure or Proximity 
			@SYMTestStatus			Implemented

			@SYMTestActions			Simulate raw events with pressure and proximity from different pointers  
			@SYMTestExpectedResults The received events must match the simulated raw events
			*/
			// Simulate Button1Down with differet pressure
			// Simulate Button1Up with zero pressure
			INFO_PRINTF1(_L("Simulate pointer events with different pressure values"));
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
			for (; (prValue <= iMaxPressure && ptrPos.iX < iPhysicalScreenSize.iWidth && ptrPos.iY < iPhysicalScreenSize.iHeight); prValue+=iPressureStep)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, 0);
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, 0);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			
			// Test with max pressure it might be that the width may cross the physical screen size
			ptrPos.SetXY(0,0);
			prValue = iMaxPressure;
			SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, prValue, 0);
			SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, prValue, 0, 0);
			AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, 0);
			break;
		case 3:
			// Simulate EPointerMove with different pressure values and for differernt pointers
			INFO_PRINTF1(_L("Simulate pointer events with different pressure values and from different pointers"));
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
			
			// Enable move events 
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			iMultiPtrEventClient->ChildWin()->BaseWin()->PointerFilter(EPointerFilterMove, 0);
			
			// Right now when 2 or more moves occur, wserv deletes all the moves except the last one.
			// So test only one move per Pointer and then one set of button down and button up
			// Then repeat the same for different pressure values
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			
			// Test moves with different pressure 
			ptrPos.SetXY(0,0);
			if ((prValue+iPressureStep) <= iMaxPressure)
				{
				prValue += iPressureStep;
				}
			ptrNum = 0;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
			break;
		case 4:
			INFO_PRINTF1(_L("Simulate pointer events with different proximity values"));
			// Repeat the same test of case 3 for different proximity values
			ptrPos.SetXY(0,0);
			
			proxValue = -iProximityStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerDownUp(ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerDownUp(ptrPos, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}

			ptrPos.SetXY(0,0);
			if ((proxValue-iProximityStep) > iMaxProximity)
				{
				proxValue -= iProximityStep;
				}
			ptrNum = 0;
			SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
			AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
			break;
		case 5:
			/**
			@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0007
			@SYMPREQ				PREQ1226
			@SYMREQ					REQ9677
			@SYMTestPriority		High 
			@SYMTestCaseDesc		To check Wserv sends move/drags of different pointers  
			@SYMTestStatus			Implemented

			@SYMTestActions			Simulate raw move and drag events from different pointers  
			@SYMTestExpectedResults The received events must match the simulated raw events
			*/
			INFO_PRINTF1(_L("Simulate pointer drag events"));
			ptrPos.SetXY(0,0);
			iMultiPtrEventClient->ParentWin()->BaseWin()->PointerFilter(EPointerFilterDrag, 0);
			
			// Simulate EButton1Down and then one move and then EButton1Up event  
			// which will lead to corresponding drag, do it for all the pointers
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				TestDragForMultiPointer(ptrPos, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}

			// Check the drag with different pressure value with different pointer
			prValue = iPressureStep;
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			if ((prValue+iPressureStep) <= iMaxPressure)
				{
				prValue += iPressureStep;
				}
			ptrNum = 0;
			TestDragForMultiPointer(ptrPos, prValue, ptrNum);
			break;
		case 6:
			{
			/**
			@SYMTestCaseID			GRAPHICS-WSERV-ADVANCEDPOINTER-0009
			@SYMPREQ				PREQ1226
			@SYMREQ					REQ9677, REQ9674
			@SYMTestPriority		High 
			@SYMTestCaseDesc		To check Wserv sends simultaneous move/drags of different pointers  
			@SYMTestStatus			Implemented

			@SYMTestActions			Simulate simulatneous raw move and drag events from different pointers  
			@SYMTestExpectedResults The received events must match the simulated raw events
			*/
			INFO_PRINTF1(_L("Simulate pointer drag events in different pattern"));
			// Dragging done in this pattern
			// pointer0Down/pointer1Down/pointer2Down/pointer3Down... pointer0Up/pointer1Up/pointer2Up/pointer3Up....
			
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			
			ptrPos.SetXY(0,0);
			TPoint interPt = ptrPos;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
		
			ptrPos.SetXY(0,0);
			interPt = ptrPos;
			for (ptrNum = 0; ptrNum < iMaxDevPointers; ptrNum++)
				{
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
			break;
			}
		case 7:
			{
			// Mixing of drag and moves of different pointers
			ptrPos.SetXY(0,0);
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				}
			ptrPos.SetXY(0,0);
			TPoint interPt = ptrPos;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EPointerMove, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EDrag, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
		
			ptrPos.SetXY(0,0);
			interPt = ptrPos;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				interPt.iX += 2;
				interPt.iY += 1;
				SimulatePointerEvent(TRawEvent::EButton1Up, interPt.iX, interPt.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, interPt, 0, 0, ptrNum);
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				interPt = ptrPos; 
				}
			
			ptrPos.SetXY(0,0);
			prValue = iPressureStep;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2 && prValue < iMaxPressure; ptrNum++, prValue+=iPressureStep)
				{
				SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, prValue, 0, ptrNum);
				ptrPos.iX += xInc;
				ptrPos.iY += yInc;
				}
			}
			break;
		case 8:
			{
			INFO_PRINTF1(_L("Check intermediate Moves and Drags are deleted except last event"));
			// Check if intermediate move and drag events are deleted
			// The deletion is done with respect to same pointer and same window
			xInc = iPhysicalScreenSize.iWidth/KMaxQueueSize;
			yInc = iPhysicalScreenSize.iHeight/KMaxQueueSize;
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			TInt count = 0;
			
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				// Make sure that ptrPos does not go out of screen area, i,e the loop count should not exceed 4
				for (count = proxValue = 0; proxValue > iMaxProximity && count < 4; proxValue=proxValue-(4*iProximityStep), count++)
					{
					ptrPos.iX += xInc/2;
					ptrPos.iY += yInc/2;
					SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, proxValue, ptrNum);
					}
				// Since the for loop increments the variables at the end
				proxValue = proxValue + (4*iProximityStep);
				AddExpectedPointerEvent(TPointerEvent::EMove, ptrPos, proxValue, 0, ptrNum);
				}
			
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				SimulatePointerEvent(TRawEvent::EButton1Down, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Down, ptrPos, 0, 0, ptrNum);
				}

			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				// Make sure that ptrPos does not go out of screen area, i,e the loop count shuld not exceed 4
				for (count = prValue = 0; prValue < iMaxPressure && count < 4; prValue=prValue+(4*iPressureStep), count++)
					{
					ptrPos.iX += xInc/2;
					ptrPos.iY += yInc/2;
					SimulatePointerEvent(TRawEvent::EPointerMove, ptrPos.iX, ptrPos.iY, prValue, ptrNum);
					}
				prValue = prValue-(4*iPressureStep);
				AddExpectedPointerEvent(TPointerEvent::EDrag, ptrPos, prValue, 0, ptrNum);
				}
			
			ptrPos.iX = xInc;
			ptrPos.iY = yInc;
			for (ptrNum = 0; ptrNum < iMaxDevPointers/2; ptrNum++)
				{
				ptrPos.iX += xInc/2;
				ptrPos.iY += yInc/2;
				SimulatePointerEvent(TRawEvent::EButton1Up, ptrPos.iX, ptrPos.iY, 0, ptrNum);
				AddExpectedPointerEvent(TPointerEvent::EButton1Up, ptrPos, 0, 0, ptrNum);
				}
			}
			break;
		default:
			ResetFlushStopAS();
			break;
		}
	
	LogEventsAdded();
	}

// Logs number of events added to test buffer for comaparison
void CTMultiPtrEventTest::LogEventsAdded()
	{
	TLogMessageText logText1;
	_LIT(KEventsAdded, "Events added to buffer = %d");
	logText1.AppendFormat(KEventsAdded, iMultiPtrEventClient->EventBuffer()->EventsRemaining());
	LOG_MESSAGE(logText1);	
	}

// If any of the Tests fails, bring the wserv in a consistent state for other test to run, 
// stop ActiveScheduler and display some logs where the fail occured.
void CTMultiPtrEventTest::Failed()
	{
	switch (iTest->iState)
		{
		case 2:		// Anim tests
			DestroyAnimDllAndAnim();
			break;
		case 3:		// MultiClient tests
			if (iMultiPtrEventClient->EventBuffer()->NestedLoopState())
				{
				iSecMultiPtrEventClient->EventBuffer()->SetNestedLoopState(EFalse);
				CActiveScheduler::Stop();
				}
			break;
		case 4:		//PtrClick tests
			if (iPtrPluginLoaded)
				{
				iPtrPluginLoaded = EFalse;
				iClick.Unload();
				iClick.Close();
				}
			if (iHeap)
				{
				iHeap->Close();
				iHeap = NULL;
				iChunk.Close();
				}
			break;
		case 5:		//Screen mode tests
			SetDefaultScreenMode();
			break;
		case 11:
			iMultiPtrEventClient->DeleteGroupBlankWin();
			break;
		case 12:	//Emulation tests, iEventSet is incremented in main switch case.
			switch (iEventSet)
				{
				case 9:
					DeleteGroupWin();
					break;
				case 10:
					DestroyAnimDllAndAnim();
					break;
				case 11:
				case 12:
					iMultiPtrEventClient->ParentWin()->BaseWin()->FreePointerMoveBuffer();
					iMultiPtrEventClient->ParentWin()->BaseWin()->DisablePointerMoveBuffer();
					break;
				case 14:
				case 15:
					iMultiPtrEventClient->iWs.SetPointerCursorArea(iOrigPtrCursorArea);
					break;
				default:
					break;
				}
			break;
		case 15:
			if (iEventSet > 1)
				{
				DeleteGroupWinForCapturing();
				}
			break;
		case 13:
		case 16:
			SetThresholdValues(KMaxTInt, KMinTInt, KMaxTInt, KMinTInt);
			break;
		default:
			break;
		}
	
	if (iActiveSchedulerStarted)
		{
		iActiveSchedulerStarted = EFalse;
		CActiveScheduler::Stop();
		}
	
	// Display the test case number, subtest case number and number of events left in the buffer which can be used to
	// find out which event actually caused the failure when any test fails
	TLogMessageText logText1;
	_LIT(KEventsAdded, "Test case number %d Sub test case number %d Events remaining in the buffer %d \n");
	logText1.AppendFormat(KEventsAdded, iTest->iState, iEventSet, iMultiPtrEventClient->EventBuffer()->EventsRemaining());
	LOG_MESSAGE(logText1);
	RDebug::Printf("Test case number %d Sub test case number %d Events remaining in the buffer %d \n", iTest->iState, iEventSet, iMultiPtrEventClient->EventBuffer()->EventsRemaining());
	
	iFailed=ETrue;
	}

// This function is called by TestExecute Framework, through which all the testcases are run
void CTMultiPtrEventTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest0, "MultiPointerEvents with Pressure and Proximity");
	_LIT(KTest1, "Purging Of MultiPointerEvents");
	_LIT(KTest2, "MultiPointerEvents for Anims");
	_LIT(KTest3, "MultiPointerEvents from multi client");
	_LIT(KTest4, "MultiPointerEvents for pointer click plugin");
	_LIT(KTest5, "MultiPointerEvents In Different ScreenModes");
	_LIT(KTest6, "Filtering Of MultiPointerEvents");
	_LIT(KTest7, "Grabbing Of MultiPointerEvents");
	_LIT(KTest8, "VirtualKeyboard for Multiple Pointers");
	_LIT(KTest9, "DoubleClick events for Multiple Pointer");
	_LIT(KTest10, "OutOfRange & EnterExit for Multiple Pointer");
	_LIT(KTest11, "Auto foreground for Multiple Pointer");
	_LIT(KTest12, "Emulation rules for Mulitple Pointers");
	_LIT(KTest13, "Close proximity and High Pressure events");
	_LIT(KTest14, "Out of bound value testing");
	_LIT(KTest15, "Capturing Of MultiPointerEvents");
	_LIT(KTest16, "Negative testing for Proximity and Pressure events");
	_LIT(KTest17, "PointerRepeatEvent for Multiple Pointers");
	
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    TestComplete();
	    return;
	    }
	
	switch(iTest->iState)
		{
	case 0:
		iTest->LogSubTest(KTest0);
		RunTestsL();
		break;
	case 1:
		iTest->LogSubTest(KTest1);
		RunTestsL();
		break;
	case 2:
		iTest->LogSubTest(KTest2);
		RunTestsL();
		break;
	case 3:
		iTest->LogSubTest(KTest3);
		RunTestsL();
		break;
	case 4:
		iTest->LogSubTest(KTest4);
		RunTestsL();
		break;
	case 5:
		iTest->LogSubTest(KTest5);
		RunTestsL();
		break;
	case 6:
		iTest->LogSubTest(KTest6);
		RunTestsL();
		break;	
	case 7:
		iTest->LogSubTest(KTest7);
		RunTestsL();
		break;	
	case 8:
		iTest->LogSubTest(KTest8);
		RunTestsL();
		break;
	case 9:
		iTest->LogSubTest(KTest9);
		RunTestsL();
		break;
	case 10:
		iTest->LogSubTest(KTest10);
		RunTestsL();
		break;
	case 11:
		iTest->LogSubTest(KTest11);
		RunTestsL();
		break;
	case 12:
		iTest->LogSubTest(KTest12);
		RunTestsL();
		break;
	case 13:
		iTest->LogSubTest(KTest13);
		RunTestsL();
		break;
	case 14:
		iTest->LogSubTest(KTest14);
		RunTestsL();
		break;
	case 15:
		iTest->LogSubTest(KTest15);
		RunTestsL();
		break;
	case 16:
		iTest->LogSubTest(KTest16);
		RunTestsL();
		break;	
	case 17:
		iTest->LogSubTest(KTest17);
		RunTestsL();
		break;	
	case 18:
		// Keep this code in last case statement for main test completion 
		TestComplete();
		break;
	default:
		AutoPanic(EAutoPanicWrongTest);
		}

	// 
	if (iFailed)
		{
		TEST(EFalse);
		iFailed=EFalse;
		}
	++iTest->iState;
	}

// Macro which is used for construction of CTMultiPtrEventTestStep object
// and also used to call CreateTestL() which creates CTMultiPtrEventTest and CTestBase objects
__WS_CONSTRUCT_STEP__(MultiPtrEventTest)
