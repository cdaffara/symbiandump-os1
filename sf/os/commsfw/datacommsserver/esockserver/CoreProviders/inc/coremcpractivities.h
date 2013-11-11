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
* generic framework node state machine states
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file coremcpractivities.h
 @internalTechnology
*/

#ifndef SYMBIAN_COREMCPRACTIVITIES_H
#define SYMBIAN_COREMCPRACTIVITIES_H

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_commsprov.h>
#include <es_connpref.h> //TConnIdList
#include <metadatabase.h> //CMDBSession
#include <comms-infras/es_availability.h> //TAvailabilityStatus
#include <comms-infras/ss_mcprnodemessages.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvCPRAH, "ESockCrPrvCPRAH");
#endif

class CCoreMetaConnectionProvider;

namespace MCprActivities
{

//DECLARE_NODEACTIVITY(MCprAvailability)
DECLARE_EXPORT_ACTIVITY_MAP(coreMCprActivitiesSuper)
DECLARE_EXPORT_ACTIVITY_MAP(coreMCprActivities)

///////////////////////////////////////////////////////////////////////////////
//CSelectNextLayerActivity
class CSelectNextLayerActivity : public MeshMachine::CNodeActivityBase
	{
public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	IMPORT_C virtual ~CSelectNextLayerActivity();
	IMPORT_C TConnIdList& SelectionChoices();
	IMPORT_C ESock::RConnPrefList& SelectionConnPrefList();

protected:
	IMPORT_C void ConstructL();
	CSelectNextLayerActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	MeshMachine::CNodeActivityBase(aActivitySig, aNode)
		{
		}

	CommsDat::CMDBSession& Dbs()
		{
		__ASSERT_DEBUG(iDbs, User::Panic(KSpecAssert_ESockCrPrvCPRAH, 1)); //Can never be NULL anyway since constructed in NewL
		return *iDbs;
		}

public:
	Messages::TNodeId iTierManager;
	TInt iCurrentIndex;
	RArray<ESock::TSigSelectComplete> iSelectCompleteMessages;

private:
	TConnIdList iSelectionChoices;
	CommsDat::CMDBSession* iDbs;
	ESock::RConnPrefList iSelectionConnPrefList;

protected:
	typedef MeshMachine::TNodeContext<CCoreMetaConnectionProvider,PRStates::TContext> TContext;

public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSelectedProviderIsNull, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSelectedProviderIsNull )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSelectedProviderIsNullOrJoinServiceProviderBackward, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TSelectedProviderIsNullOrJoinServiceProviderBackward )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagBackwardsOrJoinServiceProvider, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagBackwardsOrJoinServiceProvider )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSelectedProviderBackward, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSelectedProviderBackward )

	EXPORT_DECLARE_SMELEMENT_HEADER( TProcessSimpleSelectionPolicy, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessSimpleSelectionPolicy )

	EXPORT_DECLARE_SMELEMENT_HEADER( TProcessPrioritisedSelectionPolicy, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessPrioritisedSelectionPolicy )

	EXPORT_DECLARE_SMELEMENT_HEADER( TFindOrCreateTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TFindOrCreateTierManager )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSelectNextLayer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSelectNextLayer )

	EXPORT_DECLARE_SMELEMENT_HEADER( TProcessSimpleSelectionPolicySuper, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessSimpleSelectionPolicySuper )

	EXPORT_DECLARE_SMELEMENT_HEADER( TProcessPrioritisedSelectionPolicySuper, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessPrioritisedSelectionPolicySuper )

	EXPORT_DECLARE_SMELEMENT_HEADER( TFindOrCreateTierManagerSuper, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TFindOrCreateTierManagerSuper )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSelectNextLayerSuper, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSelectNextLayerSuper )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSelectedProviderBackwardSuper, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSelectedProviderBackwardSuper )

	EXPORT_DECLARE_SMELEMENT_HEADER( TAddProviderInfo, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TAddProviderInfo )

	EXPORT_DECLARE_SMELEMENT_HEADER( TJoinTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TJoinTierManager )

	EXPORT_DECLARE_SMELEMENT_HEADER( TLeaveTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TLeaveTierManager )

	EXPORT_DECLARE_SMELEMENT_HEADER( TJoinServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TJoinServiceProvider )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSendSelectComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendSelectComplete )
	};


///////////////////////////////////////////////////////////////////////////////
//CReConnectActivity
class CReConnectActivity : public MeshMachine::CNodeActivityBase
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CReConnectActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);

public:
	Messages::RNodeInterface* iStoppingSP; //Reference to the clients table
	Messages::RNodeInterface* iStartingSP; //Reference to the clients table

protected:
	typedef MeshMachine::TNodeContext<CCoreMetaConnectionProvider, PRStates::TContext> TContext;

public:
	DECLARE_SMELEMENT_HEADER( TBuildLowerLayer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TBuildLowerLayer )

	DECLARE_SMELEMENT_HEADER( TProcessReConnectRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessReConnectRequest )

	DECLARE_SMELEMENT_HEADER( TStopStartedServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TStopStartedServiceProvider )

	DECLARE_SMELEMENT_HEADER( TStartNextServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TStartNextServiceProvider )

	DECLARE_SMELEMENT_HEADER( TSendBuildLowerLayer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendBuildLowerLayer )

	DECLARE_SMELEMENT_HEADER( TSendReConnectResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendReConnectResponse )
	};

///////////////////////////////////////////////////////////////////////////////
//CAvailabilityActivity
class CAvailabilityActivity : public MeshMachine::CNodeActivityBase
	{

private:
	class CSubscriberInfo : public CBase
		{
	public:
		void SetSubscriptionOptions(const ESock::TAvailabilitySubscriptionOptions& aSubscriptionOptions)
			{
			iSubscriptionOptions = aSubscriptionOptions;
			}

		const ESock::TAvailabilitySubscriptionOptions& SubscriptionOptions() const
			{
			return iSubscriptionOptions;
			}

		void SetReportedStatus(const ESock::TAvailabilityStatus& aReportedStatus)
			{
			iReportedStatus = aReportedStatus;
			}

		const ESock::TAvailabilityStatus& ReportedStatus() const
			{
			return iReportedStatus;
			}

	protected:
	    ESock::TAvailabilitySubscriptionOptions iSubscriptionOptions;
	    ESock::TAvailabilityStatus iReportedStatus;
		};

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    virtual ~CAvailabilityActivity();

	//CAvailabilityActivity has no originators
	virtual void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);
	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

	void CalculateCurrentAvailabilityStatus();
	TBool IsAvailabilityKnown() const;
	void NotifyInterestedSubscribers();
	void RegisterForNotifications(Messages::TClientIterBase& aClientIter) const;

protected:
	CAvailabilityActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	MeshMachine::CNodeActivityBase(aActivitySig, aNode),
		iCurrentAvailabilityStatus()
		{
		}

private:
	ESock::TAvailabilityStatus iCurrentAvailabilityStatus;

public:
	//typedef CoreNetStates::TContext TContext;
	typedef MeshMachine::TNodeContext<CCoreMetaConnectionProvider, PRStates::TContext> TContext;

    DECLARE_SMELEMENT_HEADER(TRegisterForNotifications, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER(TRegisterForNotifications)

    DECLARE_SMELEMENT_HEADER(TProcessNotification, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER(TProcessNotification)
	};

} //namespace MCprActivities

#endif //SYMBIAN_COREMCPRACTIVITIES_H

