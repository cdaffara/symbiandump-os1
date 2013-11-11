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
// Link CPR Node Messages
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SS_NODEMESSAGES_LEGACY_INTERNAL_INCLUDED
#define SS_NODEMESSAGES_LEGACY_INTERNAL_INCLUDED

#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_dispatchers.h>
#include <elements/responsemsg.h>
#include <es_enum.h>

#include <comms-infras/api_ext_list.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_datamonitoringprovider.h>
#include <comms-infras/ss_nodemessages_legacy.h>

#include <comms-infras/ss_legacyinterfaces.h>

namespace ESock
{

//-=========================================================
//
//Extension messages
//
//-=========================================================

class TCFLegacyMessageInternal
	{
public:
	enum EMessageTypeInternal //message signatures only (NOT messages) messages are declared under class TCFMessage
		{
		EGetOrSetParameters				=200,

		ECprRetrieveProgress			=201, 
		ECprRetrieveLastProgressError	=202,
		ECprRequestServiceNotification	=203,
		
		ECprSendIoctl					=204,
		ECprAllSubConnectionNotificationEnable		=205,
		ECprEnumerateConnections		=206,
		
		ELegacyDataMonitoringTransferredRequest		= 207,
		ELegacyDataMonitoringNotificationRequest	= 208,
		
		ELegacySubConnectionActiveRequest			= 209,
		
		ELegacyEnumerateSubConnections				= 210,
		
		ELegacyControlMessage           =211,
		ELegacyConnEnumMessage          =212,
		
		ESignatureRMessage2Response     =213,
		ELegacyGetSubConnectionInfo     =214
		};
	};
/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprExtensionApi. When found, the message
calls MLinkCprExtensionApi::ProgressL and completes the original request (see aMessage
in the c'tor).

@internalComponent
*/
class TCprRetrieveProgress : public TCFSigLegacyRMessage2Ext,
							 public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ECprRetrieveProgress, TCFLegacyMessage::ERealmId>
    {
public:
    inline TCprRetrieveProgress() {};
	TCprRetrieveProgress(const Den::RSafeMessage& aMessage, Elements::TStateChange& aLastProgress)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iLastProgress(aLastProgress)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);
	virtual TInt CheckError(MeshMachine::TNodeContextBase& aContext, TInt aError);

public:
    DECLARE_MVIP_CTR(TCprRetrieveProgress)
protected:
    DATA_VTABLE

private:
    Elements::TStateChange iLastProgress;
    };


/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprExtensionApi. When found, the message
calls MLinkCprExtensionApi::LastProgressError and completes the original request (see
aMessage in the c'tor).

@internalComponent
*/
class TCprRetrieveLastProgressError : public TCFSigLegacyRMessage2Ext,
									  public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ECprRetrieveLastProgressError, TCFLegacyMessage::ERealmId>
    {
public:
    inline TCprRetrieveLastProgressError() {};
	TCprRetrieveLastProgressError(const Den::RSafeMessage& aMessage, Elements::TStateChange& aLastProgressError)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iLastProgressError(aLastProgressError)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);
	virtual TInt CheckError(MeshMachine::TNodeContextBase& aContext, TInt aError);

public:
    DECLARE_MVIP_CTR(TCprRetrieveLastProgressError)
protected:
    DATA_VTABLE

private:
    Elements::TStateChange iLastProgressError;
    };


/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprExtensionApi. When found, the message
calls MLinkCprExtensionApi::RequestServiceChangeNotificationL

@internalComponent
*/
class TCprRequestServiceNotification : public TCFSigLegacyRMessage2Ext,
									   public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ECprRequestServiceNotification, TCFLegacyMessage::ERealmId>
    {
public:
	TCprRequestServiceNotification(const Messages::TNodeId& aSender, const Den::RSafeMessage& aMessage)
		:	TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iSender(aSender)
		{
		}

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TCprRequestServiceNotification)

protected:
	inline TCprRequestServiceNotification() {};
    DATA_VTABLE
	Messages::TNodeId iSender;
    };

/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprExtensionApi. When found, the message
calls MLinkCprExtensionApi::SendIoctlMessageL and completes the original
request (see aMessage in the c'tor).

@internalComponent
*/
class TCprSendIoctl : public TCFSigLegacyRMessage2Ext,
					  public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ECprSendIoctl, TCFLegacyMessage::ERealmId>
    {
public:
    inline TCprSendIoctl() {};
	TCprSendIoctl(const Den::RSafeMessage& aMessage)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TCprSendIoctl)
protected:
    DATA_VTABLE
    };


