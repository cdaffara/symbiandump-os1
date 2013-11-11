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
// Advanced pointer event unit tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tadvancedpointerevent.h"

CTAdvancedPointerEvent::CTAdvancedPointerEvent(CTestStep* aStep)
	: CTWsGraphicsBase(aStep)
	{}

CTAdvancedPointerEvent::~CTAdvancedPointerEvent()
	{}

void CTAdvancedPointerEvent::TestPointerEventInit()
	{
	// Test size of class with hard coded values to prevent regression failure 
	TEST(sizeof(TPointerEvent) == 24);
	
	TPointerEvent ptrEvent1 = TPointerEvent();
	ptrEvent1.iType = TPointerEvent::EButton1Down;
	TPoint initCoords(10,10);
	ptrEvent1.iPosition = initCoords;
	ptrEvent1.iParentPosition = initCoords;
	ptrEvent1.iModifiers = 0;
	
	// Construct two TPointerEvent objects. The first using default constructor, the second using the copy constructor. T
		{		
		TPointerEvent ptrEvent2 = TPointerEvent(ptrEvent1);
		TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&ptrEvent1),sizeof(TPointerEvent),reinterpret_cast<TUint8*>(&ptrEvent2),sizeof(TPointerEvent)));		
		}	
	
		{		
		TPointerEvent ptrEvent2(TPointerEvent::EButton1Down, 0, initCoords, initCoords);
		TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&ptrEvent1),sizeof(TPointerEvent),reinterpret_cast<TUint8*>(&ptrEvent2),sizeof(TPointerEvent)));		
		}	
	}

void AdvancedPointerInit(TWsEvent& aEvent1, TWsEvent& aEvent2, const TPointerEvent::TType aType, const TPoint3D& aPoint3D, const TUint8 aPointerNumber)
	{
	// init first event
	TAdvancedPointerEvent& ptrEvent = *aEvent1.Pointer();
	ptrEvent.iType = aType;
	ptrEvent.iModifiers = EModifierAdvancedPointerEvent; // done implicitly by TAdvancedPointerEvent::Init method
	ptrEvent.iPosition.iX = aPoint3D.iX;
	ptrEvent.iPosition.iY = aPoint3D.iY;
	aEvent1.SetPointerNumber(aPointerNumber);
	aEvent1.SetPointerZ(aPoint3D.iZ);
	
	// init 2nd event
	aEvent2.InitAdvancedPointerEvent(aType,0,aPoint3D,aPointerNumber);
	}

void CTAdvancedPointerEvent::TestAdvancedPointerInit()
	{
	// Test size of class with hard coded values to prevent regression failure 
	TEST(sizeof(TAdvancedPointerEvent) == 32);

	TWsEvent event1;
	TWsEvent event2;
	
	// low value coords, lowest value event type
		{
		TPoint3D point3D(1,2,3);	
		AdvancedPointerInit(event1, event2, TPointerEvent::EButton1Down, point3D, 1);
		}
	TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&event1),sizeof(TWsEvent),reinterpret_cast<TUint8*>(&event2),sizeof(TWsEvent)));
	
	// high value coords, highest value event type and pointer number
		{
		TPoint3D point3D(KMaxTInt, KMaxTInt-1, KMaxTInt-3);
		AdvancedPointerInit(event1, event2, TPointerEvent::EOutOfRange, point3D, KMaxTUint8);
		}
	TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&event1),sizeof(TWsEvent),reinterpret_cast<TUint8*>(&event2),sizeof(TWsEvent)));
	
	// high negative value coords, highest value event type and pointer number
		{
		TPoint3D point3D(KMinTInt, KMinTInt+1, KMinTInt+3);
		AdvancedPointerInit(event1, event2, TPointerEvent::EOutOfRange, point3D, KMaxTUint8);
		}
	TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&event1),sizeof(TWsEvent),reinterpret_cast<TUint8*>(&event2),sizeof(TWsEvent)));		
	}

void CTAdvancedPointerEvent::TestAdvancedPointerGetPointerId()
	{
	TWsEvent event;
	TAdvancedPointerEvent& ptrEvent = *event.Pointer();
	const TPoint3D point3D(1,2,3);
	const TUint8 MaxLikelyCommonPointerNumber = 10;
	
	// Likely Common Values
	TUint8 pointerNumber = 0;
	for(;pointerNumber<=MaxLikelyCommonPointerNumber;pointerNumber++)
		{
		event.InitAdvancedPointerEvent(TPointerEvent::EMove,0,point3D,pointerNumber);
		TEST(ptrEvent.PointerNumber()==pointerNumber);
		}

	// mid-range value
	pointerNumber = 127;	
	event.InitAdvancedPointerEvent(TPointerEvent::EMove,0,point3D,pointerNumber);
	TEST(ptrEvent.PointerNumber()==pointerNumber);	
	
	// highest value
	pointerNumber = 255; // KMaxTUint8 is there was one defined.
	event.InitAdvancedPointerEvent(TPointerEvent::EMove,0,point3D,pointerNumber);
	TEST(ptrEvent.PointerNumber()==pointerNumber);	
	}

