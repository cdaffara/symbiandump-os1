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
 @internalComponent
*/

#include "ss_connlegacy.h"
#include <ss_std.h>
#include "ss_connstates.h"
#include <comms-infras/sockmes.h>
#include <cdblen.h>
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include "SS_rslv.H"
#include <ss_sock.h>
#include <comms-infras/ss_sapshim.h>
#include <comms-infras/ss_connsettings.h>
#include "ss_subconn.h"
#include <comms-infras/esockmessages.h>

#include <comms-infras/ss_datamonitoringprovider.h>
#include <comms-infras/nifif.h>
#include "SS_conn.H"
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_roles.h>
#include <commdbconnpref.h> //TCommDbConnPref
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <elements/nm_messages_child.h>
#include "ss_internal_activities.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockscnLgc, "ESockSSockscnLgc");
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;
using namespace Factories;
using namespace Den;


//************************************* old stuff ********************************//



//const TInt KNumberOfEmulatedSubConnections = 2;

AConnectionLegacy::~AConnectionLegacy()
    {
   	TSubConnectionEvent* subConnectionEvent;
	while (iSubConnectionEventQueue.Deque(subConnectionEvent))
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tDequeuing and deleting subconnection event(%08x)"), this, subConnectionEvent));
		delete subConnectionEvent;
		subConnectionEvent = NULL;
		}

	iConnectionSocketBuf.Close();
	iConnectionClientBuf.Close();
	iConnectionInfoPtrArray.ResetAndDestroy();
	FinalCompleteAllBlockedMessages(KErrCancel);
	for (TInt i = 0; i < iSubConnections.Count(); ++i)
		{
		iSubConnections.Close();
		}
	}

void AConnectionLegacy::CancelProviderRequests()
	{
//	CancelServiceChangeNotification();
	}

void AConnectionLegacy::FinalCompleteAllBlockedMessages(TInt /* aReason */)
	{
	// If the all interface notification worker is running then post a
	// destroy to it.
	if(!iAllInterfaceNotificationWorker.IsNull())
		{
		TNodeCtxId dest(ECFActivityConnectionAllInterfaceNotification, iAllInterfaceNotificationWorker);
		RNodeInterface::OpenPostMessageClose(iConnection.Id(), dest, TEBase::TCancel().CRef());
		iAllInterfaceNotificationWorker.SetNull();
		}
	}



namespace ESock
{
class XConnectionFactoryAPQuery : public Factories::MFactoryQuery
	{
public:
	XConnectionFactoryAPQuery(TInt aAp)
	:iAp( aAp ) {}

protected:
	TInt iAp;

public:
	virtual TMatchResult Match(TFactoryObjectInfo& aConnectionInfo);
	};
}

MFactoryQuery::TMatchResult XConnectionFactoryAPQuery::Match(TFactoryObjectInfo& aConnectionInfo)
	{
	CConnectionProviderBase* connProv = static_cast<CConnectionProviderBase*>(aConnectionInfo.iInfo.iFactoryObject);
	return connProv->ProviderInfo().APId() == iAp ? MFactoryQuery::EMatch : MFactoryQuery::EContinue;
	}


//static const TUid K_CIPProtoConnectionProviderFactory_iUid = {0x10281DD3};

void AConnectionLegacy::CompleteAttachL(ESock::TSelectionPrefs& aPrefs)
	{
	//We have built the new top (legacy) Cpr and now we need to make sure that
	//it is properly joined to the layer directly below it (i.e. IpProto).
	TConnPref& cp = aPrefs.Prefs();
	__ASSERT_DEBUG(cp.ExtensionId()==TConnPref::EConnPrefCommDb, User::Panic(KSpecAssert_ESockSSockscnLgc, 1)); //Must be legacy now

	__ASSERT_DEBUG(iConnection.ServiceProvider(), User::Panic(KSpecAssert_ESockSSockscnLgc, 2)); //The CConnection has been already joined
	RNodeInterface* sp = iConnection.ServiceProvider();
	Messages::ANode& connProvNode = sp->RecipientId().Node();
    MeshMachine::AMMNodeBase* cpr = reinterpret_cast<MeshMachine::AMMNodeBase*>(connProvNode.FetchNodeInterfaceL(AMMNodeBase::KInterfaceId));
    __ASSERT_DEBUG(cpr->GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider))==NULL, User::Panic(KSpecAssert_ESockSSockscnLgc, 3)); //This is why we are "legacy"

	CConnectionFactoryContainer& container = *static_cast<CPlayer&>(iConnection.Player()).SockManGlobals()->iConnectionFactories;
	CCommsFactoryBase* factory = static_cast<CCommsFactoryBase*>(container.FindFactory(TUid::Uid(KIPProtoConnectionProviderFactoryUid)));
    User::LeaveIfError(factory? KErrNone : KErrArgument);

    XConnectionFactoryAPQuery query(static_cast<const TCommDbConnPref&>(cp).IapId());
    CConnectionProviderBase* ipProtoCpr = static_cast<CConnectionProviderBase*>(factory->Find(query));
    User::LeaveIfError(ipProtoCpr? KErrNone : KErrArgument);

	cpr->AddClientL(ipProtoCpr->Id(),TClientType(TCFClientType::EServProvider, TCFClientType::EActive));
	TUint clientFlags = 0;
	if (aPrefs.Flags()&TSelectionPrefs::EMonitor)
		{
		__ASSERT_DEBUG(iConnection.IsMonitor(), User::Panic(KSpecAssert_ESockSSockscnLgc, 4)); //Well, we are attaching as a monitor, must be consistent!
		clientFlags = TCFClientType::EMonitor;
		}

	TRAPD(error,ipProtoCpr->AddClientL(cpr->Id(),TClientType(TCFClientType::ECtrl,clientFlags)));
	if (error!=KErrNone)
		{
		iConnection.ServiceProvider()->PostMessage(ipProtoCpr->Id(), TEChild::TLeft().CRef());
		User::Leave(error);
		}

	// Make sure the MCPR knows that the CPR has a new control client
	ipProtoCpr->ControlProvider()->PostMessage(ipProtoCpr->Id(), TCFControlProvider::TActive().CRef());

	iConnection.ServiceProvider()->SetFlags(TCFClientType::EStarted);
	}

