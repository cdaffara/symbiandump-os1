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
// This file defines the class for the Network Based Location 
// protocol state machine.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CNETLOCSTATEMACHINE_H__
#define __CNETLOCSTATEMACHINE_H__

#include "cstatemachinebase.h"


/** Network Based Location protocol state machine.
This class manages the Network Based Location protocol procedure and is employed
when the protocol module is required to handle interactions for retrieving a
reference location (e.g. cell-based) from the network.

The class implements the pure virtual methods defined by CStateMachineBase.
@see CStateMachineBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CNetLocStateMachine) : public CStateMachineBase, public MLbsCallbackTimerObserver
	{
public:

	/** Network Based Location specific states
	These are specific to Network Based Location and identify the current
	state within the Network Based Location protocol procedure.
	*/
	enum TNetLocState
		{
		/** Not valid */
		EStateNull,
		/** LBS Cient Network Based Loc request has been received */
		EStateClientReqRecvd,
		/** Network Network Based Loc session has started */
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
		/** Network NETLOC session about to close */
		EStateNetSessToClose,
		/** LBS client Network Based Loc session about to close */
		EStateClientSessToClose,
		/** Network and LBS sessions closed */
		EStateSessionsClosed,
		/** Protocol procedure is to be cancelled */
		EStateCancelling
		};

	/** Request identifiers used when a request is queued.
	*/
	enum TNetLocQueue
		{
		/** Location response received from LBS */
		EQueueLocResponse		 = 1000,
		/** Measurement control received from network */
		EQueueMeasurementControl = 2000,
		/** Network result received from network */
		EQueueNetworkResult		 = 3000,
		};

public:

	static CNetLocStateMachine* NewL(MStateMachineObserver& aObserver);
	virtual ~CNetLocStateMachine();
	
	CNetLocStateMachine::TNetLocState CurrentState();
	
	void StartNetLocReqTimer();
	void StartLocRespTimer();

	void NetLocReq(const TLbsNetSessionId& aSessionId);
	void QueueNetLocReq(const TLbsNetSessionId& aSessionId);

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

	CNetLocStateMachine(MStateMachineObserver& aObserver);
	
private:
	
	void ConstructL();
	void InitialiseProcedure();
	void CompleteProcedure();

protected:
	
	/** Current NETLOC state
	*/
	TNetLocState iCurrentState;
	
	/** Timer for NETLOC requests issued to network.
	*/
	CLbsCallbackTimer* iNetLocReqIssuedTimer;
	
	/** Timer for Location response expected by network
	*/
	CLbsCallbackTimer* iLocRespTimer;

	};

#endif // __CNETLOCSTATEMACHINE_H__

