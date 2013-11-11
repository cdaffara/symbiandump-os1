// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalTechnology
*/

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_subconn.h>
#include "SS_conn.H"
#include "ss_connstates.h"
#include <comms-infras/ss_datamonitoringprovider.h>
#include <comms-infras/ss_roles.h>
#include <ss_glob.h>

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>

#include <elements/sm_core.h>
#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/es_commsdataobject.h>
#include <comms-infras/ss_commsdataobject.h>
#include <nifman.h>
#include <comms-infras/nifprvar.h>

#include <in_sock.h> //KAfInet only

#include <elements/nm_signatures.h>

#include <comms-infras/ss_msgintercept.h>

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/upsmessages.h>		// reference from ESock to UpsCoreProviders - should we move the UPS messages into ESock?
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
// UNCOMMENT IF INSERTING AN ASSERT
//_LIT(KSpecAssert_ESockSSocks_cn, "ESockSSocks_cn.");
#endif



using namespace ESock;
using namespace SubSessActivities;
using namespace NetStateMachine;
using namespace ConnActivities;
using namespace SubSessStates;
using namespace CoreStates;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;
using namespace Den;

//
//Activities serving client (RConnection) requests
namespace ConnectionStartActivity
{
 //Synchronised activity, must wait for stop (exclusive with close)
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, ConnectionStart, TCFInternalEsock::TSubSess, ConnActivities::CStartAttachActivity::NewStartConnectionActivityL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNStart>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	
	// If the IPC was ECNSetStartPrefs we expect another IPC to start the connection 
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessStartBlockedByStop, ConnActivities::CStartAttachActivity::TNoTagOrStartPrefsSetTag)
	NODEACTIVITY_ENTRY(ConnActivities::CStartAttachActivity::KStartPrefsSetTag, MeshMachine::TDoNothing, SubSessStates::TAwaitingIPC<ECNStart>, ConnActivities::CStartAttachActivity::TStartPrefsSetTag)
	THROUGH_NODEACTIVITY_ENTRY(ConnActivities::CStartAttachActivity::KStartPrefsSetTag, SubSessStates::TAcquireMessageOwnership, ConnActivities::CStartAttachActivity::TStartPrefsSetTag)
	THROUGH_NODEACTIVITY_ENTRY(ConnActivities::CStartAttachActivity::KStartPrefsSetTag, ConnStates::TParseECNStart, MeshMachine::TNoTag)
	
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TClearProgressQueue, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TSendStartingSelectionStateChange, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TRequestCSRCreation, TECABState<CoreNetStates::TAwaitingCSRCreated>, MeshMachine::TNoTag)
	

	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessCSRCreation, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TSelectMetaPlane, ConnActivities::CStartAttachActivity::TAwaitingSelectCompleteOrError, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)

	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TJoinReceivedMcpr, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TRequestCommsBinderFromMcpr, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, CoreActivities::ABindingActivity::TSendBindToComplete) //Terminate the activity
	
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TSendFinishedSelectionStateChange, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessBinderResponseForCpr, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)

#ifdef SYMBIAN_NETWORKING_UPS
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnActivities::CStartAttachActivity::TSendBindToComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CStartAttachActivity::TSendPolicyCheckRequestToServiceProvider, TAcceptErrorState<TAwaitingMessageState<UpsMessage::TPolicyCheckResponse> >, CStartAttachActivity::TNoTagOrUpsErrorTag)
  	//<legacy begin> - requesting and joining the default scpr just to satisfy the subconn related datamonitoring API.
  	//If the default scpr isn't there, we'll ignore the error here but we will dissallow subscribing to subcon-related data monitoring.
  	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TRequestCommsBinder, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)
 #endif


#ifndef SYMBIAN_NETWORKING_UPS
  	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnActivities::CStartAttachActivity::TSendBindToComplete, MeshMachine::TNoTag)
  	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TRequestCommsBinder, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)
#endif


  	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TJoinReceivedSCpr, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)
  	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
  	// Act on cancellation errors, ignore all others.
  	ROUTING_NODEACTIVITY_ENTRY(KErrorTag, ConnStates::TErrorOrCancel)
  	THROUGH_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError, MeshMachine::TNoTag)
  	//</legacy end>>

	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnActivities::CStartAttachActivity::TSubscribeForAvailability, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, ConnActivities::CStartAttachActivity::TNoTagOrWaitAvailable)
	NODEACTIVITY_ENTRY(ConnActivities::CStartAttachActivity::KWaitAvailable, MeshMachine::TDoNothing, ConnActivities::CStartAttachActivity::TAwaitingAvailableOrError, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TStartConnection, TAcceptErrorState<CoreNetStates::TAwaitingStarted>, CoreStates::TCancelOrErrorOrTag<KNoTag>)
	// Handle error during start
	THROUGH_NODEACTIVITY_ENTRY(KErrorTag, ConnStates::TGenerateConnectionDownProgress, ConnActivities::CStartAttachActivity::TErrorTagOrWaitAvailableBackward)
	
	LAST_NODEACTIVITY_ENTRY(KNoTag,  ConnStates::TGenerateConnectionUpProgress )

	// Handle error during start.
	THROUGH_NODEACTIVITY_ENTRY(ConnActivities::CStartAttachActivity::KCancelTag, MeshMachine::TDoNothing, MeshMachine::TErrorTag)	// just a tag
