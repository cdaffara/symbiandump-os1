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

#ifndef SS_CONN_H
#define SS_CONN_H

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_connlegacy.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <ss_std.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_mmnode_subsess.h>

class TSockOpt;


namespace ConnStates
{
	class TEmptyProgressQueueMutex;

	class TParseECNStart;
	class TClearProgressQueue;
	class TRequestCSRCreation;
	class TProcessStateChange;
    class TEnqueueStateChange;	
	class TAwaitingStateChange;
	class TProcessIncomingConnection;
	class TProcessProgressRequest;
	class TParseECNAttach;
	class TProcessBinderResponseForCpr;
	class TCompleteLegacyAttach;
	class TProcessEnumResponse;
	class TGenerateConnectionUpProgress;
	class TGenerateConnectionDownProgress;
	class TNoTagOrCancelAllInterfaceWorker;
	class TCancelAllInterfaceNotificationWorker;
}

namespace AllInterfaceNotificationActivity
{
class TStartLinkNotification;
class TEnqueueNotification;
class TSendErrorToConnection;
}

namespace EnumerateConnectionsActivity
{
class TQueryTierStatus;
class TCacheConnectionInfo;
class TCompleteClient;
}

namespace ConnSubConnEventsActivity
{
class TProcessSubConnEvent;
}

namespace ESock
{
class TCprRetrieveProgress;
class TCprRetrieveLastProgressError;

class CConnectionInfo : public Meta::SMetaData
/**
@internalTechnology
*/
	{
public:
    enum { ETypeId = 0, EUid = 0x10283005 };

	static CConnectionInfo* NewL(const Den::TSubSessionUniqueId& aSubSessionId);

	virtual ~CConnectionInfo()
			{}

	Den::TSubSessionUniqueId SubSessionId() const
			{
			return iSubSessionId;
			}

	void SetSubSessionId(const Den::TSubSessionUniqueId& aSubSessionId)
			{
			iSubSessionId = aSubSessionId;
			}

	DATA_VTABLE

protected:
	inline CConnectionInfo(const Den::TSubSessionUniqueId& aSubSessionId)
		:	iSubSessionId(aSubSessionId)
			{
			}

	Den::TSubSessionUniqueId iSubSessionId;
	};

struct TCommsControlBinder;
struct TErrContext;
class XCommsDataObject;

/**
Server side class representing an RConnection
@internalComponent
*/
NONSHARABLE_CLASS(CConnection) : public CMMSockSubSession,
                                 private ASubSessionPlatsecApiExt,
                                 public ITFHIERARCHY_LINK_1(CConnection, MeshMachine::AMMNodeBase, MPlatsecApiExt)
	{
	friend class CSockSession;
	friend class ConnStates::TEmptyProgressQueueMutex;
#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend  CConnection* __x86gcc_connection_cast(Messages::ANode* aNode);
    friend  CConnection& __x86gcc_connection_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
    friend  CConnection* mcfnode_cast<CConnection>(Messages::ANode* aNode);
	friend  CConnection& mcfnode_cast<CConnection>(Messages::ANode& aNode);
#endif

	//Friendly activities:

	//Friendly transitions and states:
	friend class ConnStates::TParseECNStart;
	friend class ConnStates::TClearProgressQueue;
	friend class ConnStates::TRequestCSRCreation;
	friend class ConnStates::TProcessStateChange;
	friend class ConnStates::TAwaitingStateChange;
	friend class ConnStates::TEnqueueStateChange;  
	friend class ConnStates::TProcessIncomingConnection;
	friend class ConnStates::TProcessProgressRequest;
	friend class ConnStates::TParseECNAttach;
	friend class ConnStates::TProcessBinderResponseForCpr;
	friend class ConnStates::TCompleteLegacyAttach;
	friend class ConnStates::TProcessEnumResponse;
	friend class ESock::TCprRetrieveLastProgressError;
	friend class ESock::TCprRetrieveProgress;
	friend class EnumerateConnectionsActivity::TQueryTierStatus;
	friend class EnumerateConnectionsActivity::TCacheConnectionInfo;
	friend class EnumerateConnectionsActivity::TCompleteClient;
	friend class AllInterfaceNotificationActivity::TEnqueueNotification;
	friend class AllInterfaceNotificationActivity::TSendErrorToConnection;
	friend class ConnActivities::CAllInterfaceNotificationActivity;
	friend class ConnSubConnEventsActivity::TProcessSubConnEvent;

	friend class ConnStates::TGenerateConnectionUpProgress;
	friend class ConnStates::TGenerateConnectionDownProgress;

	friend class ConnStates::TNoTagOrCancelAllInterfaceWorker;
	friend class ConnStates::TCancelAllInterfaceNotificationWorker;


public:
	typedef ITFHIERARCHY_LINK_1(CConnection, MeshMachine::AMMNodeBase, MPlatsecApiExt) TIfStaticFetcherNearestInHierarchy;

public:
	// methods for creating CConnection instances
	static CConnection* NewLC(CSockSession *aSession, CPlayer* aPlayer, TUid aTierId, const Den::TSubSessionUniqueId aSubSessionUniqueId);
	static CConnection* NewLC(CSockSession *aSession, CPlayer* aPlayer, const CConnection& aExistingConnection, const Den::TSubSessionUniqueId aSubSessionUniqueId);

	virtual Den::TSubSessInfo Type() const
		{
		return TCFSubSessInfo(TCFSubSessInfo::EConnection);
		}

	//Attached to interface in monitor mode?
	inline TBool IsMonitor() const
		{
		return iIsMonitor;
		}

    void ReturnInterfacePtrL(MPlatsecApiExt*& aInterface);
    Messages::RNodeInterface* DefaultSubConnectionServiceProvider();
    TInt CheckCloneOpenPolicy(const RMessagePtr2& aMessage) const;
	inline CConnectionInfo* ConnectionInfo() const
			{
			return iConnectionInfo;
			}

protected:
	void Received(MeshMachine::TNodeContextBase& aContext);
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage); //Messages::ANode

//Signals from CSockSubSession
	virtual void FinalCompleteAllBlockedMessages(TInt aResult); //Always KErrAbort
	virtual void ProcessMessageL(); //IPC message arrived

private: //Utility functions
	void ForwardToServiceProviderL(const Messages::TSignalBase& aCFMessage);
	inline void ResetLastProgressError();
	void SetCloneOpenPolicyL(TUint aOptionName);

private:
	virtual ~CConnection(); //Only my friend or subsess can delete me

protected:
	CConnection(CSockSession* aSession, CPlayer* aPlayer, TUid aTierId, const Den::TSubSessionUniqueId aSubSessionUniqueId);
	void ConstructL();
	void CloneL(const CConnection& aExistingConnection);

private: // Underlying implementations of individual RConnection methods
	void GetReferenceL();
	void ControlL();
	void ProgressL();
	void LastProgressErrorL();
	void GetOrSetParametersL();
	void GetParametersResponseL(TBool aReturnLength);
	void CancelIoctl();

private:
	TUint iSelectedProgressStage;			// the particular stage required for selected progress requests.
	TProgressQueue iProgressQueue;			// queue of progress notifications received
	Elements::TStateChange iLastProgress;		// last progress notified, so we can filter duplicates
	Elements::TStateChange iLastProgressError;	// last progress received which had an error

