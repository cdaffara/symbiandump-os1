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

ss_mcprnodeinterfaces.cpp
*/

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/metatype.h>
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/metatypevariablelen.h>
#include <comms-infras/ss_mmnode.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksmcprn, "ESockSSocksmcprn");
#endif

using namespace ESock;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;

const TInt KESockInternalMetaPlaneMessagesImplementationUid = 0x1028300E;

enum EMessageTypeId //message signatures only (NOT messages) messages are declared under class TCFMessage
	{
	ESafeMessageCarrierBase				=0,
	ESafeMessageCarrierEast				=1,
	ESafeMessageCarrierWest				=2
	};


//
//TConnProviderInfoPref
EXPORT_C TConnProviderInfoPref::TConnProviderInfoPref(const TConnProviderInfo& aProviderInfo)
:	TConnPref(EConnPrefProviderInfo)
	{
	TConnProviderInfo* prefPtr = reinterpret_cast<TConnProviderInfo*>(UserPtr());
	*prefPtr = aProviderInfo; //Binary copy
	}

EXPORT_C const TConnProviderInfo& TConnProviderInfoPref::Info() const
/**
@publishedPartner
@return stored SNAP value
@released since v9.1
*/
	{
	return *(reinterpret_cast<TConnProviderInfo*>(UserPtr()));
	}

//
//TSelectionPrefs
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSelectionPrefs, TSelectionPrefs::EUid, TSelectionPrefs::ETypeId)
    REGISTER_ATTRIBUTE(TSelectionPrefs, iPrefs, TMeta<TConnPref>)
    REGISTER_ATTRIBUTE(TSelectionPrefs, iU.iSelectionParams, TMetaNumber)
	REGISTER_ATTRIBUTE(TSelectionPrefs, iSubSessionUniqueId, TMetaNumber)
END_ATTRIBUTE_TABLE()

EXPORT_C TSelectionPrefs::TSelectionPrefs()
	{
    iU.iS.iScope = ENone;
    iU.iS.iFlags = 0;
	}

EXPORT_C TSelectionPrefs::TSelectionPrefs(TSelectionScope aSelectionScope)
	{
    iU.iS.iScope = aSelectionScope;
    iU.iS.iFlags = 0;
	}

EXPORT_C TSelectionPrefs::TSelectionPrefs(const TConnPref& aPref)
	{
	SetPrefs(aPref);
	}

//
//TOverridenSelectionPrefsExt
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TOverridenSelectionPrefsExt, TOverridenSelectionPrefsExt::EUid, TOverridenSelectionPrefsExt::ETypeId)
	//No attributes as this is only for type id
END_ATTRIBUTE_TABLE()

EXPORT_C TOverridenSelectionPrefsExt::TOverridenSelectionPrefsExt(TUid aTierId, const TSelectionPrefs& aPrefs)
:	iPrefs(aPrefs),
	iTierId(aTierId)
	{
	}

//
//TDeferredSelectionPrefsExt
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDeferredSelectionPrefsExt, TDeferredSelectionPrefsExt::EUid, TDeferredSelectionPrefsExt::ETypeId)
	//No attributes as this is only for type id
END_ATTRIBUTE_TABLE()

EXPORT_C TDeferredSelectionPrefsExt::TDeferredSelectionPrefsExt(TUid aTierId, const TConnPref& aPrefs)
:	iPrefs(aPrefs),
	iTierId(aTierId)
	{
	}

//
//TPromptingSelectionPrefsExt
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TPromptingSelectionPrefsExt, TPromptingSelectionPrefsExt::EUid, TPromptingSelectionPrefsExt::ETypeId)
	//No attributes as this is only for type id
END_ATTRIBUTE_TABLE()

EXPORT_C TPromptingSelectionPrefsExt::TPromptingSelectionPrefsExt(TUid aTierId, const TConnPref& aPrefs)
    : iPrefs(aPrefs), iTierId(aTierId), iPromptingInProgress(ETrue)
	{
	}

