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
#include <EGL/egluids.hrh>
#include <e32uid.h>
#include <WF/openwfcuids.hrh>

// OpenWF Composition library
//
_LIT(KOwfDllName, "libWFC.dll");
const TUid KOwfDllUid1 = {KDynamicLibraryUidValue};
const TUid KOwfDllUid2 = {KUidSharedDllUidValue};
const TUid KOwfDllUid3 = {KUidOpenWfcDllUidValue};
const TUidType KOwfDllUidType = TUidType(KOwfDllUid1, KOwfDllUid2, KOwfDllUid3);

// EGL_KHR_reusable_sync extensions API
// Declare here so their addresses can be used to build extensions table
//
EGLSyncKHR eglCreateSyncKHR(EGLDisplay aDisplay, EGLenum aType, const EGLint *aAttribList);
EGLBoolean eglDestroySyncKHR(EGLDisplay aDisplay, EGLSyncKHR aSync);
EGLint eglClientWaitSyncKHR(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLint aFlags, EGLTimeKHR aTimeout);
EGLBoolean eglSignalSyncKHR(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLenum aMode);
EGLBoolean eglGetSyncAttribKHR(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLint aAttribute, EGLint *aValue);

// Private extensions
//
EGLint egl_Private_SignalSyncNOK(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLenum aMode);

// Extensions for shared heap OOM test
#ifdef _DEBUG
void egliDebugHeapMarkStart();
EGLint egliDebugHeapMarkEnd(EGLint aCount);
void egliDebugSetBurstAllocFail(EGLenum aType, EGLint aRate, EGLint aBurst);
#endif //_DEBUG

struct TEglNameProcRecord
    {
    char* iName;
    TFuncPtrEglProc iProc;
    };

#define MAKE_NAME_PROC_RECORD(f)    {#f,reinterpret_cast<TFuncPtrEglProc>(f)}

const TEglNameProcRecord KEglExtensionFuncs[] = {
    MAKE_NAME_PROC_RECORD(eglCreateSyncKHR),
    MAKE_NAME_PROC_RECORD(eglDestroySyncKHR),
    MAKE_NAME_PROC_RECORD(eglClientWaitSyncKHR),
    MAKE_NAME_PROC_RECORD(eglSignalSyncKHR),
    MAKE_NAME_PROC_RECORD(eglGetSyncAttribKHR),
    MAKE_NAME_PROC_RECORD(egl_Private_SignalSyncNOK) 
#ifdef _DEBUG
    ,
    MAKE_NAME_PROC_RECORD(egliDebugHeapMarkStart),
    MAKE_NAME_PROC_RECORD(egliDebugHeapMarkEnd),
    MAKE_NAME_PROC_RECORD(egliDebugSetBurstAllocFail)
#endif //_DEBUG
};

// Use common EglPls() function to hide difference between WINS and target in dealing with PLS
//
#ifdef __WINS__
#include <pls.h>
static inline XEglPls* EglPls()
	{
	const TUid KUidEglDll = {KUidEGLDllUidValue};
	return Pls<XEglPls>(KUidEglDll);
	}
#else
static XEglPls TheEglPls;
static inline XEglPls* EglPls()
	{
	return &TheEglPls;
	}
#endif


// Macros for placement new and delete of shared objects
//
#define EGL_PLACEMENT_NEW(heap, obj, klass)         klass* obj = NULL; \
                                                    TAny* buf = (heap).AllocZ(sizeof(klass)); \
                                                    if (buf) \
                                                        { \
                                                        obj = new(buf) klass((heap)); \
                                                        }

#define EGL_PLACEMENT_DELETE(heap, obj, klass)      obj->~klass(); \
                                                    (heap).Free(obj);

///////////////////////////////////////////////////////////////////////////////
// PLS
///////////////////////////////////////////////////////////////////////////////

XEglPls::XEglPls()
	{
	iError = iLock.CreateLocal();
	iDriver = NULL;
	}

XEglPls::~XEglPls()
	{
	iLock.Close();
	}

///////////////////////////////////////////////////////////////////////////////
// EGL driver
///////////////////////////////////////////////////////////////////////////////

CEglDriver::CEglDriver(RHeap& aHeap):
	iHeap(aHeap)
	{
	}

CEglDriver::~CEglDriver()
	{
	// EGL display was allocated on shared heap using placement new
	if (iDisplay)
		{
		EGL_PLACEMENT_DELETE(iHeap, iDisplay, CEglDisplay)
		}
	
	iOwfLib.Close();
	iLock.Close();
	}