/**
Process RConnection messages
@exception Leaves on any error processing the request
*/
TInt AConnectionLegacy::ProcessMessageL(const RMessage2& aMessage)
	{
	LOG_DETAILED( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy %08x:\tCommand %d"), this, aMessage.Function()) );
	switch (aMessage.Function())
		{
		case ECNAllSubConnectionNotification:
			AllSubConnectionNotificationL();
			break;

		case ECNCancelAllSubConnectionNotification:
			CancelAllSubConnectionNotification();
			break;

		case ECNEnumerateSubConnections:
			EnumerateSubConnectionsL(iConnection.SafeMessage());
			break;

		case ECNAllInterfaceNotification:
			AllInterfaceNotificationL(aMessage);
			break;

		case ECNCancelAllInterfaceNotification:
			CancelAllInterfaceNotification();
			break;
			
		case ECNGetConnectionInfo:
			GetConnectionInfoL(aMessage);
			break;

		case ECNServiceChangeNotification:
			RequestServiceChangeNotificationL(iConnection.SafeMessage());
			break;

		case ECNCancelServiceChangeNotification:
			CancelServiceChangeNotification(iConnection.SafeMessage());
			break;

		case ECNGetIntSetting:
			GetIntSettingL(iConnection.SafeMessage());
			break;

		case ECNGetBoolSetting:
			GetBoolSettingL(iConnection.SafeMessage());
			break;

		case ECNGetDes8Setting:
			GetDes8SettingL(iConnection.SafeMessage());
			break;

		case ECNGetDes16Setting:
			GetDes16SettingL(iConnection.SafeMessage());
			break;

		case ECNGetLongDesSetting:
			GetLongDesSettingL(iConnection.SafeMessage());
			break;

		case ESCPSProgressNotification:
			iConnection.SetReturn(KErrNotSupported);
			break;

		case ESCPSCancelProgressNotification:
			break;

		case ESCPSDataTransferred:
			DataTransferredL(iConnection.SafeMessage());
			break;

		case ESCPSDataTransferredCancel:
			break;

		case ESCPSDataSentNotificationRequest:
			DataSentNotificationRequestL(iConnection.SafeMessage());
			break;

		case ESCPSDataSentNotificationCancel:
			DataSentNotificationCancel(iConnection.SafeMessage());
			break;

		case ESCPSDataReceivedNotificationRequest:
			DataReceivedNotificationRequestL(iConnection.SafeMessage());
			break;

		case ESCPSDataReceivedNotificationCancel:
			DataReceivedNotificationCancel(iConnection.SafeMessage());
			break;

		case ESCPSIsSubConnectionActiveRequest:
			IsSubConnectionActiveRequestL(iConnection.SafeMessage());
			break;

		case ESCPSIsSubConnectionActiveCancel:
			IsSubConnectionActiveCancel(iConnection.SafeMessage());
			break;

		case ESCPSGetSubConnectionInfo:
			GetSubConnectionInfoL(iConnection.SafeMessage());
			break;

		default:
			iConnection.SetReturn(KErrNotSupported);
			LOG(ESockLog::Printf(KESockConnectionTag, _L8("CConnectionLegacy(%08x)::ProcessMessageL - unsupported IPC %d received"), this, aMessage.Function()));
		}
		return KErrNone;
	}

/**
Control method to send a general command towards the interface.
*/
void AConnectionLegacy::ControlL(TUint aOptionName, TUint aMessageId)
	{
	if (aMessageId == KCOLConnection)
		{
		switch(aOptionName)
			{
			case KCoEnumerateConnectionClients:
				EnumerateConnectionClientsL();
				return;
			case KCoGetConnectionClientInfo:
				GetConnectionClientInfoL();
				return;
			case KCoEnumerateConnectionSockets:
				EnumerateConnectionSocketsL();
				return;
			case KCoGetConnectionSocketInfo:
				GetConnectionSocketInfoL();
				return;
			default:
				User::Leave(KErrNotSupported);
			}
		}
	}


/**
Get the number of currently active subconnections
@see RConnection::EnumerateSubConnections for notes on the difference in behaviour between this function and EnumerateConnectionsL()
*/
void AConnectionLegacy::EnumerateSubConnectionsL(const Den::RSafeMessage& aMessage)
	{
	RNodeInterface* sp = iConnection.ServiceProvider();
    if(!sp || !(sp->Flags() & TCFClientType::EStarted))
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L8("CConnection (AConnectionLegacy) [this=%08x] EnumerateSubConnectionsL KErrNotReady(-18)"), &iConnection));
        iConnection.SetReturn(KErrNotReady);
        return;
		}

	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), TLegacyEnumerateSubConnections(aMessage));
	iConnection.DontCompleteCurrentRequest();
	}


