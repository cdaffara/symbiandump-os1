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
//

#ifndef AGGREGATE_H_
#define AGGREGATE_H_

#include <e32base.h>

const TInt KMaxNumItems = 6;

class CAggregateCollection;

/** These are individual items that need to comple and are awaiting completion of their TRequestStatus as
	when they complete they let the collection know that they have complteted and allow the collection object to take
	appropriate action. Objects of this class are created and then owned by the CAgregateCollection. They are then
	deleted when the CAgregateCollection object is destroyed (hence the protected destructor) 
*/
NONSHARABLE_CLASS(CAggregateItem) : public CActive
	{
	 friend class CAggregateCollection;
public:
	void Start();
	TInt ReturnCode();
protected:
	static CAggregateItem *NewL(CAggregateCollection* aColl);
	CAggregateItem(CAggregateCollection* aColl);
	~CAggregateItem();
	virtual void RunL();
	virtual void DoCancel();
private:
	CAggregateCollection* iCollection;
	TBool iIsAdded;
	};
	
/** This is a collection of CAggregateItems. The basic use of this class is to allow for waiting for completion of more than
	one TRequestStatus. This is does by creating a collection of active objects with iStatuses to wait upon. These need to be 
	used in some asynchronous call. and the control then needs to be given back to the active scheduler.
	The object will either wait for the completion of any of the items or all of the items before compltetion some
	TRequestStatus. The decision on whether to wait for any or whether to wait for all set up on construction
*/ 	
NONSHARABLE_CLASS(CAggregateCollection): public CBase
	{
	friend class CAggregateItem;
public:
	enum TType {EAny,EAll};
	static CAggregateCollection* NewL(TRequestStatus &aStatus,TType aType);
	~CAggregateCollection();
	CAggregateItem* GetNewItemL();
	void Kick();
private:
	CAggregateCollection(TRequestStatus &aStatus,TType aType);
	void UpdateCompletion();
	TRequestStatus& iStatusToComplete;
	CAggregateItem* iItems[KMaxNumItems];
	TInt iNumItems;
	TInt iNumItemsCompleted; //The number of CAggregateItems Completed
	TType iType;
	};


#endif


