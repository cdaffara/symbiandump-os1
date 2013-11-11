// stringhash.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef FSHELL_STRINGHASH_H
#define FSHELL_STRINGHASH_H

#include <e32hashtab.h>

namespace LtkUtils
	{
	template <typename T> class TStringHashIter;
	template <typename T> struct SHashVal { HBufC* iKey; T iValue; };
	
	template <typename T>
	class RStringHash
		{
	public: // RHashMap-style
		inline RStringHash();
		inline T* Find(const TDesC& aKey);
		inline T FindPtr(const TDesC& aKey); // Only use if T is a pointer
		inline T& FindL(const TDesC& aKey);
		inline T const* Find(const TDesC& aKey) const;
		inline T const FindPtr(const TDesC& aKey) const; // Only use if T is a pointer
		inline T const& FindL(const TDesC& aKey) const;
		inline TInt Insert(const TDesC& aKey, const T& aValue);
		inline void InsertL(const TDesC& aKey, const T& aValue);
		inline TInt Remove(const TDesC& aKey);

		inline TInt Count() const;
		inline TInt Reserve(TInt aCount);
		inline void ReserveL(TInt aCount);
		inline void Close();

	protected:
		inline SHashVal<T>* DoFind(const TDesC& aKey);
		inline SHashVal<T> const* DoFind(const TDesC& aKey) const;
		friend class TStringHashIter<T>;

		RHashMap<TDesC*, SHashVal<T> > iHash;
		TAny* iSpare;
		TAny* iSpare2;
		};

	template <typename T>
	class TStringHashIter
		{
	public:
		inline TStringHashIter(const RStringHash<T>& aHash);
		inline const TDesC* CurrentKey() const;
		inline const TDesC* NextKey();
		inline T* CurrentValue();
		inline const T* NextValue();
		inline void RemoveCurrent();

	private:
		THashMapIter<TDesC*, SHashVal<T> > iIter;
		};

	inline TUint32 HashFn(TDesC* const& aKey);
	inline TBool IdFn(TDesC* const& aFirst, TDesC* const& aSecond);
	}


// Nothing to see here, move along move along

template <typename T>
LtkUtils::RStringHash<T>::RStringHash()
	: iHash(THashFunction32<TDesC*>(&HashFn), TIdentityRelation<TDesC*>(&IdFn)), iSpare(NULL), iSpare2(NULL)
	{
	}

inline TUint32 LtkUtils::HashFn(TDesC* const& aKey)
	{
	return DefaultHash::Des16(*aKey);
	}

inline TBool LtkUtils::IdFn(TDesC* const& aFirst, TDesC* const& aSecond)
	{
	return DefaultIdentity::Des16(*aFirst, *aSecond);
	}

template <typename T>
inline T* LtkUtils::RStringHash<T>::Find(const TDesC& aKey)
	{
	SHashVal<T>* res = DoFind(aKey);
	if (res) return &res->iValue;
	else return NULL;
	}


template <typename T>
inline T LtkUtils::RStringHash<T>::FindPtr(const TDesC& aKey)
	{
	T* res = Find(aKey);
	if (res) return *res;
	else return NULL;
	}


template <typename T>
inline T const* LtkUtils::RStringHash<T>::Find(const TDesC& aKey) const
	{
	SHashVal<T> const* res = DoFind(aKey);
	if (res) return &res->iValue;
	else return NULL;
	}

template <typename T>
inline T const LtkUtils::RStringHash<T>::FindPtr(const TDesC& aKey) const
	{
	T const* res = Find(aKey);
	if (res) return *res;
	else return NULL;
	}

template <typename T>
inline T& LtkUtils::RStringHash<T>::FindL(const TDesC& aKey)
	{
	T* res = Find(aKey);
	if (!res) User::Leave(KErrNotFound);
	return *res;
	}

template <typename T>
inline T const& LtkUtils::RStringHash<T>::FindL(const TDesC& aKey) const
	{
	T const* res = Find(aKey);
	if (!res) User::Leave(KErrNotFound);
	return *res;
	}

