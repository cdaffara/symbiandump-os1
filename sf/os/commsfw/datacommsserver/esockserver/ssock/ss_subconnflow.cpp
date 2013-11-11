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
#include "ss_subconnflow.h"


#include <comms-infras/ss_log.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_protflow.h>
#include <es_ini.h>
#include <ss_glob.h>
#include <ss_protprov.h>
#include <comms-infras/ss_sapshim.h>
#include "ss_sapfactshim.h"
#include <elements/cftransport.h>
#include <ecom/ecom.h>
#include <elements/nm_messages_child.h>

#include <comms-infras/ss_nodemessages_factory.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksbcnfl, "ESockSSocksbcnfl");
#endif

using namespace ESock;
using namespace Messages;
using namespace CommsFW;
using namespace Factories;


//-=====================================================================
//
//
// CSubConnectionFlowFactoryContainer
//
//
//-=====================================================================
CSubConnectionFlowFactoryContainer::CSubConnectionFlowFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CSubConnectionFlowFactoryContainer::EId)
	{
	LOG_NODE_CREATE(KESockDataFactTag, CSubConnectionFlowFactoryContainer)
	}

CSubConnectionFlowFactoryContainer::~CSubConnectionFlowFactoryContainer()
	{
	TCFLegacyMessage::DeRegister();
	LOG_NODE_DESTROY(KESockDataFactTag, CSubConnectionFlowFactoryContainer)
	}

CSubConnectionFlowFactoryContainer* CSubConnectionFlowFactoryContainer::NewL()
/** Create a new instance of a sub-connection factory container

@exception Leaves in out of memory conditions
@return Pointer to new instance of a sub-connection factory container
*/
	{
	using namespace CommsFW;
	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CSubConnectionFlowFactoryContainer:\tNewL()")) );

	// For the self-dispatching TLegacyConnectionEnumRequest
	TCFLegacyMessage::RegisterL();

	CSubConnectionFlowFactoryContainer* provcont = new (ELeave) CSubConnectionFlowFactoryContainer();
	CleanupStack::PushL(provcont);
	CTransportFlowShimFactory::NewL(TUid::Uid(CTransportFlowShimFactory::iUid), *provcont);
	CleanupStack::Pop(provcont);
	return provcont;
	}

void CSubConnectionFlowFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	if (aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{
		TCFFactory::TFindOrCreatePeer& msg = message_cast<TCFFactory::TFindOrCreatePeer>(aMessage);
		CCommsFactoryBase* factory = static_cast<CCommsFactoryBase*>(FindOrCreateFactoryL(msg.iUid));
        ACommsFactoryNodeId* flow = factory->FindOrCreateObjectL(*(msg.iQuery));

        // Send "DataClientJoined"
   #ifndef __GCCXML__
   	    RClientInterface::OpenPostMessageClose(Id(), aSender,
   	    	TCFFactory::TPeerFoundOrCreated(flow? flow->Id() : TNodeId::NullId(), 0).CRef());
   #endif
        aMessage.ClearMessageId();
		}
	else
		{
		NM_LOG_START_BLOCK(KESockMetaConnectionTag, _L8("CSubConnectionFlowFactoryContainer:ReceivedL"));
		NM_LOG((KESockMetaConnectionTag, _L8("ERROR: KErrNotSupported [this=0x%08x] "), this));
		NM_LOG_MESSAGE(KESockMetaConnectionTag, aMessage);
        NM_LOG_END_BLOCK(KESockMetaConnectionTag, _L8("CSubConnectionFlowFactoryContainer:ReceivedL"));

		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksbcnfl, 1)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}



