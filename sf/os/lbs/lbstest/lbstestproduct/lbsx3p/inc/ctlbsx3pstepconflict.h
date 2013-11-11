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
// @file ctlbsx3pstepconflict.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_X3P_STEP_CONFLICT_H__
#define __CT_LBS_X3P_STEP_CONFLICT_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>

//#include <lbs/test/lbsnetsimtestobserver.h>

// LBS test includes.
#include "ctlbsx3pstep.h"
#include "ctlbsx3pserver.h"
#include "ctlbsx3pdotransmitpos.h"
#include <lbs/test/ctlbstimerutils.h>
#include "ctlbsdoposupdate.h"
//#include "ctlbsx3pnetsim.h"
//#include "ctlbsx3pdummyao.h"

// Literals used
_LIT(KLbsX3PStep_Conflict, "LbsX3PStep_Conflict");

typedef struct TimeStamp
{
	TLbsCallbackFlags	flag;
	TTime			sysTime;		
} TIME_STAMP;


class CT_LbsX3PStep_Conflict : public CT_LbsX3PStep,
										//MT_LbsTimerUtilsObserver,
										MT_LbsX3PDoTransmitPosObserver,
										MT_LbsDoPosUpdateObserver
	{
public:
	~CT_LbsX3PStep_Conflict();

	static CT_LbsX3PStep_Conflict* New(CT_LbsX3PServer& aParent);
	virtual TVerdict doTestStepL();
	void ConstructL();

	// From MT_LbsTimerUtilsObserver - keep alive timer to ensure we recvice all callbacks.
	void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	// From MT_LbsDoPosUpdateObserver - transmit position callback.
	void MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO);

	// From MT_LbsDoPosUpdateObserver - Position notify update callback.
	void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);
	void MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo);

	//From MT_LbsX3PDummyObserver - To handle callback notifications from other threads eg. netsim...
	//void MT_LbsX3PDummyCallbackHandler(TRequestStatus& aStatus);

	void Connected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyReleaseLcsMoLr(TInt aReason);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
													  const RLbsAssistanceDataBuilderSet& aData, 
													  const TLbsNetPosRequestQuality& aQuality);

	void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyMeasurementReportControlFailure(TInt aReason);
	void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);

	TInt64 CallbackTime(TLbsCallbackFlags aCallbackFlags);
	
private:
	CT_LbsX3PStep_Conflict(CT_LbsX3PServer& aParent);

	void SetCallbackFlag(TLbsCallbackFlags aCallbackFlag);
	void AnalyseCallbackFlags(TLbsCallbackFlags aCallbackFlag, RPointerArray<TTime> & aCallbackTime);

	//enum TState
	//	{
		//EWaitingNotifyRegisterLcsMolr,
		//EWaitingNotifyAssitanceData,
		//EWaitingNotifycationMeasurementReportLocation,
		//EWaitingNotifyReleaseLcsMolr,
		//EMtlrCompleted,
		//EX3PCompleted,
	//	EStartNetworkLocationRequest,
	//	EStartNetworkPrivacyRequest,
	//	EStartMtlrRequest,
	//	EStartMtlrPrivacyRequest,
	//	EStartMolrTransmitInNotifyMeasurementReportLocation,
	//	EStartMolrTransmitInNotifyMeasurementControlLocation,
	//	EWaiting,
	//	EDone
	//	};

	enum TStateX3P
		{
		//EX3PNormal,
		//EX3PPriorityInterrupt,
		//EX3PPriorityNormal,
		//EX3PControlCancel,
		//EX3PNotifyReleaseDone,
		//EX3PPhaseOneOK,
		//EX3PPhaseTwoOK,
		//EX3PAny,
		EX3POk,
		EX3PErrorReported
		};

	#if 0
	enum TCallbackNotification
		{
		EWaitingNotifyReleaseLcsLocationNotification,
		EWaitingNotifyMeasurementReportLocation,
		EWaitingNone
		};
	#endif
	
	//TRequestStatus 		m_aStatus;
	TRequestStatus 		iSignalStatus;
	//TState				iState;
	//TCallbackNotification	iCallbackNotification;
	TStateX3P			iStateX3P;
	RLbsTransmitPositionServer iServer;

	//CT_LbsTimerUtils* 		iKeepAliveTimer;
	RLbsTransmitPosition 	iTransmitPositioner;
	RLbsTransmitPosition 	iTransmitPositioner2;
	//RLbsNetSimTest 		iNetSim;
	CT_LbsX3PDoTransmitPos* 	iDoTransmitPosAO;	//AO for X3P
	CT_LbsX3PDoTransmitPos* 	iDoTransmitPosAO2;  //AO for X3P
	CT_LbsDoPosUpdate* iDoPosUpdate;	//AO for MOLR

	//CT_LbsX3PDummyAO *		iDummyAO;
	TInt 						iTestCaseId;
	RPointerArray<TIME_STAMP>	iTSArray;
	TPositionInfo* iPosInfo0;
	TPositionInfo* iPosInfo1;
	};

#endif //__CT_LBS_X3P_STEP_CONFLICT_H__
