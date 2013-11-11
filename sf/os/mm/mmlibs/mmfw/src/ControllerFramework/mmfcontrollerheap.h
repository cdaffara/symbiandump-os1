// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFCONTROLLERHEAP_H__
#define __MMFCONTROLLERHEAP_H__

#include <e32base.h>

/*
To prevent an MMF client with multiple controllers running out of chunks, all
controllers for the same client are made to use the same heap.
  
This class controls that shared heap.
  
The iRefCount tracks the number of controllers using the heap.
  
When the client requests a pointer to the heap - RegisterHeap() - it is 
assumed that a controller will be created using it, so the refcount
is automatically incremented. When the controller terminates, the client 
unregisters - ReleaseHeap() - and the ref count is decreased.
When the refcount is zero (no controller threads left) the client deletes 
this class.
*/

const TInt KMMFControllerSharedHeapMaxSize = 0x1000000;//16MB

NONSHARABLE_CLASS(CMMFControllerHeap) : public CBase
	{
public:
	static CMMFControllerHeap* NewL();
	~CMMFControllerHeap();

	RHeap* RegisterHeap();
	TInt ReleaseHeap();
private:	
	void ConstructL();
private:
	RHeap*	iHeap;
	TInt	iRefCount;
	};

#endif
