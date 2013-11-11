// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SS_NODEINTERFACES.CPP
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_dispatchers.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_api_ext.h>
#include <elements/interfacetable.h>
#include <elements/metatype.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksndntr, "ESockSSocksndntr");
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

EXPORT_C void TCFItfExt::ForwardMessageL(const TRuntimeCtxId& aSender,
                                         TSignalBase& aMessage,
                                         Messages::ANode& aNode,
                                         const TClientType& aClientType,
                                         const TClientType& aClientTypeExclude)
	{
	NetInterfaces::TInterfaceControl* ic = NULL;
    ic = aNode.FetchNodeInterfaceControlL(AMMNodeBase::KInterfaceId);
    MeshMachine::AMMNodeBase* nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(ic->FetchInterfaceL(AMMNodeBase::KInterfaceId));

    RNodeInterface* recp;
    if(aClientType.Type() == TCFClientType::ECtrl)
    	{
		recp = nodeBase->GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider),aClientTypeExclude);
    	}
    else
		{
		recp = nodeBase->GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider),aClientTypeExclude);
		}

    if(recp)
    	{
    	recp->PostMessage(aSender, aMessage);
    	}
	else
		{
		User::Leave(KErrNotFound);
		}
	}


EXPORT_C TAny* TCFItfExt::FetchInterfaceL(Messages::ANode& aNode, TInt aInterface)
	{
	NetInterfaces::TInterfaceControl* ic = NULL;
	ic = aNode.FetchNodeInterfaceControlL(aInterface);

	return ic->FetchInterfaceL(aInterface);
	}


EXPORT_C TAny* TCFItfExt::FetchInterfaceOrForwardMsgL(const TRuntimeCtxId& aSender,
                                                      TSignalBase& aMessage,
                                                      Messages::ANode& aNode,
                                                      TInt aInterface,
                                                      const TClientType& aClientType,
                                                      const TClientType& aClientTypeExclude)
    {
	TAny* itf = NULL;
	TRAPD(ret, itf = FetchInterfaceL(aNode, aInterface));

	switch(ret)
		{
	case KErrNone:
		return itf;

	case KErrInterfaceNotSupported:
		ForwardMessageL(aSender, aMessage, aNode, aClientType, aClientTypeExclude);
		break;

	default:
        __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksndntr, 1)); //unreachable
        User::Leave(ret);
        break;
		}

	//Never gets here
	return NULL;
    }


EXPORT_C NetInterfaces::TInterfaceControl* TCFItfExt::FetchInterfaceControlOrForwardMsgL(const TRuntimeCtxId& aSender,
                                                                                         TSignalBase& aMessage,
                                                                                         Messages::ANode& aNode,
																			 			 TInt aInterface,
								  		   		  							  			 const TClientType& aClientType,
								  		   		  							  			 const TClientType& aClientTypeExclude)
	{
	NetInterfaces::TInterfaceControl* ic = NULL;
	TRAPD(ret, ic = aNode.FetchNodeInterfaceControlL(aInterface));

	switch(ret)
		{
	case KErrNone:
		return ic;

	case KErrInterfaceNotSupported:
		ForwardMessageL(aSender, aMessage, aNode, aClientType, aClientTypeExclude);
		break;

	default:
        User::Leave(ret);
        __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksndntr, 2)); //unreachable
        break;
		}

	//Never gets here
	return NULL;
	}

EXPORT_C void TCFSigRMessage2Ext::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt aError)
    {
	LOG( ESockLog::Printf(KESockDCMsgs, _L8( "TCFSigRMessage2Ext:\tError(): completing message (%08X) with %d" ),
			iMessage.Handle(), aError) );
    
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete(aError);
        }
    }

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TCFSigRMessage2Ext, 0, 0 ) //has to be derived from
	REGISTER_ATTRIBUTE( TCFSigRMessage2Ext, iMessage, TMeta<RMessage2> )
END_ATTRIBUTE_TABLE_BASE(TSignalBase, 0)