//-=====================================================================
//
//
// CSubConnectionFlowFactoryBase
//
//
//-=====================================================================
EXPORT_C CSubConnectionFlowFactoryBase::CSubConnectionFlowFactoryBase(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
/** Sub-connection flow factory constructor

@param aFactoryId Unique Integer Identifier of the sub-connection provider factory
@param aParentContainer Container to add the factory to */
	: CCommsFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C ACommsFactoryNodeId* CSubConnectionFlowFactoryBase::DoCreateObjectL(TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& flowQuery = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
	TDefaultProtocolIntfFactoryQuery pintfQuery (flowQuery.iCprId);

	CProtocolIntfBase* proto = SockManGlobals::Get()->iProtocolIntfFactories->FindOrCreateProtocolIntfL(Uid(), pintfQuery);
	if (NULL == proto)
    	{
    	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CSubConnectionFlowFactoryBase %08x:\tCreateFlowL(protocolId '%08x') Cannot find protocol interface, bailing out"), this, Uid()) );
    	User::Leave(KErrNotFound);
    	}
    // A null CPR indicates that there is no real control side at all, meaning it's a legacy protocol and the FlowRequest is puppeteering this
    // creation. In such a case we must be creating a CTransportFlowShim, which understands that a null SCPR is to be ignored
    __ASSERT_DEBUG(!flowQuery.iCprId.IsNull() || proto->Factory().Uid().iUid == CTransportFlowShimFactory::iUid, User::Panic(KSpecAssert_ESockSSocksbcnfl, 2));
    ACommsFactoryNodeId* flow = DoCreateFlowL(proto, aQuery);
	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CSubConnectionFlowFactoryBase %08x:\tCreateFlowL(protocolId '%08x'): flow %08x"), this, Uid(), &flow) );
	return flow;
	}

EXPORT_C TServerProtocolDesc* CSubConnectionFlowFactoryBase::CreateFlowDescriptionL(TInt aProtocol)
	{
	// Defer to derived implementation
	return DoCreateFlowDescriptionL(aProtocol);
	}

EXPORT_C TServerProtocolDesc* CSubConnectionFlowFactoryBase::DoCreateFlowDescriptionL(TInt /*aProtocol*/)
	{
	__ASSERT_DEBUG(0, Panic(EMisconfigured));
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C CSubConnectionFlowFactoryBase::~CSubConnectionFlowFactoryBase()
	{
	}



//-=====================================================================
// Default PINT
//-=====================================================================
class CDefaultProtocolIntf : public CProtocolIntfBase
/**
This is the internal, dummy, default CProtocolIntfBase implementation
that will by default accompany any flow unless the flow factory
decides to overload its CreateProtocolIntfFactoryL method.

@internalTechnology
*/
	{
public:
	CDefaultProtocolIntf(CProtocolIntfFactoryBase& aFactory,const Messages::TNodeId& aCprId);

	// from CProtocolIntfBase
	virtual void DoFlowCreated(ESock::CSubConnectionFlowBase& aFlow);
	virtual void DoFlowBeingDeleted(ESock::CSubConnectionFlowBase& aFlow);
	};

class CDefaultProtocolIntfFactory : public CProtocolIntfFactoryBase
/**
This is the internal, dummy, default CProtocolIntfBase implementation
that will by default accompany any flow unless the flow factory
decides to overload its CreateProtocolIntfFactoryL method.

@internalTechnology
*/
	{
public:
	virtual CProtocolIntfBase* DoCreateProtocolIntfL(TFactoryQueryBase& aQuery);
	static CDefaultProtocolIntfFactory* NewL(TUid aFactoryId, ESock::CProtocolIntfFactoryContainer& aParentContainer);
protected:
	CDefaultProtocolIntfFactory(TUid aFactoryId, ESock::CProtocolIntfFactoryContainer& aParentContainer);
	};
CDefaultProtocolIntfFactory* CDefaultProtocolIntfFactory::NewL(TUid aFactoryId, ESock::CProtocolIntfFactoryContainer& aParentContainer)
	{
    CDefaultProtocolIntfFactory* factory = new (ELeave) CDefaultProtocolIntfFactory(aFactoryId, aParentContainer);
    CleanupStack::PushL(factory);
    factory->ConstructL();
    CleanupStack::Pop(factory);
    return factory;
	}

CDefaultProtocolIntf::CDefaultProtocolIntf(CProtocolIntfFactoryBase& aFactory,const Messages::TNodeId& aCprId)
:CProtocolIntfBase(aFactory,aCprId)
    {
    }

void CDefaultProtocolIntf::DoFlowCreated(ESock::CSubConnectionFlowBase& /*aFlow*/)
    {
    }

void CDefaultProtocolIntf::DoFlowBeingDeleted(ESock::CSubConnectionFlowBase& /*aFlow*/)
    {
    }

CProtocolIntfBase* CDefaultProtocolIntfFactory::DoCreateProtocolIntfL(TFactoryQueryBase& aQuery)
    {
    const TDefaultProtocolIntfFactoryQuery& query = static_cast<const TDefaultProtocolIntfFactoryQuery&>(aQuery);
    return new (ELeave) CDefaultProtocolIntf(*this,query.iCprId);
    }

CDefaultProtocolIntfFactory::CDefaultProtocolIntfFactory(TUid aFactoryId, CProtocolIntfFactoryContainer& aParentContainer)
:	CProtocolIntfFactoryBase(aFactoryId, aParentContainer)
    {
	}

EXPORT_C CProtocolIntfFactoryBase* CSubConnectionFlowFactoryBase::CreateProtocolIntfFactoryL(CProtocolIntfFactoryContainer& aParentContainer)
    {
    return CDefaultProtocolIntfFactory::NewL (Uid(), aParentContainer);
    }


//-=====================================================================
//
//
// CSubConnectionFlowBase
//
//
//-=====================================================================
EXPORT_C CSubConnectionFlowBase::CSubConnectionFlowBase(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId,
                                                        CProtocolIntfBase* aProtocolIntf)
:	ACommsFactoryNodeId(aFactory),
	iProtocolIntf(aProtocolIntf)
	{
	__ASSERT_DEBUG(iProtocolIntf, User::Panic(KSpecAssert_ESockSSocksbcnfl, 3));
    iSubConnectionProvider.Open(aSubConnId);
	// Signal the ProtocolIntf that a Flow has been created.
	iProtocolIntf->FlowCreated(*this);
	}

EXPORT_C CSubConnectionFlowBase::~CSubConnectionFlowBase()
	{
	if(iLastRequestOriginator.IsOpen())
		{
		iLastRequestOriginator.ReplyTo(Id(), TEChild::TLeft().CRef());
		}
    iProtocolIntf->FlowBeingDeleted(*this);
	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CSubConnectionFlowBase %08x:\t~CSubConnectionFlowBase"), this) );
	}

EXPORT_C void CSubConnectionFlowBase::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
    {
    if (iLastRequestOriginator.Open(iSubConnectionProvider, aSender) != KErrNone)
        {
        if(!iSubConnectionProvider.IsOpen())
	        {
	    	// Legacy flows legitimately lack a sub-connection provider
	        return;
	        }
		else if(aMessage.IsMessage<TEBase::TError>())
			{
			//[TODO EC120] RZ: WHoa! What is this?! Flow receives a TError and as a response sends TError
			//(leaves from ::ReceivedL are handled by sending TError).
			__ASSERT_DEBUG(static_cast<TEBase::TError&>(aMessage).iValue != KErrNone, User::Panic(KSpecAssert_ESockSSocksbcnfl, 4));
			User::Leave(static_cast<TEBase::TError&>(aMessage).iValue);	// don't mutate the error into KErrArgument
			}
		User::Leave(KErrArgument);
        }
    }

EXPORT_C void CSubConnectionFlowBase::SubConnectionGoingDown()
	{
	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CSubConnectionFlowBase %08x:\tSubConnectionGoingDown"), this) );
	iSubConnectionProvider.Close();
	DeleteThisFlow();
	}

EXPORT_C NetInterfaces::TInterfaceControl* CSubConnectionFlowBase::FetchNodeInterfaceControlL(TInt aInterfaceId)
    {
    return DoFetchInterfaceControlL(aInterfaceId);
    }

EXPORT_C MFlowBinderControl* CSubConnectionFlowBase::GetBinderControlL()
	{
	return DoGetBinderControlL();
	}

EXPORT_C void CSubConnectionFlowBase::DeleteThisFlow()
/**
Called from Flows to delete themselves, usually as a result of a received Destroy() message.

*/
	{

	DeleteMeNow();
	// "this" no longer valid after this point.

	}

EXPORT_C CProtocolIntfBase* CSubConnectionFlowBase::ProtocolIntf() const
/**
Return the ProtocolIntf associated with this Flow
*/
	{
	return iProtocolIntf;
	}


