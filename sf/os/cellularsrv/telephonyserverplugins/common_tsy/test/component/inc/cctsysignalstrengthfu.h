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
// The TEFUnit header file which tests the SignalStrength
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSIGNALSTRENGTHFU_H
#define CCTSYSIGNALSTRENGTHFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySignalStrengthFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetSignalCaps0001L();
	void TestGetSignalStrength0001L();
	void TestGetSignalStrength0004L();
	void TestNotifySignalStrengthChange0001L();
	void TestNotifySignalStrengthChange0002L();
	void TestNotifySignalStrengthChange0004L();


private:


	}; // class CCTsySignalStrengthFU

#endif // CCTSYSIGNALSTRENGTHFU_H

