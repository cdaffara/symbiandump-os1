// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SS_CONNLEGACY_H__
#define __SS_CONNLEGACY_H__

#include <e32def.h>
#include <elements/factorynotify.h>
#include <es_sock.h> //TNifProgressBuf & co
#include <es_prot.h> //TConnStartType &co
#include <ss_std.h>
#include <comms-infras/ss_nodemessages_legacy.h>
#include <comms-infras/ss_nodemessages_legacy_internal.h>
#include <comms-infras/ss_legacyinterfaces.h>

class TSockOpt;
class CActivityTimer;

const TInt KNifEMCompatibilityLayerEntireSubConnectionUid = 0;
const TInt KNifEMCompatibilityLayerFakeSubConnectionId = 1;

namespace ConnStates
{
class TProcessEnumResponse;
class TNoTagOrCancelAllInterfaceWorker;
class TCancelAllInterfaceNotificationWorker;
}

namespace AllInterfaceNotificationActivity
{
class TEnqueueNotification;
class TSendErrorToConnection;
}

namespace ConnActivities
{
class CAllInterfaceNotificationActivity;
}

namespace EnumerateConnectionsActivity
{
class TQueryTierStatus;
class TCacheConnectionInfo;
class TCompleteClient;
}

namespace ESock
{
class ADataMonitoringProvider;
class CSubConnection;
class CConnManager;
class CConnectionProviderBase;
class CActivityTimer;
class TSelectionPrefs;
class TLegacyConnectionEnumRequest;
class CLegacySubConnectionActiveResponder;

/**
Server side class representing an RConnection legacy API
@internalComponent
*/
NONSHARABLE_CLASS(AConnectionLegacy)
	{
	friend class CConnection;
	friend class CSockSession;
	friend class Factories::TFactoryNotify<AConnectionLegacy>;
	friend class CActivityTimer;
	friend class ConnStates::TProcessEnumResponse;
	friend class TLegacyConnectionEnumRequest;
	friend class EnumerateConnectionsActivity::TQueryTierStatus;
	friend class EnumerateConnectionsActivity::TCacheConnectionInfo;
	friend class EnumerateConnectionsActivity::TCompleteClient;
	friend class AllInterfaceNotificationActivity::TEnqueueNotification;
	friend class AllInterfaceNotificationActivity::TSendErrorToConnection;
	friend class ConnActivities::CAllInterfaceNotificationActivity;
	friend class ConnStates::TNoTagOrCancelAllInterfaceWorker;
	friend class ConnStates::TCancelAllInterfaceNotificationWorker;

public:
	enum { KIPProtoConnectionProviderFactoryUid = 0x10281DD3 };
public: //So that friends of CConnection (transitions) can access it
	void CompleteAttachL(TSelectionPrefs& aPrefs);
	void CancelServiceChangeNotification(const Den::RSafeMessage& aMessage);
    void SubConnectionEvent(const TSubConnectionEvent& aSubConnectionEvent);

protected:
    AConnectionLegacy(CConnection& aConnection) :
        iConnection(aConnection)
            {
            iServiceChangeNotificationPending = EFalse;
            }

    ~AConnectionLegacy();
	TInt ProcessMessageL(const RMessage2& aMessage);
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage); //Only called from CConnection::ReceivedL

private:
	// Underlying implementations of individual RConnection methods
	void StopL(const RMessage2& aMessage);

	void GetIntSettingL(const Den::RSafeMessage& aMessage);
	void GetBoolSettingL(const Den::RSafeMessage& aMessage);
	void GetDes8SettingL(const Den::RSafeMessage& aMessage);
	void GetDes16SettingL(const Den::RSafeMessage& aMessage);
	void GetLongDesSettingL(const Den::RSafeMessage& aMessage);
	void GetConnectionInfoL(const RMessage2& aMessage);

	void AllInterfaceNotificationL(const RMessage2& aMessage);
	void CompleteAllInterfaceNotificationL(TInt aError);
	void CancelAllInterfaceNotification();

	void DataTransferredL(const Den::RSafeMessage& aMessage);
	void DataSentNotificationRequestL(const Den::RSafeMessage& aMessage);
	void DataSentNotificationCancel(const Den::RSafeMessage& aMessage);
	void DataReceivedNotificationRequestL(const Den::RSafeMessage& aMessage);
	void DataReceivedNotificationCancel(const Den::RSafeMessage& aMessage);
	void DataMonitoringNotificationRequestL(const Den::RSafeMessage& aMessage, TDataMonitoringDirection aDirection);
	void DataMonitoringNotificationCancel(const Den::RSafeMessage& aMessage, TDataMonitoringDirection aDirection);

