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

#ifndef TCUSTOMAPITSYTESTHELPER_H
#define TCUSTOMAPITSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TCustomApiTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TCustomApiTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMmCustomAPINotifyAlsBlockedChanged( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TGetAlsBlockStatus& aBlockStatus, 
				RMmCustomAPI::TGetAlsBlockStatus aExpectedBlockStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyCellInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TMmCellInfo> &aMmCellInfo,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyCipheringInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TCipheringInfo>& aCipheringInfo,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyDtmfEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
                TCmpBase<RMmCustomAPI::TDtmfInfo>& aInfo,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyEGprsInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TGprsInformationType> &aGprsInformationType,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyHSxPAStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::THSxPAStatus& aHSxPAStatus, 
				RMmCustomAPI::THSxPAStatus aExpectedHSxPAStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyIccCallForwardingStatusChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TCFIndicators> &aCFIndicators,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyNetworkConnectionFailure( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyNSPSStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TNspsStatus& aNspsStatus, 
				RMmCustomAPI::TNspsStatus aExpectedNspsStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyPndCacheReady( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TName& aPndName, 
				TName aExpectedPndName,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyProgrammableOperatorLogoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
                TCmpBase<RMmCustomAPI::TOperatorId>& aOperatorId,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifyRauEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TRauEventStatus& aEventStatus, 
				RMmCustomAPI::TRauEventStatus aExpectedEventStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifySimCardStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TSIMCardStatus& aCardStatus, 
				RMmCustomAPI::TSIMCardStatus aExpectedCardStatus,
				TInt aWantedStatus );

	void WaitForMmCustomAPINotifySsNetworkEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
                TCmpBase<RMmCustomAPI::TSsTypeAndMode>& aSsTypeAndMode,
				RMmCustomAPI::TSsInfo &aSsInfo,
				TInt aWantedStatus );

	}; // class TCustomApiTsyTestHelper 

#endif
