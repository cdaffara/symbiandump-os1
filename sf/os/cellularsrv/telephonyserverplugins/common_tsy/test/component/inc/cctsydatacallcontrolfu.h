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
// The TEFUnit header file which tests the DataCallControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYDATACALLCONTROLFU_H
#define CCTSYDATACALLCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyDataCallControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetBearerServiceInfo0001L();
	void TestGetBearerServiceInfo0006L();
	void TestGetBearerServiceInfo0011L();
	void TestConnect0001L();
	void TestConnect0006L();
	void TestConnect0007L();
	void TestConnect0008L();
	void TestConnect0009L();
	void TestConnect0011L();
	void TestConnect0012L();
	void TestConnect0013L();
	void TestConnect0014L();
	void TestGetCurrentHscsdInfo0001L();
	void TestGetCurrentHscsdInfo0006L();
	void TestGetCurrentHscsdInfo0008L();
	void TestGetCurrentHscsdInfo0011L();
	void TestNotifyVoiceFallback0001L();
	void TestNotifyVoiceFallback0006L();
	void TestNotifyVoiceFallback00011L();
	void TestGetMobileDataCallCaps0001L();
	void TestGetMobileDataCallCaps0006L();
	void TestGetMobileDataCallCaps0008L();
	void TestGetMobileDataCallCaps00011L();
	void TestNotifyHscsdInfoChange0001L();
	void TestNotifyHscsdInfoChange0006L();
	void TestNotifyHscsdInfoChange0007L();
	void TestNotifyHscsdInfoChange0008L();
	void TestNotifyHscsdInfoChange0009L();
	void TestNotifyHscsdInfoChange00011L();
	void TestSetDynamicHscsdParams0001L();
	void TestSetDynamicHscsdParams0006L();
	void TestSetDynamicHscsdParams0007L();
	void TestSetDynamicHscsdParams0009L();
	void TestSetDynamicHscsdParams00010L();
	void TestSetDynamicHscsdParams00011L();
	void TestGetMobileDataCallRLPRange0001L();
	void TestGetMobileDataCallRLPRange0006L();
	void TestGetMobileDataCallRLPRange00011L();
	void TestNotifyMobileDataCallCapsChange0001L();
	void TestNotifyMobileDataCallCapsChange0006L();
	void TestNotifyMobileDataCallCapsChange0007L();
	void TestNotifyMobileDataCallCapsChange0008L();
	void TestNotifyMobileDataCallCapsChange0009L();
	void TestNotifyMobileDataCallCapsChange00011L();

	}; // class CCTsyDataCallControlFU

#endif // CCTSYDATACALLCONTROLFU_H