	void IsSubConnectionActiveRequestL(const Den::RSafeMessage& aMessage);
	void IsSubConnectionActiveCancel(const Den::RSafeMessage& aMessage);

	void ExtractEnumInfoL(RBuf8& aBuffer, TUint aCount, TInt aSize, TUint aIndex, TDes8& aPckg);
	void EnumerateConnectionClientsL();
	void GetConnectionClientInfoL();
	void RequestDataPlaneEnumerationL(TLegacyConnEnumClients aClientType);
	void EnumerateConnectionSocketsL();
	void CompleteDataPlaneEnumeration(const Messages::TNodeId& aPeer, TInt aCount, HBufC8* aInfo, TInt aError);
	void GetConnectionSocketInfoL();
    void ServiceChangeNotification(TUint32 aId, const TDesC& aType);
	void FinalCompleteAllBlockedMessages(TInt aResult);
	void CancelProviderRequests();


#ifndef AVAILABILITY_READY
	void EnumerateConnectionsL(const RMessage2& aMessage);
#endif

	// SubConnection-oriented RConnection method support
	void EnumerateSubConnectionsL(const Den::RSafeMessage& aMessage);
	void GetSubConnectionInfoL(const Den::RSafeMessage& aMessage);

	static TBool DoesConnectionInfoExist(const TDesC8& aInfoBuf, const TConnectionClientInfo& aInfo);
protected:
	ADataMonitoringProvider* FetchConnDataMonitoringProvider() const;
	ADataMonitoringProvider* FetchSubConnDataMonitoringProvider() const;

	//global notifications
	void RequestServiceChangeNotificationL(const Den::RSafeMessage& aMessage);
    void ControlL(TUint aOptionName, TUint aMessageId);

	// Underlying implementations of individual RConnection methods relating to subconnections
    void AllSubConnectionNotificationL();
    void CancelAllSubConnectionNotification();

private:
	RMessage2 iAllSubConnectionNotificationMessage;

	TInterfaceChangeQueue iInterfaceChangeQueue;	// queue of interface notifications received

	TUint iConnectionInfoCount;
	TUint iConnectionClientCount;
	RBuf8 iConnectionClientBuf;
	TUint iConnectionSocketCount;
	RBuf8 iConnectionSocketBuf;
	Den::RSafeMessage iEnumConnMsg;	//< During a Data plane enum holds blocked client msg
	TUint* iDPEnumCount;					//< During a Data plane enum points to count recipient
	RBuf8* iDPEnumBuf;				//< During a Data plane enum points to data buf
	RArray<Messages::TNodeId> iEnumConnSockCandidateSCPRs;	//< During a Data plane enum is list of SCPRs for connection

    RMessage2* iMessage; //current message valid only within ProcessMessageL call
    CConnection& iConnection;

	RPointerArray<CSubConnection> iSubConnections; //client classes servicing sub-connection
	                //related functionality requested via legacy RConnection API. sub-connections
	                //objects are owned by AConnectionLegacy instance and don't have RSubConnection
	                //client side.
	CConnManager* iConnManager; //client class servicing connection manager
	                //related functionality requested via legacy RConnection API. CConnManager
	                //object is owned by AConnectionLegacy instance and does't have RConnManager
	                //client side.
	// Flags for pending RConnection requests
	TBool iServiceChangeNotificationPending;

	// IsSubConnectionActive support
	ADataMonitoringProvider* iConnActivityDataMonProv;
	ESock::CActivityTimer* iActivityTimer;
	TBool iClientsSubConnectionActiveState;
	TUint iInitialSentDataVolume;
	TUint iInitialReceivedDataVolume;
	TUint iRequestedClientActivityTimerPeriod;
	TBool iOutstandingIsSubConnectionActiveRequest;
	RMessage2 iOutstandingIsSubConnectionActiveMessage;

	TBool iDeferredSubConnectionNotificationEnable;
	TSubConnectionEventQueue iSubConnectionEventQueue; // queue of subconnection events received

	// EnumerateConnection and GetConnectionInfo use this.
	RPointerArray<TSourcedConnectionInfo> iConnectionInfoPtrArray;

	// AllInterfaceNotification support
	RMessage2 iAllInterfaceNotificationMessage;
	TInterfaceChangeQueue iNotificationQueue;
  	Messages::TNodeId iAllInterfaceNotificationWorker;
	};


} //namespace ESock

#endif
// __SS_CONNLEGACY_H__