#ifdef SYMBIAN_NETWORKING_UPS
	THROUGH_NODEACTIVITY_ENTRY(CStartAttachActivity::KUpsErrorTag, TDoNothing, MeshMachine::TErrorTag)
#endif

	// If there is a stop activity outstanding (it should be waiting for this activity to complete) then we will leave that to decide the fate of our service providers
	THROUGH_NODEACTIVITY_ENTRY(KErrorTag, ConnActivities::CStartAttachActivity::TSetIdleIfStopOutstanding, MeshMachine::TErrorTag)

	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TSendClientLeavingRequestToServiceProviders, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, ConnectionCleanupActivities::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionUninitialisedProgress)
NODEACTIVITY_END()
}

namespace ConnectionAttachActivity
{
typedef SubSessStates::TAwaitingSecuredIpc<SubSessStates::TAwaitingIPC<ECNAttach>, ECapabilityNetworkServices> TAwaitingSecuredAttachIpc;

//Synchronised activity, must wait for stop (exclusive with close)
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionAttach, ConnectionAttach, TCFInternalEsock::TSubSess, ConnActivities::CStartAttachActivity::NewStartConnectionActivityL)
#ifdef SYMBIAN_NETWORKING_UPS
	// UPS support.  Platform Security check now takes place in the Cpr as part of the control client join activity.
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNAttach>, MeshMachine::TNoTag)
#else
	FIRST_NODEACTIVITY_ENTRY(TAwaitingSecuredAttachIpc, MeshMachine::TNoTag)
#endif
	// NOTE: TAcquireMessageOwnership MUST occur before any blocking takes place, otherwise the Player will
	// complete the message upon return from the blocked activity.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, CoreNetStates::TNoTagBlockedByStop)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessAttachBlockedByStop, TECABState<CoreNetStates::TAwaitingCSRCreated>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessCSRCreation, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TSelectMetaPlane, ConnActivities::CStartAttachActivity::TAwaitingSelectCompleteOrError, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)

	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TJoinReceivedMcpr, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, ConnStates::TRequestCommsBinderFromMcpr, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, CoreActivities::ABindingActivity::TSendBindToComplete) //Terminate the activity

    NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessBinderResponseForCpr, TECABState<CoreStates::TAwaitingJoinComplete>, CStartAttachActivity::TNoTagOrLegacyAttach)

    //New Attach - go here
	LAST_NODEACTIVITY_ENTRY(KNoTag, ConnActivities::CStartAttachActivity::TSendBindToComplete)
	//Legacy Attach - go here
	LAST_NODEACTIVITY_ENTRY(CStartAttachActivity::KExecuteLegacyAttach, ConnActivities::CStartAttachActivity::TSendBindToCompleteAndCompleteLegacyAttach)
NODEACTIVITY_END()
}

namespace ConnectionStopActivity
{
//Synchronised activity, must wait for start to finish
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStop, ConnectionStop, TCFInternalEsock::TSubSess, CPreallocatedESockClientActivity::New)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNStop>, MeshMachine::TNoTag)
	// NOTE: TAcquireMessageOwnership MUST occur before any blocking takes place, otherwise the Player will
	// complete the message upon return from the blocked activity.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, CoreNetStates::TActiveOrNoTagBlockedByGoneDown)
	THROUGH_NODEACTIVITY_ENTRY(KActiveTag, ConnStates::TCancelStartOrAttachConnection, ConnStates::TNoTagOrNoBearerBlockedByStartOrAttach)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TCancelAndCloseClientExtIfaces, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCancelAllLegacyRMessage2Activities, ConnStates::TNoTagBlockedByLegacyRMessage2Activities)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TSendStopConnection, TECABState<CoreNetStates::TAwaitingStopped>, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionDownProgress, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingRequestToServiceProviders, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, ConnectionCleanupActivities::TNoTagOrNoTagBackwards)
	
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionUninitialisedProgress)
NODEACTIVITY_END()
}

/**
	Legacy RConnection::Stop(TSubConnectionUniqueId aSubConnectionUniqueId) implementation
*/
namespace ConnectionStopSCPRActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStopSCPR, ConnectionStopSCPR, TCFInternalEsock::TSubSess, CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCPSStop>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TConnectionSendStopSCPR, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, TECABState<CoreNetStates::TAwaitingStopped>, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

/**
	Legacy RConnection::EnumerateConnections implementation
*/
namespace EnumerateConnectionsActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionEnumerateConnections, EnumerateConnections, TCFInternalEsock::TSubSess, CEnumerateConnectionsActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNEnumerateConnections>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)

	// Find the tier manager and query it.
	NODEACTIVITY_ENTRY(KNoTag, CTierManagerActivity::TFindTierManager, TECABState<CTierManagerActivity::TAwaitingTierManager>, MeshMachine::TErrorTagOr<MeshMachine::TTag<KNoTag> >)
		LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)

	NODEACTIVITY_ENTRY(KNoTag, CTierManagerActivity::TJoinTierManager, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, TQueryTierStatus, TECABState<TAwaitingTierStatus>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, TCacheConnectionInfo, MeshMachine::TNoTag)

	LAST_NODEACTIVITY_ENTRY(KNoTag, TCompleteClient)
NODEACTIVITY_END()
}

