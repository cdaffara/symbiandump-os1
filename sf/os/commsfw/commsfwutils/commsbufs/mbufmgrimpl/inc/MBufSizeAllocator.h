/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* MMBufSizeAllocator decleration
* 
*
*/

/**
@file

@internalTechnology

*/

#ifndef MBUF_SIZE_ALLOCATOR_H
#define MBUF_SIZE_ALLOCATOR_H

#include <e32def.h>     // for std types

// interface for the manipulation of supported mbuf sizes
class MMBufSizeAllocator
/**
Populate a free pool of specified size with RMBuf's.  Each RMBuf in this free pool will have the same size.

@internalTechnology
@released
*/
	{
public:
	virtual void AddL(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aGrowthThreshold, TInt aPoolCeiling) = 0;
	virtual TInt Commit() = 0;
	};

#endif  // MBUF_SIZE_ALLOCATOR_H
