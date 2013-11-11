// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Buffer Manager for Protocols - Memory Allocation
// 
//

/**
@file
@internalTechnology

Buffer Manager for Protocols - Memory Allocation
*/

#ifndef MBUFMEMORYALLOCATOR_H_
#define MBUFMEMORYALLOCATOR_H_

// The allocation or real memory for a CMBufPool
NONSHARABLE_CLASS(CMBufMemoryAllocator) : public CBase
	{
public:
	static CMBufMemoryAllocator* NewL(TInt aMaxHeapSize);
	~CMBufMemoryAllocator();
	
	TAny* Alloc(TInt aSize);
	void Free(TAny* aPtr);
	TInt AllocBytes();	
private:

	CMBufMemoryAllocator();
	void ConstructL(TInt aMaxHeapSize);

	RHeap* iHeap;

	};


#endif /*MBUFMEMORYALLOCATOR_H_*/
