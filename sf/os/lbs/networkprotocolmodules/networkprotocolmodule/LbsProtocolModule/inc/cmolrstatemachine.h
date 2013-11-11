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
// This file defines the class for the MO-LR protocol state machine.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CMOLRSTATEMACHINE_H__
#define __CMOLRSTATEMACHINE_H__

#include "cstatemachinebase.h"


/** MO-LR protocol state machine.
This class manages the MO-LR protocol procedure and is employed when the protocol
module is required to handle interactions for a mobile self-locate.

The class implements the pure virtual methods defined by CStateMachineBase.
@see CStateMachineBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CMoLrStateMachine) : public CStateMachineBase, public MLbsCallbackTimerObserver
	{
public:

	/** MO-LR specific states
	These are specific to MO-LR and identify the current state within the
	MO-LR self locate protocol procedure.
	*/
	enum TMoLrState
		{
		/** Not valid */
		EStateNull,
		/** LBS Cient MO-LR request has been received */
		EStateClientReqRecvd,
		/** Network MO-LR session has started */
		EStateNetSessStarted,
		/** Measurement control data received from the network */
		EStateMeasureDataRecvd,
		/** Network reference location sent to LBS */
		EStateNetBasedLocSent,
		/** Network location request sent to LBS */
		EStateLocReqByNet,
		/** Location response received from LBS */
		EStateLocRespRecvd,
		/** Location response sent to network */
		EStateLocSentToNet,
		/** Network MO-LR session about to close */
		EStateNetSessToClose,
		/** LBS client MO-LR session about to close */
		EStateClientSessToClose,
		/** Network and LBS sessions closed */
		EStateSessionsClosed,
		/** Network result location sent to LBS */
		EStateNetResultSent,
		/** Protocol procedure is to be cancelled */
		EStateCancelling
		};

	/** Request identifiers used when a request is queued.
	*/
	enum TMoLrQueue
		{
		/** Location response received from LBS */
		EQueueLocResponse		 = 100,
		/** Measurement control received from network */
		EQueueMeasurementControl = 200,
		/** Network result received from network */
		EQueueNetworkResult		 = 300,
		};

public:

	static CMoLrStateMachine* NewL(MStateMachineObserver& aObserver);
	virtual ~CMoLrStateMachine();
	
	CMoLrStateMachine::TMoLrState CurrentState();
	
	void StartMoLrReqTimer();
	void StartLocRespTimer();
	void StartFacResultTimer();

	TBool IsLocReqSentToLbs() const;

	const TPositionInfoBase& NetResultLoc() const;

	void MoLrReq(const TLbsNetSessionId& aSessionId);
	void QueueMoLrReq(const TLbsNetSessionId& aSessionId);
	void SessionResult(TInt aResult, const TPositionInfoBase* aPosInfo);

	// CStateMachineBase derived methods

	void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
							const TLbsNetPosRequestQuality& aQuality,
							const TLbsNetPosRequestMethod& aPosMethod);
	void MeasurementControlErrorInd(TInt aReason);
	void NetworkErrorInd();

	// CStateMachineBase pure virtual methods
	void StartQueuedMachine();
	void PreStateTransition();
	void StateTransition();
	void PostStateTransition();
	void CancelProcedure();
	void DoQueuedRequest(TInt aRequest);

	void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	void MeasurementControlTimeout();

	// MLbsCallbackTimerObserver methods
	
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);
	
	
protected:

	CMoLrStateMachine(MStateMachineObserver& aObserver);
	
private:
	
	void ConstructL();
	void InitialiseProcedure();
	void CompleteProcedure();

	void StoreNetResultLoc(const TPositionInfoBase& aPosInfo);

protected:
	
	/** Current MO-LR state
	*/
	TMoLrState iCurrentState;
	
	/** Timer for MO-LR requests issued to network.
	*/
	CLbsCallbackTimer* iMoLrReqIssuedTimer;
	
	/** Timer for Location response expected by network
	*/
	CLbsCallbackTimer* iLocRespTimer;
	
	/** Timer for Facility result expected from network
	*/
	CLbsCallbackTimer* iFacResultTimer;
	
	/** Flag to show location request has been sent to LBS.
	*/
	TBool iIsLocReqSentToLbs;

	/** Flag to indicate if network result location is available
	*/
	TBool iIsNetResultLocAvailable;
	
	/** Network Result Position info.
	*/
	TPositionInfo iNetResultPosInfo;

	};

#endif // __CMOLRSTATEMACHINE_H__

