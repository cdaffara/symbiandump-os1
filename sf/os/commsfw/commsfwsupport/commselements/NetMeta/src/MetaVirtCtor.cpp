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

#include <elements/metavirtctor.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNetMetaMtVrt, "ElemNetMetaMtVrt");
#endif

#if !defined(VERIFY_RESULT) && defined(_DEBUG)
#define VERIFY_RESULT(c,d) __ASSERT_DEBUG(c==d, User::Panic(KSpecAssert_ElemNetMetaMtVrt, 1))
#else
#define VERIFY_RESULT(c,d) ((void)(c))
#endif

using namespace Meta;

_LIT(KMetaDataNetCtorPanic, "MetaDataNetCtor");
enum TMetaDataNetCtorPanic
	{
	EConstructedObjectTooBig = 0
	};

/** Thin helper to in-place construction in a descriptor (eg TBuf8)
*/
EXPORT_C TAny* SMetaDataNetCtor::operator new(TUint aSize, TDes8& aBuff)
	{
	__ASSERT_ALWAYS(aSize <= static_cast<TUint>(aBuff.MaxSize()), User::Panic(KMetaDataNetCtorPanic, EConstructedObjectTooBig));
	return const_cast<TUint8*>(aBuff.Ptr());
	}

EXPORT_C SMetaDataNetCtor* CMetaDataVirtualCtorInPlace::New(const Meta::STypeId& aType, TDes8& aBuff) const
    {
    return static_cast<SMetaDataNetCtor*>(VC::CVirtualCtorInPlace::New(aType, aBuff));
    }

EXPORT_C SMetaDataNetCtor* CMetaDataVirtualCtorInPlace::New(const Meta::STypeId& aType, TPtrC8& aParams, TDes8& aBuff) const
	{
	SMetaDataNetCtor* self = CMetaDataVirtualCtorInPlace::New(aType,aBuff);
	if (self)
		{
		VERIFY_RESULT(self->Load(aParams), KErrNone);
		}
	return self;
	}

EXPORT_C SMetaDataNetCtor* CMetaDataVirtualCtorInPlace::New(TPtrC8& aParams, TDes8& aBuff) const
    {
   	__ASSERT_DEBUG(aParams.Length() >= sizeof(Meta::STypeId), User::Panic(KSpecAssert_ElemNetMetaMtVrt, 2));
	const Meta::STypeId* typeId = reinterpret_cast<const Meta::STypeId*>(aParams.Ptr());
	if (typeId==NULL)
		{
		return NULL;
		}
	typeId->Check(aParams); //Strip the buffer
	return New(*typeId, aParams, aBuff);
    }


