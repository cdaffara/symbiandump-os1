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
 @file The TEFUnit header file which tests the GetChannelStatus
*/

#ifndef CCSATGETCHANNELSTATUSFU_H
#define CCSATGETCHANNELSTATUSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"


class CCSatGetChannelStatusFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyGetChannelStatusPCmd0001L();
	void TestNotifyGetChannelStatusPCmd0002L();
	void TestNotifyGetChannelStatusPCmd0004L();
	
	static void PrepareTlv(TTlv& aTlv); 

private:
	
	void CompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes = KErrNone);	
	void PrepareDispatchL();
	void DoTestTerminalResponseL();
	
	}; // class CCSatGetChannelStatusFU

#endif // CCSATGETCHANNELSTATUSFU_H

