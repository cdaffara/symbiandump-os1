// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstepcellbasednpud.h
// This contains the header file for Notify Position Update Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_CELLBASED_NOTIFY_POS_UPDATE_H__
#define __CT_LBS_CLIENT_STEP_CELLBASED_NOTIFY_POS_UPDATE_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include "ctlbsclientnotifyposupdao.h"
#include "ctlbsnetsimstep.h"
#include "ctlbsdoposupdate.h"

#include <lbs/lbsadmin.h>
#include <lbs/test/lbsnetsimtest.h>

// Literals used
_LIT(KLbsClientStep_CellBasedNotifyPosUpdate, "LbsClientStep_CellBasedNotifyPosUpdate");

const TLbsCallbackFlags KLbsCallback_Got_NotifyPositionUpdate			= 0x1000;

class CT_LbsClientStep_CellBasedNotifyPosUpdate : public CT_LbsNetSimStep,
												  public MT_LbsDoPosUpdateObserver
	{
public:
	~CT_LbsClientStep_CellBasedNotifyPosUpdate();

	static CT_LbsClientStep_CellBasedNotifyPosUpdate* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();
    
   	// From CT_LbsNetSimStep:
	void Connected();
	void Disconnected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyReleaseLcsMoLr(TInt aReason);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	void NotifyMeasurementReportControlFailure(TInt aReason);
	void NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError);
	void ProcessMeasurementControlLocationError(TInt aError);


   // From MT_LbsDoPosUpdateObserver:
   void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);
   
   
private:
	CT_LbsClientStep_CellBasedNotifyPosUpdate(CT_LbsClientServer& aParent);

	void ConstructL();
	
private:
	
	// whether the network should be available
	TBool	iNetworkUnavailable;
	
	// whether (and when) the request should be cancelled
	TBool	iCancel;
	TBool	iCancelBeforePosUpdate;
	TBool	iCancelOnPosUpdate;
	TBool 	iIsCancelTest;
	TBool	iMultiReq;
	
	// to allow us to set a ref location in NetSim for network-based updates and monitor network:
	RLbsNetSimTest iNetSim;
   
	CT_LbsClientServer& iParent;

	CT_LbsDoPosUpdate* iDoPosUpdate;
			
	// Number of NotifyPositionUpdates the test case should do
	TUint iNumNPUDs;
	};

#endif //__CT_LBS_CLIENT_STEP_CELLBASED_NOTIFY_POS_UPDATE_H__
