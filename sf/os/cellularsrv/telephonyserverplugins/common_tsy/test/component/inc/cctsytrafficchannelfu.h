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
// The TEFUnit header file which tests the TrafficChannel
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYTRAFFICCHANNELFU_H
#define CCTSYTRAFFICCHANNELFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyTrafficChannelFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestSetTrafficChannel0001L();
	void TestSetTrafficChannel0006L();
	void TestSetTrafficChannel00011L();
	void TestNotifyTrafficChannelConfirmation0001L();
	void TestNotifyTrafficChannelConfirmation0006L();
	void TestNotifyTrafficChannelConfirmation00011L();


private:

	void TestSetTrafficChannelNotSupportedL(const TDesC& aLineName);
	void TestNotifyTrafficChannelConfirmationNotSupportedL(const TDesC& aLineName);

	}; // class CCTsyTrafficChannelFU

#endif // CCTSYTRAFFICCHANNELFU_H

