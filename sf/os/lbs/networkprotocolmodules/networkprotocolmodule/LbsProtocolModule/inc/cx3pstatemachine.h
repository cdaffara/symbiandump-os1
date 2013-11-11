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
// This file defines the class for the X3P protocol state machine.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CX3PSTATEMACHINE_H__
#define __CX3PSTATEMACHINE_H__

#include "cstatemachinebase.h"


/** Maximum size of X3P destination string
*/
#define KMaxSizeX3pDestination 64


/** X3P protocol state machine.
This class manages the X3P protocol procedure and is employed when the protocol
module is required to handle interactions to transmit the mobile location 
to a third party.

The class implements the pure virtual methods defined by CStateMachineBase.
@see CStateMachineBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CX3pStateMachine) : public CStateMachineBase, public MLbsCallbackTimerObserver
	{
public:

	/** X3P specific states
	These are specific to X3P and identify the current state within the
	X3P self locate protocol procedure.
	*/
	enum TX3pState
		{
		/** Not valid */
		EStateNull,
		/** LBS Cient X3P request has been received */
		EStateClientReqRecvd,
		/** Network X3P session has started */
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
		/** Network X3P session about to close */
		EStateNetSessToClose,
		/** Network provided location sent to LBS */
		EStateNetResultSent,
		/** LBS client X3P session about to close */
		EStateClientSessToClose,
		/** Network and LBS sessions closed */
		EStateSessionsClosed,
		/** Protocol procedure is to be cancelled */
		EStateCancelling
		};

	/** Request identifiers used when a request is queued.
	*/
	enum TX3pQueue
		{
		/** Location response received from LBS */
		EQueueLocResponse		 = 100,
		/** Measurement control received from network */
		EQueueMeasurementControl = 200,
		/** Network result received from network */
		EQueueNetworkResult		 = 300,
		};

public:

	static CX3pStateMachine* NewL(MStateMachineObserver& aObserver);
	virtual ~CX3pStateMachine();
	
	CX3pStateMachine::TX3pState CurrentState();
	
	void StartX3pReqTimer();
	void StartLocRespTimer();
	void StartFacResultTimer();
	
	const TDesC& Destination() const;

	const TPositionInfoBase& NetResultLoc() const;
	
	void X3pReq(const TLbsNetSessionId& aNewSessionId, const TDesC& aDest);
	void QueueX3pReq(const TLbsNetSessionId& aNewSessionId, const TDesC& aDest);
	void SessionResult(TInt aResult, const TPositionInfoBase* aPosInfo);
    void ResponseSent();

	void SetStillCancelling(TBool aValue);

	// CStateMachineBase derived methods

	void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
							const TLbsNetPosRequestQuality& aQuality,
							const TLbsNetPosRequestMethod& aPosMethod);
	void MeasurementControlErrorInd(TInt aReason);
	void NetworkErrorInd();
	TInt ClientCloseReason() const;

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

	CX3pStateMachine(MStateMachineObserver& aObserver);
	
private:
	
	void ConstructL();
	void InitialiseProcedure();
	void CompleteProcedure();

	void StoreDestination(const TDesC& aDest);
	void QueueDestination(const TDesC& aDest);

	void StoreNetResultLoc(const TPositionInfoBase& aPosInfo);

protected:
	
	/** Current X3P state
	*/
	TX3pState iCurrentState;
	
	/** Timer for X3P requests issued to network.
	*/
	CLbsCallbackTimer* iX3pReqIssuedTimer;
	
	/** Timer for Facility result expected from network
	*/
	CLbsCallbackTimer* iFacResultTimer;
	
	/** Timer for Location response expected by network
	*/
	CLbsCallbackTimer* iLocRespTimer;

	/** Flag to indicate if network result location is available
	*/
	TBool iIsNetResultLocAvailable;
	
	/** Network Result Position info.
	*/
	TPositionInfo iNetResultPosInfo;

private:

	/** Destination for this transmit request
	*/
	TBuf<KMaxSizeX3pDestination> iDestination;

	/** Destination for a queued transmit request
	*/
	TBuf<KMaxSizeX3pDestination> iQueueDestination;

	/** Indicates that certain queued state changes should not take place
	*/
	TBool iStillCancelling;

	};

#endif // __CX3PSTATEMACHINE_H__