void AConnectionLegacy::AllInterfaceNotificationL(const RMessage2& aMessage)
	{
	if(!iAllInterfaceNotificationMessage.IsNull() || iConnection.ServiceProvider())
		{
		iConnection.SetReturn(KErrInUse);
		return;
		}

	iAllInterfaceNotificationMessage = aMessage;
	iConnection.DontCompleteCurrentRequest();

	if(iAllInterfaceNotificationWorker.IsNull())
		{
		CAllInterfaceNotificationWorker* worker = new(ELeave) CAllInterfaceNotificationWorker(iConnection);
		iAllInterfaceNotificationWorker = worker->Id();

		TNodeCtxId dest(ECFActivityConnectionAllInterfaceNotification, iAllInterfaceNotificationWorker);
		RNodeInterface::OpenPostMessageClose(iConnection.Id(), dest, TCFServiceProvider::TStart().CRef());
		}

	CompleteAllInterfaceNotificationL(KErrNone);
	}

void AConnectionLegacy::CompleteAllInterfaceNotificationL(TInt aError)
	{
	if (!iAllInterfaceNotificationMessage.IsNull())
		{
		if(aError == KErrNone)
			{
			if (!iNotificationQueue.IsEmpty())
				{
				TInterfaceNotification notification;
				iNotificationQueue.Deque(notification);

				TInterfaceNotificationBuf buf(notification);

				// Write the buffer to the client
				iAllInterfaceNotificationMessage.WriteL(0, buf);
			    LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] CompleteAllInterfaceNotificationL, RMessage2::Complete (%08x) with 0"), &iConnection, iAllInterfaceNotificationMessage.Handle()));
				iAllInterfaceNotificationMessage.Complete(KErrNone);
				iAllInterfaceNotificationMessage = RMessage2();
				}
			}
		else
			{
            LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] CompleteAllInterfaceNotificationL, RMessage2::Complete (%08x) with %d"), &iConnection, iAllInterfaceNotificationMessage.Handle(), aError));
			iAllInterfaceNotificationMessage.Complete(aError);
			iAllInterfaceNotificationMessage = RMessage2();
			}
		}
	}

void AConnectionLegacy::CancelAllInterfaceNotification()
	{
	if(!iAllInterfaceNotificationMessage.IsNull())
		{
		iAllInterfaceNotificationMessage.Complete(KErrCancel);
		}
	}

void AConnectionLegacy::RequestServiceChangeNotificationL(const Den::RSafeMessage& aMessage)
	{
	RNodeInterface* currServiceProvider = iConnection.ServiceProvider();
    if (!currServiceProvider)
        {
		LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] RequestServiceChangeNotificationL KErrNotReady"), &iConnection));
        iConnection.SetReturn(KErrNotReady);
        return;
        }

	if(iServiceChangeNotificationPending)
		{
		iConnection.SetReturn(KErrInUse);
		return;
		}

    // This message needs to be directed towards the MCpr - Service change notification occurs
    // during reconnection. We violate the principle of left hand nodes not instructing right
    // hand nodes. Since this is for legacy support its ok for now. The CPr that implements
    // the Api is expected to forward this message to its MCpr
	iServiceChangeNotificationPending = ETrue;

	TCprRequestServiceNotification msg(iConnection.Id(), aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);

	iConnection.DontCompleteCurrentRequest();
	}

void AConnectionLegacy::CancelServiceChangeNotification(const Den::RSafeMessage& aMessage)
	{
    // This message needs to be directed towards the MCpr - Service change notification occurs
    // during reconnection. We violate the principle of left hand nodes not instructing right
    // hand nodes. Since this is for legacy support its ok for now. The CPr that implements
    // the Api is expected to forward this message to its MCpr
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] CancelServiceChangeNotification KErrCancel"), &iConnection));	
	const RPointerArray<CNodeActivityBase>& activities = iConnection.Activities();
	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(activities[i]);
			if (act->iSafeMessage.Function() == ECNServiceChangeNotification)
				{
				act->SetCancelRequest(aMessage);
				iConnection.CMMSockSubSession::ReceivedL(act->iSafeMessage.Function(), TEBase::TCancel().CRef());
				iConnection.DontCompleteCurrentRequest();
				}
			}
		}
	iServiceChangeNotificationPending = EFalse;
	}


/**
Return information about a single connection that was previously
enumerated using EnumerateConnectionsL

This is the server-side implementation of the RConnection
function TInt GetConnectionInfo(TUint aIndex, TDes8& aConnectionInfo).
*/
void AConnectionLegacy::GetConnectionInfoL(const RMessage2& aMessage)
	{
	TUint index = aMessage.Int0();

	// Make sure the client isn't using an invalid argument (*client* connection numbering starts from 1)
	if(index == 0)
		{
		iConnection.SetReturn(KErrArgument);
		return;
		}

	// Make sure the client isn't trying to access a connection that doesn't exist
	if(index > static_cast<TUint>(iConnectionInfoPtrArray.Count()))
		{
		iConnection.SetReturn(KErrNotFound);
		return;
		}

	index = index - 1;	// alter the client-provided index (indexed from one) to act as a index for the array (indexed from zero)

	// check the client argument's version to see which one it's using
	TConnArgBase clientArg;
	TPckg<TConnArgBase> argPckg(clientArg);
	aMessage.ReadL(1, argPckg);
	TUint8 clientVersion = clientArg.Version();

	// Translate between versions of TConnectionInfo depending on what client passes in
	__ASSERT_DEBUG(iConnectionInfoPtrArray[index]->Version() == KConnArgVersion2, User::Panic(KSpecAssert_ESockSSockscnLgc, 5));	// check that the connection provider has put TConnectionInfoV2's in the queue

	if(clientVersion == KConnArgVersion1)
		{
		TConnectionInfo connectionInfoV1;
		connectionInfoV1.iIapId = iConnectionInfoPtrArray[index]->iIapId;
		connectionInfoV1.iNetId = iConnectionInfoPtrArray[index]->iNetId;
		TPckg<TConnectionInfo> info(connectionInfoV1);
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Client called GetConnectionInfo (client ver=%d, esock ver=%d), writing [%S](%d bytes) to client side"), clientArg.Version(), iConnectionInfoPtrArray[index]->Version(), &info, info.Size()); )

		aMessage.WriteL(1, info);
		}
	else
		{
		if(clientVersion == KConnArgVersion2)
			{
			// downcast from TConnectionInfo pointer to TConnectionInfoV2 pointer; we can do this safely because the earlier ASSERT() checked that these were TConnectionInfoV2s
			TPckg<TConnectionInfoV2> info(*(static_cast<TConnectionInfoV2*>(iConnectionInfoPtrArray[index])));
			LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Client called GetConnectionInfo (client ver=%d, esock ver=%d), writing [%S](%d bytes) to client side"), clientArg.Version(), iConnectionInfoPtrArray[index]->Version(), &info, info.Size()); )
			aMessage.WriteL(1, info);
			}
		else
			{
			LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Invalid version of argument to GetConnectionInfo (client ver=%d, esock ver=%d)"), clientArg.Version(), iConnectionInfoPtrArray[index]->Version()); )
			iConnection.SetReturn(KErrArgument);
			}
		}
	}


