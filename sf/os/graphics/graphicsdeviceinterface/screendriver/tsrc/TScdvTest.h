// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TSCDVTEST_H__)
#define __TSCDVTEST_H__

#include "TGraphicsHarness.h"

class CTScdv : public CTGraphicsBase
	{
public:
	CTScdv(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	inline TInt ByteSize(const TSize& aSize, TDisplayMode aDisplayMode);
	TBool CreateScreenDeviceLC(TInt aScreenNo, 
								  TDisplayMode aDisplayMode,
								  CFbsDrawDevice*& aScreenDevice);
	TAny* InitScreenDeviceLC(CFbsDrawDevice* aScreenDevice,
							   TDisplayMode aDisplayMode);
	void TestScreenDeviceCreationL();
	void TestBitmapDeviceCreationL();
	};

class CTScdvStep : public CTGraphicsStep
	{
public:
	CTScdvStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	};

_LIT(KTScdvStep,"TScdv");


#endif

