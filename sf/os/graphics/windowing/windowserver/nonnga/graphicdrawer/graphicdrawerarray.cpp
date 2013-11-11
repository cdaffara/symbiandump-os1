// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "WSGRAPHICDRAWERARRAY.H"
#include "Graphics/WSGRAPHICDRAWER.H"
#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "panics.h"

// CWsGraphicDrawerArray::XRollBackBase \\\\\\\\\\\\\\\\\\\\\\\\

/** Base class for cleanup operations - used as a handle for the CommitP() method
	This cleanup object is created on the heap with new(ELeave).
	It must be immediately pushed onto the cleanup stack, BEFORE the operation it protects,
	and must be written so that it does nothing until "activated" by the operation completing successfully.
	iArray==NULL indicates this state.
	This is because the client does not know whether the new(ELeave), the PushL, or the operational meat 
	generated the exception, and therefore the operation must be the last exception thrown in the method.
	@internalComponent
	@released
*/
NONSHARABLE_STRUCT(CWsGraphicDrawerArray::XRollBackBase)
	{
public:
	XRollBackBase():	iArray(NULL)	{}
	virtual ~XRollBackBase()			{}
public:
	CWsGraphicDrawerArray*				iArray;
	};
// CWsGraphicDrawerArray::XAddRollBack \\\\\\\\\\\\\\\\\\\\\\\\

/** Cleanup record for Add operation. Removes the array entry at the recorded array index.
	@internalComponent
	@released
*/
NONSHARABLE_STRUCT(CWsGraphicDrawerArray::XAddRollBack) :public  CWsGraphicDrawerArray::XRollBackBase
	{
	TGraphicDrawerId iId;
	static void RollBackD(TAny* aAddRollBack);
	};

/** Rolls back an adding of a drawer to an array, but does not delete the drawer.
	Removes the array entry at the given array index.	
*/
void CWsGraphicDrawerArray::XAddRollBack::RollBackD(TAny* aAddRollBack)
	{
	__ASSERT_DEBUG(aAddRollBack,Panic(EWsGraphicDrawerPanicBadAddLCCleanup));
	if(aAddRollBack)
		{
		XAddRollBack* rollBack = static_cast<XAddRollBack*>(aAddRollBack);
		// iArray would be NULL if you pushed a NULL drawer in release builds
		//It can also be null if the Add operation Leaves
		if(rollBack->iArray) 
			{
			rollBack->iArray->Remove(rollBack->iId);
			}
		delete  rollBack;
		}
	}

// CWsGraphicDrawerArray::XRemoveRollBack \\\\\\\\\\\\\\\\\\\\\\\\

/** Cleanup record for Swap operation. Re-inserts the recorded drawer by finding its index.
	@internalComponent
	@released
*/
NONSHARABLE_STRUCT(CWsGraphicDrawerArray::XSwapRollBack) :public  CWsGraphicDrawerArray::XRollBackBase
	{
	CWsGraphicDrawer* iDrawer;
	static void RollBackD(TAny* aSwapRollBack);
	};

/** Rolls back the swapping (replacing) of a drawer to an array, but does not delete the replacing drawer.
	Re-inserts the recorded drawer by finding its index.
 */
void CWsGraphicDrawerArray::XSwapRollBack::RollBackD(TAny* aSwapRollBack)
	{
	__ASSERT_DEBUG(aSwapRollBack,Panic(EWsGraphicDrawerPanicBadSwapLCCleanup));
	if(aSwapRollBack)
		{
		XSwapRollBack* rollBack = static_cast<XSwapRollBack*>(aSwapRollBack);
		// would be NULL if you removed an id that wasn't in the array in release builds
		// or if the swap itself Leaves
		if(rollBack->iArray) 
			{
			const TInt idx = rollBack->iArray->IndexOf(rollBack->iDrawer->Id());
			__ASSERT_DEBUG(0 <= idx,Panic(EWsGraphicDrawerPanicBadSwapLCCleanup));
			if(0 <= idx) // hmm, don't see how this could ever happen.  If it does, better to leak memory etc 
				{
				rollBack->iArray->iArray[idx].iDrawer = rollBack->iDrawer;
				}
			}
		delete rollBack;
		}
	}

// CWsGraphicDrawerArray::XRemoveRollBack \\\\\\\\\\\\\\\\\\\\\\\\