/**
Extract a single enumeration information element from a buffer of elements

@param aBuffer the buffer from which to extract the information element
@param aCount the total number of elements in the buffer
@param aSize the size of each information element
@param aIndex the position of the element to retrieve in the buffer. Index is 1-based.
@param aPckg the TPckg<> in which to return the information element
@exception leaves with KErrOverflow if the index given is past the end of the buffer
*/
void AConnectionLegacy::ExtractEnumInfoL(RBuf8& aBuffer, TUint aCount, TInt aSize, TUint aIndex, TDes8& aPckg)
	{
	if (aBuffer.Length() == 0)
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy %08x ExtractEnumInfoL KErrNotReady"), this) );
		User::Leave(KErrNotReady);
		}
	else if (aIndex < 1)
		{
		User::Leave(KErrArgument);
		}
	else if (aIndex > aCount)
		{
		User::Leave(KErrNotFound);
		}

	__ASSERT_DEBUG((TUint) aBuffer.Length() == (aCount * aSize), User::Panic(KSpecAssert_ESockSSockscnLgc, 6));

	const TUint8* ptr = aBuffer.Ptr();

	TInt start = aSize * (aIndex-1);
	ptr += start;

	aPckg.Copy(ptr, aSize);
	}

/**
Gather information on the number of clients of a particular connection

This is the server-side implementation of the RConnection
function TInt EnumerateConnectionClients(TUint aIndex, TUint& aCount).
*/
void AConnectionLegacy::EnumerateConnectionClientsL()
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy::EnumerateConnectionClientsL()")));
	// Cover the remote case that another client thread is already enumerating the connection in a similar way - when it all
	// completed synchronously in the same thread there was no possibility of this problem. Attempting the same queue behaviour
	// here seems pointless: it's a very rare case of a rare case of a deprecated API which was never documented as working
	// that way. So instead we complete the client with KErrBusy
	if(!iEnumConnMsg.IsNull())
		{
		User::Leave(KErrServerBusy);
		}

	iConnectionClientCount = 0;
	iDPEnumCount = &iConnectionClientCount;
	iConnectionClientBuf.Close();
	
	iDPEnumBuf = &iConnectionClientBuf;
	RequestDataPlaneEnumerationL(EAll);
	}


/**
Returns information about a single connection client that was previously
enumerated using EnumerateConnectionClientsL

This is the server-side implementation of the RConnection
function TInt GetConnectionClientInfo(TUint aIndex, TDes8& aClientInfo).
*/
void AConnectionLegacy::GetConnectionClientInfoL()
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy::GetConnectionClientInfoL()")));
	TConnectionGetClientInfoArg args;
	TUint8 ourVersion = args.Version();

	const RSafeMessage& message(iConnection.SafeMessage());
	TPckg<TConnectionGetClientInfoArg> argsPckg(args);
	message.ReadL(2, argsPckg);

	// check that the client argument's version is the same as the one we're using
	if(args.Version() != ourVersion)
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Invalid version of argument to GetConnectionClientInfo (client ver=%d, esock ver=%d)"), args.Version(), ourVersion) );
		User::Leave(KErrArgument);
		}

	TConnectionClientInfo clientInfo;
	TPckg<TConnectionClientInfo> info(clientInfo);
	ExtractEnumInfoL(iConnectionClientBuf, iConnectionClientCount, sizeof(TConnectionClientInfo), argsPckg().iIndex, info);

	argsPckg().iClientInfo = info();
	message.WriteL(2, argsPckg);
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] GetConnectionClientInfoL message (%08X) with KErrNone"), &iConnection, message.Handle()));
	message.Complete(KErrNone);
	}

/**
Gather information on the number of sockets of a particular connection

This is the server-side implementation of the RConnection
function TInt EnumerateConnectionSockets(TUint aIndex, TUint& aCount).
*/
void AConnectionLegacy::EnumerateConnectionSocketsL()
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy::EnumerateConnectionSocketsL")));
	// Cover the remote case that another client thread is already enumerating the connection in a similar way - when it all
	// completed synchronously in the same thread there was no possibility of this problem. Attempting the same queue behaviour
	// here seems pointless: it's a very rare case of a rare case of a deprecated API which was never documented as working
	// that way. So instead we complete the client with KErrServerBusy
	if(!iEnumConnMsg.IsNull())
		{
		User::Leave(KErrServerBusy);
		}

	// delete any previous connection client enumeration info
	iConnectionSocketCount = 0;
	iDPEnumCount = &iConnectionSocketCount;
	iConnectionSocketBuf.Close();
	
	iDPEnumBuf = &iConnectionSocketBuf;
	RequestDataPlaneEnumerationL(ESocket);
	}

