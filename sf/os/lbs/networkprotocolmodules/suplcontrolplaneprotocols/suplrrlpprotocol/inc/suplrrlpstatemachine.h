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
// csuplrrlpfsm.h
// This file defines the class for the RRLP protocol state machine.
// 
//

/**
 @file
 @internalTechnology

*/

#ifndef __CSUPLRRLPFSM_H__
#define __CSUPLRRLPFSM_H__

#include <lbspositioninfo.h>

#include "rrlpmessagebase.h"
#include "rrlpmeasureposrequest.h"
#include "rrlpprotocolerror.h"
#include "suplpositioningprotocolfsm.h"

// Forwards
class CRrlpAssistanceData;
class CRrlpAssistanceDataAck;

/**  This class implementes the RRLP protocol.

Only those messages and parameters required by SUPL for AGPS positioning
are supported. Optional parameters not related to AGPS are not implemented.

@see CStateMachineBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
class CSuplRrlpFsm : public CSuplPositioningProtocolFsm, public MLbsCallbackTimerObserver
	{
public:
	// Constructors
	IMPORT_C static CSuplRrlpFsm* NewL(MSuplPositioningProtocolFsmObserver& aObserver,RLbsAssistanceDataBuilderSet& aDataBuilder);
	virtual ~CSuplRrlpFsm();
	
	// Methods derived from base class (must be implemented)
	IMPORT_C void ProcessPositioningMessage(CSuplPosPayload* aMessage);
	IMPORT_C void CancelMachine(const TCancelSource& aCancelSource, TInt aReason);
	IMPORT_C void AssistanceDataRequest(const TLbsAssistanceDataGroup& aMask);
	IMPORT_C void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	IMPORT_C bool IsAssistDataRequestAllowed();

protected:
	// CActive
	void RunL();
	void DoCancel();
	
private:
	/** RRLP states (when used with SUPL)
	These identify the current state within the
	RRLP procedure.
	*/
	enum TRrlpState
		{
		/** Not valid */
		EStateNull,
		/** Assistance Data Message received (more on the way)*/
		EStateAssitDataChunkRecvd,
		/** Assitance Data Message received (no more assistance data messages on the way) */
		EStateAssitDataAcknowledged,
		/** Measure Position Request message received from the network */
		EStateMeasureReqRecvd,
		/** A positioning request has been sent to SUPL */
		EStateRequestHandled,
		/** A request for additional Assistance Data from LBS is sent to the network */
		EStateMoreAssistDataRequested,
		/** Location response sent to network */
		EStateMeasureRespSent,
		/** An error or a cancel has occured */
		EStateErrorRecvd
		};
	
	/** Timer ID's
	 */
	enum TTimerIds
		{
		EAssitanceDataChunk,
		ERequest,
		EResponseDelay,
		EMeasureRequestWithAssistanceDataDelay
		};
private:
	// Constructor
	CSuplRrlpFsm(MSuplPositioningProtocolFsmObserver& aObserver, RLbsAssistanceDataBuilderSet& aDataBuilder);
	void ConstructL();

	// MLbsCallbackTimerObserver methods
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

	// Protocol Timers.
	void StartAssitanceDataChunkTimer();
	void ReceivedAssistanceDataChunkTimer();
	void StartRequestTimer();   
	void ReceivedRequestTimer();
	void StartPosResultTimer(TTimeIntervalMicroSeconds aMicroSeconds); 
	void ReceivedPosResultTimer();
	// Other timers
	void StartMeasureRequestWithAssitanceDataTimer(TInt aTime);
	void ReceivedMeasureRequestWithAssitanceDataTimer();
	
	// Handler functions
	void HandleAssistanceDataMessage(CRrlpAssistanceData* aData);
	void HandleMeasurementPositionRequest(CRrlpMeasurePositionRequest* aData);
	void HandleProtocolError(CRrlpProtocolError* aError);
	
	// Others
	TBool TransistionTo(TRrlpState aState);
	void TransistionToError(TInt aError);
	void AddReference(CRrlpMessageBase& aMessage);
private:
	/** Current RRLP state
	*/
	TRrlpState iCurrentState;
	TRrlpState iNextState;
	
	/** Last reference number received
	*/
	TRrlpReference iLastReferenceNumber;
	
	/** Timer in between RRLP messages
		(it is a protocol error when this timer expires and more assistance data is expected)
	*/
	static const TInt KAssistanceDataChunkTimeout; 
	CLbsCallbackTimer* iAssistanceDataChunkTimer;
	
	/** Timer for RRLP Measure Position Request after Assitance Data message
		(it is not always a protocol error when this timer expires)
	*/
	const static TInt KRequestTimeout; 
	CLbsCallbackTimer* iRequestTimer;
	
	/** Timer for overseeing that LBS responds within the expected delay
	    (the delay is specified in the RRLP Measure Position Request, this
	    timer lasts longer to give LBS's response a chance to arrive before expiry)
	*/
	const static TInt KResponseDelayAddition;
	const static TInt KResponseDelayTimeout; 
	CLbsCallbackTimer* iResponseDelayTimer;
	
	/** Timer for allowing assistance data to be processed by LBS if received
	    in the Measurement Request
	 */
	const static TInt KMeasureRequestWithAssistanceDataDelay;
	CLbsCallbackTimer* iMeasureRequestWithAssitanceDataDelayTimer;
	
	/** Flag to show location request has been sent to LBS.
	*/
	TBool iIsLocReqSentToLbs;
	
	/** Location request quality.
	*/
	TLbsNetPosRequestQuality iLocReqQuality;

	/** Location request positioning method.
	*/
	TLbsNetPosRequestMethod iPosMethod;
	
	/** Assistance data 
	 */
	RLbsAssistanceDataBuilderSet& iAssistanceData;
	
	/** Assistance data mask
	 */
	TLbsAssistanceDataGroup iAssistanceDataMask;

	};

#endif // __CSUPLRRLPFSM_H__

