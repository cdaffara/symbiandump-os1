// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSCRDEV_H__
#define __TSCRDEV_H__

#include <e32std.h>
#include "w32std.h"
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTScreenDevice : public CTWsGraphicsBase
	{
public:
	CTScreenDevice(CTestStep* aStep);
	~CTScreenDevice();
	void ConstructL();
	void TestTwipPixelConversions();
	void TestTwipPixelOverflow();
	void TwipsCacheFlushL();
	void CheckBitmapL(CFbsBitmap *aBitmap, const TRect &aRect);
	void TestScreenToBitmap();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void doTestScreenToBitmapL();
	};

class CTScreenDeviceStep : public CTGraphicsStep
	{
public:
	CTScreenDeviceStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

class CCachePanic : public CBase
	{
protected:
	CCachePanic()
		{
		};
	~CCachePanic();
	virtual void TestL();
	void ConstructL(TInt aScreenNumber, TInt aInt);
	static TInt DoTest(CCachePanic* aTestClass);

protected:
	RWsSession iWs;
	CWsScreenDevice* iWsScrDev;
	TInt iScreenNumber;
	TInt iInt;
	};

class CTwipsPanic : public CCachePanic
	{
public:
	static TInt RunTest(TInt aInt, TAny* aPtr);

private:
	virtual void TestL();
	};

class CScanLinePanic : public CCachePanic
	{
public:
	enum ScanLineTestCases
		{
		TESTCASE_GRAPHICS_WSERV_0473 = 0,
		TESTCASE_GRAPHICS_WSERV_0474
		};
public:
	static TInt RunTest(TInt aInt, TAny* aPtr);

private:
	virtual void TestL();

	// Methods for test cases go here
private:
	void TestLengthInvariant();
	void TestBufferInvariant();

	};

_LIT(KTScreenDeviceStep,"TScreenDevice");

#endif
