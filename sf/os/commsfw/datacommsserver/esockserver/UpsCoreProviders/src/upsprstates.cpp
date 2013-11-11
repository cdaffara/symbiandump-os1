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
// COREPRSTATES.CPP
// Core PR States
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/


#include <comms-infras/ss_log.h>
#include <comms-infras/upsprstates.h>
#include <comms-infras/upsmessages.h>
#include <elements/nm_interfaces.h>
using namespace ESock;
using namespace Messages;

// Support for User Prompt Service (TPolicyCheckRequest and TPolicyCheckResponse messages).

//
// States
//

EXPORT_DEFINE_SMELEMENT(UpsStates::TAwaitingPolicyCheckRequest, NetStateMachine::MState, UpsStates::TContext)
EXPORT_C TBool UpsStates::TAwaitingPolicyCheckRequest::Accept()
	{
	return iContext.iMessage.IsMessage<UpsMessage::TPolicyCheckRequest>();
	}


EXPORT_DEFINE_SMELEMENT(UpsStates::TAwaitingPolicyCheckResponse, NetStateMachine::MState, UpsStates::TContext)
EXPORT_C TBool UpsStates::TAwaitingPolicyCheckResponse::Accept()
	{
	return iContext.iMessage.IsMessage<UpsMessage::TPolicyCheckResponse>();
	}

//
// Transitions
//

EXPORT_DEFINE_SMELEMENT(UpsStates::TControlOrDataClientOriginator, NetStateMachine::MStateFork, UpsStates::TContext)
TInt UpsStates::TControlOrDataClientOriginator::TransitionTag()
    {
	TInt transitionTag = KUpsControlClientOriginator;
    
    RNodeInterface* client = iContext.Node().FindClient(iContext.iSender);
	if ((client)
		&& (client->Type() & TCFClientType::EData))
			{
			transitionTag = KUpsDataClientClientOriginator;
			}
    
    return transitionTag;
    }

//
// UPS Support
//

EXPORT_DEFINE_SMELEMENT(UpsStates::TPostToControlProvider, NetStateMachine::MStateTransition, UpsStates::TContext)
EXPORT_C void UpsStates::TPostToControlProvider::DoL()
/**
As TForwardToControlProvider, but set the message activity id to that of the current activity.

This transition was created for "stateful" request forwarding activities - i.e. that post the incoming
request message towards the control provider and remain active until a response is received.  Update
the activity id to avoid any response becoming stray.
*/
	{
	const Messages::RNodeInterface* const cp = iContext.Node().ControlProvider();
	
	if (cp == NULL)
		{
		User::Leave(KErrGeneral);
		}
		
	// Using PostRequestTo() ensures that the last request buffer (iPostedToId) has been set which,
	// in turn, ensures that TCancel messages will propagate as required.
	iContext.Activity()->PostRequestTo(*cp, iContext.iMessage);
	}

EXPORT_DEFINE_SMELEMENT(UpsStates::TPostToServiceProvider, NetStateMachine::MStateTransition, UpsStates::TContext)
EXPORT_C void UpsStates::TPostToServiceProvider::DoL()
/**
As TForwardToServiceProvider, but set the message activity id to that of the current activity.

This transition was created for "stateful" request forwarding activities - i.e. that post the incoming
request message towards the service provider and remain active until a response is received.  Update
the activity id to avoid any response becoming stray.
*/
	{
	const Messages::RNodeInterface* const cp = iContext.Node().ServiceProvider();

	if (cp == NULL)
		{
		User::Leave(KErrGeneral);
		}

	// Using PostRequestTo() ensures that the last request buffer (iPostedToId) has been set which,
	// in turn, ensures that TCancel messages will propagate as required.
	iContext.Activity()->PostRequestTo(*cp, iContext.iMessage);
	}


