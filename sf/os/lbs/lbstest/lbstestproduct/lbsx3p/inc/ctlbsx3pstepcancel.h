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
// @file ctlbsx3pstepcancel.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_X3P_STEP_CANCEL_H__
#define __CT_LBS_X3P_STEP_CANCEL_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>

//#include <lbs/test/lbsnetsimtestobserver.h>

// LBS test includes.
#include "ctlbsx3pstep.h"
#include "ctlbsx3pserver.h"
#include "ctlbsx3pdotransmitpos.h"
#include <lbs/test/ctlbstimerutils.h>
//#include "ctlbsx3pnetsim.h"

// Literals used
_LIT(KLbsX3PStep_Cancel, "LbsX3PStep_Cancel");

class CT_LbsX3PStep_Cancel : public CT_LbsX3PStep,
									MT_LbsX3PDoTransmitPosObserver
	{
public:
	~CT_LbsX3PStep_Cancel();

	static CT_LbsX3PStep_Cancel* New(CT_LbsX3PServer& aParent);
	virtual TVerdict doTestStepL();
	void ConstructL();


	// From MT_LbsTimerUtilsObserver - keep alive timer to ensure we recvice all callbacks.
	//void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	// From MT_LbsX3PDoTransmitPosObserver - Transmit Position callback.
	void MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO);
	void MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo);

	// From MLbsX3PNetSim (MLbsNetSimTestObserver).	
	void Connected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyReleaseLcsMoLr(TInt aReason);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
													  const RLbsAssistanceDataBuilderSet& aData, 
													  const TLbsNetPosRequestQuality& aQuality);
	void NotifyMeasurementReportControlFailure(TInt aReason);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);	
	
private:
	CT_LbsX3PStep_Cancel(CT_LbsX3PServer& aParent);

	//enum TState
		//{
		//EWaitingNotifyRegisterLcsMolr,
		//EWaitingNotifycationMeasurementControlLocation,
		//EWaitingNotifycationMeasurementReportLocation,
		//EWaitingNotifyReleaseLcsMolr,
		//EWaiting,
		//EDone
		//};
	
	TRequestStatus 		iStatus;
	//TRequestStatus 		iSignalStatus;
	//TState				iState;
	//CT_LbsTimerUtils* 		iKeepAliveTimer;
	RLbsTransmitPosition 	iTransmitPositioner;
	RLbsTransmitPosition 	iTransmitPositioner2;
	CT_LbsX3PDoTransmitPos* iDoTransmitPosAO;
	CT_LbsX3PDoTransmitPos* iDoTransmitPosAO2;
	TInt 					iTestCaseId;
	TBool 					iRefPosFlag;

	};

#endif //__CT_LBS_X3P_STEP_CANCEL_H__
