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
//

/**
 @file 
 @internalTechnology
*/

#include "vc.h"

#include <elements/vc.h>
#include <elements/metadata.h> //Should we decouple STypeId from NetMeta (circular dependency)?


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemVirtualCtorv, "ElemVirtualCtorv");
#endif

using namespace Meta;
using namespace VC;


#ifdef _DEBUG
	_LIT(KNetCtorPanic, "NetCtor");
#endif
enum TNetCtorPanic
	{
	EConstructorTableHasRepetitions = 0,
	EConstructorTableIsNotSorted = 1
	};

const TInt KConstructorTableArrayGranularity = 8;

EXPORT_C CVirtualCtorBase::CVirtualCtorBase()
:	iCtorTableList(KConstructorTableArrayGranularity,_FOFF(TCtorTable,iInterfaceId))
	{
	}

EXPORT_C CVirtualCtorBase::~CVirtualCtorBase()
	{
	iCtorTableList.Close();
	}

#if defined(_DEBUG)
//This fn is only a debug mode helper function.
//It may not:
//1) be exported in a header file
//2) be exported as IMPORT_C/EXPORT_C
//3) modify the object being debugged in any way

//We assume that elements of the array may never be NULL
TBool __Debug_NoRepetitions(const TImplementationProxy* aFirstEntry, TInt aLength)
	{
	__ASSERT_DEBUG(aLength>=0, User::Panic(KSpecAssert_ElemVirtualCtorv, 1));

	if (aLength<=1)
		{
		return ETrue;
		}

	for (TInt i = 0; i < aLength; ++i)
		{
		for (TInt j = i + 1; j < aLength; ++j)
			{
			if ((aFirstEntry+i)->iImplementationUid.iUid == (aFirstEntry+j)->iImplementationUid.iUid)
				{
				return EFalse;
				}
			}
		}

	return ETrue;
	}

//We assume that elements of the array may never be NULL
TBool __Debug_IsSorted(const TImplementationProxy* aFirstEntry, TInt aLength)
	{
	__ASSERT_DEBUG(aLength>=0, User::Panic(KSpecAssert_ElemVirtualCtorv, 2));

	if (aLength<=1)
		{
		return ETrue;
		}

	const TImplementationProxy* thisEntry = aFirstEntry + 1;

	TReal trend = thisEntry->iImplementationUid.iUid - aFirstEntry->iImplementationUid.iUid;

	for (TInt i = 1; i < aLength; ++i)
		{
		TReal next = (thisEntry++)->iImplementationUid.iUid;
		TReal prev = (aFirstEntry++)->iImplementationUid.iUid;
		if (((next - prev) * trend) < 0)
			{
			return EFalse;
			}
		}
	return ETrue;
	}
#endif

EXPORT_C void CVirtualCtorBase::RegisterInterfaceL(TUid aInterfaceId, TInt aNumCtors, const TImplementationProxy* aCtorTable)
	{
	TInt dummyIndex;
	const TCtorTable* table = FindCtorTable(aInterfaceId, dummyIndex);
	if(table != NULL)
		{
		table->iRegisteredCount++;
		}
	else
		{
		//Repetitions are not allowed. It is a serious programming mistake.
		__ASSERT_DEBUG(__Debug_NoRepetitions(aCtorTable,aNumCtors), User::Panic(KNetCtorPanic, EConstructorTableHasRepetitions));

		//The disadvantage of a binary search is that the array needs to be sorted
		//(in this case at compile time) or there will be a significant speed penalty.

		//We assert here (and not log only) to protect binary compatibility of future tables.
		__ASSERT_DEBUG(__Debug_IsSorted(aCtorTable,aNumCtors), User::Panic(KNetCtorPanic, EConstructorTableIsNotSorted));

		iCtorTableList.InsertInUnsignedKeyOrderL(TCtorTable(aInterfaceId, aNumCtors, aCtorTable));
		}
	}

EXPORT_C void CVirtualCtorBase::DeregisterInterface(TUid aInterfaceId)
	{
	TInt index;
	const TCtorTable* table = FindCtorTable(aInterfaceId, index);
	if(table != NULL)
		{
		if(--(table->iRegisteredCount) == 0)
			{
			iCtorTableList.Remove(index);
			}
		}
	}

EXPORT_C TBool CVirtualCtorBase::IsInterfaceRegistered(TUid aInterfaceId) const
	{
	TInt idx;
	const CVirtualCtorBase::TCtorTable* table = FindCtorTable(aInterfaceId,idx);
	return table!=NULL;
	}

const CVirtualCtorBase::TCtorTable* CVirtualCtorBase::FindCtorTable(TUid aInterfaceId, TInt& aIndex) const
	{
	return (KErrNone==iCtorTableList.FindInUnsignedKeyOrder(TCtorTable(aInterfaceId,0,0),aIndex))? &iCtorTableList[aIndex] : NULL;
	}

EXPORT_C TProxyNewLPtr CVirtualCtorBase::FindRawCtor(const Meta::STypeId& aType) const
	{
	TInt aIndex;
	const TCtorTable* ctorTable;
	if((ctorTable = FindCtorTable(aType.iUid, aIndex)) != NULL)
		{
		//In our case the only advantage of binary search over linear search is speed.
		//The number of comparisons required for a binary search on N lelments is on average log2(N),
		//while for linear search it is obviously N/2.
		//For example, for an array of 40 entries, binary search will on average require 6 comparisons
		//against 20 for linear. For an array with 1000 entries the proportion is around 1%.

		//For very small arrays the speed gain is insigniffcant, but we have decided that
		//advantages from searching of even medium sized arrays compensate disadvantages related to
		//forced sorting.

		//Currently there is no support for binary search of const (compile time) arrays like TFixedArray.
		//We use this "workaround" below to be able to use supported binary search on our const arrays.
		TImplementationProxy* entries = const_cast<TImplementationProxy*>(ctorTable->iCtorTable);
		RArray<TImplementationProxy> array(sizeof(TImplementationProxy),entries,ctorTable->iNumCtors);
		TImplementationProxy tester = {aType.iType,NULL};
		__ASSERT_COMPILE(_FOFF(TImplementationProxy,iImplementationUid)==0); //We rely on the fact that the key offset is 0
		return (KErrNone==array.FindInUnsignedKeyOrder(tester,aIndex))? array[aIndex].iNewLFuncPtr : NULL;
		}
	return NULL;
	}

EXPORT_C TAny* CVirtualCtorInPlace::New(const Meta::STypeId& aType, TDes8& aBuff) const
    {
	TInPlaceNewPtr newFn = FindCtor(aType);
	if(newFn)
		{
		return newFn(aBuff);
		}
	return NULL;
    }


