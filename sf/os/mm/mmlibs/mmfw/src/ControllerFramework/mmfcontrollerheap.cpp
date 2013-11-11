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

#include "mmfcontrollerheap.h"


CMMFControllerHeap* CMMFControllerHeap::NewL()
	{
	CMMFControllerHeap* self = new(ELeave) CMMFControllerHeap;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	
	return self;
	}

void CMMFControllerHeap::ConstructL()
	{
	TChunkHeapCreateInfo chunkSettings (KMinHeapSize, KMMFControllerSharedHeapMaxSize);
	chunkSettings.SetPaging(TChunkHeapCreateInfo::EUnpaged);
	iHeap = User::ChunkHeap(chunkSettings);
	if (iHeap == NULL)
		{
		User::Leave(KErrNoMemory);
		}

	iRefCount = 0;
	}

RHeap* CMMFControllerHeap::RegisterHeap()
	{
	iRefCount++;
	return iHeap;
	}	
	
TInt CMMFControllerHeap::ReleaseHeap()
	{
	return --iRefCount;		
	}

CMMFControllerHeap::~CMMFControllerHeap()
	{
	if( iHeap )
		{
		iHeap->Close();
		}
	}


