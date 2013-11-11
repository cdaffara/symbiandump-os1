// objectix.cpp
// 
// Copyright (c) 1994 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

/** 
@file
@internalTechnology
*/

#include <kern_priv.h>
#include "dobject.h"
#define DObjectIx DObjectIxNinePointTwoHack // So that we're not redefining DObjectIx

/**	Creates a new kernel object index.

	@param	aPtr	Value to pass to Close() when objects are closed due to index deletion.

	@return	The pointer to the new index, NULL if out of memory.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
 */
DObjectIx *DObjectIx::New(TAny* aPtr)
	{
	return new DObjectIx(aPtr);
	}

#ifndef DOBJECT_TEST_CODE
void DObjectIx::Wait()
	{
	Kern::MutexWait(*HandleMutex);
	}

void DObjectIx::Signal()
	{
	Kern::MutexSignal(*HandleMutex);
	}
#endif


/**	Construct a kernel object index
 */
DObjectIx::DObjectIx(TAny *aPtr)
	: iNextInstance(1), iPtr(aPtr), iFree(-1)
	{
	}


/**	Destroys a kernel object index.

	Any objects in the index are closed. The iPtr member of the index is passed
	as the parameter to Close() for each object.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
 */
DObjectIx::~DObjectIx()
	{
	// We have to be very careful here. Calling Close() on the objects in the array
	// may result in other entries being removed from the array before we delete
	// them here, and may result in the array being ReAlloc()ed, corrupting the removed
	// entries, hence we must check the iCount value each time round the loop.	
	TInt i=-1;
	while(++i<iCount)
		{
		SDObjectIxRec* pS=iObjects+i;
		DObject *pO=pS->obj;
		if (pO)
			{
			--iActiveCount;
			pS->obj=NULL;	// invalidate entry after closing it
			pO->Close(iPtr);
			}
		}
	SDObjectIxRec* pR=iObjects;
	iObjects=NULL;
	delete pR;
	}


/**	Adds a kernel object to an index and return a handle.

	@param	aObj	Pointer to the object to add
	@param	aHandle	Place to write returned handle to
	
	@return	KErrNone, if operation successful;
	        KErrNoMemory, if there was insufficient memory to expand the array.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
	@pre	DObject::HandleMutex held
 */
TInt DObjectIx::Add(DObject* anObj, TInt& aHandle)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	TInt index=iFree; //iFree contains the index of the first empty slot or -1 if there is no such.
	if (index<0) //Check if the free list is empty
		{
		// The free list is empty, so more slots must be allocated.
		if (iCount==KObjectIxMaxHandles)
 			return KErrNoMemory;
		
		//Those are internal checking of the object consistency
		__ASSERT_DEBUG(iAllocated==iCount,Panic(EObjInconsistent));
		__ASSERT_DEBUG(iAllocated==iActiveCount,Panic(EObjInconsistent));
		
		//Double allocated memory
		TInt newAlloc=iAllocated ? 2*iAllocated : KObjectIxGranularity;
		if(newAlloc>KObjectIxMaxHandles)
 			newAlloc=KObjectIxMaxHandles;
		TInt r=Kern::SafeReAlloc((TAny*&)iObjects,iAllocated*sizeof(SDObjectIxRec),newAlloc*sizeof(SDObjectIxRec));
		if (r)
			return r;
		//NOTE: There is no need to initialize newly allocated memory (e.g. to zero iObjects) as it all goes
		//beyond iCount and will be not considered when search in At(...) or operator[]() methods. 
		//As the free list is initially ordered, each slot goes through the states as follows:
		// - Created as the part of the free list beyond iCount. - uninitialized and not searched in any method.
		// - In use  - initialized.
		// - The part of the free list within iCount - initialized to zero.
		//Also bear in mind that UpdateState() does not reorder free list beyond iCount but keeps it preserverd.

		iAllocated=newAlloc;		   //Update the number of allocated slots
		iUpdateDisabled = iAllocated/2;//The number of Remove() calls before the object update(free list,iCount,..)

		//Connect all newly allocated slots into the list and set 'index' to point to the first one.
		SDObjectIxRec* pA = (SDObjectIxRec*)iObjects;
		index=newAlloc-1;
		pA[index].nextEmpty = -1;
		while (iCount <= --index) 
			pA[index].nextEmpty=index+1;
		index++;
		}

	//At this point, 'index' variable points to the slot that will be used for the new entry.
	//It also represents the first element in the list of empty slots.

	SDObjectIxRec *pS=iObjects+index; // pS points to the object that will be used for the new entry.
	iFree=pS->nextEmpty;			  // Update iFree to point to the next empty slot. 
	
	NKern::LockSystem();

	//Initialize data of the new element of the array.
	pS->obj=anObj;
	pS->str.uniqueID=(TUint16)anObj->UniqueID();
	pS->str.instance=(TUint16)instanceLimit(iNextInstance);
	
	iNextInstance++;
	
	if (index>=iCount)	//Update iCount to points to the slot after the last in use.
		iCount=index+1;
	
	++iActiveCount;

	NKern::UnlockSystem();

	//Internal checking of the object consistency
	__ASSERT_DEBUG( (iFree==-1 && iAllocated==iCount && iAllocated==iActiveCount)
				  ||(iFree!=-1 && iAllocated>iActiveCount),Panic(EObjInconsistent));
	
	aHandle=makeHandle(index,pS->str.instance);
	return KErrNone;
  }


