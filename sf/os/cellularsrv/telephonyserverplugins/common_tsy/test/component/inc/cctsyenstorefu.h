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
// The TEFUnit header file which tests the ENStore
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYENSTOREFU_H
#define CCTSYENSTOREFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyENStoreFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyStoreEvent0001L();
	//void TestNotifyStoreEvent0002L();
	//void TestNotifyStoreEvent0003L();
	//void TestNotifyStoreEvent0004L();
	//void TestNotifyStoreEvent0005L();
	void TestRead0001L();
	void TestRead0002L();
	void TestRead0003L();
	void TestRead0004L();
	void TestRead0005L();
	void TestDelete0001L();
	//void TestDelete0002L();
	//void TestDelete0003L();
	//void TestDelete0004L();
	//void TestDelete0005L();
	void TestGetInfo0001L();
	void TestGetInfo0002L();
	void TestGetInfo0003L();
	void TestGetInfo0004L();
	void TestGetInfo0005L();
	void TestDeleteAll0001L();
	//void TestDeleteAll0002L();
	//void TestDeleteAll0003L();
	//void TestDeleteAll0004L();
	//void TestDeleteAll0005L();
	void TestWrite0001L();
	//void TestWrite0002L();
	//void TestWrite0003L();
	//void TestWrite0004L();
	//void TestWrite0005L();
	void TestStart0001L();
	void TestStart0002L();
	//void TestStart0003L();
	void TestStart0004L();
    void TestStart0005L();

private:


	}; // class CCTsyENStoreFU

#endif // CCTSYENSTOREFU_H