/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprExtensionApi. When found, the message
calls MLinkCprExtensionApi::AllSubConnectionNotificationEnable and completes the original
request (see aMessage in the c'tor).

@internalComponent
*/
class TCprAllSubConnectionNotificationEnable : public Messages::TSignalBase, public TCFClientItfExt<TCFClientType::ECtrl>
    {
protected:
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

public:
    DECLARE_MVIP_CTR(TCprAllSubConnectionNotificationEnable)
protected:
    DATA_VTABLE
    };


/**
@internalComponent
*/
class TCprFactEnumerateConnections : public TCFSigLegacyRMessage2Ext,
									 public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ECprEnumerateConnections, TCFLegacyMessage::ERealmId>
    {
public:
    TCprFactEnumerateConnections()
        {
        }

	TCprFactEnumerateConnections(RPointerArray<TSourcedConnectionInfo>& aConnectionInfoPtrArray, const Den::RSafeMessage& aMessage)
        :TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iConnectionInfoPtrArray(&aConnectionInfoPtrArray)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TCprFactEnumerateConnections)
protected:
    DATA_VTABLE
	RPointerArray<TSourcedConnectionInfo>* iConnectionInfoPtrArray;
    };


/**
 *
 *  DATA MONITORING
 *
 */

/**
	A self-propagating extension message. It advances down the stack looking for
	a MeshMachine::AMMNodeBase that implements EDataMonitoringApiExt.

	@internalComponent
*/
class TLegacyDataMonitoringTransferredRequest : public TCFSigLegacyRMessage2Ext,
												public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacyDataMonitoringTransferredRequest, TCFLegacyMessage::ERealmId>
    {
public:
	TLegacyDataMonitoringTransferredRequest() : TCFSigLegacyRMessage2Ext() {}
												
	TLegacyDataMonitoringTransferredRequest(TSubSessionUniqueId aClientId, const Den::RSafeMessage& aMessage)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iClientId(aClientId)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

private:
	TBool HasServiceProviderL(const Messages::TRuntimeCtxId& aRecipient);

public:
    DECLARE_MVIP_CTR(TLegacyDataMonitoringTransferredRequest)

protected:
    DATA_VTABLE

    TSubSessionUniqueId iClientId;
    };

class TLegacyDataMonitoringTransferredRequest;
class TLegacyDataMonitoringNotificationRequest;	
	
/**
	IPC responder to support the legacy data monitoring APIs on RConnection.

	Inherits from CDataMonitoringResponder as there's shared code needed by the legacy and api ext versions

	@internalTechnology
*/
class CLegacyDataMonitoringResponder : public CDataMonitoringResponder
	{
friend class TLegacyDataMonitoringTransferredRequest;
friend class TLegacyDataMonitoringNotificationRequest;

public:
	virtual void DoCancelRequest();
	virtual void DoDataTransferred(TUint32 aReceivedBytes, TUint32 aSentBytes);
	virtual void DoDataSentNotification(TUint32 aSentBytes);
	virtual void DoDataReceivedNotification(TUint32 aReceivedBytes);

protected:
   virtual void DoComplete(TInt aError);


private:
	IMPORT_C static CLegacyDataMonitoringResponder* NewL(ESock::RLegacyResponseMsg& aResponseMsg);
	CLegacyDataMonitoringResponder(ESock::RLegacyResponseMsg& aResponseMsg);

private:
	ESock::RLegacyResponseMsg iLegacyResponseMsg;
	};

/**
	A self-propagating extension message. It advances down the stack looking for
	a MeshMachine::AMMNodeBase that implements EDataMonitoringApiExt.

	@internalComponent
*/
class TLegacyDataMonitoringNotificationRequest : public TCFSigLegacyRMessage2Ext,
												 public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacyDataMonitoringNotificationRequest, TCFLegacyMessage::ERealmId>
    {
public:
    inline TLegacyDataMonitoringNotificationRequest() {};
    TLegacyDataMonitoringNotificationRequest(TDataMonitoringDirection aDirection, TSubSessionUniqueId aClientId, const Den::RSafeMessage& aMessage)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iDirection(aDirection), iClientId(aClientId)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TLegacyDataMonitoringNotificationRequest)

	TDataMonitoringDirection iDirection;
    TSubSessionUniqueId iClientId;

protected:
    DATA_VTABLE
    };

/**
	A self-propagating extension message. It advances down the stack looking for
	a MeshMachine::AMMNodeBase that implements xxx.

	@internalComponent
*/
class TLegacyEnumerateSubConnections : public TCFSigLegacyRMessage2Ext,
									   public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacyEnumerateSubConnections, TCFLegacyMessage::ERealmId>
    {
public:
    TLegacyEnumerateSubConnections()
        {
        }

	TLegacyEnumerateSubConnections(const Den::RSafeMessage& aMessage)
        : TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TLegacyEnumerateSubConnections)

protected:
    DATA_VTABLE
    };

/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLegacyControlExtApi and wants to handle the
requested control message.
@internalComponent
*/
class TLegacyControlMessage : public TCFSigLegacyRMessage2PlatsecExt,
							  public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacyControlMessage, TCFLegacyMessage::ERealmId>
    {
public:
    TLegacyControlMessage()
        {
        }

	TLegacyControlMessage(const Den::RSafeMessage& aMessage)
        : TCFSigLegacyRMessage2PlatsecExt(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);
	
public:
    DECLARE_MVIP_CTR(TLegacyControlMessage)

protected:
    DATA_VTABLE
    };

