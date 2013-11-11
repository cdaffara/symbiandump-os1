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
// This file provides the implementation of the class for
// the X3P protocol state handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cx3pstatemachine.h"
#include "cx3pstatehandler.h"


/** Static constructor.
@param aMachine A reference to the parent state machine.
@return A new instance of the CX3pStateHandler class
*/  
CX3pStateHandler* CX3pStateHandler::NewL(CStateMachineBase& aMachine)
	{
	CX3pStateHandler* self = new (ELeave) CX3pStateHandler(aMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CX3pStateHandler::CX3pStateHandler(CStateMachineBase& aMachine)
: CStateHandlerBase(aMachine)
	{
	}


/** Standard destructor.
*/  
CX3pStateHandler::~CX3pStateHandler()
	{
	}


/** Private second-stage constructor.
*/  
void CX3pStateHandler::ConstructL()
	{
	}


/** Initialise state attributes
*/
void CX3pStateHandler::Initialise()
	{
	}

	
/** Perform entry actions.
This is called from the state machine to perform any actions
associated with entering the current state.
*/  
void CX3pStateHandler::EntryActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CX3pStateMachine& x3pMachine = reinterpret_cast <CX3pStateMachine&> (iMachine);
	switch(x3pMachine.CurrentState())
		{

	case CX3pStateMachine::EStateNull:
		break;

	case CX3pStateMachine::EStateClientReqRecvd:
		// No action required
		break;

	case CX3pStateMachine::EStateNetSessStarted:
		// Forward X3P request to network
		netError = x3pMachine.Observer().Network()->MoLrReq(x3pMachine.Destination());
		// If this succeeded then start a timer for the anticipated response
		if (CNetworkInterface::ENone == netError)
			{
			x3pMachine.StartX3pReqTimer();
			}
		break;

	case CX3pStateMachine::EStateMeasureDataRecvd:
		// No action required
		break;

	case CX3pStateMachine::EStateNetBasedLocSent:
		// Send network location to gateway
		x3pMachine.Observer().Gateway()->NetworkLocationInd(
							x3pMachine.SessionId(),
							x3pMachine.ReferenceLoc());
		break;

	case CX3pStateMachine::EStateLocReqByNet:
		// Send location request to gateway
		x3pMachine.Observer().Gateway()->LocationReq(
							x3pMachine.SessionId(),
							EFalse, // this is NOT emergency related
							x3pMachine.LocReqType(),
							x3pMachine.LocReqQuality(),
							x3pMachine.PosMethod());
		// Start a timer for the anticipated response
		x3pMachine.StartLocRespTimer();
		break;

	case CX3pStateMachine::EStateLocRespRecvd:
		// No action required
		break;

	case CX3pStateMachine::EStateLocSentToNet:
		// Forward location response to network - but not if an error was reported
		if (KErrNone <= x3pMachine.LocRespReason())
			{
			netError = x3pMachine.Observer().Network()->LocationResp(x3pMachine.LocRespPosition());
			
			if (CNetworkInterface::ENone == netError)
				{
				x3pMachine.StartFacResultTimer();
				x3pMachine.ResponseSent();
				}						
			}
		break;

	case CX3pStateMachine::EStateNetSessToClose:
		// If we had received a location request, but an error or cancellation was
		// reported, forward this location error to the network (if response not sent yet)
		if (x3pMachine.IsLocReqReceived() && (KErrNone > x3pMachine.LocRespReason()) &&
			(!x3pMachine.IsLocRespSent()))
			{
			netError = x3pMachine.Observer().Network()->LocationResp(x3pMachine.LocRespReason());
			x3pMachine.ResponseSent();
			}
		break;
		
	case CX3pStateMachine::EStateNetResultSent:
		// Send network result location to gateway if no error or cancellation ocurred in LBS.
		if (x3pMachine.IsLocRespSent() && (KErrNone <= x3pMachine.LocRespReason()))
		{
				
		x3pMachine.Observer().Gateway()->NetworkLocationInd(
							x3pMachine.SessionId(),
							x3pMachine.NetResultLoc());
		}
	break;
	
	case CX3pStateMachine::EStateClientSessToClose:
		// No action required
	break;

	case CX3pStateMachine::EStateSessionsClosed:
		// No action required
		break;

	case CX3pStateMachine::EStateCancelling:
		// No action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleX3pState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		x3pMachine.Observer().NetworkErrorReported();
		}

	}
	
	
/** Perform exit actions.
This is called from the state machine to perform any actions
associated with exiting from the current state.
*/  
void CX3pStateHandler::ExitActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CX3pStateMachine& x3pMachine = reinterpret_cast <CX3pStateMachine&> (iMachine);
	switch(x3pMachine.CurrentState())
		{

	case CX3pStateMachine::EStateNull:
		x3pMachine.SetStillCancelling(EFalse);
		break;

	case CX3pStateMachine::EStateClientReqRecvd:
		// no action required
		break;

	case CX3pStateMachine::EStateNetSessStarted:
		// no action required
		break;

	case CX3pStateMachine::EStateMeasureDataRecvd:
		// no action required
		break;

	case CX3pStateMachine::EStateNetBasedLocSent:
		// no action required
		break;

	case CX3pStateMachine::EStateLocReqByNet:
		// no action required
		break;

	case CX3pStateMachine::EStateLocRespRecvd:
		// no action required
		break;

	case CX3pStateMachine::EStateLocSentToNet:
		// no action required
		break;

	case CX3pStateMachine::EStateClientSessToClose:
		// Send a session completion to the gateway BUT not if we are
		// silently cancelling the client session
		if (!x3pMachine.IsSilentClientCancel())
			{
			x3pMachine.Observer().Gateway()->SessionCompleteInd(
								x3pMachine.SessionId(),x3pMachine.ClientCloseReason());
			}
		break;

	case CX3pStateMachine::EStateNetSessToClose:
		// Forward session completion to network if network is okay
		if (!x3pMachine.IsNetworkConnectionError())
			{
			netError = x3pMachine.Observer().Network()->MoLrCompleteInd(x3pMachine.NetworkCloseReason());
			}
		break;
		
	case CX3pStateMachine::EStateNetResultSent:
		// no action required
		break;
		
	case CX3pStateMachine::EStateSessionsClosed:
		// no action required
		break;

	case CX3pStateMachine::EStateCancelling:
		// Ensure that certain state transitions are prohibited when cancelling.
		x3pMachine.SetStillCancelling(ETrue);
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleX3pState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		x3pMachine.Observer().NetworkErrorReported();
		}

	}


