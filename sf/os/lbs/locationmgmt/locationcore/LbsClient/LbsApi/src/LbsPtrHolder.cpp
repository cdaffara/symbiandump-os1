// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <lbsptrholder.h>


//-----------------------------------------------------------------------------
// CPositioningPtrHolder
//-----------------------------------------------------------------------------

EXPORT_C CPositioningPtrHolder* CPositioningPtrHolder::NewL(TInt aNumberOfPtrs,
                                                            TInt aNumberOfPtrCs)
/**
 * Allocates and constructs a <code>CPositioningPtrHolder</code> object.
 *
 * The function leaves if there is insufficient memory.
 *
@param aNumberOfPtrs is the number of modifiable pointer descriptors to
 * create. These are used for asynchronous 'get' requests.
 * @param aNumberOfPtrCs is the number of read-only pointer descriptors to
 * create. These are used for asynchronous 'set' requests.
 * @return a newly created CPositioningPtrHolder object.
 */
	{
	CPositioningPtrHolder* self = new (ELeave) CPositioningPtrHolder;
	CleanupStack::PushL(self);
	self->ConstructL(aNumberOfPtrs, aNumberOfPtrCs);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CPositioningPtrHolder::ConstructL(TInt aNumberOfPtrs,
                                                TInt aNumberOfPtrCs)
/**
 * Leaving constructor for CPositioningPtrHolder.

@param aNumberOfPtrs is the number of modifiable pointer descriptors to
create. These are used for asynchronous 'get' requests.
@param aNumberOfPtrCs is the number of read-only pointer descriptors to
create. These are used for asynchronous 'set' requests.
 */
	{
	TPtr8 ptr(NULL, 0);
	TInt i;
	for (i = 0; i < aNumberOfPtrs; i++)
		{
		User::LeaveIfError(iPtrArray.Append(ptr));
		}

	TPtrC8 ptrC(NULL, 0);
	for (i = 0; i < aNumberOfPtrCs; i++)
		{
		User::LeaveIfError(iPtrCArray.Append(ptrC));
		}
	}

EXPORT_C CPositioningPtrHolder::~CPositioningPtrHolder()
/**
Destructor for CPositioningPtrHolder.

*/
	{
	iPtrCArray.Close();
	iPtrArray.Close();
	}

EXPORT_C TPtr8& CPositioningPtrHolder::Ptr(TInt aIndex)
/**
An accessor for the modifiable pointer descriptors.

@param aIndex specifies the descriptor to return.
@return a reference to one of our contained pointer descriptors.
 */
	{
	return iPtrArray[aIndex];
	}

EXPORT_C TPtrC8& CPositioningPtrHolder::PtrC(TInt aIndex)
/**
 * An accessor for the read-only pointer descriptors.

@param aIndex specifies the descriptor to return.
@return a reference to one of our contained pointer descriptors.
 */
	{
	return iPtrCArray[aIndex];
	}

//-----------------------------------------------------------------------------
// CServerPositioningPtrHolder
//-----------------------------------------------------------------------------

EXPORT_C CServerPositioningPtrHolder* CServerPositioningPtrHolder::NewL(TInt aNumberOfPtrs,
                                                                        TInt aNumberOfPtrCs)
/**
Allocates and constructs a <code>CServerPositioningPtrHolder</code> object.

The function leaves if there is insufficient memory.

@param aNumberOfPtrs is the number of modifiable pointer descriptors to
create. These are used for asynchronous 'get' requests.
@param aNumberOfPtrCs is the number of read-only pointer descriptors to
create. These are used for asynchronous 'set' requests.
@return a newly created CServerPositioningPtrHolder object.
 */
	{
	CServerPositioningPtrHolder* self = new (ELeave) CServerPositioningPtrHolder;
	CleanupStack::PushL(self);
	self->ConstructL(aNumberOfPtrs, aNumberOfPtrCs);
	CleanupStack::Pop(self);
	return self;
	}