/**
Returns information about a socket associated with this connection that has
been previously enumerated using EnumerateConnectionSocketsL

This is the server-side implementation of the RConnection
function TInt GetConnectionSocketInfo(TUint aIndex, TDes8& aSockettInfo).
*/
void AConnectionLegacy::GetConnectionSocketInfoL()
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy::GetConnectionSocketInfoL()")));
	TConnectionGetSocketInfoArg args;
	TUint8 ourVersion = args.Version();

	const RSafeMessage& message(iConnection.SafeMessage());
	TPckg<TConnectionGetSocketInfoArg> argsPckg(args);
	message.ReadL(2, argsPckg);

	if(args.Version() != ourVersion)
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Invalid version of argument to GetConnectionSocketInfo (client ver=%d, esock ver=%d)"), args.Version(), ourVersion) );
		User::Leave(KErrArgument);
		}

	TConnectionSocketInfo socketInfo;
	TPckg<TConnectionSocketInfo> info(socketInfo);
	ExtractEnumInfoL(iConnectionSocketBuf, iConnectionSocketCount, sizeof(TConnectionSocketInfo), argsPckg().iIndex, info);

	argsPckg().iSocketInfo = info();
	message.WriteL(2, argsPckg);
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("AConnectionLegacy [this=%08x] GetConnectionSocketInfoL message (%08X) with KErrNone"), &iConnection, message.Handle()));
	message.Complete(KErrNone);
	}

/**
Requests the Data plane to enumerate its subsessions checking for flows belonging to the set of SCPR ids
furnished in this request, returning either socket or client info. This is specifically to support the
legacy enumerations; no expectation that a more general scheme will ever be needed
*/
void AConnectionLegacy::RequestDataPlaneEnumerationL(TLegacyConnEnumClients aClientType)
	{
	iEnumConnSockCandidateSCPRs.Reset();
	TConnectionEnumArg args;
	TUint8 ourVersion = args.Version();

	TPckg<TConnectionEnumArg> argsPckg(args);
	const RSafeMessage& message(iConnection.SafeMessage());
	message.ReadL(2, argsPckg);

	if(args.Version() != ourVersion)
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AConnectionLegacy: Invalid version of argument to EnumerateConnectionSockets (client ver=%d, esock ver=%d)"), args.Version(), ourVersion) );
		User::Leave(KErrArgument);
		}

	TInt index = args.iIndex;

	if(index <= 0 || index > iConnectionInfoPtrArray.Count())
		{
		User::Leave(KErrArgument);
		}

	index -= 1; // convert from client index (starting from 1) to server index (starting from 0)

	TSourcedConnectionInfo* connInfo;
	connInfo = iConnectionInfoPtrArray[index];

	/**
	   For each CSockSubSession which is a connection, check if it uses the cpr identified in conninfo. If so add that connection as
	   a client (for EAll) and add any scprs to the scpr candidate list to be sent to the data plane.

	   Then query the data plane for CSockets and CHostResolvers. These are matched to the connection by the scprs in the candidate list
	*/	   
	CPlayer::TSubSessionContainer& subSess = iConnection.Player().SubSessions();
	for(TInt idx = subSess.Count() - 1; idx >=0; --idx)
		{
		TBool isClient = EFalse;
		
		const CSockSubSession* ss = static_cast<const CSockSubSession*>(subSess[idx]);
		if(ss->Type().iType == TCFSubSessInfo::EConnection)
			{
			const CConnection* conn = static_cast<const CConnection*>(ss);
			
			RNodeInterface* ipcprItf = conn->ServiceProvider();
			
			if(ipcprItf != NULL)
				{
				ACFMMNodeIdBase& ipcpr = static_cast<ACFMMNodeIdBase&>(ipcprItf->RecipientId().Node());

				RNodeInterface* lowerCprItf = ipcpr.ServiceProvider();
				while (lowerCprItf)
					{
					if(lowerCprItf->RecipientId() == connInfo->Provider())
						{
						isClient = ETrue;
						break;
						}
					
					ACFMMNodeIdBase& lowerCpr = static_cast<ACFMMNodeIdBase&>(lowerCprItf->RecipientId().Node());
					lowerCprItf = lowerCpr.ServiceProvider();
					}
				
				if (isClient)
					{
					// Add to client list
					if (aClientType == EAll)
						{
						TConnectionClientInfo info;
						TUidType uidType;
						ss->GetOwnerInfo(info.iProcessId, uidType, info.iThreadId);
						info.iUid = uidType.MostDerived();
						RBuf8& resDes = *iDPEnumBuf;
						
						if (!AConnectionLegacy::DoesConnectionInfoExist(resDes, info))
							{
							const TUint KEntrySize = sizeof(TConnectionClientInfo);
							if (resDes.Length() + KEntrySize > resDes.MaxLength())
								{
								resDes.ReAllocL(resDes.Length() + KEntrySize);
								}

							resDes.Append(reinterpret_cast<TUint8*>(&info), sizeof(info));
							++(*iDPEnumCount);
							}
						}

					// Build the list of candidate SCPR ids for the data plane to check against
					TClientIter<TDefaultClientMatchPolicy> scprIter = ipcpr.GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, 0));
					RNodeInterface* scpr;
					while((scpr = scprIter++) != NULL)
						{
						iEnumConnSockCandidateSCPRs.AppendL(scpr->RecipientId());
						}
					}
				}
			}
		}

	if(!iEnumConnSockCandidateSCPRs.Count())
		{
		// Failed to find the lower CPR; most likely it was torn down since the enumeration
		User::Leave(KErrNotReady);
		}

	iEnumConnMsg.Adopt(static_cast<Den::RSafeMessage&>(const_cast<RSafeMessage&>(iConnection.SafeMessage())));

	RNodeInterface::OpenPostMessageClose(iConnection.Id(), SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane)),
		TLegacyConnectionEnumRequest(iEnumConnSockCandidateSCPRs, aClientType));
	}

