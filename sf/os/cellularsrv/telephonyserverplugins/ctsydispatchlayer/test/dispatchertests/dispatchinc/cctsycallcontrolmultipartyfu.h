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
 @file The TEFUnit header file which tests the CallControlMultiparty
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYCALLCONTROLMULTIPARTYFU_H
#define CCTSYCALLCONTROLMULTIPARTYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallControlMultipartyFU : public CCtsyComponentTestBase
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

	//other unit tests
	void TestUnit0001L();

private:

	// Driver side helper functions
	void DriverCreateConferenceCallL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2);
	void DriverCloseTwoRemotePartyConferenceAndCallSubsessionsL(TInt aCallId1, TInt aCallId2, const RArray<TInt>& aCallIds);
	void DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL(TInt aCallId1, TInt aCallId2, TInt aCallId3, const RArray<TInt>& aCallIds);
	void DriverCreateConferenceCallAndHoldItL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2);
	void DriverCreateConferenceWithThreeRemotePartiesL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2, TInt aCallId3);

	// Client side helper functions
	void ClientCreateConferenceCallL(RMobileConferenceCall& aConfCall);
	void ClientSwapConferenceCallL(RMobileConferenceCall& aConfCall, RMobileConferenceCall::TMobileConferenceEvent aExpectedConfEvent, RMobileConferenceCall::TMobileConferenceStatus aExpectedConfStatus);
	void ClientCreateConferenceCallAndHoldItL(RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall, RMobileLine& aMobileLine, RMobileCall& aMobileCall2, RMobileLine& mobileLine2, RMobileConferenceCall& aConfCall);
	void ClientCreateConferenceCallWithThreeRemotePartiesL(RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall, RMobileLine& aMobileLine, RMobileCall& aMobileCall2, RMobileLine& aMobileLine2, RMobileCall& aMobileCall3, RMobileLine& aMobileLine3, RMobileConferenceCall& aConfCall);

	}; // class CCTsyCallControlMultipartyFU

#endif // CCTSYCALLCONTROLMULTIPARTYFU_H

