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
// the MT-LR protocol state handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cmtlrstatemachine.h"
#include "cmtlrstatehandler.h"


/** Static constructor.
@param aMachine A reference to the parent state machine.
@return A new instance of the CMtLrStateHandler class
*/  
CMtLrStateHandler* CMtLrStateHandler::NewL(CStateMachineBase& aMachine)
	{
	CMtLrStateHandler* self = new (ELeave) CMtLrStateHandler(aMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CMtLrStateHandler::CMtLrStateHandler(CStateMachineBase& aMachine)
: CStateHandlerBase(aMachine)
	{
	}


/** Standard destructor.
*/  
CMtLrStateHandler::~CMtLrStateHandler()
	{
	}


/** Private second-stage constructor.
*/  
void CMtLrStateHandler::ConstructL()
	{
	}


/** Initialise state attributes
*/
void CMtLrStateHandler::Initialise()
	{
	}

	
/** Perform entry actions.
This is called from the state machine to perform any actions
associated with entering the current state.
*/  
void CMtLrStateHandler::EntryActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CMtLrStateMachine& mtlrMachine = reinterpret_cast <CMtLrStateMachine&> (iMachine);
	switch(mtlrMachine.CurrentState())
		{

	case CMtLrStateMachine::EStateNull:
		// No action required
		break;

	case CMtLrStateMachine::EStateNetReqRecvd:
		// No action required
		break;

	case CMtLrStateMachine::EStatePrivacySessStarted:
		// Forward request to gateway
		mtlrMachine.Observer().Gateway()->PrivacyReq(
										mtlrMachine.SessionId(),
										mtlrMachine.IsEmergency(),
										mtlrMachine.PrivacyReq(),
										mtlrMachine.PrivacyReqInfo());
		break;

	case CMtLrStateMachine::EStatePrivacyRespRecvd:
		// Forward response to network
		mtlrMachine.Observer().Network()->MtLrResp(mtlrMachine.PrivacyResponseValue());
		break;

	case CMtLrStateMachine::EStatePrivacyRespRecvdAfterMeasure:
		// Forward response to network
		mtlrMachine.Observer().Network()->MtLrResp(mtlrMachine.PrivacyResponseValue());
		break;

	case CMtLrStateMachine::EStatePrivacyReject:
		// no action required
		break;

	case CMtLrStateMachine::EStatePrivacyRejectAfterMeasure:
		// no action required
		break;

	case CMtLrStateMachine::EStateMeasureDataRecvd:
		// No action required
		break;

	case CMtLrStateMachine::EStateNetBasedLocSent:
		// Send network location to gateway
		mtlrMachine.Observer().Gateway()->NetworkLocationInd(
							mtlrMachine.SessionId(),
							mtlrMachine.ReferenceLoc());
		break;

	case CMtLrStateMachine::EStateLocReqByNet:
		// Send location request to gateway
		mtlrMachine.Observer().Gateway()->LocationReq(
							mtlrMachine.SessionId(),
							mtlrMachine.IsEmergency(),
							mtlrMachine.LocReqType(),
							mtlrMachine.LocReqQuality(),
							mtlrMachine.PosMethod());
		// Start a timer for the anticipated response
		mtlrMachine.StartLocRespTimer();
		break;

	case CMtLrStateMachine::EStateLocRespRecvd:
		// No action required
		break;

	case CMtLrStateMachine::EStateLocSentToNet:
		// If a location response does not indicate an error then forward it to network
		if (KErrNone == mtlrMachine.LocRespReason())
			{
			netError = mtlrMachine.Observer().Network()->LocationResp(mtlrMachine.LocRespPosition());
			}
		break;

	case CMtLrStateMachine::EStateClientSessToCloseWaitMeasure:
		// Send session completion data to gateway
		mtlrMachine.Observer().Gateway()->SessionCompleteInd(mtlrMachine.SessionId(), 
															mtlrMachine.ClientCloseReason());
		break;

	case CMtLrStateMachine::EStateClientSessToCloseAfterMeasure:
		// Send session completion data to gateway
		mtlrMachine.Observer().Gateway()->SessionCompleteInd(mtlrMachine.SessionId(), 
															mtlrMachine.ClientCloseReason());
		break;

	case CMtLrStateMachine::EStateClientSessToCloseWaitLocResp:
		// Send session completion data to gateway
		mtlrMachine.Observer().Gateway()->SessionCompleteInd(mtlrMachine.SessionId(), 
															mtlrMachine.ClientCloseReason());
		break;

	case CMtLrStateMachine::EStateSessionsClosed:
		// If a location response error has been reported then forward it to network
		if (KErrNone != mtlrMachine.LocRespReason())
			{
			netError = mtlrMachine.Observer().Network()->LocationResp(mtlrMachine.LocRespReason());
			}
		break;

	case CMtLrStateMachine::EStateCancelling:
		// Is privacy response handled yet?
		if (!mtlrMachine.IsPrivacyResponseHandled())
			{
			mtlrMachine.Observer().Network()->MtLrResp(mtlrMachine.PrivacyResponseValue());
			}
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleMtLrState);
		break;
		}

	// Handle network-related error
	if (CNetworkInterface::ENone != netError)
		{
		mtlrMachine.Observer().NetworkErrorReported();
		}

	}
	
	
/** Perform exit actions.
This is called from the state machine to perform any actions
associated with exiting from the current state.
*/  
void CMtLrStateHandler::ExitActions()
	{
	CNetworkInterface::TNetworkError netError = CNetworkInterface::ENone;

	// Retrieve current state and act accordingly
	CMtLrStateMachine& mtlrMachine = reinterpret_cast <CMtLrStateMachine&> (iMachine);
	switch(mtlrMachine.CurrentState())
		{

	case CMtLrStateMachine::EStateNull:
		// no action required
		break;

	case CMtLrStateMachine::EStateNetReqRecvd:
		// no action required
		break;

	case CMtLrStateMachine::EStatePrivacySessStarted:
		break;

	case CMtLrStateMachine::EStatePrivacyRespRecvd:
		// no action required
		break;

	case CMtLrStateMachine::EStatePrivacyRespRecvdAfterMeasure:
		// no action required
		break;

	case CMtLrStateMachine::EStatePrivacyReject:
		// no action required
		break;

	case CMtLrStateMachine::EStatePrivacyRejectAfterMeasure:
		// no action required
		break;

	case CMtLrStateMachine::EStateMeasureDataRecvd:
		// no action required
		break;

	case CMtLrStateMachine::EStateNetBasedLocSent:
		// no action required
		break;

	case CMtLrStateMachine::EStateLocReqByNet:
		// no action required
		break;

	case CMtLrStateMachine::EStateLocRespRecvd:
		// no action required
		break;

	case CMtLrStateMachine::EStateLocSentToNet:
		// no action required
		break;

	case CMtLrStateMachine::EStateClientSessToCloseWaitMeasure:
		// no action required
		break;

	case CMtLrStateMachine::EStateClientSessToCloseAfterMeasure:
		// no action required
		break;

	case CMtLrStateMachine::EStateClientSessToCloseWaitLocResp:
		// no action required
		break;

	case CMtLrStateMachine::EStateSessionsClosed:
		// no action required
		break;

	case CMtLrStateMachine::EStateCancelling:
		// no action required
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleMtLrState);
		break;
		}

	// Handle network-related error
	// this function is stubbed for future expansion
	// coverity [dead_error_condition]
	if (CNetworkInterface::ENone != netError)
		{
		// coverity [dead_error_line]
		mtlrMachine.Observer().NetworkErrorReported();
		}


	}


