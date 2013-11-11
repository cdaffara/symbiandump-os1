// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Test shutdown cleanup functions that should be called to destroy singletons
// before the heap is destroyed, in particular when using TEF steps.
// These methods are implemented and called from Window Server - WServ, CWsTop
// but this copy can be called by any low-level unit tests that bypass WServ   
// but need to provide the same clean-up behaviour.
// @test
// @internalTechnology
// 
// Currently, three things are cleaned up:
// 1) Singleton API mutex in OpenWF implementation
// 2) Singleton Find/Acquire mutext in Native Stream implementation
// 3) EGL thread status 
//
// Note that while these cleanups are only required by OWF-C implementations,
// the methods called should be harmless if called 
// when some other composition solution is in action.

#include <e32property.h>
#include <EGL/egl.h>

// IDs of p&s properties that optionally contain onexit callbacks
// that may be used to release singletons owned by libraries at shutdown in order to make 
// the memory leak tests work.
// By convention, the ID is the same as the UID3 of the libary.
static TBool gReleaseSingletonsOnExit = EFalse;
static const TUid KOpenWfcImplCleanupKey = {0x10286FC4};     
static const TUid KOpenWfcInteropCleanupKey = {0x10286FC5};


static void DefineOwfSingletonKey(const TUid& aSingletonKey)
/**
 * Defines a new property for a singleton key. WServ must only process 
 * singleton keys that it created to prevent a malicious process with the 
 * WriteDeviceData capability causing arbitrary functions to be executed.
 * 
 * @param   aSingeltonKey   The UID of the singleton key to define.
 */
    {
    RThread t;      
    TUid category = { t.SecureId().iId };
    RProperty prop;
    
    // Write access is restricted to THIS process
    TInt err = prop.Define( category, aSingletonKey.iUid, 
            RProperty::EByteArray, TSecurityPolicy( t.SecureId() ), 
            TSecurityPolicy( t.SecureId() ), sizeof( TCallBack ) );
    
    if ( err == KErrNone || err == KErrAlreadyExists)
        {
        TCallBack cb( NULL, NULL );
        TPckgC<TCallBack> cbPckg( cb );
        
        // Any error should cause the properties to be ignored
        err = prop.Set( category, aSingletonKey.iUid, cbPckg );    
        }      
    //We presume that if property already exists it was previously set by this test code.
    if ( err != KErrNone )
        {
        // A problem occured / the property already existed so for safety
        // the release code should be skipped.
        gReleaseSingletonsOnExit = EFalse;
        }
    
    prop.Close();
    t.Close();
    }
#define DefineOwfSingletonKeys DefineOwfSingletonKeys
/** Call this method before starting the compositor.    
 * 
 */
static void DefineOwfSingletonKeys()
    {
    // Define properties for singleton callbacks. This must only be done ONCE
    // to ensure the properties can't be hijacked.
    gReleaseSingletonsOnExit = ETrue;
    DefineOwfSingletonKey(KOpenWfcInteropCleanupKey);
    DefineOwfSingletonKey(KOpenWfcImplCleanupKey);
    }

static void DeleteOwfSingleton( const TUid& aSingletonKey )
/**
 * Deletes a singleton object that was created on WServ's main heap.
 * 
 * @pre     The ws plugins have not been unloaded.
 * @param   aSingletonKey   The UID of the singleton which correponds to an
 *                          RProperty within WServ's category.                       
 */
    {   
    if ( gReleaseSingletonsOnExit )
        {
        RThread t;
        TPckgBuf<TCallBack> cb;
        RProperty prop; 
        TInt err = prop.Get(TUid::Uid(t.SecureId().iId), aSingletonKey.iUid, cb);
        if (err == KErrNone && cb.Length() == sizeof(TCallBack) && 
                cb().iFunction && cb().iPtr == &User::Heap())
            {
            // Callback is only invoked if the heap for the singleton was the 
            // WServ heap because the WServ memory leak tests only check this
            // heap.
            cb().CallBack();
            }
        // Errors are ignored because the purpose of this function is to free
        // singletons in order top make memory leak checks pass.
        prop.Close();
        t.Close();
        }
    }
/** Call this method to destroy OWF-C singletons on shut down
 * 
 */
#define DeleteOwfSingletons DeleteOwfSingletons
static void DeleteOwfSingletons()
    {
    // Free singletons on WServ heap created by libraries. Must be called
    // BEFORE iPluginManager is deleted otherwise the library code could have
    // been unloaded.
    DeleteOwfSingleton(KOpenWfcImplCleanupKey);
    DeleteOwfSingleton(KOpenWfcInteropCleanupKey);
    /* Release any use of EGL by this thread. */
    eglReleaseThread();
    }

