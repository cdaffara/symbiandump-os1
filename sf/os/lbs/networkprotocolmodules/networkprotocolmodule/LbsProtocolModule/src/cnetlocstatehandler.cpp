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
// the Network Based Location protocol state handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cnetlocstatemachine.h"
#include "cnetlocstatehandler.h"
#include "lbsdevloggermacros.h"

/** Static constructor.
@param aMachine A reference to the parent state machine.
@return A new instance of the CNetLocStateHandler class
*/  
CNetLocStateHandler* CNetLocStateHandler::NewL(CStateMachineBase& aMachine)
	{
	CNetLocStateHandler* self = new (ELeave) CNetLocStateHandler(aMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CNetLocStateHandler::CNetLocStateHandler(CStateMachineBase& aMachine)
: CStateHandlerBase(aMachine)
	{
	}


/** Standard destructor.
*/  
CNetLocStateHandler::~CNetLocStateHandler()
	{
	}


/** Private second-stage constructor.
*/  
void CNetLocStateHandler::ConstructL()
	{
	}


/** Initialise state attributes
*/
void CNetLocStateHandler::Initialise()
	{
	}

	
/** Perform entry actions.
This is called from the state machine to perform any actions
associated with entering the current state.
*/  
void CNetLocStateHandler::EntryActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CNetLocStateMachine& netlocMachine = reinterpret_cast <CNetLocStateMachine&> (iMachine);
	switch(netlocMachine.CurrentState())
		{

	case CNetLocStateMachine::EStateNull:
		break;

	case CNetLocStateMachine::EStateClientReqRecvd:
		// No action required
		break;

	case CNetLocStateMachine::EStateNetSessStarted:
		// Forward Network Based Location request to network
		netError = netlocMachine.Observer().Network()->MoLrReq(KNullDesC());
		// If this succeeded then start a timer for the anticipated response
		if (CNetworkInterface::ENone == netError)
			{
			netlocMachine.StartNetLocReqTimer();
			}
		break;

	case CNetLocStateMachine::EStateMeasureDataRecvd:
		// No action required
		break;

	case CNetLocStateMachine::EStateNetBasedLocSent:
		// Send network location to gateway
		netlocMachine.Observer().Gateway()->NetworkLocationInd(
							netlocMachine.SessionId(),
							netlocMachine.ReferenceLoc());
		break;

	case CNetLocStateMachine::EStateLocReqByNet:
		// Send location request to gateway
		netlocMachine.Observer().Gateway()->LocationReq(
							netlocMachine.SessionId(),
							EFalse, // this is NOT emergency related
							netlocMachine.LocReqType(),
							netlocMachine.LocReqQuality(),
							netlocMachine.PosMethod());
		// Start a timer for the anticipated response
		netlocMachine.StartLocRespTimer();
		break;

	case CNetLocStateMachine::EStateLocRespRecvd:
		// No action required
		break;

	case CNetLocStateMachine::EStateLocSentToNet:
		// Forward location response to network - but not if an error was reported
		if (KErrNone == netlocMachine.LocRespReason())
			{
			netError = netlocMachine.Observer().Network()->LocationResp(netlocMachine.LocRespPosition());
			}
		break;

	case CNetLocStateMachine::EStateClientSessToClose:
		// No action required
		break;

	case CNetLocStateMachine::EStateNetSessToClose:
		// If we had received a location request, but an error was reported then
		// forward this location error to the network
		if (netlocMachine.IsLocReqReceived() && (KErrNone != netlocMachine.LocRespReason()))
			{
			netError = netlocMachine.Observer().Network()->LocationResp(netlocMachine.LocRespReason());
			}
		break;

	case CNetLocStateMachine::EStateSessionsClosed:
		// No action required
		break;

	case CNetLocStateMachine::EStateCancelling:
		// No action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleNetLocState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		netlocMachine.Observer().NetworkErrorReported();
		}

	}
	
	
/** Perform exit actions.
This is called from the state machine to perform any actions
associated with exiting from the current state.
*/  
void CNetLocStateHandler::ExitActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CNetLocStateMachine& netlocMachine = reinterpret_cast <CNetLocStateMachine&> (iMachine);
	switch(netlocMachine.CurrentState())
		{

	case CNetLocStateMachine::EStateNull:
		// no action required
		break;

	case CNetLocStateMachine::EStateClientReqRecvd:
		// no action required
		break;

	case CNetLocStateMachine::EStateNetSessStarted:
		// no action required
		break;

	case CNetLocStateMachine::EStateMeasureDataRecvd:
		// no action required
		break;

	case CNetLocStateMachine::EStateNetBasedLocSent:
		// no action required
		break;

	case CNetLocStateMachine::EStateLocReqByNet:
		// no action required
		break;

	case CNetLocStateMachine::EStateLocRespRecvd:
		// no action required
		break;

	case CNetLocStateMachine::EStateLocSentToNet:
		// no action required
		break;

	case CNetLocStateMachine::EStateClientSessToClose:
		// Send a session completion to the gateway
		LBSLOG(ELogP2, "CNetLocStateHandler::ExitActions() Completing Session \n");
		netlocMachine.Observer().Gateway()->SessionCompleteInd(
							netlocMachine.SessionId(),netlocMachine.ClientCloseReason());
		break;

	case CNetLocStateMachine::EStateNetSessToClose:
		// Forward session completion to network if network is okay
		if (!netlocMachine.IsNetworkConnectionError())
			{
			netError = netlocMachine.Observer().Network()->MoLrCompleteInd(netlocMachine.NetworkCloseReason());
			}
		break;

	case CNetLocStateMachine::EStateSessionsClosed:
		// no action required
		break;

	case CNetLocStateMachine::EStateCancelling:
		// no action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleNetLocState);
		break;
		}

	// Handle any network-related error
	if (CNetworkInterface::ENone != netError)
		{
		netlocMachine.Observer().NetworkErrorReported();
		}

	}


