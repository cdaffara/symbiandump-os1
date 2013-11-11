// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ss_subconn.h"
#include "ss_subconnstates.h"
#include "ss_connstates.h"
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_roles.h>
#include <ss_glob.h>
#include <comms-infras/ss_sapshim.h>
#include "SS_conn.H"
#include <comms-infras/ss_nodemessages_subconn.h>
#include <cs_subconevents.h>

#include <comms-infras/ss_msgintercept.h>
#include "ss_internal_activities.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_sbcn, "ESockSSocks_sbcn");
#endif

using namespace ESock;
using namespace SubSessActivities;
using namespace NetStateMachine;
using namespace SubConnActivities;
using namespace SubSessStates;
using namespace CoreStates;
using namespace Messages;
using namespace MeshMachine;

#ifdef _DEBUG
_LIT (KCSubConnectionPanic,"CSubConnectionPanic");
#endif

//-=========================================================
//
//Actvities serving client (RSubConnection) requests
//
//-=========================================================
// Creates a subconnection subsession object. No longer triggers the building of the stack beneath it.
// This now occurs only when required
namespace SubConnectionCreate
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySubConnectionCreate, SubConCreate, TCFInternalEsock::TSubSess, SubConnActivities::CCreate::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCCreate>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, SubConnStates::TNoTagOrAttachToDefaultOrWaitForIncoming)

	// We are attaching to the default subconnection and can therefore build the subconnection stack immediately
	NODEACTIVITY_ENTRY(CoreNetStates::KAttachToDefault, SubConnStates::TSendBuildStack, SubConnStates::TAwaitingBuildStackResponse, MeshMachine::TNoTag)

	// Must be `ECreateNew` in which case we defer the building of the stack. We are done
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

	// Wait for incoming
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionNoBearer
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNoBearer, SubConnNoBearer, TCFControlProvider::TNoBearer, SubConnActivities::CNoBearer::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)

	// We don't have a bearer so get the stack built so that we do. Then we can proceed
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CNoBearer::TStoreFlowParams, CoreNetStates::TNoTagOrNoBearer)
	NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, SubConnStates::TSendBuildStack, SubConnStates::TAwaitingBuildStackResponse, MeshMachine::TNoTag)

	// We now/already have a bearer so go ahead with fetching a bearer for the data client that kicked us off
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TRequestServiceProvider, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)

	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CNoBearer::TSendBindToComplete, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionBuildStack
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBuildStack, SubConnBuildStack, TCFInternalEsock::TBuildStackRequest, SubConnActivities::CBuildStack::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubConnActivities::CBuildStack::TAwaitingBuildStack, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TJoinCPR, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CBuildStack::TRequestServiceProviderFromCPR, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, MeshMachine::TNoTagOrErrorTag)

	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CBuildStack::TSendBindToComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TLeaveCPR, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CBuildStack::TSendBuildStackResponse)

	NODEACTIVITY_ENTRY(KErrorTag, SubConnStates::TLeaveCPR, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TTag<KErrorTag>)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionStart
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, SubConStart, TCFInternalEsock::TSubSess, SubSessActivities::CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCStart>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TStartSubConnection, TECABState<CoreNetStates::TAwaitingStarted>, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionStop
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStop, SubConStop, TCFInternalEsock::TSubSess, SubSessActivities::CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCStop>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TStopSubConnection, TECABState<CoreNetStates::TAwaitingStopped>, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionClose
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, SubConClose, TCFInternalEsock::TSubSess, SubSessActivities::CCloseActivity::New)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCClose>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TCancelAndCloseClientExtIfaces, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCancelAllLegacyRMessage2Activities, ConnStates::TNoTagBlockedByLegacyRMessage2Activities)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TProcessClose, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TECABTransition<CoreNetStates::TSetIdleIfNoServiceProviders>, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionAddSocket
{
typedef TECABState<SubConnStates::TAwaitingBuildStackResponse> TAwaitingBuildStackResponse;
typedef MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse> TAwaitingApplyResponseOrError;

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDataClientJoin, SubConAddSocket, TCFInternalEsock::TSubSess, SubConnActivities::CRejoin::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCAddSocket>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, CoreNetStates::TNoTagOrNoBearer)

	// We have no bearer yet so must have the stack built beneath us before continuing
	NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, SubConnStates::TSendBuildStack, TECABState<SubConnStates::TAwaitingBuildStackResponse>, MeshMachine::TNoTag)

	// We have a bearer and can now go ahead adding a socket from the default sub connection
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TJoinTheOtherOwner, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendRejoinDataClientRequestToOldOwner, TAcceptErrorState<CoreNetStates::TAwaitingRejoinDataClientComplete>, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KErrorTag, SubConnStates::TLeaveTheOtherOwner, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TTag<KErrorTag>)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)

	//When adding socket, add first, remove later.
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendApplyToNewOwner, TAwaitingApplyResponseOrError, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendApplyToOldOwner, TAwaitingApplyResponseOrError, MeshMachine::TNoTagOrErrorTag)

	NODEACTIVITY_ENTRY(KErrorTag, SubConnStates::TSendCancelToOldOwner, TAcceptErrorState<CoreStates::TNeverAccept>, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag,    SubConnStates::TLeaveTheOtherOwner, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionRemoveSocket
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityClientLeave, SubConRemoveSocket, TCFInternalEsock::TSubSess, SubConnActivities::CRejoin::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCRemoveSocket>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TJoinTheOtherOwner, TECABState<CoreStates::TAwaitingJoinComplete>, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendRejoinDataClientRequestToOldOwner, TAcceptErrorState<CoreNetStates::TAwaitingRejoinDataClientComplete>, MeshMachine::TNoTagOrErrorTag)
	//When removing socket, remove first, add later.
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendApplyToOldOwner, MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse>, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendApplyToNewOwner, MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse>, MeshMachine::TNoTagOrErrorTag)

	NODEACTIVITY_ENTRY(KNoTag,    SubConnStates::TLeaveTheOtherOwner, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KErrorTag, SubConnStates::TLeaveTheOtherOwner, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionSetParams
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityParamRequest, SubConSetParams, TCFInternalEsock::TSubSess, SubConnActivities::CSetParameters::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCSetParameters>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CSetParameters::TStoreNewParams, CoreNetStates::TNoTagOrNoBearer)

	// We have no bearer so are going to have to tell ourself to build the stack beneath us
	NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, SubConnStates::TSendBuildStack, SubConnStates::TAwaitingBuildStackResponse, MeshMachine::TNoTag)

	// Proceed with sending the parameters to our service provider
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendParamsToServiceProvider, TECABState<CoreNetStates::TAwaitingParamResponse>, MeshMachine::TNoTag)

	//it's the legacy behaviour to complete setparams once they have been verified (TParamsResponse received)
	//but not yet processed.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnActivities::CSetParameters::TCompleteClient, MeshMachine::TNoTag)

	// the params received in response should be stored before send apply
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TStoreParamsAndSendApply, TECABState<MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse> >, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionEventNotificationSubscription
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNotification, SubConEventSubscr, TCFInternalEsock::TSubSess, SubConnActivities::CEventNotification::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubConnStates::TAwaitingEventNotificationSubscription, SubConnStates::TNoTagOrAllNotifications)
	NODEACTIVITY_ENTRY        (KNoTag,                   SubConnStates::TSetupFilteredEventNotification, SubSessStates::TAwaitingIPC<ESCEventNotification>, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(ESCEventAllNotifications, SubConnStates::TSetupAllEventNotification, MeshMachine::TNoTag )
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, SubConnStates::TNoTagOrActiveWhenEventEnqued)

	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingSubConEvent, TTag<KActiveTag>)
	LAST_NODEACTIVITY_ENTRY(KActiveTag, SubConnStates::TFillInEvent)
