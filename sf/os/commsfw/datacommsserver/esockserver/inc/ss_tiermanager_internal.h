// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __SS_TIERMANAGER_INTERNAL_H__
#define __SS_TIERMANAGER_INTERNAL_H__

#include <comms-infras/ss_fact_internal.h>
#include <comms-infras/ss_tiermanager.h>
#include <ss_fact.h>

namespace ESock
{

//
//Factories - container
//
class CTierManagerFactoryContainer : public CCommsFactoryContainer
/** Container for tier manager factories.
*/
	{
#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CTierManagerBase* __x86gcc_tier_manager_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CTierManagerBase* __x86gcc_tier_manager_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = ETierManagerFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = ETierManagerFactoryContainer
        };
#else
protected:
    friend CTierManagerBase* Factories::factoryobject_cast<CTierManagerBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CTierManagerBase* Factories::factoryobject_cast<const CTierManagerBase>(const Factories::AFactoryObject* aFactoryObject);
 
    enum
        {
        EId = ETierManagerFactoryContainer
        };
#endif

public:
	IMPORT_C virtual ~CTierManagerFactoryContainer();

public:
	static CTierManagerFactoryContainer* NewL();

public:
	CTierManagerFactoryBase* Factory(TInt aIndex) const;

protected:
	CTierManagerFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
private:
	void MapLegacyTierId(TUid& aTierUid);
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast...

	inline CTierManagerBase* __x86gcc_tier_manager_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CTierManagerFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CTierManagerBase*>(aFactoryObject) : NULL;
    	}

	inline const CTierManagerBase* __x86gcc_tier_manager_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CTierManagerFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CTierManagerBase*>(aFactoryObject) : NULL;
    	}
#endif
}

namespace Factories
{

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template<>
	inline ESock::CTierManagerBase* factoryobject_cast<ESock::CTierManagerBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::__x86gcc_tier_manager_base_cast(aFactoryObject);
	    }

	template<>
	inline const ESock::CTierManagerBase* factoryobject_cast<const ESock::CTierManagerBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::__x86gcc_tier_manager_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace.
	template<>
	inline ESock::CTierManagerBase* factoryobject_cast<ESock::CTierManagerBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CTierManagerFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CTierManagerBase*>(aFactoryObject) : NULL;
	    }

	template<>
	inline const ESock::CTierManagerBase* factoryobject_cast<const ESock::CTierManagerBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CTierManagerFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CTierManagerBase*>(aFactoryObject) : NULL;
	    }
#endif
    
} //namespace Factories

#endif
// __SS_TIERMANAGER_INTERNAL_H__
