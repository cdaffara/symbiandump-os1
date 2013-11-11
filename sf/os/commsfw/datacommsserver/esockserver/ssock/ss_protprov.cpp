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

/**
 @file SS_PROTPROV.CPP
*/

#include "ss_protprov.h"
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_sapshim.h>
#include "SS_rslv.H"
#include <comms-infras/ss_roles.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksprtpr, "ESockSSocksprtpr");
#endif

using namespace ESock;

CProtocolFamilyFactoryContainer* CProtocolFamilyFactoryContainer::NewL()
/** Create a new instance of a protocol family factory container

@exception Leaves in out of memory conditions
@return Pointer to new instance of a protocol family factory container
*/
	{
	return new (ELeave) CProtocolFamilyFactoryContainer();
	}

CProtocolFamilyFactoryContainer::CProtocolFamilyFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CProtocolFamilyFactoryContainer::EId)
	{
	LOG_NODE_CREATE(KESockDataFactTag, CProtocolFamilyFactoryContainer);
	}

CProtocolFamilyFactoryContainer::~CProtocolFamilyFactoryContainer()
	{
	LOG_NODE_DESTROY(KESockDataFactTag, CProtocolFamilyFactoryContainer);
	}

EXPORT_C CProtocolFamilyFactoryBase* CProtocolFamilyFactoryContainer::FindOrCreateL(const TDesC8& aName, TUid aFactoryId)
/** Search for a protocol family factory based on the Factory ID

@param aFactoryId Id of the factory to match against
@see TProtocolFamilyImplementations
@return Pointer to th protocol family factory with matching ID
*/
	{
	LOG( ESockLog::Printf(_L("CProtocolFamilyFactoryContainer::FindOrCreateL() %08x:\tId=%d"), this, aFactoryId); );
	CProtocolFamilyFactoryBase* pFactory = const_cast<CProtocolFamilyFactoryBase*>(
					static_cast<const CProtocolFamilyFactoryBase*>
							((CCommsFactoryContainer::FindFactory(aFactoryId))));
	return pFactory ? pFactory : CProtocolFamilyFactoryBase::NewL( aName, *this );
	}

CProtocolFamilyFactoryBase* CProtocolFamilyFactoryContainer::Get(TInt aIndex) const
	{
	return static_cast<CProtocolFamilyFactoryBase*>(Factory( aIndex ));
	}

    
//==========================================================

EXPORT_C CProtocolFamilyFactoryBase* CProtocolFamilyFactoryBase::NewL(const TDesC8& aName, CProtocolFamilyFactoryContainer& aParentContainer)
/** Create a new instance of a protocol family Factory (Uses the base class
    that uses ECOM to load the DLL that implements the specialised protocol family factory
@param aName Unique String Identifier of the desired factory implementation
@param aParentContainer Container to add the factory to
@return New instance of the factory
@exception KErrNoMemory in out of memory conditions or KErrNotFound if the DLL cannot be found
*/
	{
	const TUid requestedUid = { KProtocolFamilyInterfaceUid };
	return static_cast<CProtocolFamilyFactoryBase*>(CCommsFactoryBase::NewL(aName, requestedUid, aParentContainer));
	}


EXPORT_C CProtocolFamilyFactoryBase::CProtocolFamilyFactoryBase(TUid aFactoryId, CProtocolFamilyFactoryContainer& aParentContainer)
/** Sub-connection provider factory constructor

@param aFactoryId Unique Integer Identifier of the factory
@param aParentContainer Container to add the factory to
*/
	: CCommsFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C CProtocolBase* CProtocolFamilyFactoryBase::FindProtocol( TUint aProtocolId )
	{
	TSglQueIter<CProtocolRef> i(*SockManGlobals::Get()->iProtocols);
	// Run the queue looking for a match.
	do
		{
		if(((CProtocolRef *)i)->Info().iProtocol == aProtocolId)
			{
			return ((CProtocolRef *)i)->Protocol();
			}
		i++;
		}
	while((CProtocolRef *)i);
	return NULL;
	}

EXPORT_C ACommsFactoryNodeId* CProtocolFamilyFactoryBase::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
   	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSocksprtpr, 1));
    User::Leave(KErrNotSupported);
    return NULL;
	}


