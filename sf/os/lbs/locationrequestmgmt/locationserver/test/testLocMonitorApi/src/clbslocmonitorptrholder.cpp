/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "clbslocmonitorptrholder.h"


//-----------------------------------------------------------------------------
// CLbsLocMonitorPtrHolder
//-----------------------------------------------------------------------------

/**
 Allocates and constructs a CLbsLocMonitorPtrHolder object.

 The function leaves if there is insufficient memory.

@internalComponent
@param aNumberOfPtrs is the number of modifiable pointer descriptors to
  create. These are used for asynchronous 'get' requests.
@param aNumberOfPtrCs is the number of read-only pointer descriptors to
  create. These are used for asynchronous 'set' requests.
@return a newly created CLbsLocMonitorPtrHolder object.
 */
CLbsLocMonitorPtrHolder* CLbsLocMonitorPtrHolder::NewL(TInt aNumberOfPtrs,
                                                            TInt aNumberOfPtrCs)
	{

	}

void CLbsLocMonitorPtrHolder::ConstructL(TInt aNumberOfPtrs,
                                                TInt aNumberOfPtrCs)

	{

	}

/**
Destructor for CLbsLocMonitorPtrHolder.

@internalComponent
*/
CLbsLocMonitorPtrHolder::~CLbsLocMonitorPtrHolder()
	{

	}

/**
An accessor for the modifiable pointer descriptors.

@internalComponent
@param aIndex specifies the descriptor to return.
@return a reference to one of our contained pointer descriptors.
 */
TPtr8& CLbsLocMonitorPtrHolder::Ptr(TInt aIndex)
	{

	}

/**
An accessor for the read-only pointer descriptors.

@internalComponent
@param aIndex specifies the descriptor to return.
@return a reference to one of our contained pointer descriptors.
*/
TPtrC8& CLbsLocMonitorPtrHolder::PtrC(TInt aIndex)
	{

	}


