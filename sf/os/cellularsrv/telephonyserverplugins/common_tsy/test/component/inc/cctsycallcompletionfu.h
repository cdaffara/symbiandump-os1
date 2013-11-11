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
// The TEFUnit header file which tests the CallCompletion
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLCOMPLETIONFU_H
#define CCTSYCALLCOMPLETIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallCompletionFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStart0001L();
	void TestRejectCCBS0001L();
	void TestRejectCCBS0002L();
	void TestRejectCCBS0003L();
	void TestActivateCCBS0001L();
	void TestActivateCCBS0002L();
	void TestActivateCCBS0003L();
	void TestGetCCBSStatus0001L();
	void TestDeactivateCCBS0001L();
	void TestAcceptCCBSRecall0001L();
	void TestNotifyCCBSRecall0001L();
	void TestRefuseCCBSRecall0001L();
	void TestNotifyCCBSStatusChange0001L();


private:


	}; // class CCTsyCallCompletionFU

#endif // CCTSYCALLCOMPLETIONFU_H

