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
// This file contains methods for resetting the notifier queues on the etel server
// at the end of a test
// 
//

/**
 @file 
*/
 
#include "cetelsessionmgr.h"
#include "cctsytestlogging.h"
#include "pcktcs.h"
#include "e32debug.h"

//
//Cleanup methods
//
/**
* Notify cleanup for RMobileBroadcastMessaging::NotifyBroadcastIdListChange
*/	
void CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyBroadcastIdListChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyBroadcastIdListChange"));
	TRequestStatus reqStatus;
	do 
		{
		aMobileBroadcastMessaging.NotifyBroadcastIdListChange(reqStatus);
		aMobileBroadcastMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyIdListChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileBroadcastMessaging::NotifyFilterSettingChange
*/	
void CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyFilterSettingChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyFilterSettingChange"));
	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	do 
		{
		aMobileBroadcastMessaging.NotifyFilterSettingChange(reqStatus,setting);
		aMobileBroadcastMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyFilterSettingChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileBroadcastMessaging::NotifyLanguageFilterChange
*/	
void CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyLanguageFilterChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifyLanguageFilterChange"));
	TRequestStatus reqStatus;
	TBuf16<16> langFilter;
	do 
		{
		aMobileBroadcastMessaging.NotifyLanguageFilterChange(reqStatus,langFilter);
		aMobileBroadcastMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyLanguageFilterChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileLine::NotifyMobileLineStatusChange
*/	
void CEtelSessionMgr::CleanupMobileLineNotifyMobileLineStatusChange( 
					RMobileLine& aMobileLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileLineNotifyMobileLineStatusChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallStatus status;
	do 
		{
		aMobileLine.NotifyMobileLineStatusChange(reqStatus,status);
		aMobileLine.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyALSLineChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyALSLineChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyALSLineChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneALSLine aLSLine;
	do 
		{
		aMobilePhone.NotifyALSLineChange(reqStatus,aLSLine);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyALSLineChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyAlternatingCallCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyAlternatingCallCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyAlternatingCallCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifyAlternatingCallCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyAlternatingCallModeChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyAlternatingCallModeChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyAlternatingCallModeChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneAlternatingCallMode mode;
	RMobilePhone::TMobileService firstService;
	do 
		{
		aMobilePhone.NotifyAlternatingCallModeChange(reqStatus,mode,firstService);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCCBSRecall
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSRecall( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSRecall"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCCBSEntryV1 pkgType;
	RMobilePhone::TMobilePhoneCCBSEntryV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCCBSRecall(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSRecall);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCCBSRecall
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSRecall1( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSRecall1"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCCBSEntryV2 pkgType;
	RMobilePhone::TMobilePhoneCCBSEntryV2Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCCBSRecall(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSRecall);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCCBSStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCCBSStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCCBSStatus ccbsStatus;
	do 
		{
		aMobilePhone.NotifyCCBSStatusChange(reqStatus,ccbsStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCallBarringStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCallBarringStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCallBarringStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCBCondition condition;
	do 
		{
		aMobilePhone.NotifyCallBarringStatusChange(reqStatus,condition);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCallForwardingActive
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCallForwardingActive( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCallForwardingActive"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobileService serviceGroup;
	RMobilePhone::TMobilePhoneCFActive activeType;
	do 
		{
		aMobilePhone.NotifyCallForwardingActive(reqStatus,serviceGroup,activeType);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingActive);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCallForwardingStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCallForwardingStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCallForwardingStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCFCondition condition;
	do 
		{
		aMobilePhone.NotifyCallForwardingStatusChange(reqStatus,condition);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCallServiceCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCallServiceCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCallServiceCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifyCallServiceCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCallServiceCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCallWaitingStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCallWaitingStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCallWaitingStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 pkgType;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCallWaitingStatusChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCallWaitingStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCipheringIndicatorStatus
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCipheringIndicatorStatus( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCipheringIndicatorStatus"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobileCallCipheringIndicator indicatorStatus;
	do 
		{
		aMobilePhone.NotifyCipheringIndicatorStatus(reqStatus,indicatorStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCipheringIndicatorStatus);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCostCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCostCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCostCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifyCostCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCostCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyDTMFCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyDTMFCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyDTMFCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifyDTMFCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyDTMFEvent
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyDTMFEvent( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyDTMFEvent"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneDTMFEvent event;
	do 
		{
		aMobilePhone.NotifyDTMFEvent(reqStatus,event);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyFdnStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyFdnStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyFdnStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
	do 
		{
		aMobilePhone.NotifyFdnStatusChange(reqStatus,fdnStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyFdnStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIccAccessCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIccAccessCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIccAccessCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifyIccAccessCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIccAccessCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIdentityServiceStatus
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIdentityServiceStatus( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIdentityServiceStatus"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service = RMobilePhone::EIdServiceUnspecified;
	RMobilePhone::TMobilePhoneIdServiceStatus status;
	do 
		{
		aMobilePhone.NotifyIdentityServiceStatus(reqStatus,service,status);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIdentityServiceStatus);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIdentitySuppressionRejected
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIdentitySuppressionRejected( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIdentitySuppressionRejected"));
	TRequestStatus reqStatus;
	do 
		{
		aMobilePhone.NotifyIdentitySuppressionRejected(reqStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIdentitySuppressionRejected);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIncomingCallTypeChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIncomingCallTypeChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIncomingCallTypeChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIncomingCallType callType;
	RMobileCall::TMobileDataCallParamsV1 pkgType;
	RMobileCall::TMobileDataCallParamsV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyIncomingCallTypeChange(reqStatus,callType,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIncomingCallTypeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIndicatorChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIndicatorChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIndicatorChange"));
	TRequestStatus reqStatus;
	TUint32 indicator;
	do 
		{
		aMobilePhone.NotifyIndicatorChange(reqStatus,indicator);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMessageWaiting
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMessageWaiting( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMessageWaiting"));
	TRequestStatus reqStatus;
	TInt count;
	do 
		{
		aMobilePhone.NotifyMessageWaiting(reqStatus,count);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMessageWaiting);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyModeChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyModeChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyModeChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkMode networkMode;
	do 
		{
		aMobilePhone.NotifyModeChange(reqStatus,networkMode);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMulticallIndicatorChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMulticallIndicatorChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMulticallIndicatorChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMulticallIndicator multicallIndicator;
	do 
		{
		aMobilePhone.NotifyMulticallIndicatorChange(reqStatus,multicallIndicator);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMulticallIndicatorChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMulticallParamsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMulticallParamsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMulticallParamsChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMulticallSettingsV1 pkgType;
	RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyMulticallParamsChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMulticallParamsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMultimediaCallPreferenceChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMultimediaCallPreferenceChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMultimediaCallPreferenceChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMultimediaSettings mmSettings;
	do 
		{
		aMobilePhone.NotifyMultimediaCallPreferenceChange(reqStatus,mmSettings);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMultimediaCallPreferenceChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNetworkInvScanChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkInvScanChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkInvScanChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneInvestigationScan setting;
	do 
		{
		aMobilePhone.NotifyNetworkInvScanChange(reqStatus,setting);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkInvScanChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNetworkInvScanEvent
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkInvScanEvent( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkInvScanEvent"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneInvestigationScanEvent event;
	do 
		{
		aMobilePhone.NotifyNetworkInvScanEvent(reqStatus,event);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkInvScanEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNetworkRegistrationStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkRegistrationStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkRegistrationStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus status;
	do 
		{
		aMobilePhone.NotifyNetworkRegistrationStatusChange(reqStatus,status);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNetworkSecurityLevelChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkSecurityLevelChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkSecurityLevelChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkSecurity security;
	do 
		{
		aMobilePhone.NotifyNetworkSecurityLevelChange(reqStatus,security);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSecurityLevelChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySecurityCapsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySecurityCapsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySecurityCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobilePhone.NotifySecurityCapsChange(reqStatus,caps);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySecurityEvent
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySecurityEvent( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySecurityEvent"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent event;
	do 
		{
		aMobilePhone.NotifySecurityEvent(reqStatus,event);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySendNetworkServiceRequest
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySendNetworkServiceRequest( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySendNetworkServiceRequest"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNotifySendSSOperation operation = RMobilePhone::ESendSSInvoke;
	RMobilePhone::TMobilePhoneSendSSRequestV3 pkgType;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifySendNetworkServiceRequest(reqStatus,operation,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySendNetworkServiceRequest);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySignalStrengthChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySignalStrengthChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySignalStrengthChange"));
	TRequestStatus reqStatus;
	TInt32 signalStrength;
	TInt8 bar;
	do 
		{
		aMobilePhone.NotifySignalStrengthChange(reqStatus,signalStrength,bar);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyPreferredNetworksListChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyPreferredNetworksListChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyPreferredNetworksListChange"));
	TRequestStatus reqStatus;
	do 
		{
		aMobilePhone.NotifyPreferredNetworksListChange(reqStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyStorePreferredNetworksListChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyStopInDTMFString
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyStopInDTMFString( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyStopInDTMFString"));
	TRequestStatus reqStatus;
	do 
		{
		aMobilePhone.NotifyStopInDTMFString(reqStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyStopInDTMFString);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyAirTimeDurationChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyAirTimeDurationChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyAirTimeDurationChange"));
	TRequestStatus reqStatus;
	TTimeIntervalSeconds time;
	do 
		{
		aMobilePhone.NotifyAirTimeDurationChange(reqStatus,time);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAirTimeDurationChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCostInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCostInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCostInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCostInfoV1 pkgType;
	RMobilePhone::TMobilePhoneCostInfoV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCostInfoChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCostInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange1( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange1"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV2 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange2( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange2"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV5 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV5Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange3( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange3"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg pkgArg(pkgType);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg,area);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange4( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange4"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV2 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg pkgArg(pkgType);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg,area);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyCurrentNetworkChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange5( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyCurrentNetworkChange5"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV5 pkgType;
	RMobilePhone::TMobilePhoneNetworkInfoV5Pckg pkgArg(pkgType);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	do 
		{
		aMobilePhone.NotifyCurrentNetworkChange(reqStatus,pkgArg,area);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyLockInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyLockInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyLockInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneLock lock;
	RMobilePhone::TMobilePhoneLockInfoV1 pkgType;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyLockInfoChange(reqStatus,lock,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNITZInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNITZInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNITZInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNITZ nITZInfo;
	do 
		{
		aMobilePhone.NotifyNITZInfoChange(reqStatus,nITZInfo);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyNetworkSelectionSettingChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkSelectionSettingChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyNetworkSelectionSettingChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1 pkgType;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyNetworkSelectionSettingChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySecurityCodeInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySecurityCodeInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySecurityCodeInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSecurityCode securityCode;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 pkgType;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifySecurityCodeInfoChange(reqStatus,securityCode,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCodeInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyUSimApplicationsInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyUSimApplicationsInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyUSimApplicationsInfoChange"));
	TRequestStatus reqStatus;
	TInt count;
	RMobilePhone::TAID activeAID;
	do 
		{
		aMobilePhone.NotifyUSimApplicationsInfoChange(reqStatus,count,activeAID);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyUSimApplicationsInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyUSimAppsSelectionModeChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyUSimAppsSelectionModeChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyUSimAppsSelectionModeChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TUSimSelectionMode uSimSelectionMode;
	do 
		{
		aMobilePhone.NotifyUSimAppsSelectionModeChange(reqStatus,uSimSelectionMode);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyUSimAppsSelectionModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyUUSSettingChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyUUSSettingChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyUUSSettingChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneUUSSetting setting;
	do 
		{
		aMobilePhone.NotifyUUSSettingChange(reqStatus,setting);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyUUSSettingChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyIccMessageWaitingIndicatorsChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyIccMessageWaitingIndicatorsChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneMessageWaitingV1 pkgType;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyIccMessageWaitingIndicatorsChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMmsConfig
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMmsConfig( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMmsConfig"));
	TRequestStatus reqStatus;
	RMobilePhone::TMmsConnParams type = RMobilePhone::EUserPreferences;
	TBuf8<30> connectivity;
	do 
		{
		aMobilePhone.NotifyMmsConfig(reqStatus,type,connectivity);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMmsConfig);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMmsUpdate
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMmsUpdate( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMmsUpdate"));
	TRequestStatus reqStatus;
	RMobilePhone::TMmsNotificationV3 pkgType;
	RMobilePhone::TMmsNotificationV3Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyMmsUpdate(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMmsUpdate);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyBatteryInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyBatteryInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyBatteryInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneBatteryInfoV1 info;
	do 
		{
		aMobilePhone.NotifyBatteryInfoChange(reqStatus,info);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyImsAuthorizationInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyImsAuthorizationInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyImsAuthorizationInfoChange"));
	TRequestStatus reqStatus;
	do 
		{
		aMobilePhone.NotifyImsAuthorizationInfoChange(reqStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyImsAuthorizationInfoChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifySmartCardApplicationInfoChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifySmartCardApplicationInfoChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifySmartCardApplicationInfoChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TAID aID;
	RMobilePhone::TSmartCardApplicationEvent event;
	do 
		{
		aMobilePhone.NotifySmartCardApplicationInfoChange(reqStatus,aID,event);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifySmartCardApplicationInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyDefaultPrivacyChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyDefaultPrivacyChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyDefaultPrivacyChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePrivacy setting;
	do 
		{
		aMobilePhone.NotifyDefaultPrivacyChange(reqStatus,setting);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyDefaultPrivacyChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyAPNListChanged
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyAPNListChanged( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyAPNListChanged"));
	TRequestStatus reqStatus;
	do 
		{
		aMobilePhone.NotifyAPNListChanged(reqStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyAPNControlListServiceStatusChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyAPNControlListServiceStatusChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyAPNControlListServiceStatusChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus aPNControlListServiceStatus;
	do 
		{
		aMobilePhone.NotifyAPNControlListServiceStatusChange(reqStatus,aPNControlListServiceStatus);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhone::NotifyMailboxNumbersChange
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifyMailboxNumbersChange( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifyMailboxNumbersChange"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneVoicemailIdsV3 pkgType;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg pkgArg(pkgType);
	do 
		{
		aMobilePhone.NotifyMailboxNumbersChange(reqStatus,pkgArg);
		aMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyMailboxNumbersChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketQoS::NotifyProfileChanged
*/	
void CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged( 
					RPacketQoS& aPacketQoS )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged"));
	TRequestStatus reqStatus;
	RPacketQoS::TQoSGPRSNegotiated pkgType;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> pkgArg(pkgType);
	do 
		{
		aPacketQoS.NotifyProfileChanged(reqStatus,pkgArg);
		aPacketQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketQoS::NotifyProfileChanged
*/	
void CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged1( 
					RPacketQoS& aPacketQoS )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged1"));
	TRequestStatus reqStatus;
	RPacketQoS::TQoSR99_R4Negotiated pkgType;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> pkgArg(pkgType);
	do 
		{
		aPacketQoS.NotifyProfileChanged(reqStatus,pkgArg);
		aPacketQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketQoS::NotifyProfileChanged
*/	
void CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged2( 
					RPacketQoS& aPacketQoS )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketQoSNotifyProfileChanged2"));
	TRequestStatus reqStatus;
	RPacketQoS::TQoSR5Negotiated pkgType;
	TPckg<RPacketQoS::TQoSR5Negotiated> pkgArg(pkgType);
	do 
		{
		aPacketQoS.NotifyProfileChanged(reqStatus,pkgArg);
		aPacketQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileSmartCardEap::NotifyEapMethodAccessStatusChange
*/	
void CEtelSessionMgr::CleanupMobileSmartCardEapNotifyEapMethodAccessStatusChange( 
					RMobileSmartCardEap& aMobileSmartCardEap )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmartCardEapNotifyEapMethodAccessStatusChange"));
	TRequestStatus reqStatus;
	RMobileSmartCardEap::TEapMethodAccessStatus eapMethodStatus;
	do 
		{
		aMobileSmartCardEap.NotifyEapMethodAccessStatusChange(reqStatus,eapMethodStatus);
		aMobileSmartCardEap.CancelAsyncRequest(EMobileSmartCardEapNotifyEapMethodAccessStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileSmsMessaging::NotifyMoSmsBearerChange
*/	
void CEtelSessionMgr::CleanupMobileSmsMessagingNotifyMoSmsBearerChange( 
					RMobileSmsMessaging& aMobileSmsMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmsMessagingNotifyMoSmsBearerChange"));
	TRequestStatus reqStatus;
	RMobileSmsMessaging::TMobileSmsBearer bearer;
	do 
		{
		aMobileSmsMessaging.NotifyMoSmsBearerChange(reqStatus,bearer);
		aMobileSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifyMoSmsBearerChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileSmsMessaging::NotifyReceiveModeChange
*/	
void CEtelSessionMgr::CleanupMobileSmsMessagingNotifyReceiveModeChange( 
					RMobileSmsMessaging& aMobileSmsMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmsMessagingNotifyReceiveModeChange"));
	TRequestStatus reqStatus;
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;
	do 
		{
		aMobileSmsMessaging.NotifyReceiveModeChange(reqStatus,receiveMode);
		aMobileSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifyReceiveModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileSmsMessaging::NotifySmspListChange
*/	
void CEtelSessionMgr::CleanupMobileSmsMessagingNotifySmspListChange( 
					RMobileSmsMessaging& aMobileSmsMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmsMessagingNotifySmspListChange"));
	TRequestStatus reqStatus;
	do 
		{
		aMobileSmsMessaging.NotifySmspListChange(reqStatus);
		aMobileSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyAlternatingCallSwitch
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyAlternatingCallSwitch( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyAlternatingCallSwitch"));
	TRequestStatus reqStatus;
	do 
		{
		aMobileCall.NotifyAlternatingCallSwitch(reqStatus);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyAlternatingCallSwitch);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyAudioToneEvent
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyAudioToneEvent( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyAudioToneEvent"));
	TRequestStatus reqStatus;
	RMobileCall::TAudioToneV3 pkgType;
	RMobileCall::TAudioToneV3Pckg pkgArg(pkgType);
	do 
		{
		aMobileCall.NotifyAudioToneEvent(reqStatus,pkgArg);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyAudioToneEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyCallEvent
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyCallEvent( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyCallEvent"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallEvent event;
	do 
		{
		aMobileCall.NotifyCallEvent(reqStatus,event);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyCallEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyHscsdInfoChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyHscsdInfoChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyHscsdInfoChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallHscsdInfoV1 pkgType;
	RMobileCall::TMobileCallHscsdInfoV1Pckg pkgArg(pkgType);
	do 
		{
		aMobileCall.NotifyHscsdInfoChange(reqStatus,pkgArg);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyHscsdInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyMobileCallCapsChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyMobileCallCapsChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyMobileCallCapsChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallCapsV1 pkgType;
	RMobileCall::TMobileCallCapsV1Pckg pkgArg(pkgType);
	do 
		{
		aMobileCall.NotifyMobileCallCapsChange(reqStatus,pkgArg);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyMobileCallStatusChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyMobileCallStatusChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyMobileCallStatusChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallStatus status;
	do 
		{
		aMobileCall.NotifyMobileCallStatusChange(reqStatus,status);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyMobileDataCallCapsChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyMobileDataCallCapsChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyMobileDataCallCapsChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 pkgType;
	RMobileCall::TMobileCallDataCapsV1Pckg pkgArg(pkgType);
	do 
		{
		aMobileCall.NotifyMobileDataCallCapsChange(reqStatus,pkgArg);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyMobileDataCallCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyUUSCapsChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyUUSCapsChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyUUSCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobileCall.NotifyUUSCapsChange(reqStatus,caps);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyUUSCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyVoiceFallback
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyVoiceFallback( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyVoiceFallback"));
	TRequestStatus reqStatus;
	TName callName;
	do 
		{
		aMobileCall.NotifyVoiceFallback(reqStatus,callName);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyVoiceFallback);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyPrivacyConfirmation
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyPrivacyConfirmation( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyPrivacyConfirmation"));
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhonePrivacy privacySetting;
	do 
		{
		aMobileCall.NotifyPrivacyConfirmation(reqStatus,privacySetting);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyPrivacyConfirmation);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyTrafficChannelConfirmation
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyTrafficChannelConfirmation( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyTrafficChannelConfirmation"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallTch tchType;
	do 
		{
		aMobileCall.NotifyTrafficChannelConfirmation(reqStatus,tchType);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyTrafficChannelConfirmation);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileCall::NotifyRemotePartyInfoChange
*/	
void CEtelSessionMgr::CleanupMobileCallNotifyRemotePartyInfoChange( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifyRemotePartyInfoChange"));
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallRemotePartyInfoV1 pkgType;
	RMobileCall::TMobileCallRemotePartyInfoV1Pckg pkgArg(pkgType);
	do 
		{
		aMobileCall.NotifyRemotePartyInfoChange(reqStatus,pkgArg);
		aMobileCall.CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RLine::NotifyCapsChange
*/	
void CEtelSessionMgr::CleanupLineNotifyCapsChange( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifyCapsChange"));
	TRequestStatus reqStatus;
	RLine::TCaps caps;
	do 
		{
		aLine.NotifyCapsChange(reqStatus,caps);
		aLine.NotifyCapsChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RLine::NotifyCallAdded
*/	
void CEtelSessionMgr::CleanupLineNotifyCallAdded( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifyCallAdded"));
	TRequestStatus reqStatus;
	TName name;
	do 
		{
		aLine.NotifyCallAdded(reqStatus,name);
		aLine.NotifyCallAddedCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RLine::NotifyHookChange
*/	
void CEtelSessionMgr::CleanupLineNotifyHookChange( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifyHookChange"));
	TRequestStatus reqStatus;
	RCall::THookStatus hookStatus;
	do 
		{
		aLine.NotifyHookChange(reqStatus,hookStatus);
		aLine.NotifyHookChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RLine::NotifyStatusChange
*/	
void CEtelSessionMgr::CleanupLineNotifyStatusChange( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifyStatusChange"));
	TRequestStatus reqStatus;
	RCall::TStatus lineStatus;
	do 
		{
		aLine.NotifyStatusChange(reqStatus,lineStatus);
		aLine.NotifyStatusChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RLine::NotifyIncomingCall
*/	
void CEtelSessionMgr::CleanupLineNotifyIncomingCall( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifyIncomingCall"));
	TRequestStatus reqStatus;
	TName name;
	do 
		{
		aLine.NotifyIncomingCall(reqStatus,name);
		aLine.NotifyIncomingCallCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobilePhoneStore::NotifyStoreEvent
*/	
void CEtelSessionMgr::CleanupMobilePhoneStoreNotifyStoreEvent( 
					RMobilePhoneStore& aMobilePhoneStore )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneStoreNotifyStoreEvent"));
	TRequestStatus reqStatus;
	TUint32 event;
	TInt index;
	do 
		{
		aMobilePhoneStore.NotifyStoreEvent(reqStatus,event,index);
		aMobilePhoneStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileConferenceCall::NotifyCapsChange
*/	
void CEtelSessionMgr::CleanupMobileConferenceCallNotifyCapsChange( 
					RMobileConferenceCall& aMobileConferenceCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileConferenceCallNotifyCapsChange"));
	TRequestStatus reqStatus;
	TUint32 caps;
	do 
		{
		aMobileConferenceCall.NotifyCapsChange(reqStatus,caps);
		aMobileConferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileConferenceCall::NotifyConferenceEvent
*/	
void CEtelSessionMgr::CleanupMobileConferenceCallNotifyConferenceEvent( 
					RMobileConferenceCall& aMobileConferenceCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileConferenceCallNotifyConferenceEvent"));
	TRequestStatus reqStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName callName;
	do 
		{
		aMobileConferenceCall.NotifyConferenceEvent(reqStatus,event,callName);
		aMobileConferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceEvent);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileConferenceCall::NotifyConferenceStatusChange
*/	
void CEtelSessionMgr::CleanupMobileConferenceCallNotifyConferenceStatusChange( 
					RMobileConferenceCall& aMobileConferenceCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileConferenceCallNotifyConferenceStatusChange"));
	TRequestStatus reqStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	do 
		{
		aMobileConferenceCall.NotifyConferenceStatusChange(reqStatus,status);
		aMobileConferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyConnectionSpeedChange
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyConnectionSpeedChange( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyConnectionSpeedChange"));
	TRequestStatus reqStatus;
	TUint rate;
	do 
		{
		aPacketContext.NotifyConnectionSpeedChange(reqStatus,rate);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyConnectionSpeedChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyStatusChange
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyStatusChange( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyStatusChange"));
	TRequestStatus reqStatus;
	RPacketContext::TContextStatus contextStatus;
	do 
		{
		aPacketContext.NotifyStatusChange(reqStatus,contextStatus);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyConfigChanged
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfigGPRS pkgType;
	TPckg<RPacketContext::TContextConfigGPRS> pkgArg(pkgType);
	do 
		{
		aPacketContext.NotifyConfigChanged(reqStatus,pkgArg);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyConfigChanged
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged1( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged1"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfigR99_R4 pkgType;
	TPckg<RPacketContext::TContextConfigR99_R4> pkgArg(pkgType);
	do 
		{
		aPacketContext.NotifyConfigChanged(reqStatus,pkgArg);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyConfigChanged
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged2( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyConfigChanged2"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfig_R5 pkgType;
	TPckg<RPacketContext::TContextConfig_R5> pkgArg(pkgType);
	do 
		{
		aPacketContext.NotifyConfigChanged(reqStatus,pkgArg);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketContext::NotifyDataTransferred
*/	
void CEtelSessionMgr::CleanupPacketContextNotifyDataTransferred( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifyDataTransferred"));
	TRequestStatus reqStatus;
	RPacketContext::TDataVolume volume;
	TUint rcvdGranularity=0x1000;
	TUint sentGranularity=0x1000;
	do 
		{
		aPacketContext.NotifyDataTransferred(reqStatus,volume,rcvdGranularity,sentGranularity);
		aPacketContext.CancelAsyncRequest(EPacketContextNotifyDataTransferred);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileUssdMessaging::NotifyNetworkRelease
*/	
void CEtelSessionMgr::CleanupMobileUssdMessagingNotifyNetworkRelease( 
					RMobileUssdMessaging& aMobileUssdMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileUssdMessagingNotifyNetworkRelease"));
	TRequestStatus reqStatus;
	RMobileUssdMessaging::TMobileUssdAttributesV1 pkgType;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pkgArg(pkgType);
	RMobilePhone::TMobilePhoneSendSSRequestV3 pkg1;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg pkgArg1(pkg1);
	do 
		{
		aMobileUssdMessaging.NotifyNetworkRelease(reqStatus,pkgArg1,pkgArg);
		aMobileUssdMessaging.CancelAsyncRequest(EMobileUssdMessagingNotifyNetworkRelease);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyAttachModeChange
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyAttachModeChange( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyAttachModeChange"));
	TRequestStatus reqStatus;
	RPacketService::TAttachMode mode;
	do 
		{
		aPacketService.NotifyAttachModeChange(reqStatus,mode);
		aPacketService.CancelAsyncRequest(EPacketNotifyAttachModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyChangeOfNtwkRegStatus
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyChangeOfNtwkRegStatus( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyChangeOfNtwkRegStatus"));
	TRequestStatus reqStatus;
	RPacketService::TRegistrationStatus registrationStatus;
	do 
		{
		aPacketService.NotifyChangeOfNtwkRegStatus(reqStatus,registrationStatus);
		aPacketService.CancelAsyncRequest(EPacketNotifyChangeOfNtwkRegStatus);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyContextActivationRequested
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfigGPRS pkgType;
	TPckg<RPacketContext::TContextConfigGPRS> pkgArg(pkgType);
	do 
		{
		aPacketService.NotifyContextActivationRequested(reqStatus,pkgArg);
		aPacketService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyContextActivationRequested
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested1( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested1"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfigR99_R4 pkgType;
	TPckg<RPacketContext::TContextConfigR99_R4> pkgArg(pkgType);
	do 
		{
		aPacketService.NotifyContextActivationRequested(reqStatus,pkgArg);
		aPacketService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyContextActivationRequested
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested2( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyContextActivationRequested2"));
	TRequestStatus reqStatus;
	RPacketContext::TContextConfig_R5 pkgType;
	TPckg<RPacketContext::TContextConfig_R5> pkgArg(pkgType);
	do 
		{
		aPacketService.NotifyContextActivationRequested(reqStatus,pkgArg);
		aPacketService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyContextAdded
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyContextAdded( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyContextAdded"));
	TRequestStatus reqStatus;
	TBuf16<20> contextId;
	do 
		{
		aPacketService.NotifyContextAdded(reqStatus,contextId);
		aPacketService.CancelAsyncRequest(EPacketNotifyContextAdded);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyDynamicCapsChange
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyDynamicCapsChange( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyDynamicCapsChange"));
	TRequestStatus reqStatus;
	RPacketService::TDynamicCapsFlags caps;
	do 
		{
		aPacketService.NotifyDynamicCapsChange(reqStatus,caps);
		aPacketService.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyMSClassChange
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyMSClassChange( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyMSClassChange"));
	TRequestStatus reqStatus;
	RPacketService::TMSClass newClass;
	do 
		{
		aPacketService.NotifyMSClassChange(reqStatus,newClass);
		aPacketService.CancelAsyncRequest(EPacketNotifyMSClassChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyReleaseModeChange
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyReleaseModeChange( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyReleaseModeChange"));
	TRequestStatus reqStatus;
	RPacketService::TPacketReleaseMode releaseMode;
	do 
		{
		aPacketService.NotifyReleaseModeChange(reqStatus,releaseMode);
		aPacketService.CancelAsyncRequest(EPacketNotifyReleaseModeChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPacketService::NotifyStatusChange
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifyStatusChange( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifyStatusChange"));
	TRequestStatus reqStatus;
	RPacketService::TStatus packetStatus;
	do 
		{
		aPacketService.NotifyStatusChange(reqStatus,packetStatus);
		aPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RCall::NotifyCapsChange
*/	
void CEtelSessionMgr::CleanupCallNotifyCapsChange( 
					RCall& aCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupCallNotifyCapsChange"));
	TRequestStatus reqStatus;
	RCall::TCaps caps;
	do 
		{
		aCall.NotifyCapsChange(reqStatus,caps);
		aCall.NotifyCapsChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RCall::NotifyCallDurationChange
*/	
void CEtelSessionMgr::CleanupCallNotifyCallDurationChange( 
					RCall& aCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupCallNotifyCallDurationChange"));
	TRequestStatus reqStatus;
	TTimeIntervalSeconds time;
	do 
		{
		aCall.NotifyCallDurationChange(reqStatus,time);
		aCall.NotifyCallDurationChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RCall::NotifyHookChange
*/	
void CEtelSessionMgr::CleanupCallNotifyHookChange( 
					RCall& aCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupCallNotifyHookChange"));
	TRequestStatus reqStatus;
	RCall::THookStatus hookStatus;
	do 
		{
		aCall.NotifyHookChange(reqStatus,hookStatus);
		aCall.NotifyHookChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RCall::NotifyStatusChange
*/	
void CEtelSessionMgr::CleanupCallNotifyStatusChange( 
					RCall& aCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupCallNotifyStatusChange"));
	TRequestStatus reqStatus;
	RCall::TStatus callStatus;
	do 
		{
		aCall.NotifyStatusChange(reqStatus,callStatus);
		aCall.NotifyStatusChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPhone::NotifyCapsChange
*/	
void CEtelSessionMgr::CleanupPhoneNotifyCapsChange( 
					RPhone& aPhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPhoneNotifyCapsChange"));
	TRequestStatus reqStatus;
	RPhone::TCaps caps;
	do 
		{
		aPhone.NotifyCapsChange(reqStatus,caps);
		aPhone.NotifyCapsChangeCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RPhone::NotifyModemDetected
*/	
void CEtelSessionMgr::CleanupPhoneNotifyModemDetected( 
					RPhone& aPhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPhoneNotifyModemDetected"));
	TRequestStatus reqStatus;
	RPhone::TModemDetection detection;
	do 
		{
		aPhone.NotifyModemDetected(reqStatus,detection);
		aPhone.NotifyModemDetectedCancel();
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileLocationServices::NotifyMtlr
*/	
void CEtelSessionMgr::CleanupMobileLocationServicesNotifyMtlr( 
					RMobileLocationServices& aMobileLocationServices )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileLocationServicesNotifyMtlr"));
	TRequestStatus reqStatus;
	RMobileLocationServices::TNotifyMtlrV7 pkgType;
	RMobileLocationServices::TNotifyMtlrV7Pckg pkgArg(pkgType);
	do 
		{
		aMobileLocationServices.NotifyMtlr(reqStatus,pkgArg);
		aMobileLocationServices.CancelAsyncRequest(EMobileLocationServicesNotifyMtlr);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMobileLocationServices::NotifyMeasurementControl
*/	
void CEtelSessionMgr::CleanupMobileLocationServicesNotifyMeasurementControl( 
					RMobileLocationServices& aMobileLocationServices )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileLocationServicesNotifyMeasurementControl"));
	TRequestStatus reqStatus;
	RMobileLocationServices::TMeasurementControlV7 pkgType;
	RMobileLocationServices::TMeasurementControlV7Pckg pkgArg(pkgType);
	do 
		{
		aMobileLocationServices.NotifyMeasurementControl(reqStatus,pkgArg);
		aMobileLocationServices.CancelAsyncRequest(EMobileLocationServicesNotifyMeasurementControl);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

/**
* Notify cleanup for RMmCustomAPI::NotifyAlsBlockedChanged
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyAlsBlockedChanged( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyAlsBlockedChanged"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus;
	do 
		{
		aMmCustomAPI.NotifyAlsBlockedChanged(reqStatus,blockStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyAlsBlockedChangedIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyCellInfoChange
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyCellInfoChange( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyCellInfoChange"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TMmCellInfo pkgType;
	RMmCustomAPI::TMmCellInfoPckg pkgArg(pkgType);
	do 
		{
		aMmCustomAPI.NotifyCellInfoChange(reqStatus,pkgArg);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyCellInfoChangeIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyCipheringInfoChange
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyCipheringInfoChange( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyCipheringInfoChange"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TCipheringInfo cellInfo;
	do 
		{
		aMmCustomAPI.NotifyCipheringInfoChange(reqStatus,cellInfo);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyCipheringInfoChangeIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyDtmfEvent
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyDtmfEvent( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyDtmfEvent"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TDtmfInfo info;
	do 
		{
		aMmCustomAPI.NotifyDtmfEvent(reqStatus,info);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyDtmfEventIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyEGprsInfoChange
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyEGprsInfoChange( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyEGprsInfoChange"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TGprsInformationType pkgType;
	TPckg<RMmCustomAPI::TGprsInformationType> pkgArg(pkgType);
	do 
		{
		aMmCustomAPI.NotifyEGprsInfoChange(reqStatus,pkgArg);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyEGprsInfoChange);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyHSxPAStatus
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyHSxPAStatus( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyHSxPAStatus"));
	TRequestStatus reqStatus;
	RMmCustomAPI::THSxPAStatus hSxPAStatus;
	do 
		{
		aMmCustomAPI.NotifyHSxPAStatus(reqStatus,hSxPAStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyHSxPAStatusIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyIccCallForwardingStatusChange
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyIccCallForwardingStatusChange( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyIccCallForwardingStatusChange"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TCFIndicators pkgType;
	RMmCustomAPI::TCFIndicatorsPckg pkgArg(pkgType);
	do 
		{
		aMmCustomAPI.NotifyIccCallForwardingStatusChange(reqStatus,pkgArg);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyIccCallForwardingStatusChangeIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyNetworkConnectionFailure
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyNetworkConnectionFailure( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyNetworkConnectionFailure"));
	TRequestStatus reqStatus;
	do 
		{
		aMmCustomAPI.NotifyNetworkConnectionFailure(reqStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyNetworkConnectionFailureIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyNSPSStatus
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyNSPSStatus( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyNSPSStatus"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TNspsStatus nspsStatus;
	do 
		{
		aMmCustomAPI.NotifyNSPSStatus(reqStatus,nspsStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyNSPSStatusIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyPndCacheReady
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyPndCacheReady( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyPndCacheReady"));
	TRequestStatus reqStatus;
	TName pndName;
	do 
		{
		aMmCustomAPI.NotifyPndCacheReady(reqStatus,pndName);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyPndCacheReadyIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyProgrammableOperatorLogoChange
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyProgrammableOperatorLogoChange( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyProgrammableOperatorLogoChange"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TOperatorId operatorId;
	do 
		{
		aMmCustomAPI.NotifyProgrammableOperatorLogoChange(reqStatus,operatorId);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyProgrammableOperatorLogoChangeIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifyRauEvent
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifyRauEvent( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifyRauEvent"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TRauEventStatus eventStatus;
	do 
		{
		aMmCustomAPI.NotifyRauEvent(reqStatus,eventStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifyRauEventIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifySimCardStatus
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifySimCardStatus( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifySimCardStatus"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TSIMCardStatus cardStatus;
	do 
		{
		aMmCustomAPI.NotifySimCardStatus(reqStatus,cardStatus);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifySimCardStatusIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

		
/**
* Notify cleanup for RMmCustomAPI::NotifySsNetworkEvent
*/	
void CEtelSessionMgr::CleanupMmCustomAPINotifySsNetworkEvent( 
					RMmCustomAPI& aMmCustomAPI )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifySsNetworkEvent"));
	TRequestStatus reqStatus;
	RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
	RMmCustomAPI::TSsInfo ssInfo;
	do 
		{
		aMmCustomAPI.NotifySsNetworkEvent(reqStatus,ssTypeAndMode,ssInfo);
		aMmCustomAPI.CancelAsyncRequest(ECustomNotifySsNetworkEventIPC);
		User::WaitForRequest( reqStatus );
		}
	while( KErrNone == reqStatus.Int() );
	}

//
//Session Cleanups 
//
/**
* Cleanup notifiers for RMobileBroadcastMessaging
*/	
void CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifiers( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileBroadcastMessagingNotifiers"));
	CleanupMobileBroadcastMessagingNotifyBroadcastIdListChange( aMobileBroadcastMessaging );
	CleanupMobileBroadcastMessagingNotifyFilterSettingChange( aMobileBroadcastMessaging );
	CleanupMobileBroadcastMessagingNotifyLanguageFilterChange( aMobileBroadcastMessaging );

	}
/**
* Cleanup notifiers for RMobileLine
*/	
void CEtelSessionMgr::CleanupMobileLineNotifiers( 
					RMobileLine& aMobileLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileLineNotifiers"));
	CleanupMobileLineNotifyMobileLineStatusChange( aMobileLine );

	}
/**
* Cleanup notifiers for RMobilePhone
*/	
void CEtelSessionMgr::CleanupMobilePhoneNotifiers( 
					RMobilePhone& aMobilePhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneNotifiers"));
	CleanupMobilePhoneNotifyALSLineChange( aMobilePhone );
	CleanupMobilePhoneNotifyAlternatingCallCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifyAlternatingCallModeChange( aMobilePhone );
	CleanupMobilePhoneNotifyCCBSRecall( aMobilePhone );
	CleanupMobilePhoneNotifyCCBSRecall1( aMobilePhone );
	CleanupMobilePhoneNotifyCCBSStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyCallBarringStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyCallForwardingActive( aMobilePhone );
	CleanupMobilePhoneNotifyCallForwardingStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyCallServiceCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifyCallWaitingStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyCipheringIndicatorStatus( aMobilePhone );
	CleanupMobilePhoneNotifyCostCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifyDTMFCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifyDTMFEvent( aMobilePhone );
	CleanupMobilePhoneNotifyFdnStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyIccAccessCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifyIdentityServiceStatus( aMobilePhone );
	CleanupMobilePhoneNotifyIdentitySuppressionRejected( aMobilePhone );
	CleanupMobilePhoneNotifyIncomingCallTypeChange( aMobilePhone );
	CleanupMobilePhoneNotifyIndicatorChange( aMobilePhone );
	CleanupMobilePhoneNotifyMessageWaiting( aMobilePhone );
	CleanupMobilePhoneNotifyModeChange( aMobilePhone );
	CleanupMobilePhoneNotifyMulticallIndicatorChange( aMobilePhone );
	CleanupMobilePhoneNotifyMulticallParamsChange( aMobilePhone );
	CleanupMobilePhoneNotifyMultimediaCallPreferenceChange( aMobilePhone );
	CleanupMobilePhoneNotifyNetworkInvScanChange( aMobilePhone );
	CleanupMobilePhoneNotifyNetworkInvScanEvent( aMobilePhone );
	CleanupMobilePhoneNotifyNetworkRegistrationStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyNetworkSecurityLevelChange( aMobilePhone );
	CleanupMobilePhoneNotifySecurityCapsChange( aMobilePhone );
	CleanupMobilePhoneNotifySecurityEvent( aMobilePhone );
	CleanupMobilePhoneNotifySendNetworkServiceRequest( aMobilePhone );
	CleanupMobilePhoneNotifySignalStrengthChange( aMobilePhone );
	CleanupMobilePhoneNotifyPreferredNetworksListChange( aMobilePhone );
	CleanupMobilePhoneNotifyStopInDTMFString( aMobilePhone );
	CleanupMobilePhoneNotifyAirTimeDurationChange( aMobilePhone );
	CleanupMobilePhoneNotifyCostInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange5( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange1( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange2( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange3( aMobilePhone );
	CleanupMobilePhoneNotifyCurrentNetworkChange4( aMobilePhone );
	CleanupMobilePhoneNotifyLockInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyNITZInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyNetworkSelectionSettingChange( aMobilePhone );
	CleanupMobilePhoneNotifySecurityCodeInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyUSimApplicationsInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyUSimAppsSelectionModeChange( aMobilePhone );
	CleanupMobilePhoneNotifyUUSSettingChange( aMobilePhone );
	CleanupMobilePhoneNotifyIccMessageWaitingIndicatorsChange( aMobilePhone );
	CleanupMobilePhoneNotifyMmsConfig( aMobilePhone );
	CleanupMobilePhoneNotifyMmsUpdate( aMobilePhone );
	CleanupMobilePhoneNotifyBatteryInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyImsAuthorizationInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifySmartCardApplicationInfoChange( aMobilePhone );
	CleanupMobilePhoneNotifyDefaultPrivacyChange( aMobilePhone );
	CleanupMobilePhoneNotifyAPNListChanged( aMobilePhone );
	CleanupMobilePhoneNotifyAPNControlListServiceStatusChange( aMobilePhone );
	CleanupMobilePhoneNotifyMailboxNumbersChange( aMobilePhone );

	}
/**
* Cleanup notifiers for RPacketQoS
*/	
void CEtelSessionMgr::CleanupPacketQoSNotifiers( 
					RPacketQoS& aPacketQoS )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketQoSNotifiers"));
	CleanupPacketQoSNotifyProfileChanged( aPacketQoS );
	CleanupPacketQoSNotifyProfileChanged1( aPacketQoS );
	CleanupPacketQoSNotifyProfileChanged2( aPacketQoS );

	}
/**
* Cleanup notifiers for RMobileSmartCardEap
*/	
void CEtelSessionMgr::CleanupMobileSmartCardEapNotifiers( 
					RMobileSmartCardEap& aMobileSmartCardEap )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmartCardEapNotifiers"));
	CleanupMobileSmartCardEapNotifyEapMethodAccessStatusChange( aMobileSmartCardEap );

	}
/**
* Cleanup notifiers for RMobileSmsMessaging
*/	
void CEtelSessionMgr::CleanupMobileSmsMessagingNotifiers( 
					RMobileSmsMessaging& aMobileSmsMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileSmsMessagingNotifiers"));
	CleanupMobileSmsMessagingNotifyMoSmsBearerChange( aMobileSmsMessaging );
	CleanupMobileSmsMessagingNotifyReceiveModeChange( aMobileSmsMessaging );
	CleanupMobileSmsMessagingNotifySmspListChange( aMobileSmsMessaging );

	}
/**
* Cleanup notifiers for RMobileCall
*/	
void CEtelSessionMgr::CleanupMobileCallNotifiers( 
					RMobileCall& aMobileCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileCallNotifiers"));
	CleanupMobileCallNotifyAlternatingCallSwitch( aMobileCall );
	CleanupMobileCallNotifyAudioToneEvent( aMobileCall );
	CleanupMobileCallNotifyCallEvent( aMobileCall );
	CleanupMobileCallNotifyHscsdInfoChange( aMobileCall );
	CleanupMobileCallNotifyMobileCallCapsChange( aMobileCall );
	CleanupMobileCallNotifyMobileCallStatusChange( aMobileCall );
	CleanupMobileCallNotifyMobileDataCallCapsChange( aMobileCall );
	CleanupMobileCallNotifyUUSCapsChange( aMobileCall );
	CleanupMobileCallNotifyVoiceFallback( aMobileCall );
	CleanupMobileCallNotifyPrivacyConfirmation( aMobileCall );
	CleanupMobileCallNotifyTrafficChannelConfirmation( aMobileCall );
	CleanupMobileCallNotifyRemotePartyInfoChange( aMobileCall );

	}
/**
* Cleanup notifiers for RLine
*/	
void CEtelSessionMgr::CleanupLineNotifiers( 
					RLine& aLine )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupLineNotifiers"));
	CleanupLineNotifyCapsChange( aLine );
	CleanupLineNotifyCallAdded( aLine );
	CleanupLineNotifyHookChange( aLine );
	CleanupLineNotifyStatusChange( aLine );
	CleanupLineNotifyIncomingCall( aLine );

	}
/**
* Cleanup notifiers for RMobilePhoneStore
*/	
void CEtelSessionMgr::CleanupMobilePhoneStoreNotifiers( 
					RMobilePhoneStore& aMobilePhoneStore )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobilePhoneStoreNotifiers"));
	CleanupMobilePhoneStoreNotifyStoreEvent( aMobilePhoneStore );

	}
/**
* Cleanup notifiers for RMobileConferenceCall
*/	
void CEtelSessionMgr::CleanupMobileConferenceCallNotifiers( 
					RMobileConferenceCall& aMobileConferenceCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileConferenceCallNotifiers"));
	CleanupMobileConferenceCallNotifyCapsChange( aMobileConferenceCall );
	CleanupMobileConferenceCallNotifyConferenceEvent( aMobileConferenceCall );
	CleanupMobileConferenceCallNotifyConferenceStatusChange( aMobileConferenceCall );

	}
/**
* Cleanup notifiers for RPacketContext
*/	
void CEtelSessionMgr::CleanupPacketContextNotifiers( 
					RPacketContext& aPacketContext )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketContextNotifiers"));
	CleanupPacketContextNotifyConnectionSpeedChange( aPacketContext );
	CleanupPacketContextNotifyStatusChange( aPacketContext );
	CleanupPacketContextNotifyConfigChanged( aPacketContext );
	CleanupPacketContextNotifyConfigChanged1( aPacketContext );
	CleanupPacketContextNotifyConfigChanged2( aPacketContext );
	CleanupPacketContextNotifyDataTransferred( aPacketContext );

	}
/**
* Cleanup notifiers for RMobileUssdMessaging
*/	
void CEtelSessionMgr::CleanupMobileUssdMessagingNotifiers( 
					RMobileUssdMessaging& aMobileUssdMessaging )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileUssdMessagingNotifiers"));
	CleanupMobileUssdMessagingNotifyNetworkRelease( aMobileUssdMessaging );

	}
/**
* Cleanup notifiers for RPacketService
*/	
void CEtelSessionMgr::CleanupPacketServiceNotifiers( 
					RPacketService& aPacketService )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketServiceNotifiers"));
	CleanupPacketServiceNotifyAttachModeChange( aPacketService );
	CleanupPacketServiceNotifyChangeOfNtwkRegStatus( aPacketService );
	CleanupPacketServiceNotifyContextActivationRequested( aPacketService );
	CleanupPacketServiceNotifyContextActivationRequested1( aPacketService );
	CleanupPacketServiceNotifyContextActivationRequested2( aPacketService );
	CleanupPacketServiceNotifyContextAdded( aPacketService );
	CleanupPacketServiceNotifyDynamicCapsChange( aPacketService );
	CleanupPacketServiceNotifyMSClassChange( aPacketService );
	CleanupPacketServiceNotifyReleaseModeChange( aPacketService );
	CleanupPacketServiceNotifyStatusChange( aPacketService );

	}
/**
* Cleanup notifiers for RCall
*/	
void CEtelSessionMgr::CleanupCallNotifiers( 
					RCall& aCall )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupCallNotifiers"));
	CleanupCallNotifyCapsChange( aCall );
	CleanupCallNotifyCallDurationChange( aCall );
	CleanupCallNotifyHookChange( aCall );
	CleanupCallNotifyStatusChange( aCall );

	}
/**
* Cleanup notifiers for RPhone
*/	
void CEtelSessionMgr::CleanupPhoneNotifiers( 
					RPhone& aPhone )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPhoneNotifiers"));
	CleanupPhoneNotifyCapsChange( aPhone );
	CleanupPhoneNotifyModemDetected( aPhone );

	}
/**
* Cleanup notifiers for RMobileLocationServices
*/	
void CEtelSessionMgr::CleanupMobileLocationServicesNotifiers( 
					RMobileLocationServices& aMobileLocationServices )

	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMobileLocationServicesNotifiers"));
	CleanupMobileLocationServicesNotifyMtlr( aMobileLocationServices );
	CleanupMobileLocationServicesNotifyMeasurementControl( aMobileLocationServices );

	}

/**
* Cleanup notifiers for RMmCustomAPI
*/	
void CEtelSessionMgr::CleanupCustomAPINotifiers( 
					RMmCustomAPI& aMmCustomAPI )

	{
    TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupMmCustomAPINotifiers"));
	CleanupMmCustomAPINotifyAlsBlockedChanged( aMmCustomAPI );
	CleanupMmCustomAPINotifyCellInfoChange( aMmCustomAPI );
	CleanupMmCustomAPINotifyCipheringInfoChange( aMmCustomAPI );
	CleanupMmCustomAPINotifyDtmfEvent( aMmCustomAPI );
	CleanupMmCustomAPINotifyEGprsInfoChange( aMmCustomAPI );
	CleanupMmCustomAPINotifyHSxPAStatus( aMmCustomAPI );
	CleanupMmCustomAPINotifyIccCallForwardingStatusChange( aMmCustomAPI );
	CleanupMmCustomAPINotifyNetworkConnectionFailure( aMmCustomAPI );
	CleanupMmCustomAPINotifyNSPSStatus( aMmCustomAPI );
	CleanupMmCustomAPINotifyPndCacheReady( aMmCustomAPI );
	CleanupMmCustomAPINotifyProgrammableOperatorLogoChange( aMmCustomAPI );
	CleanupMmCustomAPINotifyRauEvent( aMmCustomAPI );
	CleanupMmCustomAPINotifySimCardStatus( aMmCustomAPI );
	CleanupMmCustomAPINotifySsNetworkEvent( aMmCustomAPI );

	}

