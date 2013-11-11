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
 @publishedPartner
 @released
*/

#if !defined(__SS_FACT_H__)
#define __SS_FACT_H__

#include <elements/factory.h>
#include <comms-infras/ss_nodemessages.h>
#include <elements/sd_objectbroker.h>

namespace NetMessages
{
class CMessage;
}

namespace ESock
{
class ACommsFactoryNodeId;
class CCommsFactoryContainer;

NONSHARABLE_CLASS(CCommsFactoryBase) : public Factories::CFactoryBase
/** Comms Factory Base class (Specialised by sub-connection provider factories
    and other provider factories in future. It provides basic object managment
	functionlity and the ability for a factory to be load from another Dynamic
	Linked Library via the ECOM framework.

    It also provides asynchronous destruction for it's managed objects (if
	required) but also copes with synchronous destruction, should a managed
	object get deleted by another entity.

@publishedPartner
@released Since 9.0 */
	{
public:
/*
@internalTechnology
*/
	IMPORT_C ACommsFactoryNodeId* FindOrCreateObjectL(TFactoryQueryBase& aQuery);
  	IMPORT_C ACommsFactoryNodeId* CreateObjectL(TFactoryQueryBase& aQuery);	
  	IMPORT_C ACommsFactoryNodeId* Find(Factories::MFactoryQuery& aQuery);
	IMPORT_C TInt SendMessage(NetMessages::CMessage& aNetMessage);
	IMPORT_C virtual ~CCommsFactoryBase();
protected:
/*
@internalTechnology
*/
	IMPORT_C CCommsFactoryBase(TUid aFactoryUid, CCommsFactoryContainer& aParentContainer);
  	IMPORT_C virtual TInt DoReceiveMessage(NetMessages::CMessage& aNetMessage);
  	IMPORT_C virtual ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);
	IMPORT_C virtual void DoPostCreationL(ACommsFactoryNodeId* aObject,TFactoryQueryBase& aQuery);
  	virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery) =0;
  	};

class ACommsFactoryNodeId : public Factories::AFactoryObject, 
							public Messages::ASimpleNodeIdBase
/** Base class for the comms factory object. A concrete factory object that wants
to be a node should be derived from this.

@publishedPartner
@released Since 9.5 */
	{
public:
	IMPORT_C ~ACommsFactoryNodeId ();
protected:
	IMPORT_C ACommsFactoryNodeId (CCommsFactoryBase& aFactory);
	};
	
// We do not want to include ss_fact_internal.h in this header and we cannot export
// non-inline versions of __x86gcc_meta_connection_provider_base_cast which rely on
// internals from CCommsFactoryContainer from the ss_metaconnprov.cpp file without
// changing the ARM .def file (the x86gcc .def files are built on the fly from ARM)
// so we redeclare this class here and #ifdef out the one in ss_fact_internal.h
// depending on which is included first.
#ifndef __CCOMMSFACTORYCONTAINER_DECLARED
#define __CCOMMSFACTORYCONTAINER_DECLARED
	NONSHARABLE_CLASS(CCommsFactoryContainer) : public Factories::CFactoryContainerNode
	/** Comms Factory Factory Container class is used to store comms factorys of a
	    specific type. e.g. sub-connection provider factories. It provides basic
		functionality to find, add and remove factories. It has basic functionality to
		create a factory object. If a container wants to do more than just creating a 
		factory object it should specialise the CCommsFactoryContainer

 	@internalTechnology
	@released Since 9.0 */
		{
		friend class CPitBoss;	// CPitBoss enumerates factories without needing (or wanting) derived types

	#ifdef __X86GCC__
	 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
	 	// functions to do the cast.
	    friend  CCommsFactoryContainer* __x86gcc_comms_factory_container_cast(Messages::ANode* aNode);
	    friend  CCommsFactoryContainer& __x86gcc_comms_factory_container_cast(Messages::ANode& aNode);
	#elif !defined(__GCCXML__)
		friend CCommsFactoryContainer* Messages::mnode_cast<CCommsFactoryContainer>(Messages::ANode* aNode);
	#endif
		
	public:
		IMPORT_C Factories::CFactoryBase* FindOrCreateFactoryL(TUid aUid);	
		IMPORT_C ACommsFactoryNodeId* Find (Factories::MFactoryQuery& aQuery);
	protected:
		Factories::CFactoryBase* CreateFactoryL(TUid aUid);	
		ACommsFactoryNodeId* CreateFactoryObjectNodeL(Messages::TSignatureBase& aMessage);
	    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
		
	protected:
	    //Type information for the planes.
	    enum TContaineeType
	        {
	        EUnknown = 0,
		    ETierManagerFactoryContainer,
		    EMetaConnectionFactoryContainer,
		    EConnectionFactoryContainer,
		    ESubConnectionFactoryContainer,
		    ESubConnectionFlowFactoryContainer,
			EProtocolIntfFactoryContainer,
			EProtocolFamilyFactoryContainer,
	        };
		CCommsFactoryContainer(TContaineeType aId = EUnknown);
	public:
	     const TContaineeType iId;

	public:
		static CCommsFactoryContainer* NewL (TContaineeType aId);
		};
#endif // __CCOMMSFACTORYCONTAINER_DECLARED
} // namespace ESock

#endif	// __SS_FACT_H__