EXPORT_C Meta::SMetaData* RConnPrefList::TIterBase::AtIndex(const Meta::STypeId& aType, TInt aIndex)
	{
	TInt ind = 0;
	TInt count = 0;
	while(ind < iConnPrefList.Count())
		{
		if (iConnPrefList[ind]->IsTypeOf(aType))
			{
			if(count == aIndex)
				{
				iMasterIndex = ind;
				return static_cast<Meta::SMetaData*>(iConnPrefList[ind]);
				}
			count++;
			}
		ind++;
		}
	return NULL;
	}

//
//RConnPrefList
EXPORT_C RConnPrefList::RConnPrefList()
	{
	iObject = NULL;
	}

EXPORT_C void RConnPrefList::Open(RConnPrefList& aObject)
	{
	iObject = aObject.iObject;
	}

TInt RConnPrefList::Open(TConnPrefList* aObject)
	{
	if(aObject == NULL)
		{
		return KErrNotFound;
		}
	else
		{
		iObject = aObject;
		return KErrNone;
		}
	}

EXPORT_C void RConnPrefList::Close()
	{
	iObject = NULL;
	}

EXPORT_C void RConnPrefList::AppendL(SMetaDataECom* aFamily)
	{
	if(iObject == NULL)
		{
		User::Leave(KErrNotReady);
		}
	else if(aFamily == NULL)
		{
		User::Leave(KErrArgument);
		}
	iObject->AppendL(aFamily);
	}

EXPORT_C SMetaData* RConnPrefList::operator[](TInt aIndex)
	{
	if(iObject == NULL )
		{
		return NULL;
		}
	return (*iObject)[aIndex];
	}

EXPORT_C void RConnPrefList::Remove(TInt aIndex)
	{
	if(iObject == NULL)
		{
		return;
		}
	iObject->Remove(aIndex);
	}

EXPORT_C TInt RConnPrefList::Count()
	{
	if(iObject != NULL)
		{
		return iObject->Count();
		}
	else
		{
		return 0;
		}
	}

//
//TSafeMessageCarrierBase - Safe CF Message carriers

//It is not a purposes of these carriers to secure the comunication channel between two non-peer nodes.
//Sending of such message does not quarantee the node will be there to receive it
//There is a specialisation of the request carrier "locking" the requesting node which makes sure
//that the response will always be deliverd.

TSafeMessageCarrierBase::TSafeMessageCarrierBase(const Messages::TSignatureBase& aMessage)
:	Messages::TSignatureBase(aMessage.MessageId())
	{
	__ASSERT_DEBUG(KMaxCarriedMessageSize>=aMessage.Length(), User::Panic(KSpecAssert_ESockSSocksmcprn, 1)); //TODO: Add panic code & description!!!
	VERIFY(aMessage.Store(iMessageBuf)==KErrNone); //Guaranteed to succeed now - compile assert is quarding it from the template
	}

void TSafeMessageCarrierBase::DispatchCarriedMessageL(const TRuntimeCtxId& aSender, const TNodeId& aNodeId)
    {
	TBuf8<__Align8(TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead)> msgBuf;
	Meta::CMetaDataVirtualCtorInPlace* vctr = TlsGlobals::Get().VirtualCtor();
	TPtrC8 ptr(iMessageBuf);
	iMessage = static_cast<Messages::TSignatureBase*>(vctr->New(ptr, msgBuf));
   	User::LeaveIfError(iMessage? KErrNone : KErrNotFound);

#ifdef SYMBIAN_TRACE_ENABLE
    _LIT8(KESockMeshMachine, "cftransport");
    NM_LOG_START_BLOCK(KESockMeshMachine, _L8("TransportReceiver::DispatchMessage"));
    NM_LOG_ADDRESS(KESockMeshMachine, aSender);
    NM_LOG_ADDRESS(KESockMeshMachine, aNodeId);
    NM_LOG_MESSAGE(KESockMeshMachine, *iMessage);
    NM_LOG_END_BLOCK(KESockMeshMachine, _L8("TransportReceiver::DispatchMessage"));
#endif

	iMessage->DispatchL(aSender, aNodeId);
   	}

