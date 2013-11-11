// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Graphics Resource - general inline functions
//

#ifndef SGRESOURCE_INL
#define SGRESOURCE_INL

_LIT(KSgResourcePanicCategory, "SGRES");

inline void Panic(TSgResourcePanicReason aReason)
	{
	User::Panic(KSgResourcePanicCategory, aReason);
	}

inline TSgAttribute::TSgAttribute()
	{
	}

inline TSgAttribute::TSgAttribute(TUid aUid, TInt aValue)
	: iUid(aUid), iValue(aValue)
	{
	}

inline TSgAttributeArrayBase::TSgAttributeArrayBase(TInt aCount)
	: iCount(aCount)
	{
	}

inline void TSgAttributeArrayBase::operator =(const TSgAttributeArrayBase&)
	{
	}

inline TInt TSgAttributeArrayBase::Count() const
	{
	return iCount;
	}

inline TSgAttribute& TSgAttributeArrayBase::operator [](TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iCount, Panic(ESgPanicBadAttributeArrayIndex));
	return reinterpret_cast<TSgAttribute*>(this + 1)[aIndex];
	}

inline const TSgAttribute& TSgAttributeArrayBase::operator [](TInt aIndex) const
	{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iCount, Panic(ESgPanicBadAttributeArrayIndex));
	return reinterpret_cast<const TSgAttribute*>(this + 1)[aIndex];
	}

template<TInt S>
inline TSgAttributeArray<S>::TSgAttributeArray()
	: TSgAttributeArrayBase(S)
	{
	}

inline TBool TSgDrawableId::operator ==(TSgDrawableId aId) const
	{
	return iId == aId.iId;
	}

inline TBool TSgDrawableId::operator !=(TSgDrawableId aId) const
	{
	return iId != aId.iId;
	}

inline RSgDriver::RSgDriver()
	: iImpl(NULL)
	{
	}

template<class M>
inline TInt RSgDriver::GetInterface(M*& aInterfacePtr) const
	{
	return GetInterface(TUid::Uid(M::EInterfaceUid), (TAny*&)aInterfacePtr);
	}

inline RSgDrawable::RSgDrawable()
	: iHandleType(KSgDrawableTypeUid), iImpl(NULL)
	{
	}

inline RSgDrawable::RSgDrawable(TUid aHandleType)
	: iHandleType(aHandleType), iImpl(NULL)
	{
	}

inline TBool RSgDrawable::IsNull() const
	{
	return !iImpl;
	}

template<class M>
inline TInt RSgDrawable::GetInterface(M*& aInterfacePtr) const
	{
	return GetInterface(TUid::Uid(M::EInterfaceUid), (TAny*&)aInterfacePtr);
	}

#endif // SGRESOURCE_INL
