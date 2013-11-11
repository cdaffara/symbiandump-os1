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

#ifndef PROVINFOQUERYSET_H
#define PROVINFOQUERYSET_H

#include <comms-infras/es_commsdataobject.h>
#include <comms-infras/commsdataobjectfactory.h>
#include <es_enum.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif


namespace ESock
{


const TUint KProviderInfoQuerySetUid = 0x10282FFF;


/**
*/
class CProviderInfoQuerySet;
class XProviderInfoQuerySet : public XCommsDataObject	
	{
	friend class CCommsDataObjectFactory;
	friend class CCommsDataObject<CProviderInfoQuerySet, XProviderInfoQuerySet>;
	
public:
	enum 
		{
		iUid = KProviderInfoQuerySetUid
		};
	
	static inline XProviderInfoQuerySet* NewL()
		{
		return static_cast<XProviderInfoQuerySet*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CCommsDataObjectFactory::iUid, KProviderInfoQuerySetUid)));
		}

		
	const TConnProviderInfo& ConnProviderInfo()
		{
		return iConnProviderInfo;
		}
	
	
	IMPORT_C virtual ~XProviderInfoQuerySet();
		

protected:
	// From XCommsDataObject
	IMPORT_C virtual TProgressAction DispatchL(TAny* aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf);

private:
	XProviderInfoQuerySet()
		: XCommsDataObject(XCommsDataObject::EGetSupported)
		{
		}
		
private:
	EXPORT_DATA_VTABLE_AND_FN
	TConnProviderInfo iConnProviderInfo;
	};


class CProviderInfoQuerySet : public CCommsDataObject<CProviderInfoQuerySet, XProviderInfoQuerySet>
	{
protected:
    inline CProviderInfoQuerySet(XProviderInfoQuerySet* aDataObject)
        : CCommsDataObject<CProviderInfoQuerySet, XProviderInfoQuerySet>(aDataObject)
        {
        }
	};

} // namespace ESock

#endif
// PROVINFOQUERYSET_H

