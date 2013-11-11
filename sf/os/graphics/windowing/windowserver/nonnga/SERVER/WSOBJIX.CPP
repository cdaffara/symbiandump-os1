// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Object index
// 
//

#include "WSOBJIX.H"

#include "OBJECT.H"
#include "server.h"
#include "panics.h"

const TInt KNewObjectInitialCount = 1;

CWsObjectIx::CWsObjectIx() : iObjectArray(8), iNewObjectCount(KNewObjectInitialCount)
	{
	}

CWsObjectIx::~CWsObjectIx()
	{
	TInt count=iObjectArray.Count();
	if (count==0)
		return;
	TWsObject* ptr=&iObjectArray[0];
	const TWsObject* end=ptr+count;
	for (;ptr<end;ptr++)
		{
		CWsObject* obj=ptr->iObject;
		if (obj)
			{
			ptr->iObject=NULL;
			obj->CloseObject();
			}
		}
	iObjectArray.Reset();
	}

void CWsObjectIx::ConstructL()
	{
	// Insert a dummy object into the first slot to avoid
	// having a null handle
	TWsObject nullObject(NULL,0);
	iObjectArray.AppendL(nullObject);
	}

/** Adds the object to the list.

If there is an empty slot (i.e. a slot that was previously used but has been freed) the
object is put in that slot else a new slot is created at the end of the array.

@param anObj The object to add to the list.
@return The handle to be used by the client to identify this object.
@internalComponent
@released
*/
TInt CWsObjectIx::AddL(CWsObject* anObj)
	{
	TWsObject* ptr=&iObjectArray[0];
	const TWsObject* base=ptr;
	TInt index=iObjectArray.Count();
	const TWsObject* end=ptr+index;
	WS_ASSERT_DEBUG(base->iObject==NULL, EWsPanicObjectIndexError);
	
	// Search for an empty slot
	while(++ptr<end)
		{
		if (ptr->iObject==NULL)
			{
			ptr->iObject=anObj;
			index=ptr-base;
			break;
			}
		}
	
	// create a new handle for the object
	TUint handleCount = ((iNewObjectCount<<TWsObject::ECountPosition) + TWsObject::ECountInc) & TWsObject::ECountMask;
	
	if (ptr==end)
		{
		// No available empty slot, so append this object to the queue
		if (index==TWsObject::ECountInc)
			{
			User::LeaveNoMemory();	//Things will go wrong later if we ever have more 64K objects
			}	
		TWsObject newObject(anObj,handleCount);
		iObjectArray.AppendL(newObject);
		ptr=&iObjectArray[index];
		}
 
	// assign the object a unique server-side handle (combination of handle count & object type)
	ptr->iHandle = handleCount | anObj->Type();

	// increment the object counter
	if (++iNewObjectCount==TWsObject::ECountWrapAround)
		iNewObjectCount = KNewObjectInitialCount;
 	
	// return to the client their unique handle for the object (combination of handle count & slot number)
	return (handleCount + index);
	}	 

void CWsObjectIx::Tidy()
	{
	TInt count=iObjectArray.Count()-1;
	while(count>0)	// Don't delete object [0]
		{
		if (iObjectArray[count].iObject!=NULL)
			break;
		iObjectArray.Delete(count--);
		}
	}

void CWsObjectIx::Remove(CWsObject* anObj)
	{
	const TWsObject* ptr=FirstObject();
	const TWsObject* end=ptr+iObjectArray.Count();
	WS_ASSERT_DEBUG(ptr->iObject==NULL, EWsPanicObjectIndexError);
	while(++ptr<end)
		{
		if (ptr->iObject==anObj)
			{
			Remove(ptr);
			Tidy();
			return;
			}
		}
	}

void CWsObjectIx::Remove(const TWsObject* aObject)
	{
	WS_ASSERT_DEBUG(aObject->iObject!=NULL, EWsPanicObjectIndexError);
	WS_ASSERT_DEBUG((aObject->iHandle&TWsObject::ETypeMask)==(TUint)aObject->iObject->Type(), EWsPanicObjectIndexError);
	const_cast<TWsObject*>(aObject)->iObject=NULL;
	}

CWsObject* CWsObjectIx::HandleToObject(TInt aHandle) const
	{
	TInt slot=aHandle&TWsObject::ESlotMask;
	if (slot<0 || slot>=iObjectArray.Count())
		return(NULL);
	const TWsObject* object=&iObjectArray[slot];
	if ((object->iHandle&TWsObject::ECountMask)==((TUint)aHandle&TWsObject::ECountMask))
		{
		return object->iObject;
		}
	return(NULL);
	}

const TWsObject* CWsObjectIx::FirstObject() const
	{
	return(&iObjectArray[0]);
	}

TInt CWsObjectIx::At(const CWsObject* anObj)
	{
	const TWsObject* base=FirstObject();
	const TWsObject* ptr=base;
	const TWsObject* end=base+iObjectArray.Count();
	WS_ASSERT_DEBUG(ptr->iObject==NULL, EWsPanicObjectIndexError);
	while(++ptr<end)
		{
		if (ptr->iObject==anObj && (ptr->iHandle&TWsObject::ETypeMask)==(TUint)anObj->Type())
			return(ptr-base);
		}
	return(KErrNotFound);
	}

TInt CWsObjectIx::Count() const
	{
	const TWsObject* ptr=FirstObject();
	const TWsObject* end=ptr+iObjectArray.Count();
	WS_ASSERT_DEBUG(ptr->iObject==NULL, EWsPanicObjectIndexError);
	TInt count=0;
	while(++ptr<end)
		{
		if (ptr->iObject && ptr->iObject->Type()!=WS_HANDLE_CLIENT)
			count++;
		}
	return(count);
	}

TInt CWsObjectIx::Length() const
	{
	return(iObjectArray.Count());
	}