EXPORT_C void TSafeMessageCarrierBase::Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError)
	{
	NM_LOG_START_BLOCK(KESockMeshMachine, _L8("TSafeMessageCarrierBase:Error"));
	NM_LOG((KESockMeshMachine, _L8("[this=0x%08x] [Error=%d] "), this, aError));
    NM_LOG_ADDRESS_EXT(KESockMeshMachine, aSender, _L8("Originator: "));
    NM_LOG_ADDRESS_EXT(KESockMeshMachine, aRecipient, _L8("Recipient: "));
    NM_LOG((KESockMeshMachine, _L8("[MessageId=0x%08x:0x%08x] "), MessageId().Realm(), MessageId().MessageId()));
    NM_LOG_END_BLOCK(KESockMeshMachine, _L8("TSafeMessageCarrierBase:Error"));

    if (!aSender.IsNull())
        {
        TEBase::TError errMsg(
#if defined(__GCCXML__)
        	static_cast<Messages::TNodeSignal::TMessageId>(MessageId()), 
#else
        	MessageId(),
#endif
        	aError
        	);
        	
        TCFSafeMessage::TResponseCarrierWest<TEBase::TError> responseCarrierMsg(errMsg, aSender);
        
        RClientInterface::OpenPostMessageClose(aRecipient, aSender, responseCarrierMsg);
        }

	}

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSafeMessageCarrierBase, KESockInternalMetaPlaneMessagesImplementationUid, ESafeMessageCarrierBase)
	REGISTER_ATTRIBUTE(TSafeMessageCarrierBase, iMessageBuf, TMeta<TBuf8<KMaxCarriedMessageSize> >) //TODO find variable len meta type
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, ESignatureBase)

/*
//Used to lock the requesting node until the response has been received
class ARequestingNodeLockingClient : Messages::ASimpleNodeIdBase
	{
public:
	void LockRequestingNodeL(const MeshMachine::AMMNodeBase& aRequestingNode)
		{
		//ARequestingNodeLockingClient can currently only work on MeshMachine::AMMNodeBase nodes!!!
   		MeshMachine::AMMNodeBase* nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(aRequestingNode.FetchNodeInterfaceL(AMMNodeBase::KInterfaceId));
		nodeBase.AddClientL((*this)(),RNodeInterface::ECtrl);
		}

	static void Cleanup(TAny* aThis)
		{
		ARequestingNodeLockingClient* self = static_cast<ARequestingNodeLockingClient*>(aThis);
		MeshMachine::AMMNodeBase* nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(self->iLockedNode.FetchNodeInterfaceL(AMMNodeBase::KInterfaceId));
		nodeBase.RemoveClient((*this)());
		delete self;
		}

	virtual TInt ReceivedL(Messages::TSignatureBase& aCFMessage)
		{
		if (aCFMessage.iSender == iLockedNode())
			{
			return;
			}

		//Noone else knows about this node!!!!
		__ASSERT_DEBUG(aCFMessage.iSender == iRespondingNode, User::Panic(KSpecAssert_ESockSSocksmcprn, 2));
		CleanupStack::PushL(TCleanupItem(Cleanup,this));
		TInt error = iLockedNode->ReceivedL(aCFMessage);
		CleanupStack::PopAndDestroy();
		return error;
		}
private:
	TNodeId iRespondingNode;
	MeshMachine::AMMNodeBase& iLockedNode;
	};

EXPORT_C void TSafeMessageCarrierEastBase::LockRequestingNodeAndPostL(const MeshMachine::AMMNodeBase& aRequestingNode)
   	{
   	__ASSERT_DEBUG(iMessage, User::Panic(KSpecAssert_ESockSSocksmcprn, 3)); //The local message
	__ASSERT_DEBUG(KMaxCarriedMessageSize>=aMessage.Length(), User::Panic(KSpecAssert_ESockSSocksmcprn, 4)); //TODO: Add panic code & description!!!
	iMessageBuf.FillZ();
	iMessage->
	VERIFY(aMessage.Store(iMessageBuf)==KErrNone); //Guaranteed to succeed now - compile assert is quarding it from the template

   	ARequestingNodeLockingClient* lock = new (ELeave) ARequestingNodeLockingClient;
   	CleanupStack::PushL(lock);
   	lock->LockRequestingNodeL(aRequestingNode);
   	CleanupStack::Pop(lock);
   	}
*/