/** helper method for TestAdvancedPointerGetPressureAndProximity() */
void CTAdvancedPointerEvent::TestAdvancedPointerGetPressureAndProximity(const TPoint3D aPoint3D)
	{
	TWsEvent event;
	event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, aPoint3D, 0);
	TAdvancedPointerEvent ptrEvent = *event.Pointer();
	const TPoint3D positionAndPressure3D = ptrEvent.PositionAndPressure3D();
	TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&aPoint3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&positionAndPressure3D),sizeof(TPoint3D)));
	TInt proximityAndPressure = ptrEvent.ProximityAndPressure();
	TEST(aPoint3D.iZ==proximityAndPressure);	
	}

void CTAdvancedPointerEvent::TestAdvancedPointerGetPressureAndProximity()
	{
		{
		// middle value
		TPoint3D point3D(0,0,0);
		TestAdvancedPointerGetPressureAndProximity(point3D);
		}
		{
		// Typical value
		TPoint3D point3D(10,9,8);
		TestAdvancedPointerGetPressureAndProximity(point3D);
		}
		{
		// Typical value
		TPoint3D point3D(10,9,-8);
		TestAdvancedPointerGetPressureAndProximity(point3D);
		}
		{
		// max values
		TPoint3D point3D(KMaxTInt-2,KMaxTInt-1,KMaxTInt);
		TestAdvancedPointerGetPressureAndProximity(point3D);
		}
		{
		// min values
		TPoint3D point3D(KMinTInt+2,KMinTInt+1,KMinTInt);
		TestAdvancedPointerGetPressureAndProximity(point3D);
		}
	}

void CTAdvancedPointerEvent::TestAdvancedPointerGetPressure()
	{
		{
		// middle value
		const TPoint3D point3D(0,0,0);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D pressure3D = ptrEvent.Pressure3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&pressure3D),sizeof(TPoint3D)));
		TInt pressure = ptrEvent.Pressure();
		TEST(point3D.iZ==pressure);
		}
		{
		// Typical value
		const TPoint3D point3D(10,9,8);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D pressure3D = ptrEvent.Pressure3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&pressure3D),sizeof(TPoint3D)));
		TInt pressure = ptrEvent.Pressure();
		TEST(point3D.iZ==pressure);
		}		
		{
		// max value
		const TPoint3D point3D(KMaxTInt-2,KMaxTInt-1,KMaxTInt);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D pressure3D = ptrEvent.Pressure3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&pressure3D),sizeof(TPoint3D)));
		TInt pressure = ptrEvent.Pressure();
		TEST(point3D.iZ==pressure);
		}
		{
		// min value
		TPoint3D point3D(KMinTInt+2,KMinTInt+1,KMinTInt);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D pressure3D = ptrEvent.Pressure3D();
		point3D.iZ=0; // pressure can't be negative
		TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&pressure3D),sizeof(TPoint3D)));
		TInt pressure = ptrEvent.Pressure();
		TEST(0==pressure);
		}
	}

void CTAdvancedPointerEvent::TestAdvancedPointerGetPosition()
	{
		{
		// middle value
		const TPoint3D point3D(0,0,0);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D position3D = ptrEvent.Position3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&position3D),sizeof(TPoint3D)));
		TInt proximity = ptrEvent.Proximity();
		TEST(point3D.iZ==proximity);
		}
		{
		// typical value
		const TPoint3D point3D(10,9,-8);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D position3D = ptrEvent.Position3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&position3D),sizeof(TPoint3D)));
		TInt proximity = ptrEvent.Proximity();
		TEST(point3D.iZ==proximity);
		}
		{
		// highest value
		TPoint3D point3D(KMaxTInt-2,KMaxTInt-1,KMaxTInt);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D position3D = ptrEvent.Position3D();
		point3D.iZ=0; // proximity can't be positive
		TEST(0==Mem::Compare(reinterpret_cast<TUint8*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&position3D),sizeof(TPoint3D)));
		TInt proximity = ptrEvent.Proximity();
		TEST(0==proximity);
		}
		{
		// lowest value
		const TPoint3D point3D(KMinTInt+2,KMinTInt+1,KMinTInt);
		TWsEvent event;
		event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, point3D, 0);
		TAdvancedPointerEvent ptrEvent = *event.Pointer();
		const TPoint3D position3D = ptrEvent.Position3D();
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&point3D),sizeof(TPoint3D),reinterpret_cast<TUint8 const*>(&position3D),sizeof(TPoint3D)));
		TInt proximity = ptrEvent.Proximity();
		TEST(point3D.iZ==proximity);
		}
	}