namespace ConnectionGoneUpActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityAny, ConnectionGoneUp, TCFControlClient::TGoneUp)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingGoneUp, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionUpProgress)
NODEACTIVITY_END()
}

namespace ConnectionCloseActivity
{ //Cancells all other running activities
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, ConnectionClose, TCFInternalEsock::TSubSess, CCloseActivity::New)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNClose>, MeshMachine::TNoTag)
	// NOTE: TAcquireMessageOwnership MUST occur before any blocking takes place, otherwise the Player will
	// complete the message upon return from the blocked activity.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, ConnStates::TActiveOrNoTagBlockedByStopOrGoneDown)
	THROUGH_NODEACTIVITY_ENTRY(KActiveTag, ConnStates::TCancelStartOrAttachConnection, ConnStates::TNoTagBlockedByStartOrAttach)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TCancelAndCloseClientExtIfaces, ConnStates::TNoTagOrCancelAllInterfaceWorker)
	// Handshake a shutdown of AllInterfaceNotificationWorker (send TCancel, await TError).
	NODEACTIVITY_ENTRY(ConnStates::KCancelAllInterfaceWorker, ConnStates::TCancelAllInterfaceNotificationWorker, TAwaitingMessageState<TEBase::TError>, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCancelAllLegacyRMessage2Activities, ConnStates::TNoTagBlockedByLegacyRMessage2Activities)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessClose, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, TECABState<MeshMachine::TAwaitingLeaveComplete>, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

//--------------------------------------------------
//Progress/TStateChange activities.
//ConnectionStateChangeNotificationActivity acts on the subscription and completes the client with progresses
//ConnectionStateChangeActivity acts on TStateChange coming from the stack.
namespace ConnectionStateChangeNotificationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionStateChangeRequest, ConnectionStateChangeNotification, TCFInternalEsock::TSubSess, CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNProgressNotification>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessProgressRequest, TECABState<ConnStates::TAwaitingStateChange >, MeshMachine::TNoTag)
	
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessStateChange, TECABState<ConnStates::TAwaitingStateChange >, MeshMachine::TNoTagBackward)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing) //Never gets here
NODEACTIVITY_END()
}

namespace ConnectionStateChangeActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStateChange, ConnectionStateChange, TCFMessage::TStateChange)
    NODEACTIVITY_ENTRY(KNoTag, ConnStates::TEnqueueStateChange, ConnStates::TAwaitingStateChange, MeshMachine::TNoTag)
NODEACTIVITY_END()
}


namespace ConnectionWaitForIncomingActivity
{ //Synchronised, waits for Start to complete
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionWaitForIncoming, ConnectionWaitForIncoming, TCFInternalEsock::TSubSess, CStartAttachActivity::NewWaitForIncomingConnectionActivityL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ECNWaitForIncoming>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	//We use ConnStates::TAwaitingBinderResponse (not CoreStates) for custom handling of the Cancel message
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TRequestIncomingConnectionBlockedByStartAttach, TECABState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessIncomingConnection, TECABState<CoreNetStates::TAwaitingBindToComplete>, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, ConnActivities::CStartAttachActivity::TSendBindToComplete)
NODEACTIVITY_END()
}


namespace ConnectionGoingDownActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityGoneDown, ConnectionGoingDown, TCFControlClient::TGoneDown, PRActivities::CGoneDownActivity::New)
	FIRST_NODEACTIVITY_ENTRY(ConnStates::TAwaitingGoneDown, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnectionGoingDownActivity::TStoreGoneDownError, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TCancelAndCloseClientExtIfaces, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCancelAllLegacyRMessage2Activities, ConnStates::TNoTagBlockedByLegacyRMessage2Activities)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionDownProgress, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingRequestToServiceProviders, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, ConnectionCleanupActivities::TNoTagOrNoTagBackwards)
    LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TGenerateConnectionUninitialisedProgress)
NODEACTIVITY_END()
}

//
//Activities serving framework requests for legacy actions
namespace ConnectionClientEnumActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityLegacyConnEnumResponse, ConnectionEnumResponse, TCFInternalEsock::TLegacyConnectionEnumResponse)
	NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessEnumResponse, ConnStates::TAwaitingEnumResponse, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

//
//CConn usually is a cntrl client of a default subconnection.
//The subconnection will send events, which must be handled to avoid leakages
namespace ConnSubConnectionEventNotification
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityConnectionWaitForIncoming, SubConEvent, TCFSubConnControlClient::TSubConnNotification)
	NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TBinEvent, CoreNetStates::TAwaitingSubConEvent, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace ConnSubConnEventsActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityConnSubConnEvents, ConnSubConnEvents, TNodeSignal::TNullMessageId)
	NODEACTIVITY_ENTRY(KNoTag, ConnSubConnEventsActivity::TProcessSubConnEvent, ConnSubConnEventsActivity::TAwaitingSubConnEvent, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace ConnLegacyRMessage2Activity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionLegacyRMessage2Handler, ConnectionLegacyRMessage2, TNodeSignal::TNullMessageId, CConnLegacyRMessage2Activity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingLegacyRMessage2Ext, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessLegacyRMessage2, TAcceptErrorState<CoreNetStates::TAwaitingRMessage2Processed>, MeshMachine::TNoTagOrErrorTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCompleteRMessage2)
    LAST_NODEACTIVITY_ENTRY(KErrorTag, ConnStates::THandleRMessage2Error)