EXPORT_C TSafeRequestCarrierEast::TSafeRequestCarrierEast(const Messages::TSignatureBase& aMessage)
:	TSafeMessageCarrierBase(aMessage)
   	{
   	}

EXPORT_C void TSafeRequestCarrierEast::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
    AMMNodeBase* nodeBase = reinterpret_cast<AMMNodeBase*>(nodeId.Node().FetchNodeInterfaceL(AMMNodeBase::KInterfaceId)); //Will result in Error() when not found
   	if (static_cast<ACFMMNodeIdBase*>(nodeBase)->ControlProvider()==0)
   		{
   		DispatchCarriedMessageL(aSender, nodeId);
   		}
   	else
   		{
   		nodeBase->PostToClients<TDefaultClientMatchPolicy>(aSender, *this, TClientType(TCFClientType::ECtrlProvider), TClientType(0, TClientType::ELeaving));
   		}
   	}

DEFINE_MVIP_CTR(TSafeRequestCarrierEast)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSafeRequestCarrierEast, KESockInternalMetaPlaneMessagesImplementationUid, ESafeMessageCarrierEast)
END_ATTRIBUTE_TABLE_BASE(TSafeMessageCarrierBase, ESafeMessageCarrierBase)

EXPORT_C TSafeResponseCarrierWest::TSafeResponseCarrierWest(const Messages::TSignatureBase& aMessage, const TRuntimeCtxId& aRecipient)
:	TSafeMessageCarrierBase(aMessage),
   	iRecipient(address_cast<TNodeId>(aRecipient)) //Must be a node!
   	{
   	}

EXPORT_C void TSafeResponseCarrierWest::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	if (nodeId==iRecipient)
		{
		DispatchCarriedMessageL(aSender, nodeId);
		}
	else
		{
    	MeshMachine::AMMNodeBase* nodeBase = NULL;
    	TRAP_IGNORE(nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(nodeId.Node().FetchNodeInterfaceL(AMMNodeBase::KInterfaceId)));
    	if (nodeBase)
    		{
    		//If the interface is not found, ignore
			nodeBase->PostToClients<TDefaultClientMatchPolicy>(aSender, *this, TClientType(TCFClientType::EData), TClientType(0, TClientType::ELeaving));
			}
		}
   	}

DEFINE_MVIP_CTR(TSafeResponseCarrierWest)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSafeResponseCarrierWest, KESockInternalMetaPlaneMessagesImplementationUid, ESafeMessageCarrierWest)
	REGISTER_ATTRIBUTE(TSafeResponseCarrierWest, iRecipient, TMeta<TNodeId>)
END_ATTRIBUTE_TABLE_BASE(TSafeMessageCarrierBase, ESafeMessageCarrierBase)

const TImplementationProxy SignatureImplementationTable[] =
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(ESafeMessageCarrierEast,TSafeRequestCarrierEast),
	MVIP_CTR_ENTRY(ESafeMessageCarrierWest,TSafeResponseCarrierWest),
	};

void TCFSafeMessage::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KESockInternalMetaPlaneMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

void TCFSafeMessage::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(KESockInternalMetaPlaneMessagesImplementationUid));
	}

