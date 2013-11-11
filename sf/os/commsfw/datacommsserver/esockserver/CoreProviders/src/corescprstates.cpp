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
// Core SCPR States
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/

#include "corescprstates.h"

#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_subconn.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvSCPRSC, "ESockCrPrvSCPRSC");
#endif

// Remove the "#if 0" when you need the logging subtag
#if 0
#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	#define KCoreSCPRTag KESockConnectionTag
	_LIT8(KCoreSCPRSubTag, "corescpr");
#endif
#endif

using namespace SCprStates;
using namespace SCprActivities;
using namespace NetStateMachine;
using namespace ESock;
using namespace CorePanics;
using namespace Messages;
using namespace MeshMachine;


//-=========================================================
//
// Panics
//
//-=========================================================
_LIT (KCoreSCPRPanic,"CoreSCPRPanic");

void SCPRPanic(TInt aCode)
	{
	User::Panic(KCoreSCPRPanic, aCode);
	}


//-=========================================================
//
//Start/Stop subconnection
//
//-=========================================================

EXPORT_DEFINE_SMELEMENT(SCprStates::TSendStopped, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TSendStopped::DoL()
	{
	if (iContext.iNodeActivity)
    	{
    	iContext.iNodeActivity->PostToOriginators(TCFServiceProvider::TStopped(KErrCancel).CRef());
    	}
    else
        {
        iContext.PostToSender(TCFServiceProvider::TStopped(KErrCancel).CRef());
        }
	}

EXPORT_DEFINE_SMELEMENT(SCprStates::TSendStopSubConnection, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TSendStopSubConnection::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, SCPRPanic(KPanicNoActivity));
	RNodeInterface* sp = iContext.Node().ServiceProvider();
	__ASSERT_DEBUG(sp != NULL, SCPRPanic(KPanicNoServiceProvider));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt);

	TCFServiceProvider::TStop* msg1 = message_cast<TCFServiceProvider::TStop>(&iContext.iMessage);
	TCFDataClient::TStop* msg2 = message_cast<TCFDataClient::TStop>(&iContext.iMessage);

	TInt stopCode = msg1 ? msg1->iValue : msg2 ? msg2->iValue : KErrCancel;


    iContext.iNodeActivity->PostRequestTo(*sp, TCFServiceProvider::TStop(stopCode).CRef());
	}

//-=========================================================
//
//Start/Stop Flow
//
//-=========================================================
EXPORT_DEFINE_SMELEMENT(TSendDataClientGoneDown, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TSendDataClientGoneDown::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, SCPRPanic(KPanicNoActivity));

	TInt stopCode;
	if (iContext.iMessage.IsMessage<TCFControlProvider::TDataClientGoneDown>())
		{
	    stopCode = message_cast<TCFControlProvider::TDataClientGoneDown>(iContext.iMessage).iValue1;
		}
    else if (iContext.iMessage.IsMessage<TCFDataClient::TStopped>())
        {
        stopCode = message_cast<TCFDataClient::TStopped>(iContext.iMessage).iValue;
        }
    else
        {
        stopCode = KErrCancel;
        }

	RNodeInterface* cp = iContext.Node().ControlProvider();
    __ASSERT_DEBUG(cp, SCPRPanic(KPanicNoControlProvider));
	cp->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFControlProvider::TDataClientGoneDown(stopCode).CRef());
	}

EXPORT_DEFINE_SMELEMENT(TStopYourFlows, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TStopYourFlows::DoL()
	{
	MeshMachine::CNodeActivityBase *activity = iContext.iNodeActivity;
	__ASSERT_DEBUG(activity, SCPRPanic(KPanicNoActivity));
	__ASSERT_DEBUG(NULL != iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TCFClientType::EData), SCPRPanic(KPanicIncorrectMessage));

	// If we were initiated by a TCFDataClient::TStop, then extract the stop code from it else use KErrCancel
	TCFDataClient::TStop* msg = message_cast<TCFDataClient::TStop>(&iContext.iMessage);
	TInt stopCode = msg ? msg->iValue : iContext.iNodeActivity->Error() != KErrNone ? iContext.iNodeActivity->Error() : KErrCancel;

	// Loop around all started data clients issuing a TCFDataClient::TStop to them.
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted));
	RNodeInterface* dc;
	while ((dc = iter++) != NULL)
		{
		activity->PostRequestTo(*dc, TCFDataClient::TStop(stopCode).CRef());
		}
	// Set the "SentTo" address to null so that we can receive a response from all clients (rather than the last one).
	activity->ClearPostedTo();
	}

