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
// @file ctlbsclientstepassistance.h
// This contains the header file for assistance data Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_ASSISTANCE_H__
#define __CT_LBS_CLIENT_STEP_ASSISTANCE_H__

#include <lbs/lbsadmin.h>
//Lbs test include
#include "ctlbsnetsimstep.h"
#include "ctlbsdoposupdate.h"
#include "ctlbsclientserver.h"

// Literals used
_LIT(KLbsClientStep_Assistance, "LbsClientStep_Assistance");

// Callbacks flags.
const TLbsCallbackFlags KLbsCallback_Got_NotifyUpdate			= 0x1000;

class CT_LbsClientStep_Assistance : public CT_LbsNetSimStep,	
										   MT_LbsDoPosUpdateObserver
										
	{

public:
	~CT_LbsClientStep_Assistance();

	static CT_LbsClientStep_Assistance* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

	// From CT_LbsNetSimStep via CT_LbsAssDataStep
	void Connected();
	void Disconnected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyReleaseLcsMoLr(TInt aReason);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	//void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	void NotifyMeasurementReportControlFailure(TInt aReason);

	// From MT_LbsDoPosUpdateObserver - Position notify update callback.
	void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);

private:
	
	CT_LbsClientStep_Assistance(CT_LbsClientServer& aParent);
	void ConstructL();


	enum  {	EStartMTLRPrivReq = CT_LbsNetSimStep::ELast,
			EStartMTLRLocReq};
    
    
    CT_LbsClientServer& iParent;

	/** Current test case being executed.
	*/
	TInt iTestCaseId;

	// to allow changing admin settings:
	CLbsAdmin*	iLbsAdmin;
	
	/** Active object wrapper to allow MO-LRs.
	*/
	CT_LbsDoPosUpdate* iDoPosUpdate;

	/** Used to monitor the number of assistance data events we receviced during a test.
	*/
	TUint iAssEventCount;
	
	};

#endif //__CT_LBS_CLIENT_STEP_ASSISTANCE_H__
