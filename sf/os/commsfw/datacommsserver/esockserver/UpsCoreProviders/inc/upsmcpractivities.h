/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* generic framework node state machine states related to User Prompt Service (UPS)
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file upsmcpractivities.h
 @internalTechnology
*/

#ifndef SYMBIAN_UPSMCPRACTIVITIES_H
#define SYMBIAN_UPSMCPRACTIVITIES_H


// @TODO PREQ1116 - should we put this into its own namespace?

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/netups.h>

//////////////////////////////////
// Support for UPS
//

namespace UpsMCprActivities
{

class CUpsAuthorisationActivityBase : public MeshMachine::CNodeParallelActivityBase, public NetUps::MPolicyCheckRequestOriginator
/**
Base class used for MCpr activities that need to issue authorisation requests to the NetUps.

Derives from MPolicyCheckRequestOriginator to receive NetUps callbacks when request has completed.

Contains Mesh Machine methods for interacting with the NetUps and also contains "node specific" virtual
methods for retrieving information required by NetUps.  These methods are intended to be overridden in
the specific node to delineate node specific functions from the remaining MCpr generic functions held in
this class:

GetServiceId() - retrieves the UPS ServiceId (a UID).  Although the ServiceId is defined in the
particular MCpr, a default implementation is provided that returns the TierId from the Access
Point Config as the ServiceId.

GetUpsDestinationString() - forms a human readable destination address string.  The operation of
this function depends on the form of the destination address (e.g. IPv4/IPv6) which, in turn,
depends on the MCpr.

*/
// @TODO PREQ1116 - does this need to be parallel?
	{
public:
	// CMetaConnectionProviderBase used in context below as access is needed to AP name stored therein.
	typedef MeshMachine::TNodeContext<ESock::CMetaConnectionProviderBase, PRStates::TContext> TContext;

	// Utility
	inline NetUps::CNetUps* NetUpsReference();
	inline void SetNetUpsReference(NetUps::CNetUps* aNetUps);
	inline void ResetRequestId();
	inline NetUps::TRequestId& RequestId();

	// Node specific virtuals (i.e. returning node-wide information to the activity)
	IMPORT_C virtual TInt GetServiceId(const ESock::CCommsProviderBase& aNode, TInt32& aServiceId) const;
	virtual NetUps::CNetUps* GetNetUpsL(TInt32 aServiceId) = 0;
	virtual TInt GetUpsDestinationString(TUpsDestinationAddrType aDestinationType, const TUpsDestinationAddr& aDestination, TDes& aOutputString) = 0;
	virtual const TDesC& GetUpsAccessPointNameL() = 0;

	// from MPolicyCheckRequestOriginator
	IMPORT_C TInt ProcessPolicyCheckResponse(NetUps::TNetUpsDecision aNetUpsDecision, const Messages::TNodeId& aCommsId);
	IMPORT_C TInt ProcessPolicyCheckResponse(TInt aError, const Messages::TNodeId& aCommsId);

	// Transitions
	
	// @TODO PREQ1116 - do we really need the DoL()'s etc to be exported ?
	EXPORT_DECLARE_SMELEMENT_HEADER( TIssueRequestToNetUps, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TIssueRequestToNetUps )

	EXPORT_DECLARE_SMELEMENT_HEADER( TForwardPolicyCheckResponseToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TForwardPolicyCheckResponseToOriginators )

	EXPORT_DECLARE_SMELEMENT_HEADER( TForwardCompletionPolicyCheckResponseToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TForwardCompletionPolicyCheckResponseToOriginators )
	
	// States
	EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingPolicyCheckResponseOrCancel, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
		virtual TBool Accept();
		void CancelRequest();
	DECLARE_SMELEMENT_FOOTER( TAwaitingPolicyCheckResponseOrCancel )
	
	// State Forks
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrError )

	static const TInt KUpsCompletionTag = 10010;
	
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrCompletion, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrCompletion )
	
protected:
    IMPORT_C CUpsAuthorisationActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount);
    IMPORT_C ~CUpsAuthorisationActivityBase();

    IMPORT_C virtual void PerformPolicyCheckResponseActions(NetUps::TNetUpsDecision aNetUpsDecision, const Messages::TNodeId& aCommsId);
    IMPORT_C virtual void PerformPolicyCheckRequestActionsL(const Messages::TNodeId& aCommsId);    
private:
	// This class is not intended to be instantiated as a custom activity in its own right - declare the
	// NewL() method private and don't define it, to avoid a line of the form "CUpsAuthorisationActivityBase::NewL()"
	// in an activity definition from inadvertantly invoking any of our base class NewL() methods.
	static CUpsAuthorisationActivityBase* NewL();

private:
	NetUps::CNetUps* iNetUpsReference;		// CNetUps instance to use (NOTE - not owned by this class)
	NetUps::TRequestId iRequestId;			// id of last request issued (or EUndefined)
	};

//
// Inline methods
//

NetUps::CNetUps* CUpsAuthorisationActivityBase::NetUpsReference()
	{ return iNetUpsReference; }

void CUpsAuthorisationActivityBase::SetNetUpsReference(NetUps::CNetUps* aNetUps)
	{ iNetUpsReference = aNetUps; }

NetUps::TRequestId& CUpsAuthorisationActivityBase::RequestId()
	{
	return iRequestId;
	}

void CUpsAuthorisationActivityBase::ResetRequestId()
	{
	iRequestId = 0;
	}

} // namespace UpsActivities

#endif //SYMBIAN_UPSMCPRACTIVITIES_H

