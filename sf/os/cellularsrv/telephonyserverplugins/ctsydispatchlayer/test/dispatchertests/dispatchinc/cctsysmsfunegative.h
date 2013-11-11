// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// functional unit of the Common TSY Dispatcher for all situations
// in which a IPC is not supported.
//



/**
 @file The TEFUnit header file which tests the Sms
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYSMSFUNEGATIVE_H__
#define CCTSYSMSFUNEGATIVE_H__

#include "cctsycomponenttestbase.h"

class CCTsySmsFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	void TestNackSmsStoredL();
	void TestAckSmsStoredL();
	void TestSendMessageL();
	void TestGetSmsStoreInfoL();
	void TestGetSmspListL();
	void TestSendMessageNoFdnCheckL();
	void TestResumeSmsReceptionL();

	void TestStoreSmspListEntryL();
	}; // class CCTsySmsFUNegative

#endif // CCTSYSMSFUNEGATIVE_H__