/**	Removes a kernel object from an index by handle.

	Returns a pointer to the object removed and the parameter to pass to Close().

	@param	aHandle	Handle to object to be removed.
	@param	aObj	Place to write pointer to the object to add.
	@param	aPtr	Place to write parameter to be passed to Close().
	
	@return	KErrNone, if operation successful;
	        KErrBadHandle, if the supplied handle was invalid.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
	@pre	DObject::HandleMutex held.
 */
TInt DObjectIx::Remove(TInt aHandle, DObject*& aObj, TAny*& aPtr)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	TInt i=index(aHandle);
	
	if (i >= iCount)
		return KErrBadHandle;

	SDObjectIxRec* pR=iObjects+i;
	DObject *pO=pR->obj;
	if (!pO || pR->str.instance!=instance(aHandle) || pR->str.uniqueID!=pO->UniqueID())
		return KErrBadHandle;

	NKern::LockSystem();
	pR->obj=NULL;
	--iActiveCount;
	NKern::UnlockSystem();
	
	if(iActiveCount)
		{	
		// Add the entry onto the free list
		pR->nextEmpty=iFree;
		iFree=i;

		if(iUpdateDisabled) 
			iUpdateDisabled--; //Count down till state update is enabled again.

		if (									 //Update the states(HWM, resort free list & memory shrink) if:
			(!iUpdateDisabled) &&				 //There were a number of Remove() calls since the last ReAlloc
			(iAllocated>=2*KObjectIxGranularity))//Allocated memory is above the limit.
			{
			UpdateState();
			iUpdateDisabled = iAllocated/2;//The number of Remove() calls before the next update comes.
			}
		}
	else
		{
		//There is no any CObject left. Reset the object to initial state (except iNextInstance)
		delete iObjects;
		iObjects=NULL;
		iAllocated=0;
		iCount=0;
		iFree=-1;		  //Empty free list
		}

	//This is internal checking of the object consistency
	__ASSERT_DEBUG( (iFree==-1 && iAllocated==iCount && iAllocated==iActiveCount)
				  ||(iFree!=-1 && iAllocated>iActiveCount),Panic(EObjInconsistent));
	
	aObj=pO;
	aPtr=iPtr;
	return KErrNone;
  }