/** Cleanup record for Remove operation. Re-inserts the recorded drawer by finding its index.
	@internalComponent
	@released
*/
NONSHARABLE_STRUCT(CWsGraphicDrawerArray::XRemoveRollBack) :public  CWsGraphicDrawerArray::XRollBackBase
	{
	CWsGraphicDrawer* iDrawer;
	static void RollBackD(TAny* aSwapRollBack);
	};

/** Rolls back the deleting of a drawer from an array, but does not delete the replacing drawer.
	Re-inserts the recorded drawer by finding its index.
 */
void CWsGraphicDrawerArray::XRemoveRollBack::RollBackD(TAny* aRemoveRollBack)
	{
	__ASSERT_DEBUG(aRemoveRollBack,Panic(EWsGraphicDrawerPanicBadSwapLCCleanup));
	if(aRemoveRollBack)
		{
		XRemoveRollBack* rollBack = static_cast<XRemoveRollBack*>(aRemoveRollBack);
		// would be NULL if you removed an id that wasn't in the array in release builds
		// or if the swap itself Leaves
		if(rollBack->iArray) 
			{
			TGraphic graphic;
			graphic.iId = rollBack->iDrawer->Id();
			graphic.iDrawer = rollBack->iDrawer;
			TInt errCode= rollBack->iArray->iArray.InsertInOrder(graphic,GraphicDrawerCompare); // dups not allowed
			//This should not happen unless some non-transactional method has modified the array
			//between the call and the leave.
			__ASSERT_DEBUG(KErrAlreadyExists != errCode,Panic(EWsGraphicDrawerPanicBadSwapLCCleanup));
			//Other memory failure errors should not occur unless the array has been Compress()ed
			//between the call and the leave, and then memory fails.
			__ASSERT_DEBUG(KErrNone <= errCode,Panic(EWsGraphicDrawerPanicBadSwapLCCleanup));
			}
		delete rollBack;
		}
	}

// CWsGraphicDrawerArray \\\\\\\\\\\\\\\\\\\\\\\\

/** Compares two graphic array slots for id equality; the iDrawer pointer is ignored.
	Used to order the array.
@internalComponent
@released
*/
TInt CWsGraphicDrawerArray::GraphicDrawerCompare(const TGraphic& aFirst,const TGraphic& aSecond)
	{
	return aFirst.iId.Compare(aSecond.iId);
	}

/** Adds a drawer to the array, with a transactional cleanup item that automatically removes
	it until popped (not destroying the drawer, however). This operation leaks memory and should not be used.
	@deprecated	 - Use AddL or AddRLC
*/
EXPORT_C void CWsGraphicDrawerArray::AddLC(CWsGraphicDrawer* aDrawer)
	{
	__ASSERT_DEBUG(aDrawer,Panic(EWsGraphicDrawerPanicBadArgument));
	XAddRollBack* rollBack = new(ELeave) XAddRollBack;
	CleanupStack::PushL(TCleanupItem(XAddRollBack::RollBackD,rollBack));
	if(aDrawer)
		{
		User::LeaveIfError(Add(aDrawer));
		rollBack->iArray = this;
		rollBack->iId = aDrawer->Id();		
		}
	}

/** Adds a drawer to the array, with a transactional cleanup item that automatically removes
	it (not destroying the drawer, however).
	CommitP() must be called on the returned pointer to release resources owned by the cleanup item,
	unless a Leave occurs.
	@param 	 aDrawer	the drawer to add
	@return		cleanup record
*/
EXPORT_C CWsGraphicDrawerArray::XRollBackBase* CWsGraphicDrawerArray::AddTLC(CWsGraphicDrawer* aDrawer)
	{
	if(!aDrawer)
		{
		User::Leave(KErrArgument);
		}
	//need to create the rollback before the add because the client can't tell the difference between 
	//the add failing and the new(ELeave) failing!
	XAddRollBack* rollBack = new(ELeave) XAddRollBack;
	CleanupStack::PushL(TCleanupItem(XAddRollBack::RollBackD,rollBack));
	User::LeaveIfError(Add(aDrawer));
	rollBack->iId = aDrawer->Id();		
	rollBack->iArray = this;
	
	return rollBack;
	}

/** Adds a drawer to the array. No cleanup - no leak 
	@param 	 aDrawer	the drawer to add
	@return		error code if the Add did not take place
*/
EXPORT_C TInt CWsGraphicDrawerArray::Add(CWsGraphicDrawer* aDrawer)
	{
	if(aDrawer)
		{
		TGraphic graphic;
		graphic.iId = aDrawer->Id();
		graphic.iDrawer = aDrawer;
		return iArray.InsertInOrder(graphic,GraphicDrawerCompare); // dups not allowed
		}
	else
		return KErrArgument;
	}

