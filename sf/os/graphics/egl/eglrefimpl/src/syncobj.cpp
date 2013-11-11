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

CEglSync::CEglSync(CEglDisplay& aDisplay):
    iDisplay(aDisplay),
    iType(EGL_SYNC_REUSABLE_KHR),
    iStatus(EGL_UNSIGNALED_KHR)
    {
    }

CEglSync::~CEglSync()
    {
    iCondVar.Close();
    iMutex.Close();
    }

TInt CEglSync::Construct()
    {
    TInt err = iMutex.CreateLocal();
    if (err != KErrNone)
        {
        return err;
        }

    err = iCondVar.CreateLocal();
    if (err != KErrNone)
        {
        return err;
        }
    
    err = iDisplay.RegisterSyncObj(*this);
    if (err != KErrNone)
        {
        return err;
        }

    iRefCount = 1;
    return KErrNone;
    }

CEglSync* CEglSync::Create(CEglDisplay& aDisplay)
    {
    // sync object will be allocated in the EGL shared heap and added to sync object list
    // we need to switch current heap to EGL shared heap
    //
    RHeap* callerHeap = User::SwitchHeap(&aDisplay.Heap());

    CEglSync* syncObj = new CEglSync(aDisplay);
    if (!syncObj)
        {
        User::SwitchHeap(callerHeap);
        return NULL;
        }
    
    const TInt err = syncObj->Construct();
    if (err != KErrNone)
        {
        delete syncObj;
        User::SwitchHeap(callerHeap);
        return NULL;
        }
    
    User::SwitchHeap(callerHeap);
    return syncObj;
    }

void CEglSync::Destroy()
    {
    // multiple calls to Destroy() is not allowed, it's either coming from eglDestroySyncKHR or eglTerminate
    //
    __ASSERT_DEBUG(!iIsDestroyed, User::Panic(KEglPanicCategory, EEglPanicSyncObjHasBeenDestroyed));
    
    iIsDestroyed = ETrue;

    // wake up all waiting threads
    iCondVar.Broadcast();

    // always remove sync obj from hash map when it is destroyed, the actual deletion will be done from Close(),
    // which can happen when eglClientWaitSyncKHR is called by user
    RHeap* callerHeap = User::SwitchHeap(&iDisplay.Heap());
    iDisplay.UnregisterSyncObj(*this);
    User::SwitchHeap(callerHeap);
    
    // decrement refcount for this sync obj, it will delete the object if refcount is zero
    Close();
    }

void CEglSync::Close()
    {
    if (--iRefCount == 0)
        {
        // we're here either from Destroy() or eglClientWaitSyncKHR
        RHeap* callerHeap = User::SwitchHeap(&iDisplay.Heap());
        delete this;
        User::SwitchHeap(callerHeap);
        }
    }

void CEglSync::Signal(EGLenum aMode)
    {
    iMutex.Wait();
    if (iStatus != aMode)
        {
        iStatus = aMode;
        if (iStatus == EGL_SIGNALED_KHR)
            {
            iCondVar.Broadcast();
            }
        }
    iMutex.Signal();
    }

EGLint CEglSync::Wait(EGLTimeKHR aTimeOut)
    {
    // driver display lock is not held when we're about to enter block wait on condition variable
    // we use sync object mutex to synchronise threads access from this point until end of this function
    iMutex.Wait();
    EGLint errCode = EGL_CONDITION_SATISFIED_KHR;
    
    if (iStatus == EGL_UNSIGNALED_KHR)
        {
        switch(aTimeOut) 
            {
            case EGL_FOREVER_KHR:
                {
                const TInt res = iCondVar.Wait(iMutex); 
                //we do not expect to fail here
                __ASSERT_DEBUG(res == KErrNone, User::Panic(KEglPanicCategory, EEglPanicCondVarWaitFail));
                break;
                }
            case 0:
                {
                //by setting this we notify the caller that the sync object is in unsignaled state
                errCode = EGL_TIMEOUT_EXPIRED_KHR; 
                break;
                }
            default:
                {
                // Since the supported range of timeout at function RCondVar::TimedWait(mutex, timeout) 
                // is 0 to KMaxTInt, looping mechanism below is used to support 64bit timeout.      
                //
                TInt res = KErrTimedOut;
                for(TInt64 timeoutMicroseconds = aTimeOut/1000; (res == KErrTimedOut) && (timeoutMicroseconds > 0); timeoutMicroseconds -= KMaxTInt)
                    {
                    res = iCondVar.TimedWait(iMutex, (timeoutMicroseconds > KMaxTInt?KMaxTInt:timeoutMicroseconds));
                    //we do not expect to fail here
                    __ASSERT_DEBUG(res == KErrNone || res == KErrTimedOut, User::Panic(KEglPanicCategory, EEglPanicCondVarTimedWaitFail));
                    }
                if(res == KErrTimedOut)
                    {
                    errCode = EGL_TIMEOUT_EXPIRED_KHR;
                    }
                break;
                }
            }
        }

    iMutex.Signal();
    return errCode;
    }
