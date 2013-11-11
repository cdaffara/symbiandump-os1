// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of object index class
// 
//

#ifndef __WSOBJIX_H__
#define __WSOBJIX_H__

#include <e32std.h>
#include <e32base.h>

class CWsObject;

/**
A simple structure to hold a CWsObject and the associated handle.

@internalComponent
@released
*/
class TWsObject
	{
public:
	enum
		{
		ESlotMask=0x0000FFFF,			//The handle on the client side uses these 16 bits to store the slot position
		ECountMask=0x7FFF0000,			//These 15 bits are used to keep a unique number (the slot reuse count)
		ETypeMask=ESlotMask,			//The handle on the server side uses these 16 bits to store the type of the object
		ECountPosition=16,
		ECountBits=15,
		ECountInc=1<<ECountPosition,
		ECountWrapAround=1<<ECountBits,
		};
public:
	inline TWsObject(CWsObject* aObject,TInt aHandle);
public:
	CWsObject* iObject;
	TUint iHandle;
	};

/**
An array of CWsObject instances and their handles. 

Each client session maintains a list of the objects it owns.

Note that the first item in the iObjectArray array is a dummy item. This is because part of the 
handle is used as an index in the array and we want to avoid index 0 because a null handle has a special
meaning.

@see CWsClient
@internalComponent
@released
*/
class CWsObjectIx : public CBase
	{
public:
	CWsObjectIx();
	~CWsObjectIx();
	void ConstructL();
	TInt AddL(CWsObject* anObj);
	void Remove(const TWsObject* aObject);
	void Remove(CWsObject* anObj);
	CWsObject* HandleToObject(TInt aHandle) const;
	inline const CWsObject* At(TInt aPos) const;
	const TWsObject* FirstObject() const;
	TInt At(const CWsObject* anObj);
	void Tidy();
	TInt Count() const;		// Count of non NULL objects in the array
	TInt Length() const;	// Length of array including empty slots
private:
	CArrayFixFlat<TWsObject> iObjectArray;
	TInt iNewObjectCount;
	};


//
// inlines			//
//

//
// TWsObject
//
inline TWsObject::TWsObject(CWsObject* aObject,TInt aHandle) :iObject(aObject), iHandle(aHandle) {}
//
// CWsObjectIx
//
inline const CWsObject* CWsObjectIx::At(TInt aPos) const
	{return iObjectArray[aPos].iObject;}
#endif