/** Self-dispatching messages supporting the legacy enumeration of connection clients and sockets
*/
NONSHARABLE_CLASS(TLegacyConnectionEnumRequest) : public Messages::TSignalBase
	{
public:
	TLegacyConnectionEnumRequest()
		{
		}

    TLegacyConnectionEnumRequest(RArray<Messages::TNodeId>& aSCPRCandidates, TLegacyConnEnumClients aClientType);

public:
    DECLARE_MVIP_CTR(TLegacyConnectionEnumRequest)
protected:
    DATA_VTABLE

	void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);
public:
	RArray<Messages::TNodeId>* iSCPRCandidates;	// owned by sender; data thread only references
	TLegacyConnEnumClients iClientType;
	};

/**
	A self-propagating extension message. It advances down the stack looking for
	a MeshMachine::AMMNodeBase that implements ELegacySubConnectionActiveApiExt.

	@internalComponent
*/
NONSHARABLE_CLASS(TLegacySubConnectionActiveRequest) : public TCFSigLegacyRMessage2Ext,
													   public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacySubConnectionActiveRequest, TCFLegacyMessage::ERealmId>
    {
public:
    inline TLegacySubConnectionActiveRequest() {};
	inline TLegacySubConnectionActiveRequest(TSubSessionUniqueId aClientId, const Den::RSafeMessage& aMessage)
		: TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iClientId(aClientId)
		{}

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);
	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TLegacySubConnectionActiveRequest)

	TSubSessionUniqueId iClientId;
protected:
    DATA_VTABLE
    };

/**
	A self-propagating extension message. It advances down the stack looking for
	a MeshMachine::AMMNodeBase that implements ELegacySubConnectionInfoxt.

	@internalComponent
*/
NONSHARABLE_CLASS(TLegacyGetSubConnectionInfo) : public TCFSigLegacyRMessage2Ext,
												 public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::ELegacyGetSubConnectionInfo, TCFLegacyMessage::ERealmId>
    {
public:
    inline TLegacyGetSubConnectionInfo() {};
	inline TLegacyGetSubConnectionInfo(TSubSessionUniqueId aClientId, TSubConnectionInfo& aSubConnectionInfo, const Den::RSafeMessage& aMessage)
		: TCFSigLegacyRMessage2Ext(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iClientId(aClientId), iSubConnectionInfo(aSubConnectionInfo)
		{}

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TLegacyGetSubConnectionInfo)

	TSubSessionUniqueId iClientId;
	TSubConnectionInfo iSubConnectionInfo;
	
protected:
    DATA_VTABLE
    };


class CClientRequest;
class ALegacySubConnectionActiveApiExt;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CLegacySubConnectionActiveResponder) : public CCommsApiExtLegacyIpcResponder
	{
	friend class TLegacySubConnectionActiveRequest;
	friend class ALegacySubConnectionActiveApiExt;
	friend class CClientRequest;

public:
	static CLegacySubConnectionActiveResponder* NewL(ESock::RLegacyResponseMsg& aResponseMsg, TUint aTimerPeriod, TUint aClientId);

	inline TUint ClientId()
		{
		return iClientId;
		}

	inline TInt TimerPeriod()
		{
		return iTimerPeriod;
		}

	inline TBool ClientPerceivedState()
		{
		return iClientPerceivedState;
		}
	
protected:
	CLegacySubConnectionActiveResponder(ESock::RLegacyResponseMsg& aResponseMsg, TUint aTimerPeriod, TUint aClientId)
		: CCommsApiExtLegacyIpcResponder(aResponseMsg), iTimerPeriod(aTimerPeriod), iClientId(aClientId)
		{
		}

private:
	void SubConnectionActive(TBool aState);
	void Error(TInt aError);
	void Complete(TInt aError); 
	
private:
	TBool iClientPerceivedState;
	TInt iTimerPeriod;
	TUint iClientId;
	};


/**
	@internalComponent
*/

/**
	@internalComponent
*/
class CLegacyEnumerateSubConnectionsResponder : public CCommsApiExtLegacyIpcResponder
	{
	friend class TLegacyEnumerateSubConnections;

public:
	IMPORT_C static void CompleteClient(CLegacyEnumerateSubConnectionsResponder*& aThis, TInt aCount);

protected:
	CLegacyEnumerateSubConnectionsResponder(RLegacyResponseMsg& aResponseMsg)
		: CCommsApiExtLegacyIpcResponder(aResponseMsg)
		{
		}

private:
	static CLegacyEnumerateSubConnectionsResponder* NewL(RLegacyResponseMsg& aResponseMsg);
	};

}//namespace ESock

#endif

