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
// The TEFUnit header file which tests the PhoneIdentity
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONEIDENTITYFU_H
#define CCTSYPHONEIDENTITYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPhoneIdentityFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	CCTsyPhoneIdentityFU();

public:
	// Individual test steps

	void TestGetPhoneId0001L();
	// Test split into 0001b-j to allow for replacing deviceattributes.ini using TEF script in between.
	void TestGetPhoneId0001bL();
	void TestGetPhoneId0001cL();
	void TestGetPhoneId0001dL();
	void TestGetPhoneId0001eL();
	void TestGetPhoneId0001fL();
	void TestGetPhoneId0001gL();
	void TestGetPhoneId0001hL();
	void TestGetPhoneId0001iL();
	void TestGetPhoneId0001jL();
	void TestGetPhoneId0002L();
	void TestGetPhoneId0004L();
	void TestGetPhoneId0004bL();
	void TestGetPhoneId0004cL();
	void TestGetPhoneId0005L();
	void TestGetIdentityCaps0001L();
	void TestGetIdentityServiceStatus0001L();
	void TestGetIdentityServiceStatus0002L();
	void TestGetIdentityServiceStatus0004L();
	void TestGetIdentityServiceStatus0005L();
	void TestSetIdentityServiceStatus0001L();
	void TestNotifyIdentityServiceStatus0001L();
	void TestNotifyIdentitySuppressionRejected0001L();


private:
	TInt DeletePSProperties();


	}; // class CCTsyPhoneIdentityFU

#endif // CCTSYPHONEIDENTITYFU_H