NODEACTIVITY_END()
}

namespace SubConnectionEventNotification
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityConnectionWaitForIncoming, SubConEvent, TCFSubConnControlClient::TSubConnNotification)
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TEnqueueEvent, CoreNetStates::TAwaitingSubConEvent, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SubConnectionGetParamLength
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityGetParamLength, SubConGetParamLength, TCFInternalEsock::TSubSess, CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCGetParametersLength>, MeshMachine::TNoTag)
	// We must block if SubConnectionSetParams activity is currently running (the activity continues to run after it has completed its client)
	// as otherwise the stored parameters can be changed underneath us.  In particular, this can occur in-between
	// SubConnectionGetParamLength and SubConnectionGetParam, with the resulting length mismatch causing a KERN-EXEC-3 in the original
	// client RSubConnection::GetParameters() call.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, SubConnStates::TNoTagOrParamsPresentBlockedBySetParams )
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendParamRequest, CoreNetStates::TAwaitingParamResponse, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TStoreParams, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TWriteSubConnParamsLength)
	LAST_NODEACTIVITY_ENTRY(PRStates::KParamsPresent, SubConnStates::TWriteSubConnParamsLength)
NODEACTIVITY_END()
}

namespace SubConnectionGetParam
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityGetParam, SubConGetParams, TCFInternalEsock::TSubSess, CESockClientActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(SubSessStates::TAwaitingIPC<ESCGetParameters>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubSessStates::TAcquireMessageOwnership, SubConnStates::TNoTagOrParamsPresent )
	NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TSendParamRequest, CoreNetStates::TAwaitingParamResponse, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TStoreParams, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SubConnStates::TWriteSubConnParams)
	LAST_NODEACTIVITY_ENTRY(PRStates::KParamsPresent, SubConnStates::TWriteSubConnParams)
