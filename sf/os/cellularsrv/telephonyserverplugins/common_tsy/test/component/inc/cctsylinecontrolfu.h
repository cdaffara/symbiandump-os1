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
// The TEFUnit header file which tests the LineControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYLINECONTROLFU_H
#define CCTSYLINECONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyLineControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetHookStatus0001L();
	void TestGetHookStatus0002L();
	void TestGetHookStatus0003L();

	void TestGetCaps0001L();
	void TestGetCaps0006L();
	void TestGetCaps00011L();

	void TestGetInfo0001L();
	void TestGetInfo0006L();
	void TestGetInfo00011L();

	void TestGetStatus0001L();
	void TestGetStatus0006L();
	void TestGetStatus00011L();

	void TestGetLineInfo0001L();
	void TestGetLineInfo0003L();

	void TestEnumerateLines0001L();

	void TestNotifyCapsChange0002L();
	void TestNotifyCapsChange0007L();
	void TestNotifyCapsChange00012L();

	void TestNotifyStatusChange0001L();
	void TestNotifyStatusChange0001aL();
	void TestNotifyStatusChange0002L();
	void TestNotifyStatusChange0004L();
	void TestNotifyStatusChange0006L();
	void TestNotifyStatusChange0007L();
	void TestNotifyStatusChange0009L();
	void TestNotifyStatusChange00011L();
	void TestNotifyStatusChange00012L();
	void TestNotifyStatusChange00013L();
	void TestNotifyStatusChange00014L();

	void TestGetMobileLineStatus0001L();
	void TestGetMobileLineStatus0006L();
	void TestGetMobileLineStatus00011L();

	void TestNotifyMobileLineStatusChange0001L();
	void TestNotifyMobileLineStatusChange0002L();
	void TestNotifyMobileLineStatusChange0004L();
	void TestNotifyMobileLineStatusChange0006L();
	void TestNotifyMobileLineStatusChange0007L();
	void TestNotifyMobileLineStatusChange0009L();
	void TestNotifyMobileLineStatusChange00011L();
	void TestNotifyMobileLineStatusChange00012L();
	void TestNotifyMobileLineStatusChange00014L();

private:

	}; // class CCTsyLineControlFU

#endif // CCTSYLINECONTROLFU_H