NODEACTIVITY_END()
}

namespace ConnectionActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(connectionActivities)
	//-----Framework originated activities-----//
	ACTIVITY_MAP_ENTRY(ConnectionStateChangeActivity, ConnectionStateChange)
	ACTIVITY_MAP_ENTRY(ConnectionGoingDownActivity, ConnectionGoingDown)
	ACTIVITY_MAP_ENTRY(ConnSubConnectionEventNotification, SubConEvent)
	ACTIVITY_MAP_ENTRY(ConnSubConnEventsActivity, ConnSubConnEvents)
	ACTIVITY_MAP_ENTRY(ConnectionStopSCPRActivity, ConnectionStopSCPR)
	ACTIVITY_MAP_ENTRY(ConnectionGoneUpActivity,ConnectionGoneUp)
	//-----Client (IPC) originated activities-----//
	ACTIVITY_MAP_ENTRY(ConnectionWaitForIncomingActivity, ConnectionWaitForIncoming)
	ACTIVITY_MAP_ENTRY(ConnectionCloseActivity, ConnectionClose)
	ACTIVITY_MAP_ENTRY(ConnectionStartActivity, ConnectionStart)
	ACTIVITY_MAP_ENTRY(ConnectionAttachActivity, ConnectionAttach)
	ACTIVITY_MAP_ENTRY(ConnectionStateChangeNotificationActivity, ConnectionStateChangeNotification)
	ACTIVITY_MAP_ENTRY(ConnectionStopActivity, ConnectionStop)

	ACTIVITY_MAP_ENTRY(ConnLegacyRMessage2Activity, ConnectionLegacyRMessage2)

#ifdef AVAILABILITY_READY
	ACTIVITY_MAP_ENTRY(EnumerateConnectionsActivity, EnumerateConnections)
#endif

	//-----Responses from Data plane to legacy queries-----//
	ACTIVITY_MAP_ENTRY(ConnectionClientEnumActivity, ConnectionEnumResponse)
ACTIVITY_MAP_END()
}

// attribute table for CConnectionIfno
START_ATTRIBUTE_TABLE(CConnectionInfo, CConnectionInfo::EUid, CConnectionInfo::ETypeId)
END_ATTRIBUTE_TABLE()


namespace ESock
{
CConnectionInfo* CConnectionInfo::NewL(const Den::TSubSessionUniqueId& aSubSessionId)
	{
	return new(ELeave) CConnectionInfo(aSubSessionId);
	}
}

/**
Constructor
*/
CConnection::CConnection(CSockSession* aSession, CPlayer* aPlayer, TUid aTierId, const Den::TSubSessionUniqueId aSubSessionUniqueId)
:	CMMSockSubSession(ConnectionActivities::connectionActivities::Self(), aSession, aPlayer, aSubSessionUniqueId),
    ASubSessionPlatsecApiExt(UniqueId()),
    TIfStaticFetcherNearestInHierarchy(this),
	iLastProgress(KConnectionUninitialised, KErrNone),
	iLastProgressError(KConnectionUninitialised, KErrNone),
    iTierId(aTierId),
    iLegacyConnection(*this)
	{
	LOG_NODE_CREATE1(KESockConnectionTag, CConnection, ", session %08x", aSession);
	}
/**
Destructor
*/
CConnection::~CConnection()
	{
	//Do not call CConnection::FinalCompleteAllBlockedMessages from here!
	//At this stage we must be completed.
	iLegacyConnection.FinalCompleteAllBlockedMessages(KErrCancel); //complete any outstanding legacy messages
	delete iConnectionInfo;
    LOG_NODE_DESTROY(KESockConnectionTag, CConnection);
	}

//MZTODO - only client activities? Is this a shutdown?
//Signal from subsession to abbort all (client?) activities (always KErrAbort)
void CConnection::FinalCompleteAllBlockedMessages(TInt /*aResult*/)
	{
	TNodeNullContext ctx(*this);
	AbortActivitiesOriginatedBy(ctx); //Abort all activities
	iLegacyConnection.FinalCompleteAllBlockedMessages(KErrCancel); //complete any outstanding legacy messages
	}

void CConnection::Received(TNodeContextBase& aContext)
    {
    TNodeSignal::TMessageId noPeerIds[] = {
    	TCFInternalEsock::TSubSess::Id(),
    	TEBase::TError::Id(), //May be comming from the CSR
    	TCFInternalEsock::TLegacyConnectionEnumResponse::Id(),	// self-dispatching helper from the data plane
    	TNodeSignal::TMessageId()	// list terminator
    	};

	MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CConnection::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aCFMessage)
    {
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aCFMessage, Id());

	//First check if this is not our own error
	TEBase::TError* msg = message_cast<TEBase::TError>(&aCFMessage);
	if (msg	&& aSender == Id() && msg->iMsgId == TCFInternalEsock::TSubSess::Id())
		{
		//This is our own error, generated from the client (IPC) activity
		//It is safe to just ignore it here, as the activity is
		//completed (and so is the client RMessage2). If the activity didn't have
		//the chance to acquire ownership of the message it is still safe to ignore it
		//because there was no call to DoNotCompleteCurrentMessage().
		return;
		}

	TNodeContext<CConnection> ctx(*this, aCFMessage, aSender, aRecipient);
    CConnection::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CConnection::ForwardToServiceProviderL(const TSignalBase& aCFMessage)
	{
	RNodeInterface* sp = ServiceProvider();
	User::LeaveIfError(sp? KErrNone : KErrNotReady);
	sp->PostMessage(Id(), aCFMessage);
	}

