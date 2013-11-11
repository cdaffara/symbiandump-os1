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

#ifndef EGLPRIVATE_H
#define EGLPRIVATE_H

#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <EGL/egl.h>

#ifndef EGL_VERSION_1_4
#error Reference EGL requires at least EGL version 1.4 headers
#endif

// Panic category and codes
//
_LIT(KEglPanicCategory, "EGL");
enum TEglPanicCode
    {
    EEglPanicNotSupported = 1,
    EEglPanicPlsMutexError,
    EEglPanicDriverNull,
    EEglPanicInvalidHeap,
    EEglPanicDisplayStateInvalid,
    EEglPanicInvalidSyncObj,
    EEglPanicCondVarWaitFail,
    EEglPanicCondVarTimedWaitFail,
    EEglPanicInvalidSession,
    EEglPanicOutstandingSyncObj,
    EEglPanicSyncObjHasBeenDestroyed
    };

// Forward declarations
//
class CEglDriver;
class CEglDisplay;
class CEglThreadSession;
class CEglSync;

// Helper class to manage PLS
//
NONSHARABLE_CLASS(XEglPls)
	{
public:
	XEglPls();
	~XEglPls();
	
	inline void Lock();
	inline void Unlock();

public:
	RMutex iLock;
	TInt iError;
	CEglDriver* iDriver;
	};

// Function pointers
//
extern "C" 
{
typedef void (*TFuncPtrEglProc)(...);
}

typedef TFuncPtrEglProc (*TFuncPtrOwfGetProcAddress)(const char*);

// Function table used by OWF implementation
// The actual type and name of the structure is irrelevant here, 
// as long as its size and offset match it should be ok
//
NONSHARABLE_STRUCT(TOwfFuncTable)
    {
    TFuncPtrOwfGetProcAddress iOwfGetProcAddress;
    };

// EGL process state
//
NONSHARABLE_CLASS(CEglDriver): public CBase
	{
public:
	static TInt Open();
	static void Close();
	static CEglDriver* GetDriver();

	CEglDisplay* FindDisplay(EGLDisplay aDisplay) const;
    TFuncPtrEglProc GetProcAddress(const char* aName) const;
    inline void Lock();
    inline void Unlock();
    
#ifdef _DEBUG
    inline RHeap& Heap() const;
#endif

private:
	CEglDriver(RHeap& aHeap);
	~CEglDriver();
	TInt Construct();
	inline TBool IsDisplayValid(EGLDisplay aDisplay) const;
	
private:
	RMutex iLock;
	RHeap& iHeap;
	CEglDisplay* iDisplay;
	RLibrary iOwfLib;
	TOwfFuncTable iOwfFuncTable;
	TInt iRefCount;
	};

// Thread state and thread-specific tasks
//
NONSHARABLE_CLASS(CEglThreadSession): public CBase
	{
public:
	static CEglThreadSession* Static();
	~CEglThreadSession();
	
	// EGL APIs
	EGLint EglGetError();
	EGLDisplay EglGetDisplay(NativeDisplayType aDisplayId);
	EGLBoolean EglInitialize(EGLDisplay aDisplay, EGLint* aMajor, EGLint* aMinor);
	EGLBoolean EglTerminate(EGLDisplay aDisplay);
	const char* EglQueryString(EGLDisplay aDisplay, EGLint aName);
	TFuncPtrEglProc EglGetProcAddress(const char* aName);
	
	// EGLSync APIs
	EGLSyncKHR EglCreateSyncKhr(EGLDisplay aDisplay, EGLenum aType, const EGLint *aAttribList);
	EGLBoolean EglDestroySyncKhr(EGLDisplay aDisplay, EGLSyncKHR aSync);
	EGLint EglClientWaitSyncKhr(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLint aFlags, EGLTimeKHR aTimeout);
	EGLBoolean EglSignalSyncKhr(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLenum aMode);
	EGLBoolean EglGetSyncAttribKhr(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLint aAttribute, EGLint *aValue);
	
	// Private APIs
	EGLint EglSignalSyncInternal(EGLDisplay aDisplay, EGLSyncKHR aSync, EGLenum aMode);

	// Debug APIs
#ifdef _DEBUG
	void EglHeapMarkStart();
	EGLint EglHeapMarkEnd(EGLint aCount);
	void EglHeapSetBurstAllocFail(EGLenum aType, EGLint aRate, EGLint aBurst);
#endif //_DEBUG
	
private:
	CEglThreadSession(CEglDriver& aDriver);
    void SetError(EGLint aErrror);

private:
	CEglDriver& iDriver;
	EGLint iError;
	};


// EGLDisplay implementation
//
const EGLDisplay KEglDefaultDisplayHandle = 0x01;

NONSHARABLE_CLASS(CEglDisplay): public CBase
	{
public:
	CEglDisplay(RHeap& aHeap);
	~CEglDisplay();
	
	TInt Initialize();
	void Terminate();
	inline TBool IsInitialized() const;
	inline RHeap& Heap() const;
	inline EGLDisplay Handle() const;

	CEglSync* CreateSyncObj();
	TInt DestroySyncObj(EGLSyncKHR aSyncObj);
    CEglSync* FindSyncObj(EGLSyncKHR aSyncObj) const;

    TInt RegisterSyncObj(CEglSync& aSyncObj);
    void UnregisterSyncObj(CEglSync& aSyncObj);

private:
    typedef RHashMap<TInt,CEglSync*> REglSyncHashMap;

    EGLDisplay iHandle;
	RHeap& iHeap;
	TBool iIsInitialized;
	REglSyncHashMap* iSyncObjList;
	};

// EGLSync implementation
//
NONSHARABLE_CLASS(CEglSync): public CBase 
    {
public:
    static CEglSync* Create(CEglDisplay& aDisplay);
    void Destroy();
    inline void Open();
    void Close();

    void Signal(EGLenum aMode);
    EGLint Wait(EGLTimeKHR aTimeOut);
    inline EGLenum Type() const;
    inline EGLenum Status() const;
    inline TBool IsDestroyed() const;

private:
    CEglSync(CEglDisplay& aDisplay);
    ~CEglSync();    
    TInt Construct();

private:
    CEglDisplay& iDisplay;
    EGLenum iType;
    EGLenum iStatus;
    RMutex iMutex;
    RCondVar iCondVar;
    TInt iRefCount;     // to determine when to delete this object
    TBool iIsDestroyed; // to prevent object from being used when its handle has been explicitly destroyed by client
    };

#include "eglprivate.inl"

#endif /* EGLPRIVATE_H */