/*Internal method to swap the given drawer into the array, removing the existing one and returning a pointer to it.
	Note that in an error just a NULL pointer is returned.
	Internal caller must infer KErrNotFound error code.
	@param 	 aDrawer	the drawer to add
	@return				the drawer displaced, or NULL if the operation did not take place
*/
CWsGraphicDrawer* CWsGraphicDrawerArray::SwapIn(CWsGraphicDrawer* aDrawer)
	{
	if (aDrawer==NULL)
		return NULL;
	const TInt idx = IndexOf(aDrawer->Id());
	if (idx<KErrNone)
		return NULL;
	CWsGraphicDrawer* rv= iArray[idx].iDrawer;
	iArray[idx].iDrawer=aDrawer;
	return rv;
	}

/** Replaces the drawer with the existing Id with this newer one.  
	Pushes a transactional cleanup item to restore the previous drawer. 
	This operation leaks memory when it does not get Leave clean-up and should not be used.
	@deprecated	 - Use SwapL or SwapRLC
*/
EXPORT_C TInt CWsGraphicDrawerArray::SwapLC(CWsGraphicDrawer* aDrawer)
	{
	__ASSERT_DEBUG(aDrawer,Panic(EWsGraphicDrawerPanicBadArgument));
	XSwapRollBack* rollBack = new(ELeave) XSwapRollBack;
	CleanupStack::PushL(TCleanupItem(XSwapRollBack::RollBackD,rollBack));
	CWsGraphicDrawer* rollBackDrawer=SwapIn(aDrawer);
	if (rollBackDrawer)
		{
		rollBack->iArray = this;
		rollBack->iDrawer = rollBackDrawer;
		return KErrNone;
		}
	else
		{
		__ASSERT_DEBUG(0,Panic(EWsGraphicDrawerPanicBadArgument));
		return KErrNotFound;
		}
	}

/** Replaces the drawer with the existing Id with this newer one.  
	Pushes a transactional cleanup item to restore the previous drawer. 
	CommitP() must be called on the returned pointer to release resources owned by the cleanup item,
	unless a Leave occurs.
	@param 	 aDrawer	the drawer to add
	@return		cleanup record
*/
EXPORT_C CWsGraphicDrawerArray::XRollBackBase* CWsGraphicDrawerArray::SwapTLC(CWsGraphicDrawer* aDrawer)
	{
	if (!aDrawer)
		User::Leave(KErrArgument);
	XSwapRollBack* rollBack = new(ELeave) XSwapRollBack;
	CleanupStack::PushL(TCleanupItem(XSwapRollBack::RollBackD,rollBack));
	CWsGraphicDrawer* rollBackDrawer=SwapIn(aDrawer);
	if (!rollBackDrawer)
		User::Leave(KErrNotFound);
	rollBack->iDrawer = rollBackDrawer;
	rollBack->iArray = this;
	return rollBack;
	}

/** Replaces the drawer with the existing Id with this newer one. No cleanup - no leak 
	@param 	 aDrawer	the drawer to add
	@return		error code if the Swap did not take place
*/
EXPORT_C TInt CWsGraphicDrawerArray::Swap(CWsGraphicDrawer* aDrawer)
	{
	if (!aDrawer)
		return KErrArgument;
	CWsGraphicDrawer* oldDrawer=SwapIn(aDrawer);
	if (!oldDrawer)
		return KErrNotFound;
	else
		return KErrNone;
	}
	
/**	Removes the cleanup record after a SwapTLC or AddTLC operation,
	removing the opportunity to back out of the operation.
	Note that SwapTLC, AddTLC, and RemoveTLC cleanup operations can be stacked and should be committed 
	in the opposite order. They cannot be safely intermingled with non-transactional operations.
	This method can be safely called with a NULL parameter indicating a failed operation.
	@param aRollBack	handle to rollback information that will be discarded.
**/
EXPORT_C void  CWsGraphicDrawerArray::CommitP(CWsGraphicDrawerArray::XRollBackBase* aRollBack)
	{
	if (aRollBack)
		{
		CleanupStack::Pop(aRollBack);
		delete aRollBack;
		}
	}

