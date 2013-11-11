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
// The TEFUnit header file which tests the CallEmergency
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLEMERGENCYFU_H
#define CCTSYCALLEMERGENCYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

const TUid KThisProcessUID = {0x101F777C}; 
//UID of the test suite process, used to publish the properties that control the SSM Stub

class CCTsyCallEmergencyFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestDialEmergencyCall0001L();
	void TestDialEmergencyCall0002L();
	void TestDialEmergencyCall0003L();
	void TestDialEmergencyCall0004L();
	void TestDialEmergencyCall0005L();
	void TestDialEmergencyCall0001bL();
	void TestDialEmergencyCall0001cL();
	
	// test for differen answer from SSMPlugin
	void TestDialEmergencyCall0001dL();
	void TestDialEmergencyCall0001eL();
	
	//test for dialling an emergency call 
	//when the modem boot sequence has not run
	void TestDialEmergencyCall0001fL();


private:

	void FillInEmergencyCallInfo(RMobileCall::TMobileCallInfoV1 &aInfo,
			const TInt aCallId,
			const TName& aCallName,
			const RMobilePhone::TMobileService aService);

	}; // class CCTsyCallEmergencyFU

#endif // CCTSYCALLEMERGENCYFU_H