NODEACTIVITY_END()
}

//
//Activities serving framework requests
namespace SubConnectionWaitForIncomming
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBindTo, SubConnBindToRequest, TCFDataClient::TBindTo, PRActivities::CBindToActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBindTo, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBindToComplete)
NODEACTIVITY_END()
}

namespace SubConnectionLegacyRMessage2Activity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySubConnectionLegacyRMessage2Handler, SubConnectionLegacyRMessage2, TNodeSignal::TNullMessageId, ConnActivities::CConnLegacyRMessage2Activity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingLegacyRMessage2Ext, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, ConnStates::TProcessLegacyRMessage2, TAcceptErrorState<CoreNetStates::TAwaitingRMessage2Processed>, MeshMachine::TNoTagOrErrorTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, ConnStates::TCompleteRMessage2)
    LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}


namespace SubConnectionActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(subconnectionActivities)
	ACTIVITY_MAP_ENTRY(CoreErrorActivity, CoreError) //Must be first in the table
	ACTIVITY_MAP_ENTRY(SubConnectionStart, SubConStart)
	ACTIVITY_MAP_ENTRY(SubConnectionStop, SubConStop)
	ACTIVITY_MAP_ENTRY(SubConnectionCreate, SubConCreate)
	ACTIVITY_MAP_ENTRY(SubConnectionClose, SubConClose)
	ACTIVITY_MAP_ENTRY(SubConnectionAddSocket, SubConAddSocket)
	ACTIVITY_MAP_ENTRY(SubConnectionRemoveSocket, SubConRemoveSocket)
	ACTIVITY_MAP_ENTRY(SubConnectionSetParams, SubConSetParams)
	ACTIVITY_MAP_ENTRY(SubConnectionEventNotificationSubscription, SubConEventSubscr)
	ACTIVITY_MAP_ENTRY(SubConnectionEventNotification, SubConEvent)
	ACTIVITY_MAP_ENTRY(SubConnectionGetParamLength, SubConGetParamLength)
	ACTIVITY_MAP_ENTRY(SubConnectionGetParam, SubConGetParams)
	ACTIVITY_MAP_ENTRY(SubConnectionWaitForIncomming, SubConnBindToRequest)
	ACTIVITY_MAP_ENTRY(SubConnectionBuildStack, SubConnBuildStack)
	ACTIVITY_MAP_ENTRY(SubConnectionNoBearer, SubConnNoBearer)
    ACTIVITY_MAP_ENTRY(SubConnectionLegacyRMessage2Activity, SubConnectionLegacyRMessage2)
	ACTIVITY_MAP_ENTRY(PRDataClientJoinActivity, PRDataClientJoin)
	ACTIVITY_MAP_ENTRY(PRClientLeftActivity, PRClientLeft)
	ACTIVITY_MAP_ENTRY(PRClientLeaveActivity, PRClientLeave)
ACTIVITY_MAP_END()
}