// Private method which:
//1. Reorders free list.
//2. Updates iCount. This is the only place where HWM is decreased, while it can be increased during AddL().
//3. Shrinks the heap memory (pointed by iObjects) if it can be at least halved.
//The function is entered with at least one occupied slot in iObjects array.
//The array is searched from its end. Special care is given to the  case where
//iCount is less then KObjectIxGranularity as the size of the arrey does not go below it.
//Note: HighWaterMark (HWM) is a term used for iCount.
void DObjectIx::UpdateState()
	{
	TBool toShrink = EFalse;
	TBool foundFreeBelowLimit = EFalse;//Used to handle spacial case when HWM is below the minimum alloc. limit
	TInt newHWM = 0;

	//Start from the HWM as all slots beyond are free and sorted already.
	TInt current = iCount;
	TInt prevFreeSlot = iCount == iAllocated ? -1 : iCount;
	while (--current>=0)
		{
		if (iObjects[current].obj)
			{
			//This is the slot with the valid entry. Check if this is the last in the array.
			if(!newHWM)
				{
				//This is the first occupied slot we found => It is new HWM.
				newHWM=current+1;
				if (current < iAllocated/2)
					{
					//At this point we decide to shrink memory.
					toShrink = ETrue;
					//Once we find HWM and decide to shrink, all slots after that point should be removed
					//from the free list as that memory will be freed. The exception is the case when HWM is below
					//the minimum of allocated memory (8 slots as the moment).
					if((current >= KObjectIxGranularity) || (!foundFreeBelowLimit))
						prevFreeSlot = -1; //The next free slot to find will be the last one in the list.
					}
				}
			}
		else
			{
			//This is the free slot.
			if ((!newHWM) && (!foundFreeBelowLimit) &&(current<KObjectIxGranularity))
				{
				//The special case. 
				//We just reached the first free slot below minimum alloc. size and still we found no occupied slots.
				iObjects[current].nextEmpty = -1; //This will be the end of free list.
				foundFreeBelowLimit = ETrue; //Mark that we found the special case
				}
			else
				{
				iObjects[current].nextEmpty = prevFreeSlot;//Link the empty slot in the free list.
				}
			prevFreeSlot = current;
			}
		}

	iCount = newHWM;
	iFree = prevFreeSlot;

	if (toShrink)
		{
		//Do not reallocate less then the initial value.
		TInt newAlloc = Max(newHWM,KObjectIxGranularity);
		//Update member data and re-allocate memory. ReAlloc cannot return NULL as we are asking for less memory.
		Kern::SafeReAlloc((TAny*&)iObjects,iAllocated*sizeof(SDObjectIxRec),newAlloc*sizeof(SDObjectIxRec));
		iAllocated = newAlloc;
		}
	}


/** Counts the number of times an object appears in this index.

	@param	aObject	Object whose occurrences are to be counted.

	@return	Number of times aObject appears in the index.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
	@pre	DObject::HandleMutex held
 */
TInt DObjectIx::Count(DObject* aObject)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	TInt c=0;
	if (iCount)
		{
		SDObjectIxRec* pS=iObjects;
		SDObjectIxRec* pE=pS+iCount;
		do
			{
			if (pS->obj==aObject)
				c++;
			} while (++pS<pE);
		}
	return c;
	}


#ifndef __DOBJECT_MACHINE_CODED__
/**	Looks up an object in the index by handle.
	
	The object must be of a specified type (specified by container ID)

	@param	aHandle		Handle to look up.
	@param	aUniqueID	Unique ID (container ID) that object should have.
	
	@return	Pointer to object or NULL if handle invalid.

	@pre	Call in a thread context.
	@pre    System lock must be held.
 */
DObject* DObjectIx::At(TInt aHandle, TInt aUniqueID)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	TInt i=index(aHandle);
	if (i>=iCount)
		return(NULL);
	SDObjectIxRec *pS=iObjects+i;
	if (pS->str.instance!=instance(aHandle) || pS->str.uniqueID!=aUniqueID)
		return(NULL);
	return(pS->obj);
	}


