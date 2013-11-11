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

#ifndef CCTSYCALLCONTROLMULTIPARTYFU_HNEGATIVE_H__
#define CCTSYCALLCONTROLMULTIPARTYFU_HNEGATIVE_H__

#include "cctsycomponenttestbase.h"

class CCTsyCallControlMultipartyFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	void TestHangUpL();
	void TestAddCallL();
	void TestCreateConferenceL();
	void TestSwapL();
	void TestGoOneToOneL();
	
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
	}; // class CCTsyCallControlMultipartyFUNegative

#endif // CCTSYCALLCONTROLMULTIPARTYFU_HNEGATIVE_H__

