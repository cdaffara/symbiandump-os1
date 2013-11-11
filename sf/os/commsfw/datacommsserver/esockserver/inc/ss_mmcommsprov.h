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
 @file
 @internalTechnology 
*/


#ifndef SYMBIAN_MMCOMMSPROV_H
#define SYMBIAN_MMCOMMSPROV_H

#include <comms-infras/ss_mmnode.h>
#include <ss_fact.h>
#include <comms-infras/ss_parameterfamilybundle.h>

namespace PRStates
{
class TStoreParams;
class TRespondWithCurrentParams;
class TSendParamsToServiceProvider;
class TRespondWithRetrievedParams;
}
namespace IpCprStates
{
class TInitialiseParams;
class TSendParamsToSelf;
class TUpdateNetMCPR;
}

namespace ESock
{

//base class for mesh-machine based providers
class CMMCommsProviderBase : public CCommsProviderBase,
							 public ACommsFactoryNodeId,
                             public ACFMMNodeBase,
                             private MAccessPointConfigApi,
                             public ITFHIERARCHY_LINK_1(CMMCommsProviderBase, MeshMachine::AMMNodeBase, MAccessPointConfigApi)

    {
#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend  CMMCommsProviderBase* __x86gcc_mm_comms_provider_base_cast(Messages::ANode* aNode);
    friend  CMMCommsProviderBase& __x86gcc_mm_comms_provider_base_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
	friend  CMMCommsProviderBase* Messages::mnode_cast<CMMCommsProviderBase>(Messages::ANode* aNode);
#endif
	friend class CSubConnectionFactoryContainer;
	friend class CMetaConnectionFactoryContainer;
	friend class CConnectionFactoryContainer;
	friend class PRStates::TStoreProvision;

	friend class PRStates::TStoreParams;
	friend class PRStates::TRespondWithCurrentParams;
	friend class PRStates::TSendParamsToServiceProvider;
	friend class IpCprStates::TInitialiseParams;
	friend class IpCprStates::TSendParamsToSelf;
	friend class IpCprStates::TUpdateNetMCPR;
	friend class PRStates::TRespondWithRetrievedParams;
	
public:
	typedef ITFHIERARCHY_LINK_1(CMMCommsProviderBase, MeshMachine::AMMNodeBase, MAccessPointConfigApi) TIfStaticFetcherNearestInHierarchy;
	IMPORT_C void ReturnInterfacePtrL(MAccessPointConfigApi*& aInterface);

	IMPORT_C virtual Messages::RNodeInterface* AddClientL(const Messages::TNodeId& aClientId, const Messages::TClientType& aClientType, TAny* aClientInfo = NULL);

	void DestroyOrphanedDataClients();

	IMPORT_C void SetParametersL(const RCFParameterFamilyBundleC& aParameterBundle);

	inline RCFParameterFamilyBundleC& GetParameterBundle()
		{
		return iParameterBundle;
		}
	
	IMPORT_C RCFParameterFamilyBundleC& GetOrCreateParameterBundleL();
	IMPORT_C RCFParameterFamilyBundleC& CreateParameterBundleL();
	const Messages::TNodeId& Id () const
		{
		return NodeId ();	
		}



protected:
	CMMCommsProviderBase(CCommsFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	virtual ~CMMCommsProviderBase();
    IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);

private:
	// From ESock::MAccessPointConfigApi
	IMPORT_C virtual const ESock::RMetaExtensionContainerC& GetAccessPointConfig() const;

protected:
	RCFParameterFamilyBundleC iParameterBundle;
    };

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	inline CMMCommsProviderBase* __x86gcc_mm_comms_provider_base_cast(Messages::ANode* aNode)
    	{
    	return static_cast<CMMCommsProviderBase*>(aNode);
    	}

	inline CMMCommsProviderBase& __x86gcc_mm_comms_provider_base_cast(Messages::ANode& aNode)
    	{
    	return static_cast<CMMCommsProviderBase&>(aNode);
    	}
#endif
} //namespace ESock


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CMMCommsProviderBase* mnode_cast<ESock::CMMCommsProviderBase>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_mm_comms_provider_base_cast(aNode);
		}

	template <>
	inline ESock::CMMCommsProviderBase& mnode_cast<ESock::CMMCommsProviderBase>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_mm_comms_provider_base_cast(aNode);
		}
	}
#endif

#endif //SYMBIAN_MMCOMMSPROV_H

