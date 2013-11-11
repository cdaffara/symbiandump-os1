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

#ifndef TPACKETSERVICETSYTESTHELPER_H
#define TPACKETSERVICETSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TPacketServiceTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TPacketServiceTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForPacketContextNotifyConnectionSpeedChange( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TUint& aRate, 
				TUint aExpectedRate,
				TInt aWantedStatus );

	void WaitForPacketContextNotifyStatusChange( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				RPacketContext::TContextStatus& aContextStatus, 
				RPacketContext::TContextStatus aExpectedContextStatus,
				TInt aWantedStatus );

	void WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigGPRS> &aContextConfigGPRS,
				TInt aWantedStatus );

	void WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigR99_R4> &aContextConfigR99_R4,
				TInt aWantedStatus );

	void WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfig_R5> &aContextConfig_R5,
				TInt aWantedStatus );

	void WaitForPacketContextNotifyDataTransferred( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				RPacketContext::TDataVolume& aVolume, 
				RPacketContext::TDataVolume& aExpectedVolume,
				TUint &aRcvdGranularity,
				TUint &aSentGranularity,
				TInt aWantedStatus );

	void WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSGPRSNegotiated> &aQoSGPRSNegotiated,
				TInt aWantedStatus );

	void WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSR99_R4Negotiated> &aQoSR99_R4Negotiated,
				TInt aWantedStatus );

	void WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSR5Negotiated> &aQoSR5Negotiated,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyAttachModeChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TAttachMode& aMode, 
				RPacketService::TAttachMode aExpectedMode,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyChangeOfNtwkRegStatus( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TRegistrationStatus& aRegistrationStatus, 
				RPacketService::TRegistrationStatus aExpectedRegistrationStatus,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TPckg<RPacketContext::TContextConfigGPRS> &aContextConfigGPRS,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigR99_R4> &aContextConfigR99_R4,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfig_R5> &aContextConfig_R5,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyContextAdded( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TDes& aContextId, 
				TDes aExpectedContextId,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyDynamicCapsChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TDynamicCapsFlags& aCaps, 
				RPacketService::TDynamicCapsFlags aWantedCaps,
				RPacketService::TDynamicCapsFlags aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyMSClassChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TMSClass& aNewClass, 
				RPacketService::TMSClass aExpectedNewClass,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyReleaseModeChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TPacketReleaseMode& aReleaseMode, 
				RPacketService::TPacketReleaseMode aExpectedReleaseMode,
				TInt aWantedStatus );

	void WaitForPacketServiceNotifyStatusChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TStatus& aPacketStatus, 
				RPacketService::TStatus aExpectedPacketStatus,
				TInt aWantedStatus );
				
	void GetGprsSettings(
				RMobilePhone::TMobilePhoneNetworkInfoV1 info,
				TPtrC& aAccessPoint,
				TPtrC& aUser,
				TPtrC& aPassword,
                TUint aNumber=1);

    void ActivatePrimaryPacketContextL(RMobilePhone& aMobilePhone, 
                                       RPacketContext& aPacketContext,
                                       TInt aNum = 1);
	}; // class TPacketServiceTsyTestHelper 

#endif
