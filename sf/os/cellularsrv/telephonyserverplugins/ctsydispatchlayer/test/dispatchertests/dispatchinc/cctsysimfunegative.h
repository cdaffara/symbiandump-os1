// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalAll 
*/

#ifndef CCTSYSIMFUNEGATIVE_H_
#define CCTSYSIMFUNEGATIVE_H_

#include "cctsycomponenttestbase.h"

class CCTsySimFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	void TestGetServiceTableL();
	void TestSetIccMessageWaitingIndicatorsL();
	void TestGetIccMessageWaitingIndicatorsL();
	void TestGetAPNControlListServiceStatusL();
	void TestSetApnControlListServiceStatusL();
	void TestEnumerateAPNEntriesL();
	void TestGetAPNnameL();
	void TestDeleteAPNNameL();
	void TestAppendAPNNameL();
	void TestChangeSecurityCodeL();
	void TestGetSubscriberIdL();

	void TestPowerSimOffL();
	void TestSendAPDUReqL();
	
	void TestSimWarmResetL();

	void TestReadSimFileL();
	void TestActivateSimLockL();
	void TestDeActivateSimLockL();
	void TestGetAnswerToResetL();
	void TestGetSimCardReaderStatusL();
	void TestGetSimAuthenticationEapSimDataL();
	void TestGetSimAuthenticationEapAkaDataL();

	}; // class CCTsySimFUNegative



#endif /* CCTSYSIMFUNEGATIVE_H_ */
