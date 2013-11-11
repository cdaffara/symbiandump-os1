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

#ifndef __TSCRDEVRESSWITCH_H__
#define __TSCRDEVRESSWITCH_H__

#include <e32std.h>
#include "w32std.h"
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTScreenDeviceResSwitch : public CTWsGraphicsBase
	{
public:
	CTScreenDeviceResSwitch(CTestStep* aStep);
	~CTScreenDeviceResSwitch();
	void ConstructL();
	void DeviceResSwitchL();
	void DeviceResSwitchWithRotationsL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TInt NumDisplayModeColors(TDisplayMode aDispMode);
	void CalculateDisplayPropertiesL();
	TInt IsTransparencySupportedL();
private:
	TDisplayMode iMaxDisplayMode;
	TInt iMaxDisplayModeColors;
	};

class CTScreenDeviceResSwitchStep : public CTGraphicsStep
	{
public:
	CTScreenDeviceResSwitchStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTScreenDeviceResSwitchStep,"TScreenDeviceResSwitch");

#endif
