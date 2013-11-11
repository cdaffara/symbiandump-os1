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
// This file defines the base class and observer to allow a SUPL protocol
// to send/receive messages to/from positioning protocol state machines.
// 
//

/**
 @file
 @internalTechnology
 
*/

#ifndef CSUPLPOSITIONINGPROTOCOLFSM_H
#define CSUPLPOSITIONINGPROTOCOLFSM_H

#include <e32base.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbspositioninfo.h>
#include <lbs/lbsloccommon.h>
#include "lbstimer.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "suplpos.h"


class CStateHandlerBase;

/** Positioning Protocol observer mixin definition.
	This defines an interface to be implemented by an observer of the Positioning Protocol.
	The SUPL state machines implement this.
*/
class MSuplPositioningProtocolFsmObserver
	{
public:
		
	/** Pass assistance data to the observer.
	@return
	*/
	virtual void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason)= 0;
	
	/** Send a positioning request to the observer.
	*/
	virtual void ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality,
										   const TLbsNetPosRequestMethod& aPosMethod) = 0;

	/** Request from the observer that a SUPL POS is sent to the SLP with the provided payload
	*/						
	virtual void PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload) = 0;
	
	/** Inform the observer that there has been an error in the positioning protocol. Expected
	action is termination of the SUPL session via SUPL END.
	*/
	virtual void PositioningProtocolError(const TInt& aError) = 0;
	
	/** Inform observer that the Positioning session has finished **/
	virtual void PositioningSessionEnded() = 0;
	};


//-----------------------------------------------------------------------------
// Positioning Protocol State Machine Base Class
//-----------------------------------------------------------------------------

/** Base class definition for positioning protocol state machines.
This class provides support for common features of all state machines
of positioning protocols associated to SUPL.

A significant aspect incorporated in this base class is the active object
mechanism for performing state transitions. This feature is used to provide
state machines with the ability to perform autonomous and asynchronous actions
e.g. to receive a indication of positioning protocol messages arriving from the
network and to perform interactions with SUPL state machines.

The base class includes various attributes that describe protocol activity,
together with access methods used to retrieve the value of these attributes.
*/
class CSuplPositioningProtocolFsm : public CActive
	{
public:

	/** State machine protocol state.
	This defines the general protocol state for state machines.
	*/
	enum TPositioningProtocolState
		{
		/** Not valid */
		EStateNull,
		/** Ready to use, but not currently active. */
		EStateReady,
		/** Actively performing a protocol procedure.*/
		EStateActive,
		/** In the process of cancelling. */
		EStateCancelling
		};

	/** State machine cancel source.
	This defines the source of a cancellation.
	*/
	enum TCancelSource
		{
		/** Not cancelling */
		ECancelNone,
		/** The SUPL FSM cancelled the procedure */
		ESuplFsmCancel,
		/** A network error occurred */
		ECancelNetworkError,
		/** A network timeout occurred */
		ECancelNetworkTimeout,
		/** State machine is closing down (destruction)*/
		ECancelClosing
		};


public:
	
	virtual ~CSuplPositioningProtocolFsm();

	// CActive derived methods
	void RunL() = 0;
	void DoCancel() = 0;
	
	// Methods that must be overriden in derived classes
	virtual void ProcessPositioningMessage(CSuplPosPayload* aMessage) = 0;
	virtual void CancelMachine(const TCancelSource& aCancelSource, TInt aReason) = 0;
	virtual void AssistanceDataRequest(const TLbsAssistanceDataGroup& aMask) = 0;
	virtual void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo) = 0;
	virtual bool IsAssistDataRequestAllowed() = 0; 

protected:

	CSuplPositioningProtocolFsm(MSuplPositioningProtocolFsmObserver& aObserver);
	
protected:

	/** Reference to State machine observer
	*/
	MSuplPositioningProtocolFsmObserver& iObserver;
	};

#endif // CSUPLPOSITIONINGPROTOCOLFSM_H