TInt CEglDriver::Open()
	{
	// we're in trouble if mutex creation failed during DLL initialisation
	XEglPls& pls = *EglPls();
	if (pls.iError != KErrNone)
		{
		return pls.iError;
		}
	
	pls.Lock();

	if (pls.iDriver)
		{
		++(pls.iDriver->iRefCount);
		pls.Unlock();
		return KErrNone;
		}
	
	// create shared heap in a local chunk to allow access to shared objects from any threads within Driver
	// allow heap to grow from 256 bytes up to 1MB
	const TInt KMaxHeapSize = 0x100000;
	const TInt KWordAlign = 4;
	RHeap* heap = User::ChunkHeap(NULL, KMinHeapSize, KMaxHeapSize, KMinHeapGrowBy, KWordAlign);
	if (!heap)
		{
		pls.Unlock();
		return KErrNoMemory;
		}
	
	EGL_PLACEMENT_NEW(*heap, drv, CEglDriver)
	if (!drv)
		{
		heap->Close();
		pls.Unlock();
		return KErrNoMemory;
		}
	
	const TInt err = drv->Construct();
	if (err != KErrNone)
		{
		EGL_PLACEMENT_DELETE(*heap, drv, CEglDriver)
		heap->Close();
		pls.Unlock();
		
		return err;
		}
	
	pls.iDriver = drv;
	pls.iDriver->iRefCount = 1;

	pls.Unlock();
	
	return KErrNone;
	}

void CEglDriver::Close()
	{
	XEglPls& pls = *EglPls();
	// must never happen if CEglDriver::Open() succeed
	__ASSERT_DEBUG(pls.iError == KErrNone, User::Panic(KEglPanicCategory, EEglPanicPlsMutexError));

	pls.Lock();

	if (pls.iDriver)
		{
		if (--(pls.iDriver->iRefCount) == 0)
			{
			// copy shared heap pointer out as we're about to destroy the driver
			RHeap& heap = pls.iDriver->iHeap;
			
			// driver was allocated on shared heap using placement new
			EGL_PLACEMENT_DELETE(heap, pls.iDriver, CEglDriver)
			pls.iDriver = NULL;
			heap.Close();
			}
		}

	pls.Unlock();
	}

CEglDriver* CEglDriver::GetDriver()
    {
    XEglPls& pls = *EglPls();
    return pls.iDriver;    
    }

// called when lock is held
//
TInt CEglDriver::Construct()
	{
	TInt err = iLock.CreateLocal();
	if (err != KErrNone)
		{
		return err;
		}

	// create default EGL display as part of driver initialisation, it will be destroyed when the driver is destroyed
	// and recreated when driver is reinitialised
    EGL_PLACEMENT_NEW(iHeap, disp, CEglDisplay)
    if (!disp)
        {
        return KErrNoMemory;
        }

    iDisplay = disp;
	
	// for security reason, use TUidType when loading OWF library to make sure it is not a spoofed DLL with the same name
	err = iOwfLib.Load(KOwfDllName, KOwfDllUidType);
	// allow initialisation to proceed even if OWF library is not found
	if (err == KErrNone)
	    {
        // promote library handle from thread-relative to process-wide to allow closing from any thread
        RLibrary dup = iOwfLib;
        err = iOwfLib.Duplicate(RThread());
        if (err != KErrNone)
            {
            return err;
            }
        // if Duplicate succeed, it will overwrite iOwfLib with process-wide handle
        // we need to close the original thread-relative handle
        dup.Close();
    
        // In SymbianOS implementation of Khronos API, ordinal #1 is always reserved for internal implementation use as defined in
        // SymbianOS standard DEF files for that API. This DEF file ensures BC among different implementations of that API.
        // In OWF case, ordinal #1 refers to GetExtensionFunctionTable, a function which will return a table of function pointer.
        // One of them is a function that implement GetProcAddress to retrieve OWF extension functions.
        //
        const TInt KOwfGetFuncTableOrdinalNum = 1;
        typedef void (*TFuncPtrOwfGetFuncTable)(TOwfFuncTable*);
        
        TFuncPtrOwfGetFuncTable owfGetFuncTable = reinterpret_cast<TFuncPtrOwfGetFuncTable>(iOwfLib.Lookup(KOwfGetFuncTableOrdinalNum));
        if (!owfGetFuncTable)
            {
            return KErrNotFound;
            }
    
        owfGetFuncTable(&iOwfFuncTable);
    
        if (!iOwfFuncTable.iOwfGetProcAddress)
            {
            return KErrNotSupported;
            }
	    }

	return  KErrNone;
	}

TFuncPtrEglProc CEglDriver::GetProcAddress(const char* aName) const
   {
   // start with EGL extension functions
   //
   TPtrC8 procName(reinterpret_cast<const TUint8*>(aName));
   const TInt nExts = sizeof(KEglExtensionFuncs) / sizeof(KEglExtensionFuncs[0]);
   for (TInt idx=0; idx<nExts; ++idx)
       {
       if (procName == TPtrC8(reinterpret_cast<TUint8*>(KEglExtensionFuncs[idx].iName)))
           {
           return KEglExtensionFuncs[idx].iProc;
           }
       }
   
   // not an EGL extension, pass it on to OWF
   //
   return iOwfFuncTable.iOwfGetProcAddress ? iOwfFuncTable.iOwfGetProcAddress(aName) : NULL;
   }