//-=========================================================
//
//
//CSubConnection implementation
//
//
//-=========================================================
CSubConnection* CSubConnection::NewL(CConnection& aConnection, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/** Create a new CSubConnection instance

@param aConnProvider Connection Provider that this subconnection is a member of
@param aType Default or new subconnection
@param aSession Socket Session this subsession is to be a member of
@return pointer to new CSubConnection instance on success
@exception leaves if could not allocate memory
*/
	{
	RNodeInterface* conn = aConnection.ServiceProvider();
	__ASSERT_DEBUG(conn, User::Panic(KSpecAssert_ESockSSocks_sbcn, 1));
	User::LeaveIfError(conn? KErrNone : KErrNotReady);

	CSubConnection* sc = new (ELeave) CSubConnection(aPlayer, aSubSessionUniqueId);
    CleanupStack::PushL(sc);
	sc->ConstructL(aConnection);
	CleanupStack::Pop(sc);

	ESOCK_DEBUG_REGISTER_GENERAL_NODE(ESockDebug::KSubConnectionNodeUid, sc);

	return sc;
	}


void CSubConnection::ProcessMessageL()
	{
	LOG_DETAILED(ESockLog::Printf(_L("CSubConnection(%08x)::ProcessMessageL:\tCommand %d"), this, Message().Function()));

	switch (Message().Function())
     {
		case ESCAddSocket:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCAddSocket aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCAddSocket, TCFInternalEsock::TSubSess(ESCAddSocket,Message()).CRef());
			break;

		case ESCRemoveSocket:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCRemoveSocket aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCRemoveSocket, TCFInternalEsock::TSubSess(ESCRemoveSocket,Message()).CRef());
			break;

		case ESCSetParameters:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCSetParameters aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCSetParameters, TCFInternalEsock::TSubSess(ESCSetParameters,Message()).CRef());
			break;

		case ESCGetParameters:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCGetParameters aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCGetParameters, TCFInternalEsock::TSubSess(ESCGetParameters,Message()).CRef());
			break;

		case ESCGetParametersLength:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCGetParametersLength aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCGetParametersLength, TCFInternalEsock::TSubSess(ESCGetParametersLength,Message()).CRef());
			break;

        case ESCEventNotificationSetup:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCEventNotificationSetup aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ESCEventNotification, TCFInternalEsock::TSubSess(ESCEventNotificationSetup,Message()).CRef());
			break;

        case ESCEventNotification:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCEventNotification aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ECFActivityNotification, TCFInternalEsock::TSubSess(ESCEventNotification,Message()).CRef());
			break;

        case ESCEventAllNotifications:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCEventAllNotifications aMessage %08x"), this, &Message()) );
			CMMSockSubSession::ReceivedL(ESCEventNotification, TCFInternalEsock::TSubSess(ESCEventAllNotifications,Message()).CRef());
			break;

        case ESCEventNotificationCancel:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCEventNotificationCancel aMessage %08x"), this, &Message()) );
			//CANCELATION - Do not call DontCompleteCurrentRequest() for ECFActivityNotification to be completed!
			CMMSockSubSession::ReceivedL(ESCEventNotification, TEBase::TCancel().CRef());
			break;

		case ESCControl:
			{
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCControl aMessage %08x"), this, &Message()) );
			if (ServiceProvider() == NULL)
    			{
    			User::Leave(KErrNotReady);
    			}
			TLegacyControlMessage msg(SafeMessage());
			CMMSockSubSession::ReceivedL(SafeMessage().Function(), msg);
			DontCompleteCurrentRequest(); //TLegacyControlMessage will complete the message
			}
			break;

		case ESCStart:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCStart aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCStart, TCFInternalEsock::TSubSess(ESCStart,Message()).CRef());
			break;

		case ESCStop:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCStop aMessage %08x"), this, &Message()) );
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCStop, TCFInternalEsock::TSubSess(ESCStop,Message()).CRef());
			break;

		case ESCClose:
			LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCClose aMessage %08x"), this, &Message()) );
			SetClosing();
			//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
			CMMSockSubSession::ReceivedL(ESCClose, TCFInternalEsock::TSubSess(ESCClose,Message()).CRef());
			break;

		default:
			SetReturn(KErrNotSupported);
		}
	}

/**
Constructor - set up name
*/
CSubConnection::CSubConnection(CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
	: CMMSockSubSession(SubConnectionActivities::subconnectionActivities::Self(), aPlayer->CurrentSession(), aPlayer, aSubSessionUniqueId),
	ASubSessionPlatsecApiExt(UniqueId()),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	LOG_NODE_CREATE(KESockComponentTag, CSubConnection);
	}

