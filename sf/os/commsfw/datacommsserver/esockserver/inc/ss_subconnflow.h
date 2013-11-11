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
 @file
 @internalComponent 
*/

#if !defined(__SS_SUBCONNFLOW_H__)
#define __SS_SUBCONNFLOW_H__

#include <e32def.h>
#include <es_sock.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_nodeinterfaces.h>

class TSockAddr;
class CPlayer;

namespace ESock
{
class CConnectionProviderBase;
class CSubConnectionProviderBase;
class CSubConnectionFlowFactoryBase;
class CSubConnectionFlowBase;

// =========================================================================================

class CSubConnectionFlowFactoryContainer : public CCommsFactoryContainer
/** Container for sub-connection flow factories

@internalTechnology
@released Since 9.2 */
	{
#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = ESubConnectionFlowFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = ESubConnectionFlowFactoryContainer
        };
#else
protected:
    friend CSubConnectionFlowBase* Factories::factoryobject_cast<CSubConnectionFlowBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CSubConnectionFlowBase* Factories::factoryobject_cast<const CSubConnectionFlowBase>(const Factories::AFactoryObject* aFactoryObject);
    enum
        {
        EId = ESubConnectionFlowFactoryContainer
        };
#endif

public:
	static CSubConnectionFlowFactoryContainer* NewL();
	~CSubConnectionFlowFactoryContainer();

protected:
	CSubConnectionFlowFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	};

const TInt KSubConnectionFlowFactoryInterfaceUid = 0x10281C33;

class CSubConnectionProviderBase;
class CProtocolIntfFactoryBase;
class CProtocolIntfBase;
class CProtocolIntfFactoryContainer;

// =========================================================================================

class CSubConnectionFlowFactoryBase : public CCommsFactoryBase
/** Base class for all flow factories. Specifies the interface
    that all flow factories must implement. It also providers the
	ECOM framework (via CCommsFactoryBase) to load flow factories
	from other Dynamic Linked Libraries.

@internalTechnology
@released Since 9.2 */
	{
    friend class CProtocolIntfFactoryContainer;
	friend class CSubConnectionFlowFactoryContainer;
public:
	IMPORT_C virtual ~CSubConnectionFlowFactoryBase();


	/** Obtain protocol description of the flow - caller takes custody
	@param aProtocol Protocol id of the flow type for which a description is being requested.
	*/
	IMPORT_C TServerProtocolDesc* CreateFlowDescriptionL(TInt aProtocol);

protected:
	IMPORT_C CSubConnectionFlowFactoryBase(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer);

	/**	Override this to provide your own implementation for creating a flow

	@param aSubConnectionProviderBase The sub-connection provider for the flow to be attached to could be NULL
			meaning that sub-connection is yet be found(selected).
			Until than all calls are KErrNotReady except JoinL
	@param aSubConnType The creation type */
	IMPORT_C virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);
	virtual CSubConnectionFlowBase* DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery) =0;

	/**	Override this to provide your own implementation of the ProtocolInft factory.

	@param aParentContainer The protocol factory container 'this' is expected to insert self to */
    IMPORT_C virtual CProtocolIntfFactoryBase* CreateProtocolIntfFactoryL(CProtocolIntfFactoryContainer& aParentContainer);

	/** Override this to provide flow specific "protocol" description
	@param aProtocol Protocol id of the flow type for which a description is being requested.
	*/
	IMPORT_C virtual TServerProtocolDesc* DoCreateFlowDescriptionL(TInt aProtocol);
	};

// =========================================================================================

// =========================================================================================
class MFlowBinderControl;
//!!!instance goes down when iSubConnection instance goes down => all clients of iSubConnection leave
class CSubConnectionFlowBase : public CCommsProviderBase,                           
								public ACommsFactoryNodeId
