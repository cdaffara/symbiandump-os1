
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
// tadvancedpionter.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef tadvancedpointerevent_h
#define tadvancedpointerevent_h

#include "w32std.h"
#include "AUTO.H"
#include "../tlib/testbase.h"
#include "TGraphicsHarness.h"

class CTAdvancedPointerEvent : public CTWsGraphicsBase
	{
public:
	CTAdvancedPointerEvent(CTestStep* aStep);
	virtual ~CTAdvancedPointerEvent();
protected:
	//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestPointerEventInit();
	void TestAdvancedPointerInit();
	void TestAdvancedPointerGetPointerId();	
	void TestAdvancedPointerGetPressureAndProximity();
	void TestAdvancedPointerGetPressureAndProximity(const TPoint3D aPoint3D);
	void TestAdvancedPointerGetPressure();
	void TestAdvancedPointerGetPosition();	
	void TestPointerEventCopying();
	};

class CTAdvancedPointerEventStep : public CTGraphicsStep
	{
public:
	CTAdvancedPointerEventStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTAdvancedPointerEventStep,"TAdvancedPointerEvent");

#endif // tadvancedpointerevent_h
