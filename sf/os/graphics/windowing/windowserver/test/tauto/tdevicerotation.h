// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Set of tests for Tracing Device Rotation. These tests generally test
// the RWsSession::IndicateAppOrientation(...) API.
//

/**
 @file
 @test
 @internalComponent - Internal Nokia test code
*/

#ifndef __TDEVICERORATION_H__
#define __TDEVICERORATION_H__

#include <e32std.h>
#include <w32std.h>
#include <e32property.h>
#include <test/testexecutestepbase.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"
#include <wspublishandsubscribedata.h>

//
// CTDeviceRotation Definition
//

class CTDeviceRotation: public CTGraphicsBase
	{
public:
    CTDeviceRotation(CTestStep* aStep);
	~CTDeviceRotation();
	/*Sets the windowing environment,*/
	void ConstructL();

protected:
	//from 	CTGraphicsStep - Calls the device rotation test.
	virtual void RunTestCaseL(TInt aCurTestCase);
private: 
	//tool functions
	void SimulateThemeServerOrientation(TRenderOrientation aExpectedaOrientation);
	void CheckHalSetting(TRenderOrientation aOrientation);
	void IsOrientationCorrect(TRenderOrientation aOrientation);
	void TestIndicateAppOrientation(TRenderOrientation aOrientation);
	void TestIndicateAppOrientation(TRenderOrientation aOrientation, TRenderOrientation aSecondOrientation);
	void TestIgnoredAppOrientation(TRenderOrientation aSecondOrientation, TInt aThemeServerOrientation, TInt aExpectedOrientation);
	void TestAppOrientationOnSwap(TRenderOrientation aOrientation, TRenderOrientation aSecondOrientation);
	//For TESTL
	inline void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine) 
	    {
	    iStep->testBooleanTrueL(aCondition, aFile, aLine, ETrue);
	    }
private:
	//Test Cases
	void TestInitialOrientation();
	void TestFixedOrientations();
	void TestAutoOrientation();
	void TestIgnoreOrientation();
	void TestFixedOrientationsOnWindowSwap();
	void TestAutoOrientationOnWindowSwap();
	void TestIgnoreOrientationOnWindowSwap();
	void TestIgnoreAutoOrientationOnWindowSwap();
	void TestInvalidAppOrientation();
	void TestInvalidThemeServerOrientation();
	
private:
	//members
	// First Session and Window Group
	RWsSession iWs;
	RWindowGroup iWindowGroup;
    RWindow iChildWindow;
    // Second Session and Window Group
	RWsSession iSecondWs;
	RWindowGroup iSecondWindowGroup;
	RWindow iSecondChildWindow;
		
	RProperty iRenderOrientationProperty;
	RTimer iPublishTimer;
	TRequestStatus iPublishTimerStatus;
	TRequestStatus iRenderOrientationStatus;
	RProperty iThemeServerOrientationProperty;

	TRenderOrientation iCurrentThemeServerOrientation;
	TBool iWaitForPublishOnNextTest; //Set to EFalse to skip waiting for publish on next test, default ETrue, Resets to ETrue on each Test.
	};

class CTDeviceRotationStep : public CTGraphicsStep
	{
public:
	CTDeviceRotationStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTDeviceRotationStep,"TDeviceRotation");

#endif //TDEVICERORATION
