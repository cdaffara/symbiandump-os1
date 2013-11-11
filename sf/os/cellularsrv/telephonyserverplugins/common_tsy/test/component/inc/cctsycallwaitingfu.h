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
// The TEFUnit header file which tests the CallWaiting
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLWAITINGFU_H
#define CCTSYCALLWAITINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallWaitingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStart0001L();
	void TestStart0002L();
	void TestStart0004L();
	void TestStart0005L();
	void TestSetCallWaitingStatus0001L();
	void TestSetCallWaitingStatus0002L();
	void TestSetCallWaitingStatus0003L();
	void TestSetCallWaitingStatus0004L();
	void TestSetCallWaitingStatus0005L();
	void TestNotifyCallWaitingStatusChange0001L();
	void TestNotifyCallWaitingStatusChange0002L();
	void TestNotifyCallWaitingStatusChange0003L();
	void TestNotifyCallWaitingStatusChange0004L();


private:

	void AuxTestStart0001L(RMobilePhone::TMobileInfoLocation aLocation);

	void AuxTestSetCallWaitingStatus0001L(
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobilePhoneServiceAction aAction);

	void AuxTestSetCallWaitingStatus0002L(
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobilePhoneServiceAction aAction);

	void AuxTestSetCallWaitingStatus0003L(
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobilePhoneServiceAction aAction);
	
	void GetNextUnsupportedServiceAction(RMobilePhone::TMobilePhoneServiceAction &aAction);

	void FillInCWInfoEntry(
			RMobilePhone::TMobilePhoneCWInfoEntryV1 &aCWInfoEntry,
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobilePhoneServiceAction aAction);

	}; // class CCTsyCallWaitingFU

#endif // CCTSYCALLWAITINGFU_H

