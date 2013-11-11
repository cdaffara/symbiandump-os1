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
// The TEFUnit header file which tests the MessageWaiting
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYMESSAGEWAITINGFU_H
#define CCTSYMESSAGEWAITINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyMessageWaitingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyMessageWaiting0001L();
	void TestGetIccMessageWaitingIndicators0001L();
	void TestGetIccMessageWaitingIndicators0002L();
	void TestGetIccMessageWaitingIndicators0003L();
	void TestGetIccMessageWaitingIndicators0004L();
	void TestGetIccMessageWaitingIndicators0005L();
	void TestGetIccMessageWaitingIndicators0001aL();
	void TestSetIccMessageWaitingIndicators0001L();
	void TestSetIccMessageWaitingIndicators0002L();
	void TestSetIccMessageWaitingIndicators0003L();
	void TestSetIccMessageWaitingIndicators0004L();
	void TestSetIccMessageWaitingIndicators0005L();
	void TestNotifyIccMessageWaitingIndicatorsChange0001L();
	void TestNotifyIccMessageWaitingIndicatorsChange0002L();
	void TestNotifyIccMessageWaitingIndicatorsChange0003L();
	void TestNotifyIccMessageWaitingIndicatorsChange0004L();


private:


	}; // class CCTsyMessageWaitingFU

#endif // CCTSYMESSAGEWAITINGFU_H