/**
Called when the Data plane returns the result of a flow enumeration, requested through RequestDataPlaneEnumerationL()
*/
void AConnectionLegacy::CompleteDataPlaneEnumeration(const TNodeId& aPeer, TInt aCount, HBufC8* aInfo, TInt aError)
	{
	iEnumConnSockCandidateSCPRs.Reset();
	if(aError == KErrNone)
		{
		__ASSERT_DEBUG(iDPEnumBuf, User::Panic(KSpecAssert_ESockSSockscnLgc, 7));

		/**
		   Merge the results from the dataplane with the results from
		   this plane if we are enumerating all connection clients.
		*/		
		if (iEnumConnMsg.Int1() == KCoEnumerateConnectionClients)
			{
			RBuf8& resDes = *iDPEnumBuf;
			TConnectionClientInfo info;
			TPckg<TConnectionClientInfo> des(info);
			
			const TUint KEntrySize = sizeof(TConnectionClientInfo);
			ASSERT((aCount * KEntrySize) == aInfo->Length());
			const TUint8* ptr = aInfo->Ptr();
			for (TInt i = 0; i < aCount && aError == KErrNone; i++)
				{
				des.Copy(ptr, KEntrySize);
				if (!DoesConnectionInfoExist(resDes, info))
					{
					aError = resDes.ReAlloc(resDes.Length()+KEntrySize);
					if (aError == KErrNone)
						{
						resDes.Append(des);
						(*iDPEnumCount)++;
						}
					}
				ptr += KEntrySize;
				}
			}
		else
			{
			*iDPEnumCount += aCount;
			aError = iDPEnumBuf->ReAlloc(iDPEnumBuf->Length() + aInfo->Length());
			iDPEnumBuf->Append(*aInfo);
			}
		}
	if(aError == KErrNone)
		{
		TConnectionEnumArg args;
		args.iCount = *iDPEnumCount;
		TPckg<TConnectionEnumArg> argsPckg(args);
		aError = iEnumConnMsg.Read(2, argsPckg);
		if (aError == KErrNone)
			{
			
			argsPckg().iCount = *iDPEnumCount;
			aError = iEnumConnMsg.Write(2, argsPckg);
			}
		}

	iEnumConnMsg.Complete(aError);
	THeapSwitcher switcher(SockManGlobals::Get()->SelfWorker()->PitBoss(), aPeer);
	delete aInfo;
	}


/**
Retrieve a TBool parameter setting.
*/
void AConnectionLegacy::GetBoolSettingL(const Den::RSafeMessage& aMessage)
	{
	TMCprGetConnectionSetting msg(TMCprGetConnectionSetting::EBoolSetting, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}


/**
Retrieve a TUint32 parameter setting.
*/
void AConnectionLegacy::GetIntSettingL(const Den::RSafeMessage& aMessage)
	{
	TMCprGetConnectionSetting msg(TMCprGetConnectionSetting::EIntSetting, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}


/**
Retrieve a TDes8 parameter setting.
*/
void AConnectionLegacy::GetDes8SettingL(const Den::RSafeMessage& aMessage)
	{
	TMCprGetConnectionSetting msg(TMCprGetConnectionSetting::EDes8Setting, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}


/**
Retrieve a TDes16 parameter setting.
*/
void AConnectionLegacy::GetDes16SettingL(const Den::RSafeMessage& aMessage)
	{
	TMCprGetConnectionSetting msg(TMCprGetConnectionSetting::EDes16Setting, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}


/**
Retrieve a Long TDes parameter setting.
*/
void AConnectionLegacy::GetLongDesSettingL(const Den::RSafeMessage& aMessage)
	{
	TMCprGetConnectionSetting msg(TMCprGetConnectionSetting::ELongDesSetting, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}

void AConnectionLegacy::SubConnectionEvent(const TSubConnectionEvent& aSubConnectionEvent)
	{
	// Pass the message onto the client if possible, or buffer if necessary
	if(!iAllSubConnectionNotificationMessage.IsNull())
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tdelivering new subconnection event to client (subconnection id: %d, event: %d)"), this, aSubConnectionEvent.iSubConnectionUniqueId, aSubConnectionEvent.iEventType));

		TPtrC8 tempDes(reinterpret_cast<const TUint8*>(&aSubConnectionEvent), aSubConnectionEvent.Length());
		TSubConnectionNotificationBuf subConnectionEventBuf(tempDes);
		TInt ret = iAllSubConnectionNotificationMessage.Write(0, subConnectionEventBuf);
		iConnection.CompleteMessage(iAllSubConnectionNotificationMessage,ret);
		}
	else	// no outstanding client messages - just buffer it
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tbuffering new subconnection event (subconnection id: %d, event: %d)"), this, aSubConnectionEvent.iSubConnectionUniqueId, aSubConnectionEvent.iEventType));

		// Create copy of event on heap
		TSubConnectionEvent* subConnectionEvent = 0;
		TRAPD(ret, subConnectionEvent = aSubConnectionEvent.CloneL());

		if(ret)	// if there's an error, write a message to the log - not much else we can do
			{
			LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tERROR - could not enque new subconnection event, error %d (subconnection id: %d, event: %d)"), this, ret, aSubConnectionEvent.iSubConnectionUniqueId, aSubConnectionEvent.iEventType));
			}
		else
			{
			if (iSubConnectionEventQueue.IsFull())
			    {
			    TSubConnectionEvent*& oldestSubConnectionEvent = iSubConnectionEventQueue.GetTheOldestElem();
			    LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tSubConnectionEvent - removing oldest subconnection event: %08x)"), this, oldestSubConnectionEvent));
			    delete oldestSubConnectionEvent;
			    }
			LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tSubConnectionEvent - enquing new subconnection event: %08x)"), this, subConnectionEvent));
			iSubConnectionEventQueue.Enque(subConnectionEvent);
			}
		}
	}

void AConnectionLegacy::AllSubConnectionNotificationL()
/**
Request for notification of subconnection events
@exception Leaves with KErrInUse if there is already an outstanding RMessage for all subconnection notification
*/
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tClient requested all subconnection notification..."), this));

	if(!iAllSubConnectionNotificationMessage.IsNull())
		{
		iConnection.SetReturn(KErrInUse);
		return;
		}

	if(iSubConnectionEventQueue.IsEmpty())	// if there is nothing in the queue, keep the RMessage
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection[%08x]:\tqueueing request."), this));
		iAllSubConnectionNotificationMessage = iConnection.Message();
    	iConnection.DontCompleteCurrentRequest();
		}
	else	// otherwise, send back the first item in the queue
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection[%08x]:\tsending back first event from queue."), this));

		TSubConnectionEvent* subConnectionEvent = NULL;
		iSubConnectionEventQueue.Deque(subConnectionEvent);
		LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tAllSubConnectionNotificationL - dequing subconnection event: %08x)"), this, subConnectionEvent));
		TPtrC8 tempDes(reinterpret_cast<const TUint8*>(subConnectionEvent), subConnectionEvent->Length());
		TSubConnectionNotificationBuf subConnectionEventBuf(tempDes);
		TInt err = iConnection.Message().Write(0, subConnectionEventBuf);

		// Delete original copy of event from heap
		delete subConnectionEvent;
		subConnectionEvent=NULL;

		if(err != KErrNone)
			{
			iConnection.SetReturn(err);
			}
		}
	}

