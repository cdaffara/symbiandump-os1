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
// The TEFUnit header file which tests the MailboxNumbers
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYMAILBOXNUMBERSFU_H
#define CCTSYMAILBOXNUMBERSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyMailboxNumbersFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

    void TestGetMailboxNumbers0001L();
    void TestGetMailboxNumbers0001bL();
    void TestGetMailboxNumbers0001cL();
    void TestGetMailboxNumbers0001dL();
	void TestGetMailboxNumbers0002L();
	void TestGetMailboxNumbers0003L();
	void TestGetMailboxNumbers0004L();
	void TestGetMailboxNumbers0005L();
	void TestNotifyMailboxNumbersChange0001L();


private:


	}; // class CCTsyMailboxNumbersFU

#endif // CCTSYMAILBOXNUMBERSFU_H

