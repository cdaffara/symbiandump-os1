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
// *** STUB CLASS HEADER FOR UNIT TESTS ***
// 
//


#ifndef LBSNETSIMTESTGATEWAYOBSERVER_H
#define LBSNETSIMTESTGATEWAYOBSERVER_H

// forwards
class TLbsNetPosRequestPrivacy;
class TLbsNetPosRequestQuality;
class TPositionInfoBase;
class TLbsPrivacyResponse;
class RLbsAssistanceDataBuilderSet;

class MLbsNetSimObserver
	{
	public:
		virtual void ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& aInfo, const TLbsNetPosRequestPrivacy& aPrivacy) = 0;
		virtual void ProcessReleaseLcsLocationNotification(TInt aReason) = 0;
		virtual void ProcessFacilityLcsMoLrResult(TInt aReason, const TPositionInfoBase* aPosition) = 0;
		virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality) = 0;
		virtual void ProcessMeasurementControlLocationError(TInt aError) = 0;
		virtual void ProcessResetAssistanceData(TLbsAssistanceDataGroup aMask) = 0;
		virtual void ProcessError(TInt aError) = 0;
	};
	
	
#endif // LBSNETSIMTESTGATEWAYOBSERVER_H
