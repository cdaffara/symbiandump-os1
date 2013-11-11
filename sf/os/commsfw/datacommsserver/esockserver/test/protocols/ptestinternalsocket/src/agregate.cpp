// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Aggregate.cpp
// 
//

#include "agregate.h"


//----------------------------------------------------------------------------------------------------------------
//	CAggregateItem
//----------------------------------------------------------------------------------------------------------------
	
/**	Constructs a new CAggregateItem object. This is private becuase
	it should only be called from a CAggregateCollection class
	@param coll a pointer to the collection class
	@return the newly created item
 */ 
CAggregateItem* CAggregateItem::NewL(CAggregateCollection* aColl)
	{
	CAggregateItem* ret = new (ELeave) CAggregateItem(aColl);
	return ret;
	}
	
/**	Constructor for new CAggregateItem.
	@param coll a pointer to the collection class
 */	
CAggregateItem::CAggregateItem(CAggregateCollection *aColl) :
	CActive(EPriorityNormal),
	iCollection(aColl),
	iIsAdded(EFalse)
	{	
	}
/**	Destructor for new CAggregateItem.
	The destructor is private because only a CAggregateCollection should have the right to delete it
 */	
CAggregateItem::~CAggregateItem()
	{
	if (iIsAdded)
		{
		Deque();
		}
	}
	
/**	Returns what was returned on the completion of the objects iStatus
  	@return the TRequestStatus return value
  */
TInt CAggregateItem::ReturnCode()
	{
	return iStatus.Int();
	}
	
/** Places the object on the active scheduler and does all the necessary stuff to allow completion to happen
 */
void CAggregateItem::Start()
	{
	CActiveScheduler::Add(this);
	iIsAdded = ETrue;
	SetActive();
	}
	
void CAggregateItem::RunL()
	{
	Deque();
	iIsAdded = EFalse;
	iCollection->UpdateCompletion();
	}
	
void CAggregateItem::DoCancel()
	{
	
	}
	
//----------------------------------------------------------------------------------------------------------------
//	CAggregateCollection
//----------------------------------------------------------------------------------------------------------------

/**	Constructor for a CAggregateCollection class
	@param status The TRequestStatus that will be completed when either one or all items complete
	@param type whether the completion of the above status should happen on completion of the first item or
	whether we need to wait for completion of all items
 */
CAggregateCollection* CAggregateCollection::NewL(TRequestStatus &aStatus,TType aType)
	{
	CAggregateCollection* ret = new (ELeave) CAggregateCollection(aStatus,aType);
	return ret;
	}
/**	Constructor for a CAggregateCollection class
	@param status The TRequestStatus that will be completed when either one or all items complete
	@param type whether the completion of the above status should happen on completion of the first item or
	whether we need to wait for completion of all items
 */
CAggregateCollection::CAggregateCollection(TRequestStatus &aStatus,TType aType) :
	iStatusToComplete(aStatus),
	iType(aType)
	{	
	}
	
CAggregateCollection::~CAggregateCollection()
	{
	while (iNumItems > 0)
		{
		delete iItems[--iNumItems];
		}
	}

/** Creates a new CAggregateItem and passes the item back to the caller
	The CAggregateItem has the same lifetime as the CAggregateCollection and cannot be deleted independently
	@return the new CAggregateItem
 */		
CAggregateItem* CAggregateCollection::GetNewItemL()
	{
	CAggregateItem* ret = CAggregateItem::NewL(this);
	CleanupStack::PushL(ret);
	if (iNumItems == KMaxNumItems - 1)
		{
		User::Leave(KErrOverflow);
		}
	iItems[iNumItems++] = ret;
	CleanupStack::Pop(ret);
	return ret;
	}
	
/**	This is called everytime a CActiveItem is completed. It is responsible for incrementing the objects completed count
	as well as completing the TRequestStatus handed to it on construction.
	The above TRequestStatus is completed on the first completion of one of its item if type = EAny and
	only after completion of all the items if type = EAll
*/
void CAggregateCollection::UpdateCompletion()
	{
	iNumItemsCompleted++;
	if (iType == EAny || iNumItemsCompleted == iNumItems)
		{
		TRequestStatus* stat = &iStatusToComplete;
		User::RequestComplete(stat,KErrNone); 
		}
	}
	
void CAggregateCollection::Kick()
	{
	iStatusToComplete = KRequestPending;
	}


	


	

