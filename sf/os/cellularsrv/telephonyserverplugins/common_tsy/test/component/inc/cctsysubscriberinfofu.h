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
// The TEFUnit header file which tests the SubscriberInfo
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSUBSCRIBERINFOFU_H
#define CCTSYSUBSCRIBERINFOFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySubscriberInfoFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetServiceTable0001L();
	void TestGetServiceTable0001aL();
	void TestGetServiceTable0001bL();
	void TestGetServiceTable0001cL();
	void TestGetServiceTable0002L();
	void TestGetServiceTable0003L();
	void TestGetServiceTable0004L();
	void TestGetServiceTable0005L();
	void TestGetSubscriberId0001L();
	void TestGetSubscriberId0002L();
	void TestGetSubscriberId0004L();
	void TestGetIccAccessCaps0001L();
	void TestGetCustomerServiceProfile0001L();
	void TestGetCustomerServiceProfile0002L();
    void TestGetCustomerServiceProfile0002bL();	
	void TestGetCustomerServiceProfile0003L();
	void TestGetCustomerServiceProfile0004L();
	void TestGetCustomerServiceProfile0005L();
	void TestNotifyIccAccessCapsChange0001L();
	void TestNotifyIccAccessCapsChange0002L();
	void TestNotifyIccAccessCapsChange0004L();
	void TestGetIccIdentity0001L();


private:

    void OpenPhone();
	void InitAndCacheSimServiceTableL(
	    const RMobilePhone::TMobilePhoneServiceTableV1* aSimServiceTable,
	    const RMobilePhone::TMobilePhoneServiceTableV1* aUSimServiceTable,
	    TInt aSimServiceTableError);

	}; // class CCTsySubscriberInfoFU

#endif // CCTSYSUBSCRIBERINFOFU_H

