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
// Mobility Meta Connection Provider Activities
//
//

/**
 @file
 @internalComponent
*/


#ifndef MOBILITYMCPRACTIVITIES_H
#define MOBILITYMCPRACTIVITIES_H

#include <comms-infras/coremcpractivities.h>
#include <comms-infras/mobilitymcprstates.h>
#include <elements/nm_messages_errorrecovery.h>

enum TMobilityMCprActivities
    {
    ECFActivityMCprMobility = ESock::ECFActivityCustom + 1,
    };

namespace MobilityMCprActivities
{
DECLARE_EXPORT_ACTIVITY_MAP(mobilityMCprActivities)


///////////////////////////////////////////////////////////////////////////////
//CMobilityActivity
class CMobilityActivity : public MeshMachine::CNodeRetryActivity
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CMobilityActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CMobilityActivity();
	typedef MeshMachine::TNodeContext<CMobilityMetaConnectionProvider, MCprStates::TContext> TContext;

private:
	TBool EvaluatePreference(TContext& aContext);

    void SetHandshakingFlag();
    void ClearHandshakingFlag();

private:
	ESock::RMetaServiceProviderInterface* iCurrent;
	ESock::RMetaServiceProviderInterface* iAvailable;
	ESock::RMetaServiceProviderInterface* iCandidate;
	const TUint iAvailabilityScoreTreshold;


public:

	DECLARE_SMELEMENT_HEADER(TAwaitingCurrentCarrierRejectedOrAvailabilityChange, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER(TAwaitingCurrentCarrierRejectedOrAvailabilityChange)

	DECLARE_SMELEMENT_HEADER(TNoTagOrAwaitMobility, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, CMobilityActivity::TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrAwaitMobility)

	DECLARE_SMELEMENT_HEADER(TNoTagOrReConnectOrStartMobilityHandshakeBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, CMobilityActivity::TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrReConnectOrStartMobilityHandshakeBackwards)

	DECLARE_SMELEMENT_HEADER(TNoTagOrStartMobilityHandshakeBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, CMobilityActivity::TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrStartMobilityHandshakeBackwards)

	DECLARE_SMELEMENT_HEADER(TNoTagOrAwaitMobilityBackwardsOnMobilityTrigger, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, CMobilityActivity::TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrAwaitMobilityBackwardsOnMobilityTrigger)

	DECLARE_SMELEMENT_HEADER(TRejectedOrStartMobilityHandshakeBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER(TRejectedOrStartMobilityHandshakeBackwards)

	DECLARE_SMELEMENT_HEADER(TClearHandshakingFlag, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TClearHandshakingFlag)

	DECLARE_SMELEMENT_HEADER(TSendAvailabilityRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TSendAvailabilityRequest)

	DECLARE_SMELEMENT_HEADER(TInformMigrationAvailableAndSetHandshakingFlag, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TInformMigrationAvailableAndSetHandshakingFlag)

	// For the moment it is sufficient to use the re-connect activity, in the future we may want to
	// customise the behavior, for example start the new layer before rebinding it, etc.
	DECLARE_SMELEMENT_HEADER(TRequestReConnect, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TRequestReConnect)

	DECLARE_SMELEMENT_HEADER(TInformMigrationCompleted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, CMobilityActivity::TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TInformMigrationCompleted)

	typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityConnectionStartRecovery,
											ESock::ECFActivityConnectionGoneDownRecovery> TActivityErrorRecoveryMutex;
	DECLARE_SERIALIZABLE_STATE(
	    TNoTagOrAwaitMobilityBackwardsOnMobilityTriggerBlockedByErrorRecovery,
		TActivityErrorRecoveryMutex,
		TNoTagOrAwaitMobilityBackwardsOnMobilityTrigger
		)

	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrAwaitMobilityBlockedByErrorRecovery,
		TActivityErrorRecoveryMutex,
		TNoTagOrAwaitMobility
		)
	};

///////////////////////////////////////////////////////////////////////////////
//CConnectionRecoveryActivity
class CConnectionRecoveryActivity : public MeshMachine::CNodeRetryActivity
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    void ReplyToOriginators(Messages::TEErrorRecovery::TErrorRecoveryResponse& aCFMessageSig);

protected:
	CConnectionRecoveryActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

public:
	Messages::TErrContext iOriginalErrContext; //Error context on which this activity started

protected:
	typedef MeshMachine::TNodeContext<CMobilityMetaConnectionProvider, MCprStates::TContext> TContext;

public:
	DECLARE_SMELEMENT_HEADER(TAwaitingReConnectComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER(TAwaitingReConnectComplete)

	class TTransitionBase : public MeshMachine::TStateTransition<TContext>
		{
	public:
		TTransitionBase(TContext& aContext)
		:	MeshMachine::TStateTransition<TContext>(aContext)
			{
			}

		virtual void Error(TInt aError);
		};

	DECLARE_SMELEMENT_HEADER(TStoreErrorContext, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TStoreErrorContext)

	DECLARE_SMELEMENT_HEADER(TProcessConnectionStartRecoveryRequest, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TProcessConnectionStartRecoveryRequest)

	DECLARE_SMELEMENT_HEADER(TProcessConnectionGoneDownRecoveryRequest, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TProcessConnectionGoneDownRecoveryRequest)

	DECLARE_SMELEMENT_HEADER(TSendRetryRecoveryResponse, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TSendRetryRecoveryResponse)

	DECLARE_SMELEMENT_HEADER(TSendPropagateRecoveryResponse, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TSendPropagateRecoveryResponse)

	DECLARE_SMELEMENT_HEADER(TSendIgnoreRecoveryResponse, CConnectionRecoveryActivity::TTransitionBase, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TSendIgnoreRecoveryResponse)
	};

}	// namespace MobilityMCprActivities

#endif // MOBILITYMCPRACTIVITIES_H

