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

#ifndef TSUPPLEMENTALTSYTESTHELPER_H
#define TSUPPLEMENTALTSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TSupplementalTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TSupplementalTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobilePhoneNotifyCCBSRecall( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCCBSEntryV1> &aMobilePhoneCCBSEntryV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCCBSRecall( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCCBSEntryV2> &aMobilePhoneCCBSEntryV2,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCCBSStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCCBSStatus& aCcbsStatus, 
				RMobilePhone::TMobilePhoneCCBSStatus aExpectedCcbsStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCallBarringStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCBCondition& aCondition, 
				RMobilePhone::TMobilePhoneCBCondition aExpectedCondition,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCallForwardingActive( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobileService& aServiceGroup, 
				RMobilePhone::TMobileService aExpectedServiceGroup,
				RMobilePhone::TMobilePhoneCFActive &aActiveType,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCallForwardingStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCFCondition& aCondition, 
				RMobilePhone::TMobilePhoneCFCondition aExpectedCondition,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCallServiceCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCallWaitingStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCWInfoEntryV1> &aMobilePhoneCWInfoEntryV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCipheringIndicatorStatus( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus, 
				RMobilePhone::TMobileCallCipheringIndicator aExpectedIndicatorStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIdentityServiceStatus( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneIdService& aService, 
				RMobilePhone::TMobilePhoneIdService aExpectedService,
				RMobilePhone::TMobilePhoneIdServiceStatus &aStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIdentitySuppressionRejected( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySendNetworkServiceRequest( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNotifySendSSOperation &aOperation,
				TCmpBase<RMobilePhone::TMobilePhoneSendSSRequestV3> &aMobilePhoneSendSSRequestV3,
				TInt aWantedStatus );

	void WaitForMobileUssdMessagingNotifyNetworkRelease( 
				RMobileUssdMessaging& aMobileUssdMessaging,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileUssdMessaging::TMobileUssdAttributesV1> &aMobileUssdAttributesV1,
				TDes8 &aMsgAttributes,
				TInt aWantedStatus );

	}; // class TSupplementalTsyTestHelper 

#endif