EXPORT_DEFINE_SMELEMENT(TStopSendingFlow, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TStopSendingFlow::DoL()
	{
  	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 1));
  	__ASSERT_DEBUG(iContext.iPeer->Type() == TCFClientType::EData, User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 2));
  	iContext.iPeer->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFDataClient::TStop(KErrCancel).CRef());
	}

//-=========================================================
//
//Set Parameters
//
//-=========================================================
EXPORT_DEFINE_SMELEMENT(TPassEventToControlClients, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TPassEventToControlClients::DoL()
	{
	TCFSubConnControlClient::TSubConnNotification& event = message_cast<TCFSubConnControlClient::TSubConnNotification>(iContext.iMessage);

   	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
    RNodeInterface* ctl;
	while ((ctl = iter++) != NULL)
		{
		event.iRefCountOwnedSubConNotification->Open();
		ctl->PostMessage(iContext.NodeId(), event);
		}

	// This Close() matches the Open performed by the NotifyClientsL() method
    event.iRefCountOwnedSubConNotification->Close();
	}

EXPORT_DEFINE_SMELEMENT(TPassToServiceProvider, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TPassToServiceProvider::DoL()
	{
	//If you don't have the service provider, don't use this transition.
	__ASSERT_DEBUG(iContext.Node().ServiceProvider(), User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 3));
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 4));
  	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(), iContext.iMessage);
	}

EXPORT_DEFINE_SMELEMENT(TStoreParams, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TStoreParams::DoL()
	{
#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if (iContext.Node().CountActivities(ECFActivityApplyChanges))
    	{
    	User::Leave(KErrInUse);
    	}

	TCFScpr::TParamsRequest* setParamsRequest = message_cast<TCFScpr::TParamsRequest>(&iContext.iMessage);
	TCFScpr::TParamsResponse* setParamsResponse = message_cast<TCFScpr::TParamsResponse>(&iContext.iMessage);
	__ASSERT_DEBUG(setParamsRequest || setParamsResponse, User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 5));

	RCFParameterFamilyBundleC newParamBundle = setParamsRequest ? setParamsRequest->iFamilyBundle : setParamsResponse->iFamilyBundle;

	if ( ! newParamBundle.IsNull()
		 && (iContext.Node().iParameterBundle.IsNull() ||
			 iContext.Node().iParameterBundle != newParamBundle))
    	{
    	if ( ! iContext.Node().iParameterBundle.IsNull())
        	{
        	iContext.Node().iParameterBundle.Close();
        	}

    	iContext.Node().iParameterBundle.Open(newParamBundle);
    	}
#endif
	}

EXPORT_DEFINE_SMELEMENT(TRespondWithCurrentParams, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TRespondWithCurrentParams::DoL()
	{
#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFScpr::TParamsRequest& paramRequest = message_cast<TCFScpr::TParamsRequest>(iContext.iMessage);
	if ( (! paramRequest.iFamilyBundle.IsNull()) && iContext.Node().iParameterBundle != paramRequest.iFamilyBundle )
    	{
    	//default behaviour. Parameters where supplied, though haven't been processed
    	User::Leave(KErrNotSupported);
    	}
	iContext.PostToSender(TCFScpr::TParamsResponse(iContext.Node().iParameterBundle).CRef());
#endif
	}

EXPORT_DEFINE_SMELEMENT(TSendApplyResponse, NetStateMachine::MStateTransition, SCprStates::TContext)
EXPORT_C void TSendApplyResponse::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrPrvSCPRSC, 6));
  	iContext.iNodeActivity->PostToOriginators(TCFScpr::TApplyResponse().CRef());
    }

EXPORT_DEFINE_SMELEMENT(TAwaitingParamRequest, NetStateMachine::MState, SCprStates::TContext)
EXPORT_C TBool TAwaitingParamRequest::Accept()
	{
#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	return iContext.iMessage.IsMessage<TCFScpr::TParamsRequest>();
#else
	return EFalse;
#endif
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingApplyRequest, NetStateMachine::MState, SCprStates::TContext)
EXPORT_C TBool TAwaitingApplyRequest::Accept()
    {
	return iContext.iMessage.IsMessage<TCFScpr::TApplyRequest>();
    }