class TRevertAddClient
    {
    public:
    TRevertAddClient(MeshMachine::AMMNodeBase& aOwner, const TNodeId& aClient)
    :iOwner(aOwner),
     iClient(aClient)
     {}

    static void Do(TAny* aRevertAddClient)
        {
        TRevertAddClient& This = *reinterpret_cast<TRevertAddClient*>(aRevertAddClient);
        This.Do();
        }

    void Do()
        {
        iOwner.RemoveClient(iClient);
        }

    private:
    MeshMachine::AMMNodeBase&       iOwner;
    const TNodeId&    iClient;
    };

void CSubConnection::ConstructL(CConnection& aConnection)
	{
	CMMSockSubSession::ConstructL();
	AddClientL(aConnection.Id(), TClientType(TCFClientType::ECtrlProvider));
	TRevertAddClient cleanupRevert(*this, aConnection.Id());
	CleanupStack::PushL(TCleanupItem(TRevertAddClient::Do, &cleanupRevert));
	aConnection.AddClientL(Id(), TClientType(TCFClientType::EData));
	CleanupStack::Pop();
	}

/**
Destructor, clean up connection preferences and the subconnection provider
*/
CSubConnection::~CSubConnection()
	{
	if (!iParameterBundle.IsNull())
		{
		iParameterBundle.Close();
		}

	if (CMMSockSubSession::ControlProvider())
    	{
        ControlProvider().RemoveClient(Id());
    	RemoveClient(ControlProvider().Id());
    	}

    CRefCountOwnedSubConNotification* event = NULL;
    while (iEventQueue.Deque(event))
        {
        __ASSERT_DEBUG(event, User::Panic(KSpecAssert_ESockSSocks_sbcn, 2));
        event->Close();
        event = NULL;
        }

    LOG_NODE_DESTROY(KESockComponentTag, CSubConnection);
	}

void CSubConnection::Received(TNodeContextBase& aContext)
    {
    TNodeSignal::TMessageId noPeerIds[] = {
    	TCFInternalEsock::TSubSess::Id(),
    	TCFPeer::TJoinRequest::Id(),
    	TEPeer::TLeaveRequest::Id(),
    	TNodeSignal::TMessageId()
    	};
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CSubConnection::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	//First check if this is not our own error
	if (aMessage.IsMessage<TEBase::TError>()
		&& aSender == Id())
		{
		//This is our own error, generated from the client (IPC) activity
		//It is safe to just ignore it here, as the activity is
		//completed (and so is the client RMessage2). If the activity didn't have
		//the chance to acquire ownership of the message it is still safe to ignore it
		//because there was no call to DoNotCompleteCurrentMessage().
		//return KErrNone;
		}

	TNodeContext<CSubConnection> ctx(*this, aMessage, aSender, aRecipient);
    CSubConnection::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CSubConnection::FinalCompleteAllBlockedMessages(TInt /*aResult*/)
	{
	TNodeNullContext ctx(*this);
	AbortActivitiesOriginatedBy(ctx); //Abort all activities
	}

CConnection& CSubConnection::ControlProvider()
    {
    RNodeInterface* controlProv = CMMSockSubSession::ControlProvider();
	__ASSERT_DEBUG(controlProv, User::Panic(KCSubConnectionPanic,KPanicNoConnection));
#if defined(__GCCXML__)
    return *reinterpret_cast<CConnection*>(&(controlProv->RecipientId().Node()));
#else
    return mcfnode_cast<CConnection>(controlProv->RecipientId().Node());
#endif
    }

EXPORT_C RCFParameterFamilyBundleC& CSubConnection::GetOrCreateParameterBundleL()
	{
	if( ! iParameterBundle.IsNull())
		{
		return iParameterBundle;
		}
	return CreateParameterBundleL();
	}

EXPORT_C RCFParameterFamilyBundleC& CSubConnection::CreateParameterBundleL()
	{
	__ASSERT_DEBUG(iParameterBundle.IsNull(), User::Panic(KSpecAssert_ESockSSocks_sbcn, 3));

	RCFParameterFamilyBundle newBundle;
	newBundle.CreateL();
	iParameterBundle.Open(newBundle);

	return iParameterBundle;
	}


