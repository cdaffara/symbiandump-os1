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
// The TEFUnit header file which tests the FixedDialling
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYFIXEDDIALLINGFU_H
#define CCTSYFIXEDDIALLINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyFixedDiallingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestDialNoFdnCheck0001L();
	void TestDialNoFdnCheck0001bL();
	void TestDialNoFdnCheck0001cL();
	void TestDialNoFdnCheck0001dL();
	void TestDialNoFdnCheck0002L();
	void TestDialNoFdnCheck0004L();
	void TestDialNoFdnCheck0005L();
	void TestDialNoFdnCheck0006L();
	void TestDialNoFdnCheck0006bL();
	void TestDialNoFdnCheck0007L();
	void TestDialNoFdnCheck0008L();
	void TestDialNoFdnCheck0009L();
	void TestDialNoFdnCheck00010L();
	void TestDialNoFdnCheck00011L();
	void TestDialNoFdnCheck00012L();
	void TestDialNoFdnCheck00013L();
	void TestDialNoFdnCheck00014L();
	void TestDialNoFdnCheck00015L();
	void TestGetFdnStatus0001L();
	void TestGetFdnStatus0002L();
	//void TestGetFdnStatus0003L();
	void TestGetFdnStatus0004L();
	void TestGetFdnStatus0005L();
	void TestSetFdnSetting0001L();
	void TestSetFdnSetting0002L();
	//void TestSetFdnSetting0003L();
	void TestSetFdnSetting0004L();
	void TestSetFdnSetting0005L();
	void TestNotifyFdnStatusChange0001L();
	void TestNotifyFdnStatusChange0002L();
	//void TestNotifyFdnStatusChange0003L();
	void TestNotifyFdnStatusChange0004L();
	//void TestNotifyFdnStatusChange0005L();


private:


	}; // class CCTsyFixedDiallingFU

#endif // CCTSYFIXEDDIALLINGFU_H

