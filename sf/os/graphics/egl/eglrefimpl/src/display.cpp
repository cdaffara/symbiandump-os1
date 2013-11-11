// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference EGL implementation to support EGL sync objects and OpenWF extensions

#include "eglprivate.h"

CEglDisplay::CEglDisplay(RHeap& aHeap):
    iHandle(KEglDefaultDisplayHandle),
	iHeap(aHeap)
	{
	}

CEglDisplay::~CEglDisplay()
	{
	Terminate();
	
	if (iSyncObjList)
	    {
        __ASSERT_DEBUG(iSyncObjList->Count() == 0, User::Panic(KEglPanicCategory, EEglPanicOutstandingSyncObj));

        // we're about to free hash map and its underlying memory, make sure EGL shared heap is used
        RHeap* callerHeap = User::SwitchHeap(&iHeap);
        iSyncObjList->Close();
        delete iSyncObjList;
        User::SwitchHeap(callerHeap);
	    }
	}

TInt CEglDisplay::Initialize()
	{
	TInt err = KErrNone;
	if (iIsInitialized)
		{
		return err;
		}

	// hash map is only created once, check that when re-initialising display
	if (iSyncObjList)
	    {
	    iIsInitialized = ETrue;
	    }
	else
	    {
	    // make sure underlying hash implementation memory is allocated in share heap
	    //
        RHeap* callerHeap = User::SwitchHeap(&iHeap);
        iSyncObjList = new REglSyncHashMap;
        if (iSyncObjList)
            {
            iIsInitialized = ETrue;
            }
        else
            {
            err = KErrNoMemory;
            }
        User::SwitchHeap(callerHeap);        
	    }
	
	return err;
	}

void CEglDisplay::Terminate()
	{
	if (!iIsInitialized)
		{
		return;
		}

    REglSyncHashMap::TIter iter(*iSyncObjList);
    // iter begin at index -1, must move it once to get to first item
    while (iter.NextKey())
        {
        CEglSync** ppSyncObj = iter.CurrentValue();
        __ASSERT_DEBUG(ppSyncObj, User::Panic(KEglPanicCategory, EEglPanicInvalidSyncObj));
        
        // In a regular iteration, we should not modify the hash map itself while it is being iterated.
        // However, this is not a regular iteration, we need to remove destroyed object from
        // this hash map without using additional memory e.g. copying destroyed object key temporarily.
        // The reason we do not want to do that is because allocating memory can fail and Terminate
        // must not fail.
        CEglSync* syncObj = *ppSyncObj;
        
        // Destroy() will switch current heap to the EGL shared heap and restore it back.
        // It will also remove the sync object from hash map, but not necessarily delete it (because
        // some other threads may still use it)
        syncObj->Destroy();
        iter.Reset();
        }
    
	iIsInitialized = EFalse;
	}

CEglSync* CEglDisplay::FindSyncObj(EGLSyncKHR aObj) const
	{
	__ASSERT_DEBUG(iSyncObjList, User::Panic(KEglPanicCategory, EEglPanicDisplayStateInvalid));

	const TInt key = reinterpret_cast<TInt>(aObj);
	CEglSync** ppso = iSyncObjList->Find(key);
	CEglSync* syncObj = ppso ? *ppso : NULL;
	
	return syncObj;
	}

CEglSync* CEglDisplay::CreateSyncObj()
	{
    __ASSERT_DEBUG(iSyncObjList, User::Panic(KEglPanicCategory, EEglPanicDisplayStateInvalid));
    
    CEglSync* syncObj = CEglSync::Create(*this);

    return syncObj;
	}

TInt CEglDisplay::DestroySyncObj(EGLSyncKHR aSyncObj)
	{
    __ASSERT_DEBUG(iSyncObjList, User::Panic(KEglPanicCategory, EEglPanicDisplayStateInvalid));

    CEglSync* syncObj = reinterpret_cast<CEglSync*>(aSyncObj);
    const TInt key = reinterpret_cast<TInt>(syncObj);
    CEglSync** ppso = iSyncObjList->Find(key);
    if (!ppso)
        {
        return KErrNotFound;
        }

    // Destroy() will not delete sync obj or remove it from the list if it is still being used in other threads
    // if there is no more reference to this sync obj when Destroy() is called, it will delete the object and remove it from the list
    __ASSERT_DEBUG(syncObj == *ppso, User::Panic(KEglPanicCategory, EEglPanicInvalidSyncObj));
    syncObj->Destroy();

    return KErrNone;
	}

TInt CEglDisplay::RegisterSyncObj(CEglSync& aSyncObj)
    {
    __ASSERT_DEBUG(&iHeap == &User::Heap(), User::Panic(KEglPanicCategory, EEglPanicInvalidHeap));
    __ASSERT_DEBUG(iSyncObjList, User::Panic(KEglPanicCategory, EEglPanicDisplayStateInvalid));
    
    const TInt key = reinterpret_cast<TInt>(&aSyncObj);
    const TInt err = iSyncObjList->Insert(key, &aSyncObj);

    return err;
    }

void CEglDisplay::UnregisterSyncObj(CEglSync& aSyncObj)
    {
    __ASSERT_DEBUG(&iHeap == &User::Heap(), User::Panic(KEglPanicCategory, EEglPanicInvalidHeap));
    __ASSERT_DEBUG(iSyncObjList, User::Panic(KEglPanicCategory, EEglPanicDisplayStateInvalid));
    
    const TInt key = reinterpret_cast<TInt>(&aSyncObj);
    const TInt err = iSyncObjList->Remove(key);
    // the only possible error is KErrNotFound which should never happen in our case
    __ASSERT_DEBUG(err == KErrNone, User::Panic(KEglPanicCategory, EEglPanicInvalidSyncObj));
    }
