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

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/ss_protflow.h>
#include <comms-infras/ss_subconnflow.h>
#include "SS_conn.H"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <ecom/ecom.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_prt2, "ESockSSocks_prt2");
#endif

using namespace ESock;
using namespace Factories;
using namespace Messages;

CProtocolIntfFactoryContainer::CProtocolIntfFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CProtocolIntfFactoryContainer::EId)
	{
	LOG_NODE_CREATE(KESockDataFactTag, CProtocolIntfFactoryContainer);
	}

CProtocolIntfFactoryContainer::~CProtocolIntfFactoryContainer()
	{
	LOG_NODE_DESTROY(KESockDataFactTag, CProtocolIntfFactoryContainer);
	}

CProtocolIntfFactoryContainer* CProtocolIntfFactoryContainer::NewL()
/** Create a new instance of a sub-connection factory container

@exception Leaves in out of memory conditions
@return Pointer to new instance of a sub-connection factory container
*/
	{
	return new (ELeave) CProtocolIntfFactoryContainer();
	}


EXPORT_C CProtocolIntfFactoryBase* CProtocolIntfFactoryContainer::FindOrCreateFactoryL(TUid aFactoryUid)
    {
	CProtocolIntfFactoryBase* factory = static_cast<CProtocolIntfFactoryBase*>(FindFactory(aFactoryUid));
	if (NULL==factory)
		{
		CSubConnectionFlowFactoryBase* flowFactory = static_cast <CSubConnectionFlowFactoryBase* > (SockManGlobals::Get()->iSubConnectionFlowFactories->FindOrCreateFactoryL(aFactoryUid));
		if (NULL==flowFactory)
    		{
    		User::Leave(KErrNotFound);
    		}
		factory = flowFactory->CreateProtocolIntfFactoryL(*this);
		}
	__ASSERT_DEBUG(factory, User::Panic(KSpecAssert_ESockSSocks_prt2, 1));
	return static_cast<CProtocolIntfFactoryBase*>(factory);
    }

EXPORT_C CProtocolIntfBase* CProtocolIntfFactoryContainer::FindOrCreateProtocolIntfL( TUid aFactoryId, TFactoryQueryBase& aQuery)
/**
@internalTechnology
@released Since 9.2 */
	{
	CProtocolIntfFactoryBase* factory = CProtocolIntfFactoryContainer::FindOrCreateFactoryL(aFactoryId);
	__ASSERT_DEBUG(factory, User::Panic(KSpecAssert_ESockSSocks_prt2, 2));
	return static_cast<CProtocolIntfBase*>(factory->FindOrCreateProtocolIntfL( aQuery ));
	}


EXPORT_C CProtocolIntfFactoryBase::CProtocolIntfFactoryBase(TUid aFactoryId, CProtocolIntfFactoryContainer& aParentContainer)
/** Sub-connection flow factory constructor

@param aFactoryId Unique Integer Identifier of the sub-connection provider factory
@param aParentContainer Container to add the factory to */
	: CFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C CProtocolIntfBase* CProtocolIntfFactoryBase::FindOrCreateProtocolIntfL(TFactoryQueryBase& aQuery)
	{
	CProtocolIntfBase* prov = FindProtocolIntf(aQuery);
	return prov ? prov : CreateProtocolIntfL(aQuery);
	}

EXPORT_C CProtocolIntfBase* CProtocolIntfFactoryBase::FindProtocolIntf(TFactoryQueryBase& aQuery)
	{
	return static_cast<CProtocolIntfBase*>(FindObject(aQuery));
	}

EXPORT_C CProtocolIntfBase* CProtocolIntfFactoryBase::CreateProtocolIntfL(TFactoryQueryBase& aQuery)
	{
	//at this point we are either:
	//1. Given aSubConnectionProviderBase => control side initialised sub-connection<->flow creation
	//2. aSubConnectionProviderBase == NULL => data side initialised sub-connection<->flow creation for
	//	implicit connection
	//in all other cases we should find a proper sub-connection flow already created
	CProtocolIntfBase* pintf = DoCreateProtocolIntfL(aQuery);
	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CProtocolIntfFactoryBase %08x:\t\tCreateProtocolIntfL(): pintf %08x"), this, pintf) );
	CleanupStack::PushL(pintf);
	AddManagedObjectL(*pintf);
	CleanupStack::Pop(pintf);
	return pintf;
	}

EXPORT_C CProtocolIntfBase::CProtocolIntfBase(CProtocolIntfFactoryBase& aFactory,const Messages::TNodeId& aCprId)
:	AFactoryObject(aFactory),
	iCprId(aCprId)
	{
	}

EXPORT_C CProtocolIntfBase::~CProtocolIntfBase()
	{
	// All Flows must have been deleted as it is last Flow deleted causes
	// the ProtocolIntf to be deleted.
	__ASSERT_DEBUG(iFlowCount == 0, User::Panic(KSpecAssert_ESockSSocks_prt2, 3));
	}


TBool CProtocolIntfBase::FlowBeingDeleted(CSubConnectionFlowBase& aFlow)
/**
Called when one of the Flows associated with this ProtocolIntf is being
deleted.  Called from the context of the Flow.

@return ETrue if this is the last Flow on this ProtocolIntf and the intf deletes itslef, else EFalse.
*/
	{
	__ASSERT_DEBUG(iFlowCount > 0, User::Panic(KSpecAssert_ESockSSocks_prt2, 4));
	DoFlowBeingDeleted(aFlow);
	TBool b = --iFlowCount == 0;
	if (b)
	    {
    	DeleteMeNow();
	    }
	return b;
	}

void CProtocolIntfBase::FlowCreated(CSubConnectionFlowBase& aFlow)
	{
	__ASSERT_DEBUG(iFlowCount >= 0, User::Panic(KSpecAssert_ESockSSocks_prt2, 5));
	DoFlowCreated(aFlow);
	++iFlowCount;
	}


