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

// The TEFUnit header file which tests the CallInformation
// functional unit of the Common TSY.
//

#ifndef CCTSYCALLINFORMATIONFU_H
#define CCTSYCALLINFORMATIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallInformationFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetCallParams0001L();
	void TestGetCallParams0003L();
	void TestGetCallParams0006L();
	void TestGetCallParams0006bL();
	void TestGetCallParams0006cL();
	void TestGetCallParams0008L();
	void TestGetCallParams00011L();
	void TestGetCallDuration0001L();
	void TestGetCallDuration0006L();
	void TestGetCallDuration00011L();
	void TestGetOwnershipStatus0001L();
	void TestGetOwnershipStatus0006L();
	void TestGetOwnershipStatus00011L();
	void TestGetCaps0001L();
	void TestGetCaps0006L();
	void TestGetCaps00011L();
	void TestGetInfo0001L();
	void TestGetInfo0006L();
	void TestGetInfo00011L();
	void TestGetStatus0001L();
	void TestGetStatus0006L();
	void TestGetStatus00011L();
	void TestGetCallInfo0001L();
	void TestGetCallInfo0003L();
	void TestGetCallInfo0006L();
	void TestGetCallInfo0008L();
	void TestGetCallInfo00011L();
	void TestGetCallInfo00013L();
	void TestEnumerateCall0001L();
	void TestEnumerateCall0006L();
	void TestEnumerateCall00011L();
	void TestNotifyCapsChange0001L();
	void TestNotifyCapsChange0002L();
	void TestNotifyCapsChange0004L();
	void TestNotifyCapsChange0006L();
	void TestNotifyCapsChange0007L();
	void TestNotifyCapsChange0009L();
	void TestNotifyCapsChange00011L();
	void TestNotifyCapsChange00012L();
	void TestNotifyCapsChange00014L();
	void TestNotifyHookChange0001L();
	void TestNotifyHookChange0006L();
	void TestNotifyHookChange00011L();
	void TestNotifyStatusChange0001L();
	void TestNotifyStatusChange0002L();
	void TestNotifyStatusChange0004L();
	void TestNotifyStatusChange0006L();
	void TestNotifyStatusChange0007L();
	void TestNotifyStatusChange0009L();
	void TestNotifyStatusChange00011L();
	void TestNotifyStatusChange00012L();
	void TestNotifyStatusChange00014L();
	void TestNotifyCallDurationChange0001L();
	void TestNotifyCallDurationChange0002L();
	void TestNotifyCallDurationChange0004L();
	void TestNotifyCallDurationChange0006L();
	void TestNotifyCallDurationChange0007L();
	void TestNotifyCallDurationChange0009L();
	void TestNotifyCallDurationChange00011L();
	void TestNotifyCallDurationChange00012L();
	void TestNotifyCallDurationChange00014L();
	void TestNotifyCallAdded0001L();
	void TestNotifyCallAdded0002L();
	void TestNotifyCallAdded0004L();
	void TestNotifyCallAdded0006L();
	void TestNotifyCallAdded0007L();
	void TestNotifyCallAdded0009L();
	void TestNotifyCallAdded00011L();
	void TestNotifyCallAdded00012L();
	void TestNotifyCallAdded00014L();
	void TestNotifyIncomingCall0001L();
	void TestNotifyIncomingCall0002L();
	void TestNotifyIncomingCall0004L();
	void TestNotifyIncomingCall0006L();
	void TestNotifyIncomingCall0007L();
	void TestNotifyIncomingCall0009L();
	void TestNotifyIncomingCall00011L();
	void TestNotifyIncomingCall00012L();
	void TestNotifyIncomingCall00014L();
	void TestNotifyCallEvent0001L();
	void TestNotifyCallEvent0002L();
	void TestNotifyCallEvent0004L();
	void TestNotifyCallEvent0006L();
	void TestNotifyCallEvent0007L();
	void TestNotifyCallEvent0009L();
	void TestNotifyCallEvent00011L();
	void TestNotifyCallEvent00012L();
	void TestNotifyCallEvent00014L();
	void TestGetMobileCallCaps0001L();
	void TestGetMobileCallCaps0003L();
	void TestGetMobileCallCaps0006L();
	void TestGetMobileCallCaps0008L();
	void TestGetMobileCallCaps00011L();
	void TestGetMobileCallCaps00013L();
	void TestGetMobileCallInfo0001L();
	void TestGetMobileCallInfo0003L();
	void TestGetMobileCallInfo0006L();
	void TestGetMobileCallInfo0006bL();
	void TestGetMobileCallInfo0008L();
	void TestGetMobileCallInfo00011L();
	void TestGetMobileCallInfo00013L();
	void TestGetMobileCallStatus0001L();
	void TestGetMobileCallStatus0006L();
	void TestGetMobileCallStatus00011L();
	void TestNotifyAudioToneEvent0001L();
	void TestNotifyAudioToneEvent0006L();
	void TestNotifyAudioToneEvent00011L();
	void TestNotifyMobileCallCapsChange0001L();
	void TestNotifyMobileCallCapsChange0002L();
	void TestNotifyMobileCallCapsChange0003L();
	void TestNotifyMobileCallCapsChange0004L();
	void TestNotifyMobileCallCapsChange0006L();
	void TestNotifyMobileCallCapsChange0007L();
	void TestNotifyMobileCallCapsChange0008L();
	void TestNotifyMobileCallCapsChange0009L();
	void TestNotifyMobileCallCapsChange00011L();
	void TestNotifyMobileCallCapsChange00012L();
	void TestNotifyMobileCallCapsChange00013L();
	void TestNotifyMobileCallCapsChange00014L();
	void TestNotifyRemotePartyInfoChange0001L();
	void TestNotifyRemotePartyInfoChange0002L();
	void TestNotifyRemotePartyInfoChange0003L();
	void TestNotifyRemotePartyInfoChange0004L();
	void TestNotifyRemotePartyInfoChange0006L();
	void TestNotifyRemotePartyInfoChange0007L();
	void TestNotifyRemotePartyInfoChange0008L();
	void TestNotifyRemotePartyInfoChange0009L();
	void TestNotifyRemotePartyInfoChange00011L();
	void TestNotifyRemotePartyInfoChange00012L();
	void TestNotifyRemotePartyInfoChange00013L();
	void TestNotifyRemotePartyInfoChange00014L();
	void TestNotifyMobileCallStatusChange0001L();
	void TestNotifyMobileCallStatusChange0002L();
	void TestNotifyMobileCallStatusChange0004L();
	void TestNotifyMobileCallStatusChange0006L();
	void TestNotifyMobileCallStatusChange0007L();
	void TestNotifyMobileCallStatusChange0009L();
	void TestNotifyMobileCallStatusChange00011L();
	void TestNotifyMobileCallStatusChange00012L();
	void TestNotifyMobileCallStatusChange00014L();
	void TestGetIncomingCallType0001L();
	void TestGetIncomingCallType0006L();
	void TestGetIncomingCallType00011L();
	void TestNotifyIncomingCallTypeChange0001L();
	void TestNotifyLineHookChange0001L();
	void TestNotifyLineHookChange0006L();
	void TestNotifyLineHookChange00011L();

	private:
	
	void CallGetMobileCallInfoL(TInt aCallId, 
                                RMobilePhone::TMobileService aService, 
                                const TDesC &aTelNumber);
                           
                                
	void AuxGetCallDuration0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	void AuxGetOwnershipStatus0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	void AuxGetInfo0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	void AuxGetStatus0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	void AuxGetCallInfo0003L(const TDesC& aLineName);
	void AuxEnumerateCall0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	
	void AuxGetIncomingCallType0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService);
	
	
	void OpenLineLC(RLine& aLine, RPhone& aPhone, const TDesC& aName);
	void CreateAndOpenIncomingCalLC(RLine &aLine, 
									RCall &aCall,
									TName &aCallName,
									const TDesC& aLineName,
									const TInt aCallId,
									RMobileCall::TMobileCallStatus aMobileCallStatus,
									RMobilePhone::TMobileService aMobileService
									);
	void OpenNewCallLC(RCall& aCall, RLine& aLine, TDes* aCallName);
	void ValidateMobileCallInfo(TDes8* aCallInfo, TName &aCallName, const TDesC& aLineName, TBool aCallFlagActive, RMobileCall::TMobileCallStatus aStatus=RMobileCall::EStatusRinging);
	void ValidateCallParams(const TDesC8* aCallParams, TBool aCallFlagActive);
	void ValidateDataCallParams(const TDesC8* aCallParams, TBool aDataCallFlagActive);
	void ValidateHscsdCallParams(const TDesC8* aCallParams, TBool aHscsdCallFlagActive);

	}; // class CCTsyCallInformationFU

#endif // CCTSYCALLINFORMATIONFU_H

