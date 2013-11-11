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
// The TEFUnit header file which tests the UserNetworkAccess
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYUSERNETWORKACCESSFU_H
#define CCTSYUSERNETWORKACCESSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyUserNetworkAccessFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetFeatureCode0001L();
	void TestProgramFeatureCode0001L();
	void TestSendNetworkServiceRequest0001L();
	void TestSendNetworkServiceRequest0002L();
	void TestSendNetworkServiceRequest0003L();
	void TestSendNetworkServiceRequest0004L();
	void TestSendNetworkServiceRequest0005L();
	void TestNotifySendNetworkServiceRequest0001L();
	void TestNotifySendNetworkServiceRequest0002L();
	void TestNotifySendNetworkServiceRequest0003L();
	void TestNotifySendNetworkServiceRequest0004L();
    void TestSendNetworkServiceRequestNoFdnCheck0001L();
    void TestSendNetworkServiceRequestNoFdnCheck0002L();
    void TestSendNetworkServiceRequestNoFdnCheck0003L();
    void TestSendNetworkServiceRequestNoFdnCheck0004L();
    void TestSendNetworkServiceRequestNoFdnCheck0005L();


private:


	}; // class CCTsyUserNetworkAccessFU

#endif // CCTSYUSERNETWORKACCESSFU_H

