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
 @internalComponent 
*/

#if !defined(SS_PROTFLOW_H_INCLUDED_)
#define SS_PROTFLOW_H_INCLUDED_

#include <e32def.h>
#include <e32base.h>
#include <ss_fact.h>
#include <comms-infras/ss_fact_internal.h>

/***********************************************************************************/
// ProtocolIntf related classes

namespace ESock
{
class CConnectionProviderBase;


class CProtocolIntfFactoryBase;
class CSubConnectionFlowBase;

// =========================================================================================

class CProtocolIntfBase : public CBase, public Factories::AFactoryObject
/** CProtocolIntf class
	
@internalTechnology
@released Since 9.3 */
	{
	friend class CProtocolIntfFactoryBase;

public:
	void FlowCreated(CSubConnectionFlowBase& aFlow);
	TBool FlowBeingDeleted(CSubConnectionFlowBase& aFlow);

	virtual void DoFlowCreated(CSubConnectionFlowBase& aFlow) = 0;
	virtual void DoFlowBeingDeleted(CSubConnectionFlowBase& aFlow) = 0;
	
	const Messages::TNodeId& ControlProviderId() const
    	{
    	return iCprId;
    	}

protected:
	IMPORT_C CProtocolIntfBase(CProtocolIntfFactoryBase& aFactory, const Messages::TNodeId& aCprId);
	IMPORT_C virtual ~CProtocolIntfBase();

private:
	Messages::TNodeId iCprId;	// Connection Provider cookie
	TInt iFlowCount;			// count of Flows associated with this ProtocolIntf
	};

class CProtocolIntfFactoryBase;

// =========================================================================================

class CProtocolIntfFactoryContainer : public CCommsFactoryContainer
/** Container for protocol2 factories

@internalTechnology
@released Since 9.3 */
	{
#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CProtocolIntfBase* __x86gcc_protocol_intf_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CProtocolIntfBase* __x86gcc_protocol_intf_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = EProtocolIntfFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = EProtocolIntfFactoryContainer
        };
#else
protected:
    friend CProtocolIntfBase* Factories::factoryobject_cast<CProtocolIntfBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CProtocolIntfBase* Factories::factoryobject_cast<const CProtocolIntfBase>(const Factories::AFactoryObject* aFactoryObject);
    enum
        {
        EId = EProtocolIntfFactoryContainer
        };
#endif

public:
	static CProtocolIntfFactoryContainer* NewL();
	IMPORT_C CProtocolIntfFactoryBase* FindOrCreateFactoryL(TUid aFactoryUid);
	IMPORT_C CProtocolIntfBase* FindOrCreateProtocolIntfL(TUid aFactoryId, TFactoryQueryBase& aQuery);
	~CProtocolIntfFactoryContainer();

protected:
	CProtocolIntfFactoryContainer();
	};

// =========================================================================================

class CProtocolIntfFactoryBase : public Factories::CFactoryBase
/** Base class for all ProtocolIntf factories. 

@internalTechnology
@released Since 9.3 */
	{
public:
	IMPORT_C CProtocolIntfBase* FindOrCreateProtocolIntfL(TFactoryQueryBase& aQuery);
	IMPORT_C CProtocolIntfBase* FindProtocolIntf(TFactoryQueryBase& aQuery);
	IMPORT_C CProtocolIntfBase* CreateProtocolIntfL(TFactoryQueryBase& aQuery);

protected:
	IMPORT_C CProtocolIntfFactoryBase(TUid aFactoryId, CProtocolIntfFactoryContainer& aParentContainer);
	
	/**	Override this to provide your own implementation for creating a flow
	
	@param aSubConnectionProviderBase The sub-connection provider for the flow to be attached to could be NULL
			meaning that sub-connection is yet be found(selected).
			Until than all calls are KErrNotReady except JoinL
	@param aSubConnType The creation type */
	virtual CProtocolIntfBase* DoCreateProtocolIntfL(TFactoryQueryBase& aQuery) =0;	
	};


#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	inline CProtocolIntfBase* __x86gcc_protocol_intf_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CProtocolIntfFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CProtocolIntfBase*>(aFactoryObject) : NULL;
    	}

	inline const CProtocolIntfBase* __x86gcc_protocol_intf_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CProtocolIntfFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CProtocolIntfBase*>(aFactoryObject) : NULL;
    	}
#endif
} // namespace ESock

namespace Factories
{

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template<>
	inline ESock::CProtocolIntfBase* factoryobject_cast<ESock::CProtocolIntfBase>(Factories::AFactoryObject* aFactoryObject)
	    { 
	    return ESock::__x86gcc_protocol_intf_base_cast(aFactoryObject);
	    }
	    
	template<>
	inline const ESock::CProtocolIntfBase* factoryobject_cast<const ESock::CProtocolIntfBase>(const Factories::AFactoryObject* aFactoryObject)
	    { 
	    return ESock::__x86gcc_protocol_intf_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace.
	template<>
	inline ESock::CProtocolIntfBase* factoryobject_cast<ESock::CProtocolIntfBase>(Factories::AFactoryObject* aFactoryObject)
	    { 
	    return ESock::CProtocolIntfFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CProtocolIntfBase*>(aFactoryObject) : NULL;
	    }
	    
	template<>
	inline const ESock::CProtocolIntfBase* factoryobject_cast<const ESock::CProtocolIntfBase>(const Factories::AFactoryObject* aFactoryObject)
	    { 
	    return ESock::CProtocolIntfFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CProtocolIntfBase*>(aFactoryObject) : NULL;
	    }
#endif	

} // namespace Factories

#endif	// SS_PROTFLOW_H_INCLUDED_

