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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the CallControl
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYCALLCONTROLFU_H
#define CCTSYCALLCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	void TestUseCase0004L();
	void TestUseCase0005L();
	void TestUseCase0006L();
	void TestUseCase0007L();
	void TestUseCase0008L();
	void TestUseCase0009L();
	void TestUseCase0010L();
	void TestUseCase0011L();
	void TestUseCase0012L();
	void TestUseCase0013L();
	void TestUseCase0014L();
	void TestUseCase0015L();
	void TestUseCase0016L();
	void TestUseCase0017L();
	void TestUseCase0018L();
	void TestUseCase0019L();
	void TestUseCase0020L();
	void TestUseCase0021L();
	void TestUseCase0022L();
	void TestUseCase0023L();
	void TestUseCase0024L();
	void TestUseCase0025L();

	// Other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	void TestUnit0016L();
	void TestUnit0017L();
	void TestUnit0018L();
	void TestUnit0019L();
	void TestUnit0020L();
	void TestUnit0021L();

private:

	// Client calls
	void ClientWaitForIncomingCallNotificationLC(RMobileCall& aMobileCall, RMobileLine& aMobileLine, TName& aCallName);
	void ClientAnswerIncomingCallL(RMobileCall& aMobileCall, TInt aExpectedAnswerError = KErrNone);
	void ClientSetIsvPubSubKeyL();

	// Driver calls
	void DriverCreateIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode);
	void DriverAnswerIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode, TBool aIsIsvCall = EFalse, TInt aAnswerError = KErrNone);
	void DriverResumeCallL(TInt aCallId, TInt aResumeError);
	void DriverSwapCallL(TInt aCallId, TInt aSwapError, RMobileCall::TMobileCallStatus aNewCallStatus);
	void DriverSwapCallL(TInt aCallId1, TInt aCallId2, TInt aSwapError, RMobileCall::TMobileCallStatus aNewCall1Status, RMobileCall::TMobileCallStatus aNewCall2Status);

	}; // class CCTsyCallControlFU

#endif // CCTSYCALLCONTROLFU_H