/**	Looks up an object in the index by handle.

	The object may be of any type.

	@param	aHandle		Handle to look up.
	
	@return	Pointer to object or NULL if handle invalid.

	@pre	Call in a thread context.
	@pre    System lock must be held.
 */
DObject* DObjectIx::At(TInt aHandle)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	TInt i=index(aHandle);
	if (i>=iCount)
		return NULL;
	SDObjectIxRec *pS=iObjects+i;
	if (pS->str.instance!=instance(aHandle))
		return NULL;
	return pS->obj;
	}
#endif


/**	Looks up an object in the index by object pointer.

	Returns a handle to the object.

	@param	aObj	Pointer to the object to look up.
	
	@return	Handle to object (always >0);
	        KErrNotFound, if object not present in index.

    @pre    Calling thread must be in a critical section.
    @pre    No fast mutex can be held.
	@pre	Call in a thread context.
	@pre	DObject::HandleMutex held.
 */
TInt DObjectIx::At(DObject* aObj)
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	if (iCount)
		{
		SDObjectIxRec* pS=iObjects;
		SDObjectIxRec* pE=pS+iCount;
		TInt i=0;
		while(pS<pE && pS->obj!=aObj)
			pS++, i++;
		if (pS<pE)
			return(makeHandle(i,pS->str.instance));
		}
	return KErrNotFound;
	}


#ifndef __DOBJECT_MACHINE_CODED__
/** Finds the object at a specific position in the index array.

	@param	aIndex	Index into array.
	
	@return	Pointer to the object at that position (could be NULL).

	@pre	Call in a thread context. 
    @pre    System lock must be held.
 */
DObject* DObjectIx::operator[](TInt aIndex)
	{
	__ASSERT_SYSTEM_LOCK; //Check preconditions (debug build only)
	__ASSERT_ALWAYS(aIndex>=0 && aIndex<iCount,Panic(EArrayIndexOutOfRange));
	return iObjects[aIndex].obj;
	}
#else
GLDEF_C void PanicDObjectIxIndexOutOfRange(void)
	{
	Panic(EArrayIndexOutOfRange);
	}
#endif

TInt DObjectIx::LastHandle()
//
// Return the last valid handle
// Must wait on HandleMutex before calling this.
//
	{
	//Check preconditions(debug build only)
	__ASSERT_CRITICAL;
	__ASSERT_NO_FAST_MUTEX;

	if (iActiveCount)
		{
		SDObjectIxRec* p=iObjects+iCount;
		while(--p>=iObjects && !p->obj) {}
		return makeHandle(p-iObjects,p->str.instance);
		}
	return 0;
	}

//
// Include these in exactly one CPP file somewhere
//

extern TBool gRunningWithOldDefinition = ETrue;

NONSHARABLE_CLASS(DObjectWithPaddingOnly) : public DObject
	{
public:
	DOBJECT_PADDING;
	};

TBool CalculateDObjectSize()
	{
	DObjectWithPaddingOnly* obj = new DObjectWithPaddingOnly;
	if (!obj) return EFalse;
	
	// objectId points to the mem location where iObjectId will be. So if running on a system with the new size DOBject it will always be non-zero (because objectIds are set in the DObject constructor, and are always non-zero), but if running on earlier systems it will be zero because DBase zero-fills the object
	TUint64& objectId = *reinterpret_cast<TUint64*>((TUint8*)&obj->iName + sizeof(HBuf*));
	
	if (objectId != 0)
		{
		//Kern::Printf("Detected MemoryAccess is running with new larger DObject");
		gRunningWithOldDefinition = EFalse;
		}
	else
		{
		//Kern::Printf("Detected MemoryAccess is running with old-format DObject");		
		}
	obj->Close(NULL);
	return ETrue;
	}