void AConnectionLegacy::CancelAllSubConnectionNotification()
/**
Complete any outstanding subconnection notification message
*/
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tClient cancelled all subconnection notification"), this));
	iDeferredSubConnectionNotificationEnable = EFalse;	// Just in case
	iConnection.CompleteMessage(iAllSubConnectionNotificationMessage, KErrCancel);
	}

void AConnectionLegacy::GetSubConnectionInfoL(const Den::RSafeMessage& aMessage)
	{
	RNodeInterface* sp = iConnection.ServiceProvider();
    if(!sp)
		{
		LOG(ESockLog::Printf(KESockConnectionTag, _L8("CConnection (AConnectionLegacy) [this=%08x] GetSubConnectionInfo KErrNotReady"), &iConnection));
        iConnection.SetReturn(KErrNotReady);
        return;
		}

	TInt subConnIdx = static_cast<TInt>(aMessage.Int0());

   	// Find the size of the client's descriptor
   	TInt sizeOfSubConnInfo = aMessage.GetDesLengthL(1);

   	// Create an appropriately sized descriptor server-side
   	HBufC8* subConnInfoBuf;
   	subConnInfoBuf = HBufC8::NewL(sizeOfSubConnInfo);
   	CleanupStack::PushL(subConnInfoBuf);

   	TPtr8 subConnInfoPtr(subConnInfoBuf->Des());

   	// Read the client data across
	aMessage.ReadL(1, subConnInfoPtr);

	TSubConnectionInfo& subConnInfo = *(reinterpret_cast<TSubConnectionInfo*>(const_cast<TUint8*>(subConnInfoBuf->Ptr())));

	if(subConnInfo.iConnectionType != EConnectionGeneric)
		{
		CleanupStack::PopAndDestroy(subConnInfoBuf);
		iConnection.SetReturn(KErrArgument);
		return;
		}

	switch(subConnIdx)
		{
		case KNifEMCompatibilityLayerEntireSubConnectionUid:
			if(subConnInfo.iSubConnectionUniqueId != KNifEMCompatibilityLayerEntireSubConnectionUid &&
			   subConnInfo.iSubConnectionUniqueId != KNifEMCompatibilityLayerFakeSubConnectionId)
				{
				CleanupStack::PopAndDestroy(subConnInfoBuf);
				iConnection.SetReturn(KErrArgument);
				return;
				}			
			break;

		case KNifEMCompatibilityLayerFakeSubConnectionId:
		default:
			subConnInfo.iSubConnectionUniqueId = KNifEMCompatibilityLayerFakeSubConnectionId;

			break;
		}

	TLegacyGetSubConnectionInfo msg(iConnection.UniqueId(), subConnInfo, aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();

	CleanupStack::PopAndDestroy(subConnInfoBuf);
	}

void AConnectionLegacy::DataTransferredL(const RSafeMessage& aMessage)
	{
	TLegacyDataMonitoringTransferredRequest dmReq(iConnection.UniqueId(), aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), dmReq);
	iConnection.DontCompleteCurrentRequest();
	}

void AConnectionLegacy::DataSentNotificationRequestL(const RSafeMessage& aMessage)
	{
	DataMonitoringNotificationRequestL(aMessage, ESent);
	}

void AConnectionLegacy::DataReceivedNotificationRequestL(const RSafeMessage& aMessage)
	{
	DataMonitoringNotificationRequestL(aMessage, EReceived);
	}

void AConnectionLegacy::DataMonitoringNotificationRequestL(const RSafeMessage& aMessage, TDataMonitoringDirection aDirection)
	{
	TLegacyDataMonitoringNotificationRequest dmReq(aDirection, iConnection.UniqueId(), aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), dmReq);
	iConnection.DontCompleteCurrentRequest();
	}


