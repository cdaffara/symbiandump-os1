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
//

#ifndef TNETWORKTSYTESTHELPER_H
#define TNETWORKTSYTESTHELPER_H



#include "ttsytesthelperbase.h"

enum TOperatorName
	{
	EOperatorO2,
	EOperatorVodafone,
	EOperatorOrange,
	EOperatorTMobile,
	EOperatorElisa,
	EOperatorDNA,
	EOperatorSonera,
	EOperatorAnite,
	EOperatorNTN,
	EOperatorUnknown
	};
	
class TNetworkTsyTestHelper : public TTsyTestHelperBase
	{
public:
	
	TNetworkTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobilePhoneNotifyModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode, 
				RMobilePhone::TMobilePhoneNetworkMode aExpectedNetworkMode,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNetworkInvScanChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneInvestigationScan& aSetting, 
				RMobilePhone::TMobilePhoneInvestigationScan aExpectedSetting,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNetworkInvScanEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneInvestigationScanEvent& aEvent, 
				RMobilePhone::TMobilePhoneInvestigationScanEvent aExpectedEvent,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNetworkRegistrationStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneRegistrationStatus& aStatus, 
				RMobilePhone::TMobilePhoneRegistrationStatus aExpectedStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNetworkSecurityLevelChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkSecurity& aSecurity, 
				RMobilePhone::TMobilePhoneNetworkSecurity aExpectedSecurity,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyPreferredNetworksListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV1> &aMobilePhoneNetworkInfoV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV2> &aMobilePhoneNetworkInfoV2,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV5> &aMobilePhoneNetworkInfoV5,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV1> &aMobilePhoneNetworkInfoV1,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV2> &aMobilePhoneNetworkInfoV2,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV5> &aMobilePhoneNetworkInfoV5,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyAPNListChanged( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyAPNControlListServiceStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus, 
				RMobilePhone::TAPNControlListServiceStatus aExpectedAPNControlListServiceStatus,
				TInt aWantedStatus );
				
	void WaitForMobilePhoneNotifyNetworkSelectionSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkSelectionV1 &aNetSel,
				RMobilePhone::TMobilePhoneSelectionMethod aDesiredMethod,
				TInt aWantedStatus );

	TInt GetCurrentNetworkL( RMobilePhone& aPhone, 
			RMobilePhone::TMobilePhoneNetworkInfoV1Pckg& aPhoneNetwork, 
			RMobilePhone::TMobilePhoneLocationAreaV1& aArea,
			TOperatorName& aOperatorName);

	
	TInt GetNetworkRegistrationStatusL( RMobilePhone& aPhone, 
							RMobilePhone::TMobilePhoneRegistrationStatus &aRegistrationStatus );
							
							
	TInt WaitForRegisteredOnHomeNetworkL( RMobilePhone& aPhone );
	void CheckPhoneConnectedToAniteL(RMobilePhone &aPhone);

	void GetNetworkPasswordsSectionL(RMobilePhone& aPhone, const TDesC*& aNetworkSection);

	}; // class TNetworkTsyTestHelper 

#endif