template <typename T>
TInt LtkUtils::RStringHash<T>::Insert(const TDesC& aKey, const T& aValue)
	{
	SHashVal<T>* valPtr = DoFind(aKey);
	if (valPtr)
		{
		// Just update the value, no need to realloc anything
		memcpy(&valPtr->iValue, &aValue, sizeof(T));
		return KErrNone;
		}

	HBufC* buf = aKey.Alloc();
	if (!buf) return KErrNoMemory;
	// Avoid declaring an SHashVal<T> as that requires T to have a default constructor
	TUint8 valBuf[sizeof(SHashVal<T>)];
	SHashVal<T>& val = *reinterpret_cast<SHashVal<T>*>(valBuf);
	val.iKey = buf;
	memcpy(&val.iValue, &aValue, sizeof(T));
	TInt err = iHash.Insert(buf, val);
	if (err)
		{
		delete buf;
		}
	return err;
	}

template <typename T>
inline TInt LtkUtils::RStringHash<T>::Remove(const TDesC& aKey)
	{
	SHashVal<T>* valPtr = DoFind(aKey);
	if (valPtr)
		{
		HBufC* key = valPtr->iKey;
		iHash.Remove(key);
		delete key;
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

template <typename T>
inline void LtkUtils::RStringHash<T>::Close()
	{
	THashMapIter<TDesC*, SHashVal<T> > iter(iHash);
	while (iter.NextValue() != NULL)
		{
		SHashVal<T>* current = const_cast<SHashVal<T>*>(iter.CurrentValue()); // const_cast not needed except on 9.1
		HBufC* key = current->iKey;
		iter.RemoveCurrent();
		delete key;
		}
	iHash.Close();
	}

template <typename T>
inline void LtkUtils::RStringHash<T>::InsertL(const TDesC& aKey, const T& aValue)
	{
	User::LeaveIfError(Insert(aKey, aValue));
	}

template <typename T>
inline TInt LtkUtils::RStringHash<T>::Count() const
	{
	return iHash.Count();
	}

template <typename T>
inline TInt LtkUtils::RStringHash<T>::Reserve(TInt aCount)
	{
	return iHash.Reserve(aCount);
	}

template <typename T>
inline void LtkUtils::RStringHash<T>::ReserveL(TInt aCount)
	{
	User::LeaveIfError(Reserve(aCount));
	}

template <typename T>
inline LtkUtils::SHashVal<T>* LtkUtils::RStringHash<T>::DoFind(const TDesC& aKey)
	{
	return iHash.Find(const_cast<TDesC*>(&aKey));
	}

template <typename T>
inline LtkUtils::SHashVal<T> const* LtkUtils::RStringHash<T>::DoFind(const TDesC& aKey) const
	{
	return iHash.Find(const_cast<TDesC*>(&aKey));
	}

/// Iterator support

template <typename T>
inline const TDesC* LtkUtils::TStringHashIter<T>::CurrentKey() const
	{
	return *const_cast<TDesC**>(iIter.CurrentKey());
	}

template <typename T>
inline const TDesC* LtkUtils::TStringHashIter<T>::NextKey()
	{
	return *const_cast<TDesC**>(iIter.NextKey());
	}

template <typename T>
inline T* LtkUtils::TStringHashIter<T>::CurrentValue()
	{
	SHashVal<T>* val = const_cast<SHashVal<T>*>(iIter.CurrentValue()); // The const_cast isn't needed, except on 9.1
	if (val) return &val->iValue;
	return NULL;
	}

template <typename T>
inline const T* LtkUtils::TStringHashIter<T>::NextValue()
	{
	const SHashVal<T>* val = iIter.NextValue();
	if (val) return &val->iValue;
	return NULL;
	}

template <typename T>
inline void LtkUtils::TStringHashIter<T>::RemoveCurrent()
	{
	SHashVal<T>* val = iIter.CurrentValue();
	if (val)
		{
		HBufC* key = val->iKey;
		iIter.RemoveCurrent();
		delete key;
		}
	}

template <typename T>
inline LtkUtils::TStringHashIter<T>::TStringHashIter(const RStringHash<T>& aHash)
	: iIter(aHash.iHash)
	{}

#endif
