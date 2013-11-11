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
// The TEFUnit header file which tests the CallForwarding
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLFORWARDINGFU_H
#define CCTSYCALLFORWARDINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallForwardingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStart0001L();
	void TestStart0002L();
	void TestStart0003L();
	void TestStart0004L();
	void TestStart0005L();
	void TestSetCallForwardingStatus0001L();
	void TestSetCallForwardingStatus0002L();
	void TestSetCallForwardingStatus0003L();
	void TestSetCallForwardingStatus0004L();
	void TestSetCallForwardingStatus0005L();
	void TestGetCallForwardingIndicator0001L();
	void TestNotifyCallForwardingActive0001L();
	void TestNotifyCallForwardingActive0002L();
	void TestNotifyCallForwardingActive0004L();
	void TestNotifyCallForwardingStatusChange0001L();
	void TestNotifyCallForwardingStatusChange0002L();
	void TestNotifyCallForwardingStatusChange0004L();


private:

	void AuxTestStart0001L(
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobileInfoLocation aLocation);

	}; // class CCTsyCallForwardingFU

#endif // CCTSYCALLFORWARDINGFU_H

