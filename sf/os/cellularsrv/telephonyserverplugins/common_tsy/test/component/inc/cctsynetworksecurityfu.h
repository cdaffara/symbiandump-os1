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
// The TEFUnit header file which tests the NetworkSecurity
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYNETWORKSECURITYFU_H
#define CCTSYNETWORKSECURITYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyNetworkSecurityFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetNetworkSecurityLevel0001L();
	void TestGetNetworkSecurityLevel0004L();
	void TestGetCipheringIndicatorStatus0001L();
	void TestGetCipheringIndicatorStatus0002L();
	void TestGetCipheringIndicatorStatus0004L();
	void TestNotifyCipheringIndicatorStatus0001L();
	void TestNotifyNetworkSecurityLevelChange0001L();
	void TestNotifyNetworkSecurityLevelChange0002L();
	void TestNotifyNetworkSecurityLevelChange0004L();


private:


	}; // class CCTsyNetworkSecurityFU

#endif // CCTSYNETWORKSECURITYFU_H

