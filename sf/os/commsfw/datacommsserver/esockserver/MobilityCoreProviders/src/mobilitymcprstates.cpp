// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// basic meta connection states and transitions
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalTechnology
*/


#include <comms-infras/ss_coreprstates.h>
#include "mobilitymcprstates.h"
#include <comms-infras/mobilitymcpractivities.h>
#include <comms-infras/ss_nodemessages_mobility.h>

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace CorePanics;
using namespace MobilityMCprStates;
using namespace MobilityMCprActivities;

//
//Panics
#ifdef _DEBUG
_LIT (KCoreMobileMCprPanic,"CoreMobileMCprPanic");
#endif

//-=========================================================
//
//Mutexes
//
//-=========================================================

//This mutex blocks when the mobility is handshaking on the node
TBool THandshakingMobilityMutex::IsBlocked(TNodeContextBase& aContext)
  	{
  	return static_cast<CMobilityMetaConnectionProvider&>(aContext.iNode).IsHandshakingNow();
   	}

//-=========================================================
//
//States
//
//-=========================================================

DEFINE_SMELEMENT(TAwaitingStartMobility, NetStateMachine::MState, MobilityMCprStates::TContext)
TBool TAwaitingStartMobility::Accept()
	{
	if (!iContext.iMessage.IsMessage<TCFMobilityProvider::TStartMobility>())
		{
		return EFalse;
		}
	if (iContext.Node().ServiceProvider()==NULL)
		{
		//There is no mobility on the lowest layer of the stack
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
	return ETrue;
	}

void MobilityMCprStates::TStateBase::Cancel()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));

	// Cancel availability
	RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
		TEBase::TCancel().CRef());
	}

DEFINE_SMELEMENT(TAwaitingMigrationRequestedOrRejected, NetStateMachine::MState, MobilityMCprStates::TContext)
TBool TAwaitingMigrationRequestedOrRejected::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>())
	    {
	    //The handshake is terminated with rejecting the currently offered bearer. This
	    //puts more stress on the MCPR. The MCPR has to offer another bearer otherwise the
	    //request to reject the bearer produces an error. Without the error the client
	    //rejecting the bearer won't know whether preferredcarrieravailable will follow
	    //or not. See CMobilityActivity::EvaluatePreference how this error is dealt with
	    iContext.Activity()->SetError(KErrNotFound);
	    return ETrue;
	    }
	return (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRequested>());
	}

DEFINE_SMELEMENT(TAwaitingMigrationAcceptedOrRejected, NetStateMachine::MState, MobilityMCprStates::TContext)
TBool TAwaitingMigrationAcceptedOrRejected::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>())
	    {
	    //The handshake is terminated with rejecting the currently offered bearer. This
	    //puts more stress on the MCPR. The MCPR has to offer another bearer otherwise the
	    //request to reject the bearer produces an error. Without the error the client
	    //rejecting the bearer won't know whether preferredcarrieravailable will follow
	    //or not. See CMobilityActivity::EvaluatePreference how this error is dealt with 	
	    iContext.Activity()->SetError(KErrNotFound);
	    return ETrue;
	    }
	return (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationAccepted>());
	}

//-=========================================================
//
//Transitions
//
//-=========================================================
DEFINE_SMELEMENT(TNoTagOrErrorTagIfMobilityRunning, NetStateMachine::MStateFork, MobilityMCprStates::TContext)
TInt TNoTagOrErrorTagIfMobilityRunning::TransitionTag()
    {
    if (iContext.Node().CountActivities(ECFActivityMCprMobility))
        {
        return KErrorTag;
        }
    return KNoTag;
    }

//-=========================================================
//
//Transitions
//
//-=========================================================

DEFINE_SMELEMENT(TReplyMobilityStarted, NetStateMachine::MStateTransition, MobilityMCprStates::TContext)
void TReplyMobilityStarted::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileMCprPanic, KPanicNoActivity));
	iContext.iNodeActivity->PostToOriginators(TCFMobilityProvider::TMobilityStarted().CRef());
	iContext.iNodeActivity->ClearPostedTo();
	}


