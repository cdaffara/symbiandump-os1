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
 @internalTechnology
*/

#ifndef SYMBIAN_VC_H
#define SYMBIAN_VC_H

#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>	// for TImplementationProxy

//VIP == "virtual-ctor in place"

#define DECLARE_MVIP_CTR(class_name)				\
	static class_name* PlacementNew(TDes8& aBuff);	

#define DEFINE_MVIP_CTR(class_name)					\
class_name* class_name::PlacementNew(TDes8& aBuff)	\
	{												\
	return new(aBuff) class_name;					\
	}

#define MVIP_CTR_ENTRY(aInterfaceUid, aMessageClass)  {{aInterfaceUid},(TProxyNewLPtr)(aMessageClass::PlacementNew)}

namespace Meta
{
	struct STypeId;
}

namespace VC
{

class CVirtualCtorBase : public CBase
	{
public:
	IMPORT_C CVirtualCtorBase();
	IMPORT_C ~CVirtualCtorBase();
	IMPORT_C void RegisterInterfaceL(TUid aInterfaceId, TInt aNumCtors, const TImplementationProxy* aCtorTable);
	IMPORT_C void DeregisterInterface(TUid aInterfaceId);
	IMPORT_C TBool IsInterfaceRegistered(TUid aInterfaceId) const;

protected:
	IMPORT_C TProxyNewLPtr FindRawCtor(const Meta::STypeId& aType) const;

private:
	class TCtorTable
		{
	public:
		TCtorTable(TUid aInterfaceId, TInt aNumCtors, const TImplementationProxy* aCtorTable)
		: iInterfaceId(aInterfaceId), 
		  iNumCtors(aNumCtors), 
		  iRegisteredCount(1),
		  iCtorTable(aCtorTable)
			{
			}
	public:
		TUid iInterfaceId;
		TInt iNumCtors;		
		mutable TInt iRegisteredCount;
		const TImplementationProxy* iCtorTable;		
		};
	typedef RArray<TCtorTable> TCtorTableList;
	TCtorTableList iCtorTableList;
private:
	const TCtorTable* FindCtorTable(TUid aInterfaceId, TInt& aIndex) const;
	};

typedef TAny* (*TInPlaceNewPtr)(TDes8& aBuff);

class CVirtualCtorInPlace : public CVirtualCtorBase
	{
public:
	//Constructs only from the type
	IMPORT_C TAny* New(const Meta::STypeId& aType, TDes8& aBuff) const;
	
	inline const TInPlaceNewPtr FindCtor(const Meta::STypeId& aType) const
		{
		return reinterpret_cast<const TInPlaceNewPtr>(FindRawCtor(aType));
		}
	};


} //namespace VC

#endif
//SYMBIAN_VC_H

