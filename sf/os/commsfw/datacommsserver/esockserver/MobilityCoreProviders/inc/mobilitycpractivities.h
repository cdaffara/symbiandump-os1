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
// Mobility Connection Provider activity declarations.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef MOBILITYCPR_ACTIVITIES_H
#define MOBILITYCPR_ACTIVITIES_H

#include <comms-infras/ss_mobility_apiext.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/mobilitycprstates.h>

enum TMobilityCprActivities
    {
    ECFActivityCprMobility = ESock::ECFActivityCustom + 1,
    };

class CMobilityConnectionProvider;

namespace MobilityCprActivities
{
DECLARE_EXPORT_ACTIVITY_MAP(mobilityCprActivities)

//
// CMobilityActivity

class CMobilityActivity : public MeshMachine::CNodeRetryActivity
	{
private:
	class TMobilityClientNotReadyMutex
		{
	public:
		static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
		};

	friend class TMobilityPreferenceMutex;

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    ~CMobilityActivity();
    void Cancel(MeshMachine::TNodeContextBase& aContext);
    void SetResponder(ESock::CCommsApiExtResponder& aResponder);

protected:
	CMobilityActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	//Cpr::CMobilityActivity has no originators
	void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);

protected:
	typedef MeshMachine::TNodeContext<CMobilityConnectionProvider, CprStates::TContext> TContext;
	
	
public:
	TSubSessionUniqueId iClientId;
	TUint iCurrentAp;
	TUint iPreferredAp;
	TBool iIsUpgrade;
	ESock::CCommsApiExtResponder* iResponder;
	
public:
	static const TInt KWaitForMobility         = 10001;
    static const TInt KStartHandshake          = 10002;


	// Transition tags
	DECLARE_SMELEMENT_HEADER(TNoTagOrWaitForMobilityBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrWaitForMobilityBackwards)

	DECLARE_SMELEMENT_HEADER(TNoTagBackwardsOrRecoverableErrorOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagBackwardsOrRecoverableErrorOrError)
	
    DECLARE_SERIALIZABLE_STATE(
        TStartHandshakeOrErrorTagBlockedByClientNotReady,
        CMobilityActivity::TMobilityClientNotReadyMutex,
        MeshMachine::TErrorTagOr<MeshMachine::TTag<KStartHandshake | NetStateMachine::EForward> >
        )
	
	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrErrorTagBlockedByClientNotReady,
		CMobilityActivity::TMobilityClientNotReadyMutex,
		MeshMachine::TNoTagOrErrorTag
		)

	DECLARE_SERIALIZABLE_STATE(
		TNoTagBackwardsOrRecoverableErrorOrErrorBlockedByClientNotReady,
		CMobilityActivity::TMobilityClientNotReadyMutex,
		TNoTagBackwardsOrRecoverableErrorOrError
		)
		
	// Transitions
	DECLARE_SMELEMENT_HEADER(TCompleteMobilityClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TCompleteMobilityClient)

	DECLARE_SMELEMENT_HEADER(TNotifyClientPreferredCarrierAvailable, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TNotifyClientPreferredCarrierAvailable)

	DECLARE_SMELEMENT_HEADER(TNotifyClientNewCarrierActive, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TNotifyClientNewCarrierActive)

	};
}

#endif // MOBILITYCPR_ACTIVITIES_H

