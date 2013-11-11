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
// The TEFUnit header file which tests the APNControlList
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYAPNCONTROLLISTFU_H
#define CCTSYAPNCONTROLLISTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyAPNControlListFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetAPNname0001L();
	void TestGetAPNname0002L();
	void TestGetAPNname0003L();
	void TestGetAPNname0004L();
	void TestGetAPNname0005L();
	void TestEnumerateAPNEntries0001L();
	void TestEnumerateAPNEntries0002L();
	void TestEnumerateAPNEntries0003L();
	void TestEnumerateAPNEntries0004L();
	void TestEnumerateAPNEntries0005L();
	void TestAppendAPNName0001L();
	void TestAppendAPNName0002L();
	void TestAppendAPNName0003L();
	void TestAppendAPNName0004L();
	void TestAppendAPNName0005L();
	void TestDeleteAPNName0001L();
	void TestDeleteAPNName0002L();
	void TestDeleteAPNName0003L();
	void TestDeleteAPNName0004L();
	void TestDeleteAPNName0005L();
	void TestNotifyAPNListChanged0001L();
	void TestNotifyAPNListChanged0002L();
	void TestNotifyAPNListChanged0003L();
	void TestNotifyAPNListChanged0004L();
	void TestNotifyAPNListChanged0005L();
	void TestGetAPNControlListServiceStatus0001L();
	void TestGetAPNControlListServiceStatus0002L();
	void TestGetAPNControlListServiceStatus0003L();
	void TestGetAPNControlListServiceStatus0004L();
	void TestGetAPNControlListServiceStatus0005L();
	void TestSetAPNControlListServiceStatus0001L();
	void TestSetAPNControlListServiceStatus0002L();
	void TestSetAPNControlListServiceStatus0003L();
	void TestSetAPNControlListServiceStatus0004L();
	void TestSetAPNControlListServiceStatus0005L();
	void TestNotifyAPNControlListServiceStatusChange0001L();
	void TestNotifyAPNControlListServiceStatusChange0002L();
	void TestNotifyAPNControlListServiceStatusChange0003L();
	void TestNotifyAPNControlListServiceStatusChange0004L();
	void TestNotifyAPNControlListServiceStatusChange0005L();


private:


	}; // class CCTsyAPNControlListFU

#endif // CCTSYAPNCONTROLLISTFU_H