/**	Base class specifying the interfaces required to implement a flow.
	Contains collections of sub connection control and data clients and defines operations on them
	Instance of the object dies when the coresponding instance of CSubConnectionProvider goes

@internalTechnology
@released Since 9.2 */
	{
	friend class CSubConnectionFlowFactoryBase;
	friend class CSubConnectionFlowFactoryContainer;
#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend  CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(Messages::ANode* aNode);
    friend  CSubConnectionFlowBase& __x86gcc_subconnection_flow_base_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
    friend  CSubConnectionFlowBase* mcfnode_cast<CSubConnectionFlowBase>(Messages::ANode* aNode);
    friend  CSubConnectionFlowBase& mcfnode_cast<CSubConnectionFlowBase>(Messages::ANode& aNode);
#endif

	// Downcalls from clients
public:
	IMPORT_C MFlowBinderControl* GetBinderControlL();

    CSubConnectionFlowFactoryBase& Factory() const
        {
        return static_cast<CSubConnectionFlowFactoryBase&>(AFactoryObject::Factory());
        }

	const Messages::RNodeInterface& ControlProvider() const
		{
		return iSubConnectionProvider;
		}

	/** Legacy flows have no permanent control plane, giving them more responsibility for the layer below
	*/
	TBool HasControlPlane() const
		{
		return iSubConnectionProvider.IsOpen();
		}

protected:
	IMPORT_C CSubConnectionFlowBase(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId,
	                                CProtocolIntfBase* aProtocolIntf);

	// Don't allow clients to invoke the destructor.
	// (Only the CCommsFactoryBase should do this)
	IMPORT_C virtual ~CSubConnectionFlowBase();

	virtual MFlowBinderControl* DoGetBinderControlL() = 0;
	IMPORT_C void DeleteThisFlow();
	IMPORT_C CProtocolIntfBase* ProtocolIntf() const;


protected:
	IMPORT_C virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);
    IMPORT_C void SubConnectionGoingDown();

protected:

	Messages::RNodeInterface iSubConnectionProvider;
	Messages::RRequestOriginator iLastRequestOriginator;
	CProtocolIntfBase* iProtocolIntf;
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	inline CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(Messages::ANode* aNode)
    	{
    	return static_cast<CSubConnectionFlowBase*>(aNode);
    	}

	inline CSubConnectionFlowBase& __x86gcc_subconnection_flow_base_cast(Messages::ANode& aNode)
    	{
    	return static_cast<CSubConnectionFlowBase&>(aNode);
    	}

	inline CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CSubConnectionFlowFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CSubConnectionFlowBase*>(aFactoryObject) : NULL;
    	}

	inline const CSubConnectionFlowBase* __x86gcc_subconnection_flow_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CSubConnectionFlowFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CSubConnectionFlowBase*>(aFactoryObject) : NULL;
    	}
#endif
//==========================================================================================
} //namespace ESock


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CSubConnectionFlowBase* mnode_cast<ESock::CSubConnectionFlowBase>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_subconnection_flow_base_cast(aNode);
		}

	template <>
	inline ESock::CSubConnectionFlowBase& mnode_cast<ESock::CSubConnectionFlowBase>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_subconnection_flow_base_cast(aNode);
		}
	}
#endif


namespace Factories
{

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template<>
	inline ESock::CSubConnectionFlowBase* factoryobject_cast<ESock::CSubConnectionFlowBase>(Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_subconnection_flow_base_cast(aFactoryObject);
	    }

	template<>
	inline const ESock::CSubConnectionFlowBase* factoryobject_cast<const ESock::CSubConnectionFlowBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_subconnection_flow_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace...
	template<>
	inline ESock::CSubConnectionFlowBase* factoryobject_cast<ESock::CSubConnectionFlowBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CSubConnectionFlowFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CSubConnectionFlowBase*>(aFactoryObject) : NULL;
	    }

	template<>
	inline const ESock::CSubConnectionFlowBase* factoryobject_cast<const ESock::CSubConnectionFlowBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CSubConnectionFlowFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CSubConnectionFlowBase*>(aFactoryObject) : NULL;
	    }
#endif

} //namespace Factories

#endif	// __SS_SUBCONNFLOW_H__