	TUid iTierId;
	TSecurityPolicy	iCloneOpenPolicy;       // used for security checking during RConnection::Open(..., TName&)
    TBool iCloneOpenEnabled   :1;  			// flag indicating that iCloneOpenPolicy is initialized and "clone" open is enabled
    TBool iIsMonitor          :1;
	XCommsDataObject* iCommsDataObject;
	TUint iCommsDataObjectLength;	// output query length

protected:

	//-=============================================
	//Legacy function
	friend class AConnectionLegacy;
	AConnectionLegacy iLegacyConnection;
	//-=============================================

	CConnectionInfo *iConnectionInfo;
	};


//inline TBool CConnection::IsConnectionActive() const
//	{ return iConnectionProvider.Type() == RNodeInterface::EServProvider; } //WHO ADDS THIS THINGS?????????????

inline void CConnection::ResetLastProgressError()
	{ iLastProgressError.iError = KErrNone; iLastProgressError.iStage = KConnectionUninitialised; };

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	inline CConnection* __x86gcc_connection_cast(Messages::ANode* aNode)
    	{
    	return static_cast<CConnection*>(aNode);
    	}

	inline CConnection& __x86gcc_connection_cast(Messages::ANode& aNode)
    	{
    	return static_cast<CConnection&>(aNode);
    	}
#endif
} // namespace ESock


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CConnection* mnode_cast<ESock::CConnection>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_connection_cast(aNode);
		}

	template <>
	inline ESock::CConnection& mnode_cast<ESock::CConnection>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_connection_cast(aNode);
		}
	}
#endif


NONSHARABLE_CLASS(CAllInterfaceNotificationWorker) :
	public CBase, public ESock::ACFMMNodeIdBase
	{
friend class AllInterfaceNotificationActivity::TStartLinkNotification;
friend class AllInterfaceNotificationActivity::TEnqueueNotification;
friend class AllInterfaceNotificationActivity::TSendErrorToConnection;
friend class ConnActivities::CAllInterfaceNotificationActivity;

public:
    CAllInterfaceNotificationWorker(ESock::CConnection& aConnection);
    virtual ~CAllInterfaceNotificationWorker();
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
    void Received(MeshMachine::TNodeContextBase& aContext);

	ESock::CConnection& iConnection;
	};


#endif
// SS_CONN_H
