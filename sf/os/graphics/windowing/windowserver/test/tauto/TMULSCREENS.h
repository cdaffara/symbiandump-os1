// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Enable Multiple Displays
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TMULSCREENS_H__
#define __TMULSCREENS_H__

#include <e32err.h>
#include <w32std.h>
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTMulScreens : public CTWsGraphicsBase
	{
public:
	CTMulScreens(CTestStep* aStep);
	~CTMulScreens();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void ConstructL();
	void TestCreateGroupWindowsL();
	void TestCreateBlankWindowsL();
	void TestGetFocusWindow();
	void TestGetDefaultOwningWindow();
	void TestSetBackgroundColour();
	void TestSetShadowVector();
	void TestPanicsL();
	static TInt TestInvalidScreenNumberL(TInt aOption,TAny* aScreenNumber);
	void TestInitaliseScreenDeviceL();
	void TestScreenNumbersOfWindowsL();
	void TestDeviceL();
private:
	RWsSession iRws;
	CWsScreenDevice* iFstScreenDevice;
	CWsScreenDevice* iSndScreenDevice;
	RWindowGroup iFstWinGp;
	RWindowGroup iSndWinGp;
	TInt iNumScreens;
	};

class CTMulScreensStep : public CTGraphicsStep
	{
public:
	CTMulScreensStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMulScreensStep,"TMulScreens");

#endif