CConnection* CConnection::NewLC(CSockSession *aSession, CPlayer* aPlayer, TUid aId, const Den::TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new CConnection instance

@param aSession Session under which CConnection was created
@param aId Type Id of CConnection
@return pointer to new CConnection instance on success
@exception leaves if could not allocate memory
*/
	{
	CConnection* h = new (ELeave) CConnection(aSession, aPlayer, aId, aSubSessionUniqueId);
	CleanupStack::PushL(h);
	h->ConstructL();
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(ESockDebug::KConnectionNodeUid, h);
	return h;
	}

CConnection* CConnection::NewLC(CSockSession* aSession, CPlayer* aPlayer, const CConnection& aExistingConnection, const Den::TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new CConnection object associated with the same interface as that of an existing CConnection object.

@param aSession Session under which CConnection was created
@param aExistingConnection Existing CConnection object whose interface to associate with
@return pointer to new CConnection instance on success
@exception leaves if could not allocate memory
*/
	{
	CConnection* h = new (ELeave) CConnection(aSession, aPlayer, aExistingConnection.iTierId, aSubSessionUniqueId);
	CleanupStack::PushL(h);
	h->ConstructL();
	h->CloneL(aExistingConnection);
	return h;
	}

void CConnection::ConstructL()
	{
	MeshMachine::AMMNodeBase::ConstructL();
	CSockSubSession::ConstructL(NULL);

	iConnectionInfo = CConnectionInfo::NewL(UniqueId());
	}

void CConnection::CloneL(const CConnection& aExistingConnection)
/**
Main body of CConnection::NewL(CSockSession* aSession, const CConnection& aExistingConnection)
*/
	{
	// will need to be brought over
	iLastProgress = aExistingConnection.iLastProgress;
	iLastProgressError = aExistingConnection.iLastProgressError;
	iProgressQueue = aExistingConnection.iProgressQueue;

	/*
	  This function looks like it'd be better to do in one loop. dont do this though. All fallible parts need to be done before
	  sending the messages to ourselves, otherwise the mesh machine will panic.
	*/
	TInt numSP = 0;
	TClientIter<TDefaultClientMatchPolicy> iter = aExistingConnection.GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider),
		Messages::TClientType(TCFClientType::EServProvider, Messages::TClientType::ELeaving));

	/**
	   Add clients to client list. If this fails at any point, remove anything we've added.
	   Makes assumption that we are the only thing adding service providers.
	*/
	RNodeInterface* sp = iter++;
	while (sp)
		{
		TRAPD(err, AddClientL(sp->RecipientId(), sp->ClientType()));
		if (err != KErrNone)
			{
			sp = GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
			while (sp)
				{
				RemoveClient(sp->RecipientId());
				sp = GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
				}
			User::Leave(err);
			}

		sp = iter++;
		numSP++;
        }

	/**
	   If we managed to add anything, post messages to them so that we get added as control clients
	*/
	if (numSP == 0)
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x CloneL KErrNotReady"), this) );
		User::Leave(KErrNotReady);
		}
	else
		{
		TClientIter<TDefaultClientMatchPolicy> iter = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
		sp = iter++;
		while (sp)
			{
			// TODO IK: This is the wrong message to be using here, should use JoinRequest/Complete handshake
			sp->PostMessage(Id(), TCFFactory::TPeerFoundOrCreated(Id(), 0).CRef());
			sp = iter++;
			}
		}
	}


RNodeInterface* CConnection::DefaultSubConnectionServiceProvider()
 	{
 	RNodeInterface* dscp = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider,TCFClientType::EDefault))[0];
 	return dscp;
 	}

