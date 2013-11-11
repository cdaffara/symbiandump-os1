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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the SetUpMenu
*/

#ifndef CCSATSETUPMENUFU_H
#define CCSATSETUPMENUFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSetUpMenuFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySetUpMenuPCmd0001L();
	void TestNotifySetUpMenuPCmd0002L();
	void TestNotifySetUpMenuPCmd0004L();
	void TestNotifySetUpMenuPCmd0001bL();
	void TestNotifySetUpMenuPCmd0001cL();
	void TestNotifySetUpMenuPCmd0001dL();
	void TestNotifySetUpMenuPCmd0001eL();
	void TestNotifySetUpMenuPCmd0001fL();
	void TestNotifySetUpMenuPCmd0001gL();
	void TestNotifySetUpMenuPCmd0001hL();
	void TestNotifySetUpMenuPCmd0001iL();
	void TestNotifySetUpMenuPCmd0001jL();

private:

	void EndProactiveSimSessionL();	
	
	void SetUpMenuL(TUint8 aCmdNum, TUint8 aCmdQual, const TDesC8& aAlpha, 
	  TUint8 aNumItems = 0, TUint8 aItemIds[] = NULL, TPtrC8 aItemNames[] = NULL, 
	  TUint8 aAction[] = NULL,
      RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet, TUint8 aIcon = 0,
      RSat::TIconQualifier aIconListQual = RSat::EIconQualifierNotSet, TUint8 aIconList[] = NULL,
	  TUint8 aAlphaFormating[] = NULL, 
	  TUint8 aItemFormating[][RSat::KTextAttributeBufSize] = NULL);
	
	void SelectMenuItemL(TUint8 aItemId, TBool aRequestHelp = EFalse, TInt aErrorForExpect = KErrNone);
	
	void CheckMenuAlignmentL(TUint8 aAlign);

	void CheckMenuFontL(TUint8 aFontParam);	
		
	void CheckForeignSetUpMenuL(const TDesC8& aTitle, TPtrC8 aItemNames[], TPtrC8 aItemNames2[]);
	
	}; // class CCSatSetUpMenuFU

#endif // CCSATSETUPMENUFU_H

