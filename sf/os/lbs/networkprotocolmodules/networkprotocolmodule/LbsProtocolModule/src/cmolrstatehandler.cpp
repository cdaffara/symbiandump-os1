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
// the MO-LR protocol state handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cmolrstatemachine.h"
#include "cmolrstatehandler.h"


/** Static constructor.
@param aMachine A reference to the parent state machine.
@return A new instance of the CMoLrStateHandler class
*/  
CMoLrStateHandler* CMoLrStateHandler::NewL(CStateMachineBase& aMachine)
	{
	CMoLrStateHandler* self = new (ELeave) CMoLrStateHandler(aMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CMoLrStateHandler::CMoLrStateHandler(CStateMachineBase& aMachine)
: CStateHandlerBase(aMachine)
	{
	}


/** Standard destructor.
*/  
CMoLrStateHandler::~CMoLrStateHandler()
	{
	}


/** Private second-stage constructor.
*/  
void CMoLrStateHandler::ConstructL()
	{
	}


/** Initialise state attributes
*/
void CMoLrStateHandler::Initialise()
	{
	}

	
/** Perform entry actions.
This is called from the state machine to perform any actions
associated with entering the current state.
*/  
void CMoLrStateHandler::EntryActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CMoLrStateMachine& molrMachine = reinterpret_cast <CMoLrStateMachine&> (iMachine);
	switch(molrMachine.CurrentState())
		{

	case CMoLrStateMachine::EStateNull:
		break;

	case CMoLrStateMachine::EStateClientReqRecvd:
		// No action required
		break;

	case CMoLrStateMachine::EStateNetSessStarted:
		// Forward MO-LR request to network
		netError = molrMachine.Observer().Network()->MoLrReq(KNullDesC());
		// If this succeeded then start a timer for the anticipated response
		if (CNetworkInterface::ENone == netError)
			{
			molrMachine.StartMoLrReqTimer();
			}
		break;

	case CMoLrStateMachine::EStateMeasureDataRecvd:
		// No action required
		break;

	case CMoLrStateMachine::EStateNetBasedLocSent:
		// Send network location to gateway
		molrMachine.Observer().Gateway()->NetworkLocationInd(
							molrMachine.SessionId(),
							molrMachine.ReferenceLoc());
		break;

	case CMoLrStateMachine::EStateLocReqByNet:
		// Send location request to gateway
		molrMachine.Observer().Gateway()->LocationReq(
							molrMachine.SessionId(),
							EFalse, // this is NOT emergency related
							molrMachine.LocReqType(),
							molrMachine.LocReqQuality(),
							molrMachine.PosMethod());
		// Start a timer for the anticipated response
		molrMachine.StartLocRespTimer();
		break;

	case CMoLrStateMachine::EStateLocRespRecvd:
		// No action required
		break;

	case CMoLrStateMachine::EStateLocSentToNet:
		// Forward location response to network - but not if an error was reported
		if (KErrNone == molrMachine.LocRespReason())
			{
			netError = molrMachine.Observer().Network()->LocationResp(molrMachine.LocRespPosition());
			// If this succeeded then start a timer for the anticipated network result
			if (CNetworkInterface::ENone == netError)
				{
				molrMachine.StartFacResultTimer();
				}
			}
		break;

	case CMoLrStateMachine::EStateNetSessToClose:
		// If we had received a location request, but an error was reported then
		// forward this location error to the network
		if (molrMachine.IsLocReqReceived() && (KErrNone != molrMachine.LocRespReason()))
			{
			netError = molrMachine.Observer().Network()->LocationResp(molrMachine.LocRespReason());
			}
		break;

	case CMoLrStateMachine::EStateNetResultSent:
		// Send network result location to gateway
		molrMachine.Observer().Gateway()->NetworkLocationInd(
							molrMachine.SessionId(),
							molrMachine.NetResultLoc());
		break;

	case CMoLrStateMachine::EStateClientSessToClose:
		// No action required
		break;

	case CMoLrStateMachine::EStateSessionsClosed:
		// No action required
		break;

	case CMoLrStateMachine::EStateCancelling:
		// No action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleMoLrState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		molrMachine.Observer().NetworkErrorReported();
		}

	}
	
	
/** Perform exit actions.
This is called from the state machine to perform any actions
associated with exiting from the current state.
*/  
void CMoLrStateHandler::ExitActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CMoLrStateMachine& molrMachine = reinterpret_cast <CMoLrStateMachine&> (iMachine);
	switch(molrMachine.CurrentState())
		{

	case CMoLrStateMachine::EStateNull:
		// no action required
		break;

	case CMoLrStateMachine::EStateClientReqRecvd:
		// no action required
		break;

	case CMoLrStateMachine::EStateNetSessStarted:
		// no action required
		break;

	case CMoLrStateMachine::EStateMeasureDataRecvd:
		// no action required
		break;

	case CMoLrStateMachine::EStateNetBasedLocSent:
		// no action required
		break;

	case CMoLrStateMachine::EStateLocReqByNet:
		// no action required
		break;

	case CMoLrStateMachine::EStateLocRespRecvd:
		// no action required
		break;

	case CMoLrStateMachine::EStateLocSentToNet:
		// no action required
		break;

	case CMoLrStateMachine::EStateNetSessToClose:
		// Forward session completion to network if network is okay
		if (!molrMachine.IsNetworkConnectionError())
			{
			netError = molrMachine.Observer().Network()->MoLrCompleteInd(molrMachine.NetworkCloseReason());
			}
		break;

	case CMoLrStateMachine::EStateNetResultSent:
		// no action required
		break;

	case CMoLrStateMachine::EStateClientSessToClose:
		molrMachine.Observer().Gateway()->SessionCompleteInd(
						molrMachine.SessionId(),molrMachine.ClientCloseReason());
		break;

	case CMoLrStateMachine::EStateSessionsClosed:
		// no action required
		break;

	case CMoLrStateMachine::EStateCancelling:
		// no action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleMoLrState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		molrMachine.Observer().NetworkErrorReported();
		}

	}


