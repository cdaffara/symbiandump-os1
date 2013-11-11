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
// TDeAllocator.h
// 
//

/**
 @file
 @internalTechnology
*/

#if (!defined TDEALLOCATOR_H)
#define TDEALLOCATOR_H

#include <e32base.h>
#include <e32std.h>

namespace Elements
{


template <class TYPE>
class TDeAllocator
/**
	Provides destruction on the right heap
*/
	{
public:
	static TDeAllocator* NewL(TYPE* aObject);
	static TDeAllocator* NewLC(TYPE* aObject);
	~TDeAllocator();

	//Only to be called before deleting TDeAllocator object.
	inline void LeaveObject()
		{
		iObject = NULL;
		}

	inline TYPE* Object()
		{
		return iObject;
		}

protected:
	TDeAllocator(TYPE* aObject)
	:	iAllocator(&User::Heap()),
		iObject(aObject)
		{
		}

private:
	RAllocator* iAllocator;
	TYPE* iObject;
	};

template <class TYPE>
TDeAllocator<TYPE>* TDeAllocator<TYPE>::NewL(TYPE* aObject)
	{
	User::LeaveIfError(aObject? KErrNone : KErrArgument);
	return new (ELeave) TDeAllocator(aObject);
	}

template <class TYPE>
TDeAllocator<TYPE>* TDeAllocator<TYPE>::NewLC(TYPE* aObject)
	{
	TDeAllocator* deallocator = TDeAllocator::NewL(aObject);
	CleanupStack::PushL(deallocator);
	return deallocator;
	}

template <class TYPE>
TDeAllocator<TYPE>::~TDeAllocator()
	{
	if (iObject)
		{
		RAllocator* heap = User::SwitchAllocator(iAllocator);
		delete iObject;
		if(heap != iAllocator)
			{
			User::SwitchAllocator(heap);
			}
		}
	}

} //namespace Elements


#endif //TDEALLOCATOR_H


