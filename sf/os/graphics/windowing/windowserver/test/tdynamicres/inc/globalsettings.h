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
// Test class that allows a script to set global values 
// 
//

#ifndef __GLOBALSETTINGS_H__
#define __GLOBALSETTINGS_H__

#include <test/tefunit.h>
#include <w32std.h>
#include <pixelformats.h>
class CTestExecuteLogger;

class TGlobalSettings
	{
public:
	static const TGlobalSettings& Instance();
	
public:
	TInt			iScreen;
	TBool			iDisconnected;  //This value might be changed from CWsDynamicResWinBase::SetupL
	};								//once the screen has been connected.

class CGlobalSettings : public CTestFixture 
	{
public:
	static const TGlobalSettings& Instance();

	// SetUp and TearDown code 

	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
	//these tests are public to allow the old CScreenSelect to get to them!
	void SetScreenNoL(TInt aScreenNo);
	void ExpectDisconnectedScreenL(TBool aValue);
	};


#endif	//__GLOBALSETTINGS_H__