void AConnectionLegacy::DataSentNotificationCancel(const Den::RSafeMessage& aMessage)
	{
	DataMonitoringNotificationCancel(aMessage, ESent);
	}

void AConnectionLegacy::DataReceivedNotificationCancel(const Den::RSafeMessage& aMessage)
	{
	DataMonitoringNotificationCancel(aMessage, EReceived);
	}

void AConnectionLegacy::DataMonitoringNotificationCancel(const Den::RSafeMessage& aMessage, TDataMonitoringDirection aDirection)
	{
	const RPointerArray<CNodeActivityBase>& activities = iConnection.Activities();
	TBool cancelled = EFalse;
	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(activities[i]);

			ASSERT(act->Message().IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
			ESock::TCFSigLegacyRMessage2Ext& msg = static_cast<TCFSigLegacyRMessage2Ext&>(act->Message());

			if (act->iSafeMessage.Function() == ESCPSDataSentNotificationRequest
				|| act->iSafeMessage.Function() == ESCPSDataReceivedNotificationRequest)
				{
				TLegacyDataMonitoringNotificationRequest& dmmsg = static_cast<TLegacyDataMonitoringNotificationRequest&>(msg);

				if (act->iSafeMessage.Int0() == aMessage.Int0()
					&& dmmsg.iDirection == aDirection)
					{
					ASSERT(iConnection.UniqueId() == dmmsg.iClientId);
					act->SetCancelRequest(aMessage);
					iConnection.CMMSockSubSession::ReceivedL(act->iSafeMessage.Function(), TEBase::TCancel().CRef());
					iConnection.DontCompleteCurrentRequest();
					cancelled = ETrue;
					}
				}
			}
		}
	if (!cancelled)
		{
		if (aMessage.Int0() == KNifEMCompatibilityLayerEntireSubConnectionUid && !iConnection.ServiceProvider())
			{
			iConnection.SetReturn(KErrNotReady);
			}
		else if (aMessage.Int0() == KNifEMCompatibilityLayerFakeSubConnectionId && !iConnection.DefaultSubConnectionServiceProvider())
			{
			iConnection.SetReturn(KErrNotReady);
			}
		else
			{
			LOG(ESockLog::Printf(KESockConnectionTag,
					_L8("CConnectionLegacy(%08x)::DataMonitoringNotificationCancel - direction %d - client return set to %d. Bad subconnection id. (%d) specified."),
					this, aDirection, KErrArgument, aMessage.Int0()));
			
			iConnection.SetReturn(KErrArgument);
			}
		}
	}

void AConnectionLegacy::IsSubConnectionActiveRequestL(const Den::RSafeMessage& aMessage)
	{
	TInt subConnUniqueId = aMessage.Int0();
	
	switch(subConnUniqueId)
		{
	case KNifEMCompatibilityLayerEntireSubConnectionUid:
		if(!iConnection.ServiceProvider())
			{
			iConnection.SetReturn(KErrNotReady);
			return;
			}
		break;
	case KNifEMCompatibilityLayerFakeSubConnectionId:
		if(!iConnection.DefaultSubConnectionServiceProvider())
			{
			iConnection.SetReturn(KErrNotReady);
			return;
			}
		break;
    default:
		iConnection.SetReturn(KErrArgument);
		return;
		}

	const RPointerArray<CNodeActivityBase>& activities = iConnection.Activities();
	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(activities[i]);
			
			if (act->iSafeMessage.Function() == ESCPSIsSubConnectionActiveRequest
				&& act->iSafeMessage.Int0() == aMessage.Int0())
				{
				iConnection.SetReturn(KErrInUse);
				return;
				}
			}
		}

	TLegacySubConnectionActiveRequest msg(iConnection.UniqueId(), aMessage);
	iConnection.CMMSockSubSession::ReceivedL(aMessage.Function(), msg);
	iConnection.DontCompleteCurrentRequest();
	}

void AConnectionLegacy::IsSubConnectionActiveCancel(const Den::RSafeMessage& aMessage)
	{
	const RPointerArray<CNodeActivityBase>& activities = iConnection.Activities();
	TBool cancelled = EFalse;
	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(activities[i]);
			
			if (act->iSafeMessage.Function() == ESCPSIsSubConnectionActiveRequest
				&& act->iSafeMessage.Int0() == aMessage.Int0())
				{
				act->SetCancelRequest(aMessage);
				iConnection.CMMSockSubSession::ReceivedL(act->iSafeMessage.Function(), TEBase::TCancel().CRef());
				iConnection.DontCompleteCurrentRequest();
				cancelled = ETrue;
				}
			}
		}
	if (!cancelled)
		{
		iConnection.SetReturn(KErrArgument);
		}	
	}

TBool AConnectionLegacy::DoesConnectionInfoExist(const TDesC8& aInfoBuf, const TConnectionClientInfo& aInfo)
	{
	TConnectionClientInfo info;
	TPckg<TConnectionClientInfo> des(info);
	const TUint KEntrySize = sizeof(TConnectionClientInfo);
	const TUint8* ptr = aInfoBuf.Ptr();
	const TUint8* end = ptr + aInfoBuf.Length();
	while(ptr < end)
		{
		__ASSERT_DEBUG((end - ptr) >= KEntrySize, User::Panic(KSpecAssert_ESockSSockscnLgc, 8));	// array size is multiple of element size
		des.Copy(ptr, KEntrySize);
		if(info.iProcessId == aInfo.iProcessId && info.iUid == aInfo.iUid && info.iThreadId == aInfo.iThreadId)
			{
			return ETrue;
			}
		ptr += KEntrySize;
		}
	return EFalse;
	}