void CTAdvancedPointerEvent::TestPointerEventCopying()
	{
	TWsEvent event;
	event.InitAdvancedPointerEvent(TPointerEvent::EMove, 0, TPoint3D(2,3,4),1);
	TPointerEvent testPointerEvent;
	testPointerEvent.iModifiers =0;
	testPointerEvent.iParentPosition=TPoint(0,0);
	testPointerEvent.iPosition = TPoint(2,3);
	testPointerEvent.iType=TPointerEvent::EMove;

	// TPointerEvent to TPointerEvent
	// Copy Constructor
		{
		TPointerEvent* pointerEventPtr = event.Pointer(); 
		TPointerEvent pointerEvent = *pointerEventPtr;
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}
	// Assignment operator
		{
		TPointerEvent* pointerEventPtr = event.Pointer(); 
		TPointerEvent pointerEvent;
		pointerEvent = *pointerEventPtr;
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}		
	// TAdvancedPointerEvent to TPointerEvent
	// Copy Constructor
		{
		TAdvancedPointerEvent* advancedPointerEventPtr = event.Pointer();		
		TPointerEvent pointerEvent = *advancedPointerEventPtr;
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}
	// Assignment operator
		{
		TAdvancedPointerEvent* advancedPointerEventPtr = event.Pointer(); 
		TPointerEvent pointerEvent;
		pointerEvent = *advancedPointerEventPtr;
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}

	// TPointerEvent to TPointerEvent
	// Copy Method
		{
		TPointerEvent* pointerEventPtr = event.Pointer(); 
		TPointerEvent pointerEvent;
		pointerEvent.Copy(*pointerEventPtr);
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}
		
	// TAdvancedPointerEvent to TPointerEvent
	// Copy Constructor
		{
		TAdvancedPointerEvent* advancedPointerEventPtr = event.Pointer();		
		TPointerEvent pointerEvent;
		pointerEvent.Copy(*advancedPointerEventPtr);
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&pointerEvent),sizeof(TPointerEvent),reinterpret_cast<TUint8 const*>(&testPointerEvent),sizeof(TPointerEvent)));
		}
		
	// Assignment operator
		{
		TAdvancedPointerEvent* advancedPointerEventPtr = event.Pointer(); 
		TAdvancedPointerEvent advancedPointerEventPtr2;
		advancedPointerEventPtr2 = *advancedPointerEventPtr;
		TEST(0==Mem::Compare(reinterpret_cast<TUint8 const*>(&advancedPointerEventPtr2),sizeof(TAdvancedPointerEvent),reinterpret_cast<TUint8 const*>(event.Pointer()),sizeof(TAdvancedPointerEvent)));
		}			
	}

void CTAdvancedPointerEvent::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"PointerEventInit");
	_LIT(KTest2,"AdvancedPointerEventInit");
	_LIT(KTest3,"PointerEventCopying");
	_LIT(KTest4,"AdvancedPointerEventGetPointerId");
	_LIT(KTest5,"AdvancedPointerEventGetPressureAndProximity");
	_LIT(KTest6,"AdvancedPointerEventGetPressure");
	_LIT(KTest7,"AdvancedPointerEventGetPosition");
	
	((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
	case 1:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0542
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0542"));
		iTest->LogSubTest(KTest1);
		TestPointerEventInit();
		break;	
	case 2:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0543
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0543"));
		iTest->LogSubTest(KTest2);
		TestAdvancedPointerInit();
		break;
	case 3:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0544
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0544"));
		iTest->LogSubTest(KTest3);			
		TestPointerEventCopying();
		break;
	case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0545
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0545"));
		iTest->LogSubTest(KTest4);
		TestAdvancedPointerGetPointerId();
		break;		
	case 5:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0546
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0546"));
		iTest->LogSubTest(KTest5);
		TestAdvancedPointerGetPressureAndProximity();
		break;
	case 6:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0547
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0547"));
		iTest->LogSubTest(KTest6);
		TestAdvancedPointerGetPressure();
		break;
	case 7:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0548
*/
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0548"));
		iTest->LogSubTest(KTest7);			
		TestAdvancedPointerGetPosition();
		break;
	default:
		((CTAdvancedPointerEventStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAdvancedPointerEventStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTAdvancedPointerEventStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(AdvancedPointerEvent)