void CConnection::ProcessMessageL()
/**
	Process RConnection messages

	@exception Leaves on any error processing the request
*/
	{
	LOG_DETAILED( ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tCommand %d"), this, aMessage.Function()) );

	//Some of the client (RMessage2) messages may need to be posted, for most it
	//will be sufficient to dispatch them directly via ReceivedL (advantage of posting
	//rather than dispatching -> of course mesh logging!)
	switch (Message().Function())
		{
		case ECNReference:
			GetReferenceL();
			break;

		case ECNStart:
		case ECNSetStartPrefs:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNStart aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ECNStart, TCFInternalEsock::TSubSess(ECNStart,Message()).CRef());
			break;

		case ECNAttach:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNAttach aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ECNAttach, TCFInternalEsock::TSubSess(ECNAttach,Message()).CRef());
			break;

		case ECNStop:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNStop aMessage %08x"), this, &Message()) );

			//Legacy support - if there is nothing to stop, return KErrNotReady
			if (ServiceProvider() == NULL && CountActivities(ECFActivityStart) == 0)
				{
				User::Leave(KErrNotReady);
				}

			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ECNStop, TCFInternalEsock::TSubSess(ECNStop,Message()).CRef());
			break;

		case ECNWaitForIncoming:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNWaitForIncoming aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ECNWaitForIncoming, TCFInternalEsock::TSubSess(ECNWaitForIncoming,Message()).CRef());
            break;

		case ECNCancelWaitForIncoming:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNCancelWaitForIncoming aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ECNWaitForIncoming, TEBase::TCancel().CRef());
			//CANCELATION - Do not call DontCompleteCurrentRequest() for the ECNCancelWaitForIncoming to be completed!
            break;

		case ECNProgressNotification:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNProgressNotification aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ECNProgressNotification, TCFInternalEsock::TSubSess(ECNProgressNotification,Message()).CRef()); //the ownership of the RMessage2 is taken by the activity
			break;

		case ECNCancelProgressNotification:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNCancelProgressNotification aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ECNProgressNotification, TEBase::TCancel().CRef());
			//CANCELATION - Do not call DontCompleteCurrentRequest() for the ECNCancelProgressNotification to be completed!
			break;

		case ECNIoctl:
		{
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNIoctl aMessage %08x"), this, &Message()) );
			RNodeInterface* sp = ServiceProvider();
			User::LeaveIfError(sp? KErrNone : KErrNotReady);

			CMMSockSubSession::ReceivedL(SafeMessage().Function(), TCprSendIoctl(SafeMessage()).CRef());
			DontCompleteCurrentRequest(); //TLegacyControlMessage will complete the message
		}
			break;

		case ECNCancelIoctl:
		{
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNCancelIoctl aMessage %08x"), this, &Message()) );
			RNodeInterface* sp = ServiceProvider();
			User::LeaveIfError(sp? KErrNone : KErrNotReady);

			CancelIoctl();
		}
			break;

#ifdef AVAILABILITY_READY
		case ECNEnumerateConnections:
			LOG(ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNEnumerateConnections aMessage %08x"), this, &Message()));
			CMMSockSubSession::ReceivedL(ECFActivityConnectionEnumerateConnections, TCFInternalEsock::TSubSess(ECNEnumerateConnections, Message()).CRef());
 			break;
#endif

		case ECNControl:
			ControlL();	//Decides how message gets completed
			break;

		case ECNProgress:
		    ProgressL();
			break;

		case ECNLastProgressError:
            LastProgressErrorL();
			break;

		case ECNGetOrSetParameters:
			GetOrSetParametersL();
			break;

		case ECNGetParametersResponseLength:
			GetParametersResponseL(ETrue);
			break;

		case ECNGetParametersResponse:
			GetParametersResponseL(EFalse);
			break;


		case ECNClose:
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tECNClose aMessage %08x"), this, &Message()) );
			SetClosing();
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ECNClose, TCFInternalEsock::TSubSess(ECNClose,Message()).CRef());
			break;

		case ESCPSStop:
            {
			LOG( ESockLog::Printf(KESockConnectionTag, _L8("CConnection %08x\tESCPSStop aMessage %08x"), this, &Message()) );

			// The filters match the legacy behaviour - subconnection has
			// to exist (implies Connection started).
			// Note: CNifAgentRef used to throw away the error returned
			// by the EMI compatibility layer hence we return KErrNone here.
		    TSubConnectionUniqueId subConnectionUniqueId = static_cast<TSubConnectionUniqueId>(Message().Int0());
		    if (ServiceProvider() &&
		        (ServiceProvider()->Type() & TCFClientType::EServProvider) &&
		        subConnectionUniqueId <= KNifEntireConnectionSubConnectionId+1)
    		    {
    			RNodeInterface* scpr = DefaultSubConnectionServiceProvider();

				if(scpr)
					{ // We can only start the 'stop' activity if we've got a service provider.
					RConnection::TConnStopType stopType = static_cast<RConnection::TConnStopType>(Message().Int1());

					// Validate the stop code.
					switch (stopType)
						{
					case RConnection::EStopNormal:
					case RConnection::EStopAuthoritative:
						break;

					default:
						User::Leave(KErrArgument);
						}

					// We start an activity to stop the subconnection and wait for it to send the TStopped response
					// message.  We cannot simply post the subconnection a message to stop because it causes the
					// response message to go stray as we would be performing an operation outside of an activity.
					// If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed
					// on leave.
					CMMSockSubSession::ReceivedL(
						ESCPSStop,
						TCFInternalEsock::TSubSess(
							ESCPSStop,
							Message()
							).CRef()
						);
					}
				else
					{ // Nothing to do
				    SetReturn(KErrNone);
					}
    		    }
    		else
        		{
    			User::Leave(KErrNotReady);
        		}
            }
			break;

		default:
			//iLegacyConnection must handle lifetimes of its own RMessage2s.
			iLegacyConnection.ProcessMessageL(Message());
			break;
		}
	}

void CConnection::GetReferenceL()
	{
	TName name;
	ComposeSubSessionName(this, name);
	Message().WriteL(0, name);
	}

_LIT_SECURITY_POLICY_C1(NifmanPolicyNetworkControl, ECapabilityNetworkControl);