/** Removes the specified drawer from the array.
	Pushes a transactional cleanup item to restore the previous drawer.
	@note 	The array should not be Compressed() during the transaction period 
			to ensure that the RollBack operation will always succeed.
	@param aId the ID of the drawer to remove
	@return	cleanup record
*/
EXPORT_C CWsGraphicDrawerArray::XRollBackBase*  CWsGraphicDrawerArray::RemoveTLC(const TGraphicDrawerId& aId)
	{
	XRemoveRollBack* rollBack = new(ELeave) XRemoveRollBack;
	CleanupStack::PushL(TCleanupItem(XRemoveRollBack::RollBackD,rollBack));
	const TInt idx = IndexOf(aId);
	if(0 > idx)
		{
		User::Leave(idx);
		}
	rollBack->iDrawer=iArray[idx].iDrawer;
	iArray.Remove(idx);
	rollBack->iArray=this;
	return rollBack;
	}
	
/** Removes the specified drawer from the array
	@param aId the ID of the drawer to remove
	@return KErrNone if the drawer was removed, KErrNotFound if the drawer was not in the array
*/
EXPORT_C TInt CWsGraphicDrawerArray::Remove(const TGraphicDrawerId& aId)
	{
	const TInt idx = IndexOf(aId);
	if(0 <= idx)
		{
		iArray.Remove(idx);
		return KErrNone;
		}
	return idx;
	}
	
EXPORT_C TInt CWsGraphicDrawerArray::RemoveAndDestroy(const TGraphicDrawerId& aId)
/** Removes and deletes the specified drawer from the array
	@param aId the ID of the drawer to remove and delete
	@return KErrNone if the drawer was removed and deleted, KErrNotFound if the drawer was not in the array
*/	{
	const TInt idx = IndexOf(aId);
	if(0 <= idx)
		{
		delete iArray[idx].iDrawer;
		iArray.Remove(idx);
		return KErrNone;
		}
	return idx;
	}

/** Removes all drawers from the array which are owned by the specified client session
	@param aOwner the client session that owns the drawers to be removed
	@return the number of drawers that were removed
*/
EXPORT_C TInt CWsGraphicDrawerArray::RemoveAll(const MWsClient& aOwner)
	{
	TInt removed = 0;
	TInt count = iArray.Count();
	TInt i = 0;
	while(i < count)
		{
		if(iArray[i].iDrawer && (&(iArray[i].iDrawer->Owner()) == &aOwner))
			{
			iArray.Remove(i);
			count--;
			removed++;
			}
		else
			{
			i++;
			}
		}
	return removed;
	}

/** Removes and deletes all drawers from the array which are owned by the specified client session
	@param aOwner the client session that owns the drawers to be removed and deleted
	@return the number of drawers that were removed and deleted
*/
EXPORT_C TInt CWsGraphicDrawerArray::RemoveAndDestroyAll(const MWsClient& aOwner)
	{
	TInt removed = 0;
	TInt count = iArray.Count();
	TInt i = 0;
	while(i < count)
		{
		if(iArray[i].iDrawer && (&(iArray[i].iDrawer->Owner()) == &aOwner))
			{
			delete iArray[i].iDrawer;
			iArray.Remove(i);
			count--;
			removed++;
			}
		else
			{
			i++;
			}
		}
	return removed;
	}

EXPORT_C const CWsGraphicDrawer* CWsGraphicDrawerArray::ResolveGraphic(const TGraphicDrawerId& aId) const
/** Find a graphic by it's ID
	@param aId the ID of the graphic to find
	@return the drawer if it is in the array, else NULL
*/	{
	const TInt idx = IndexOf(aId);
	if(KErrNotFound != idx)
		{
		return iArray[idx].iDrawer;
		}
	return NULL;
	}

EXPORT_C void CWsGraphicDrawerArray::Close()
/** Close the array, not destroying the drawers that it indexes
*/	{
	iArray.Close();
	}

EXPORT_C void CWsGraphicDrawerArray::ResetAndDestroy()
/** Reset the array, destroying the drawers that it indexes.
Only the 'owner' array should call this
*/	{
	const TInt count = iArray.Count();
	for(TInt i=0; i<count; i++)
		{
		delete iArray[i].iDrawer;
		}
	iArray.Reset();
	}
	
EXPORT_C TBool CWsGraphicDrawerArray::IsEmpty() const
/** @return ETrue if the array contains no drawers
*/	{
	return !iArray.Count();
	}
	
TInt CWsGraphicDrawerArray::IndexOf(const TGraphicDrawerId& aId) const
/** @internalComponent
*/	{
	return iArray.FindInOrder(reinterpret_cast<const TGraphic&>(aId),GraphicDrawerCompare);
	}

