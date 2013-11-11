// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SS_METACONNPROV.H
// 
//

/**
 @file
 @internalAll
*/

#ifndef SYMBIAN_SS_METACONNPROV_INTERNAL_H
#define SYMBIAN_SS_METACONNPROV_INTERNAL_H

#include <comms-infras/ss_fact_internal.h>
#include <comms-infras/ss_metaconnprov.h>

namespace ESock
{

class CMetaConnectionProviderBase;

/**
   Configuration classes for Config Access points
*/
class TConfigAccessPointLayer
	{
    public:
	inline TConfigAccessPointLayer() {};
	inline TConfigAccessPointLayer(const TUint aId,
								   const TUid aSCprUid,
								   const TUid aProtocolUid,
								   const TUint iLayerBelow=0,
								   const TBool aTopLevel=EFalse);

	inline TUint Id() const {return iId;}
	inline TUid SCprUid() const {return iSCprUid;}
	inline TUid ProtocolUid() const {return iProtocolUid;}
	inline TUint LayerBelow() const {return iLayerBelow; }
	inline TBool TopLevel() const {return iTopLevel;}

	inline void SetId(const TUint aId) { iId = aId; }
	inline void SetSCprUid(const TUid aUid) { iSCprUid = aUid; }
	inline void SetProtocolUid(const TUid aUid) { iProtocolUid = aUid; }
	inline void SetLayerBelow(const TUint aLayerBelow) { iLayerBelow = aLayerBelow; }
	inline void SetTopLevel(const TBool aTopLevel) { iTopLevel = aTopLevel; }

    private:
	TUint iId;
	TUid iSCprUid;
	TUid iProtocolUid;

	TUint iLayerBelow;
	TBool iTopLevel;
	};

TConfigAccessPointLayer::TConfigAccessPointLayer(const TUint aId, const TUid aSCprUid,
												 const TUid aProtocolUid, const TUint aLayerBelow,
												 const TBool aTopLevel)
	{
	SetId(aId);
	SetSCprUid(aSCprUid);
	SetProtocolUid(aProtocolUid);
	SetLayerBelow(aLayerBelow);
	SetTopLevel(aTopLevel);
	}

class CConfigAccessPointConfig : public CBase, public Meta::SMetaData
	{
    public:
	enum { ETypeId = 0, EUid = 0x102822FF };

	IMPORT_C static CConfigAccessPointConfig* NewL();
	IMPORT_C ~CConfigAccessPointConfig();

	IMPORT_C void AppendLayerL(const TConfigAccessPointLayer& aLayer);
	IMPORT_C TBool LayerExists(const TConfigAccessPointLayer& aLayer) const;
	IMPORT_C void GetTopLayersL(const TUid aScprUid, RPointerArray<TConfigAccessPointLayer>& aTopLayers) const;
	IMPORT_C const TConfigAccessPointLayer& GetLayerL(const TUint aId) const;
	IMPORT_C TUint GetLayerCount() const;

	IMPORT_C void AppendSCprL(const TUid& aUid);
	IMPORT_C const RArray<TUid>& SCprs() const;

	EXPORT_DATA_VTABLE

    private:
	inline CConfigAccessPointConfig() {};

	RArray<TUid> iSCprs;
	RArray<TConfigAccessPointLayer> iLayers;
	};

//
//Factories - container
//
//There are no exports by design. The container is not supposed to be accessed
//synchronously via a function call (other than ReceivedL()).

NONSHARABLE_CLASS(CMetaConnectionFactoryContainer) : public CCommsFactoryContainer
/** Container for meta connection provider factories.
*/
	{
	friend class CMetaConnectionProviderFactoryBase;

#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CMetaConnectionProviderBase* ESock::__x86gcc_meta_connection_provider_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CMetaConnectionProviderBase* ESock::__x86gcc_meta_connection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = EMetaConnectionFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = EMetaConnectionFactoryContainer
        };
#else
protected:
    friend CMetaConnectionProviderBase* Factories::factoryobject_cast<CMetaConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CMetaConnectionProviderBase* Factories::factoryobject_cast<const CMetaConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = EMetaConnectionFactoryContainer
        };
#endif

public:
	virtual ~CMetaConnectionFactoryContainer();

public:
	static CMetaConnectionFactoryContainer* NewL();

public:
	CMetaConnectionProviderFactoryBase* Factory(TInt aIndex) const;

protected:
	CMetaConnectionFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	};
} // namespace Esock


namespace Factories
{

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CMetaConnectionProviderBase* factoryobject_cast(AFactoryObject* aFactoryObject)
	    {
	    return ESock::__x86gcc_meta_connection_provider_base_cast(aFactoryObject);
	    };

	template <>
	inline const ESock::CMetaConnectionProviderBase* factoryobject_cast(const AFactoryObject* aFactoryObject)
	    {
	    return ESock::__x86gcc_meta_connection_provider_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace.
	template<>
	inline ESock::CMetaConnectionProviderBase* factoryobject_cast<ESock::CMetaConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CMetaConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CMetaConnectionProviderBase*>(aFactoryObject) : NULL;
	    }

	template<>
	inline const ESock::CMetaConnectionProviderBase* factoryobject_cast<const ESock::CMetaConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CMetaConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CMetaConnectionProviderBase*>(aFactoryObject) : NULL;
	    }
#endif

} //namespace Factories

#endif
// SYMBIAN_SS_METACONNPROV_INTERNAL_H