/**
	Control method to send a general command towards the interface.
*/
void CConnection::ControlL()
	{
	TUint optionLevel = static_cast<TUint>(Message().Int0());
	TUint optionName  = static_cast<TUint>(Message().Int1());

	// Check that clients are not trying to use internal connection options
   	// make sure this doesn't colide with new control levels
	User::LeaveIfError((optionName & KConnInternalOptionBit)? KErrAccessDenied : KErrNone);

	switch(optionLevel)
		{
	case KCOLInterface:
		// This PlatSec check replaces that from Nifman's CNifSecureSession.
		if(!NifmanPolicyNetworkControl.CheckPolicy(Message()))
			{
			User::Leave(KErrPermissionDenied);
			}
		break;

	case KCOLConnection:
		switch(optionName)
			{
			case KCoEnableCloneOpen:
			case KCoDisableCloneOpen:
				SetCloneOpenPolicyL(optionName);
				break;

			default:
				iLegacyConnection.ControlL(optionName, optionLevel); //AConnectionLegacy decides when to complete message
				DontCompleteCurrentRequest(); //TLegacyControlMessage will complete the message
			}
		// KCOLConnection is all legacy stuff; no providers can/should help with it
		return;
		}

	TInt optionLength = SafeMessage().GetDesLengthL(2);
	if (optionName & (KConnReadUserDataBit | KConnWriteUserDataBit) && optionLength == 0)
		{
		User::Leave(KErrArgument); //No buffer for data
		}

	RNodeInterface* sp = ServiceProvider();
	User::LeaveIfError(sp? KErrNone : KErrNotReady);

	TLegacyControlMessage msg(SafeMessage());
	CMMSockSubSession::ReceivedL(SafeMessage().Function(), msg);
	DontCompleteCurrentRequest(); //TLegacyControlMessage will complete the message
	}


void CConnection::ProgressL()
    {
	if (iLastProgress.iStage != KConnectionUp
		&& iLastProgress.iStage != KConnectionDown
		&& iLastProgress.iStage != KConnectionUninitialised)
        {
		RNodeInterface* sp = ServiceProvider();
		User::LeaveIfError(sp? KErrNone : KErrNotReady);

		TCprRetrieveProgress msg(SafeMessage(), iLastProgress);
		CMMSockSubSession::ReceivedL(SafeMessage().Function(), msg);
        DontCompleteCurrentRequest(); //TCprRetrieveProgress will complete the message
        return;
        }

    TPckgBuf<TStateChange> progressBuf;
    progressBuf().iStage = iLastProgress.iStage;
    progressBuf().iError = iLastProgress.iError;

    Message().WriteL(0, progressBuf);
    SetReturn(KErrNone);
    }

void CConnection::LastProgressErrorL()
    {
    if (iLastProgress.iStage != KConnectionUp
		&& iLastProgress.iStage != KConnectionDown
		&& iLastProgress.iStage != KConnectionUninitialised)
        {
		RNodeInterface* sp = ServiceProvider();
		User::LeaveIfError(sp? KErrNone : KErrNotReady);

        TCprRetrieveLastProgressError msg(SafeMessage(), iLastProgressError);
		CMMSockSubSession::ReceivedL(SafeMessage().Function(), msg);
        DontCompleteCurrentRequest(); //TCprRetrieveLastProgressError will complete the message
        return;
        }

    TPckgBuf<TStateChange> progressBuf;
    progressBuf().iStage = iLastProgressError.iStage;
    progressBuf().iError = iLastProgressError.iError;
    ResetLastProgressError();

    Message().WriteL(0, progressBuf);
    SetReturn(KErrNone);
    }


void CConnection::GetOrSetParametersL()
    {
    if (ServiceProvider() == NULL)
        {
    	SetReturn(KErrNotReady);
    	return;
        }

    if (iCommsDataObject)
    	{
    	SetReturn(KErrInUse);
    	return;
    	}

	TInt cdoLength = Message().GetDesLengthL(0);
	RBuf8 cdoBuffer;
	cdoBuffer.CreateL(cdoLength);
	CleanupClosePushL(cdoBuffer);
	SafeMessage().ReadL(0, cdoBuffer);

   	TPtrC8 des(cdoBuffer);
   	iCommsDataObject = static_cast<XCommsDataObject*>(SMetaDataECom::LoadL(des));

	// Last minute sanity check
	if ((iCommsDataObject->OperationMode() == XCommsDataObject::EOperationGet
		&& !iCommsDataObject->IsGetSupported())
		|| (iCommsDataObject->OperationMode() == XCommsDataObject::EOperationSet
		&& !iCommsDataObject->IsSetSupported()))
		{
		CleanupStack::PopAndDestroy(); // cdoBuffer
		delete iCommsDataObject;
		iCommsDataObject = NULL;
		SetReturn(KErrCorrupt);
		return;
		}

   	// iCommsDataObject passed as *& and will be updated by the TGetOrSetParameters message
   	// upon error
	RNodeInterface* sp = ServiceProvider();
	if (sp == NULL)
		{
		delete iCommsDataObject;
		iCommsDataObject = NULL;
		SetReturn(KErrNotReady);
		return;
		}

	TGetOrSetParameters msg(SafeMessage(), iCommsDataObject);
	CMMSockSubSession::ReceivedL(SafeMessage().Function(), msg);
	DontCompleteCurrentRequest(); //TLegacyControlMessage will complete the message
	CleanupStack::PopAndDestroy(); // cdoBuffer
    }


