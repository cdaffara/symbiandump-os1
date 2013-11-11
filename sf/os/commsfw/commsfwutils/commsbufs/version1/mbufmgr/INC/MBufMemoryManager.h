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
*/

#ifndef MBUFMEMORYMANAGER_H_
#define MBUFMEMORYMANAGER_H_

// The allocation or real memory for a CMBufPool
class CMBufMemoryManager : public CBase
	{
public:
	static CMBufMemoryManager* NewL(TInt aMaxHeapSize);
	~CMBufMemoryManager();
	
	TAny* Alloc(TInt aSize);
	void Free(TAny* aPtr);
	TInt AllocBytes();

private:

	CMBufMemoryManager();
	void ConstructL(TInt aMaxHeapSize);

	RHeap* iHeap;
	TUint iLogger;			// alternative to __FLOG_DECLARATION_MEMBER, refer MBuf_Log.h notes

	};


#endif /*MBUFMEMORYMANAGER_H_*/
