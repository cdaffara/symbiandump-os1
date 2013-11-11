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
// The TEFUnit header file which tests the ConferenceCallControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCONFERENCECALLCONTROLFU_H
#define CCTSYCONFERENCECALLCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

#include <ctsy/mmtsy_names.h>

class CCTsyConferenceCallControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGoOneToOne0001L();
	void TestGoOneToOne0002L();
	void TestGoOneToOne0004L();
	void TestGoOneToOne0005L();
	void TestGoOneToOne0006L();
	void TestGoOneToOne00011L();
	void TestEnumerateCalls0001L();
	void TestCreateConference0001L();
	void TestCreateConference0002L();
	void TestCreateConference0004L();
	void TestCreateConference0005L();
	void TestBreakDownConference0001aL();
	void TestBreakDownConference0001bL();
	void TestBreakDownConference0001cL();
	void TestGhostConference0001L();
	void TestNotifyCapsChange0001L();
	void TestNotifyCapsChange0002L();
	void TestNotifyCapsChange0004L();
	void TestGetConferenceStatus0001L();
	void TestNotifyConferenceEvent0001L();
	void TestNotifyConferenceEvent0002L();
	void TestNotifyConferenceEvent0004L();
	void TestNotifyConferenceStatusChange0001L();
	void TestNotifyConferenceStatusChange0002L();
	void TestNotifyConferenceStatusChange0004L();
	void TestSwap0001L();
	void TestSwap0002L();
	void TestSwap0004L();
	void TestSwap0005L();
	void TestHangUp0001L();
	void TestHangUp0001bL();
	void TestHangUp0002L();
	void TestHangUp0004L();
	void TestHangUp0005L();
	void TestAddCall0001L();
	void TestAddCall0002L();
	void TestAddCall0003L();
	void TestAddCall0004L();
	void TestAddCall0005L();
	void TestGetCaps0001L();
	void TestGetMobileCallInfo0001L();
	void TestGetMobileCallInfo0003L();


private:
	void CreateConferenceLC(RPhone& aPhone,
			RMobileConferenceCall& aConferenceCall,
			RLine& aLine,
			RCall& aCall,
			TInt aCallId1,
			const TDesC &aTelNumber1,
			RCall& aCall2,
			TInt aCallId2,
			const TDesC &aTelNumber2);

	void OpenConferenceLC(RMobileConferenceCall& aConferenceCall, RMobilePhone& aPhone);

	void OpenLineLC(RLine& aLine, const TDesC& aName = KMmTsyVoice1LineName);

	void OpenNewCallLC(RCall& aCall, RLine& aLine, TDes* aCallName = NULL);

	void MakeCallAndAddToConferenceLC(
			RCall& aCall,
			RLine& aLine,
			RMobileConferenceCall& aConferenceCall,
			TInt aCallId,
			const TDesC &aTelNumber,
			const RArray<TInt>& aCallsInConference);

	void ChangeCallStatusL(TInt aCallId,
			RMobileCall::TMobileCallStatus aCallStatus);

	void AddCallL(RMobileConferenceCall& aConferenceCall,
			TInt aCallId,
			const TName& aCallName,
			const RArray<TInt>& aCallsInConference);
	
	void ChangeConferenceCallStatusL(const RArray<TInt>& aCallsInConference, 
			RMobileCall::TMobileCallStatus aCallStatus);
	
	void ChangeCallStatusInOrderL(TInt aStartCallId,
				TInt aEndCallId,
				RMobileCall::TMobileCallStatus aCallStatus);
	
	void ConnectAndPutCallOnHoldInRightOrderL(TInt aCallId);
	
	}; // class CCTsyConferenceCallControlFU

#endif // CCTSYCONFERENCECALLCONTROLFU_H