void CConnection::GetParametersResponseL(TBool aReturnLength)
	{
    if (ServiceProvider() == NULL || !iCommsDataObject)
        {
    	SetReturn(KErrNotReady);
    	return;
        }

    if (aReturnLength)
    	{
    	// Client requesting required buffer length for the serialised object
    	iCommsDataObjectLength = iCommsDataObject->Length();
    	SetReturn(iCommsDataObjectLength);
    	}
    else
    	{
		RBuf8 cdoBuffer;
		cdoBuffer.CreateL(iCommsDataObjectLength);
		CleanupClosePushL(cdoBuffer);
		User::LeaveIfError(iCommsDataObject->Store(cdoBuffer));
		Message().WriteL(0, cdoBuffer);
		CleanupStack::PopAndDestroy();	// queryBundleBuffer

		delete iCommsDataObject;
		iCommsDataObject = NULL;
		iCommsDataObjectLength = 0;

		SetReturn(KErrNone);
    	}
	}

void CConnection::CancelIoctl()
	{
	const RPointerArray<CNodeActivityBase>& activities = Activities();
	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(activities[i]);
			if (act->iSafeMessage.Function() == ECNIoctl)
				{
				act->SetCancelRequest(SafeMessage());
				CMMSockSubSession::ReceivedL(act->iSafeMessage.Function(), TEBase::TCancel().CRef());
				DontCompleteCurrentRequest();
				}
			}
		}
	}

void CConnection::SetCloneOpenPolicyL(TUint aOptionName)
	{
	const TInt policyParamIndex = 2;

	switch (aOptionName)
		{
		case KCoEnableCloneOpen:
			{
			// Enable the ability to perform a clone open towards this instance.  Read the
			// security policy passed in the option and store it in this instance.
			TSecurityPolicyBuf cloneOpenPolicy;
			SafeMessage().ReadL(policyParamIndex, cloneOpenPolicy);
			User::LeaveIfError(iCloneOpenPolicy.Set(cloneOpenPolicy));
			iCloneOpenEnabled = ETrue;
			break;
			}
		case KCoDisableCloneOpen:
			{
			// Disable the ability to perform a clone open towards this instance.
			iCloneOpenEnabled = EFalse;
			break;
			}
		}
	}

TInt CConnection::CheckCloneOpenPolicy(const RMessagePtr2& aMessage) const
	{
	if (iCloneOpenEnabled && iCloneOpenPolicy.CheckPolicy(aMessage))
		return KErrNone;
	else
		return KErrPermissionDenied;
	}


/**
@internalTechnology
*/
void CConnection::ReturnInterfacePtrL(MPlatsecApiExt*& aInterface)
    {
    aInterface = this;
    }

// AllInterfaceNotification

/**
	Legacy RConnection::AllInterfaceNotification implementation
*/
namespace AllInterfaceNotificationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionAllInterfaceNotification, AllInterfaceNotification, TCFServiceProvider::TStart, CAllInterfaceNotificationActivity::NewL)
	NODEACTIVITY_ENTRY(KNoTag, TAddClient, TAwaitingStart, MeshMachine::TNoTag)
	// Find the tier manager and request notification from it.
	NODEACTIVITY_ENTRY(KNoTag, TFindTierManager, TAwaitingTierManager, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TJoinTierManager, TAwaitingJoinComplete, MeshMachine::TNoTag)

	// Start the notification activity and wait for notification
	NODEACTIVITY_ENTRY(KNoTag, TStartLinkNotification, TAwaitingLinkNotification, CoreStates::TCancelOrErrorOrTag<KNoTag>)

	// Enqueue the notification and wait for the next one - we loop here forever
	NODEACTIVITY_ENTRY(KNoTag, TEnqueueNotification, TAwaitingLinkNotification, CoreStates::TCancelOrErrorOrTag<KNoTag|EBackward>)

	// If we received a TCancel from CConnection, reply with TError to complete shutdown handshake. 
    NODEACTIVITY_ENTRY(KCancelTag, TCancelLinkNotification, TAwaitingLinkNotificationError, MeshMachine::TTag<KErrorTag>)
    THROUGH_TRIPLE_ENTRY(KErrorTag, TSendErrorToConnection, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, TLeaveTierManager) // no other sessions with tier so can safely fire & forget this
NODEACTIVITY_END()
}

namespace AllInterfaceNotificationActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(allInterfaceNotificationActivities)
	ACTIVITY_MAP_ENTRY(AllInterfaceNotificationActivity, AllInterfaceNotification)
ACTIVITY_MAP_END()
}

CAllInterfaceNotificationWorker::CAllInterfaceNotificationWorker(ESock::CConnection& aConnection) :
	ACFMMNodeIdBase(AllInterfaceNotificationActivities::allInterfaceNotificationActivities::Self()),
	iConnection(aConnection)
	{
	LOG_NODE_CREATE(KESockConnectionTag, CAllInterfaceNotificationWorker);
	}

CAllInterfaceNotificationWorker::~CAllInterfaceNotificationWorker()
	{
	LOG_NODE_DESTROY(KESockConnectionTag, CAllInterfaceNotificationWorker);
	}

void CAllInterfaceNotificationWorker::Received(TNodeContextBase& aContext)
    {
	TNodeSignal::TMessageId noPeerIds[] = {
    	TCFServiceProvider::TStart::Id(),
    	TNodeSignal::TMessageId()	// list terminator
    	};
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CAllInterfaceNotificationWorker::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CAllInterfaceNotificationWorker> ctx(*this, aMessage, aSender, aRecipient);
    CAllInterfaceNotificationWorker::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


