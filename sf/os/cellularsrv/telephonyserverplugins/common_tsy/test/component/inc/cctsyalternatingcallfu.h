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
// The TEFUnit header file which tests the AlternatingCall
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYALTERNATINGCALLFU_H
#define CCTSYALTERNATINGCALLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyAlternatingCallFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestSwitchAlternatingCall0001L();
	void TestSwitchAlternatingCall0002L();
	void TestSwitchAlternatingCall0003L();
	void TestNotifyAlternatingCallSwitch0001L();
	void TestNotifyAlternatingCallSwitch0002L();
	void TestNotifyAlternatingCallSwitch0003L();
	void TestGetAlternatingCallCaps0001L();
	void TestGetAlternatingCallMode0001L();
	void TestSetAlternatingCallMode0001L();
	void TestNotifyAlternatingCallCapsChange0001L();
	void TestNotifyAlternatingCallModeChange0001L();


private:

	TInt OpenNewCall(RLine& aLine,
						RMobileCall& aMobileCall,
						const TDesC& aLineName);

	}; // class CCTsyAlternatingCallFU

#endif // CCTSYALTERNATINGCALLFU_H

