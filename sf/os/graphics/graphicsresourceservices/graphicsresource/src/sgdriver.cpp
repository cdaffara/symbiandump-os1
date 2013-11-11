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
//

#include <e32uid.h>
#include "sgdriver.h"
#include "sgresourceadapter.h"

/**
Function pointer type used to instantiate the Graphics Resource Adapter singleton.
*/
typedef TInt (*TSgDriverCreateFunction)(MSgDriverAdapter*&);

XSgDriverPls::XSgDriverPls()
	{
	iError = iMutex.CreateLocal();
	iOpenCount = 0;
	iDriver = NULL;
	}


#ifndef __WINS__
XSgDriverPls gPls;
#endif


/**
@publishedPartner
@prototype
@deprecated

Opens the Graphics Resource driver in the context of the calling process.

This function must be called by any process using the Graphics Resource API before
any other function in the API. When finished the process should call Close(). If
Open() has already been called from the same process then calling it again just
increments a process-local open count. Each successful call to Open() must be
balanced by a corresponding call to Close() later on.

Note that, in a multi-threaded process, it is not necessary for each thread to call
this function. A possible strategy is therefore for the main thread to call
SgDriver::Open() at the beginning and SgDriver::Close() at the end, while the rest
of the threads do not call either of these but simply use the Graphics Resource API.

@pre None.
@post The Graphics Resource driver is initialised for use in the context of the
      calling process and the process-local open count is incremented by one.
@return KErrNone if successful.
@return KErrNotSupported if no implementation of the Graphics Resource API is present.
@return KErrNoMemory if there is not enough system memory.
@see SgDriver::Close()
*/
EXPORT_C TInt SgDriver::Open()
	{
	if (gPls.iError != KErrNone)
		{
		return gPls.iError;
		}
	gPls.iMutex.Wait();
	if (gPls.iDriver)
		{
		++gPls.iOpenCount;
		gPls.iMutex.Signal();
		return KErrNone;
		}
#ifdef __WINS__
	const TUidType KSgResourceAdapterLibraryUidType(KDynamicLibraryUid, KSharedLibraryUid, KSgResourceAdapterLibraryUid);
	RLibrary lib;
	TInt err = lib.Load(KSgResourceAdapterLibraryName, KSgResourceAdapterLibraryUidType);
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			err = KErrNotSupported;
			}
		gPls.iMutex.Signal();
		return err;
		}
	gPls.iLibrary = lib;
	err = gPls.iLibrary.Duplicate(RThread()); // Get a process-wide handle
	lib.Close();
	if (err != KErrNone)
		{
		gPls.iMutex.Signal();
		return err;
		}
	TSgDriverCreateFunction create = reinterpret_cast<TSgDriverCreateFunction>(gPls.iLibrary.Lookup(1));
	err = create(gPls.iDriver);
	if (err != KErrNone)
		{
		gPls.iLibrary.Close();
		gPls.iMutex.Signal();
		return err;
		}
#else
	TInt err = MSgDriverAdapter::New(gPls.iDriver);
	if (err != KErrNone)
		{
		gPls.iMutex.Signal();
		return err;
		}
#endif
	gPls.iOpenCount = 1;
	gPls.iMutex.Signal();
	return KErrNone;
	}


/**
@publishedPartner
@prototype
@deprecated

Closes the Graphics Resource driver in the context of the calling process.

This function must be called by a process when finished using the Graphics Resource
API. It decrements the process-local open count and, when the count becomes zero,
it carries out termination tasks needed to release the internal resources allocated
for the calling process.

Each call to Close() must correspond to a prior successful call to Open(). Note that,
in a multi-threaded process, it is not generally safe to call Close() regardless of
whether the corresponding call to Open() succeeded or failed, since too many calls to
Close() from a thread could have an unexpected effect on all the other threads in the
process. The following example demonstrates how to open and close the Graphics
Resource driver safely from a worker thread.

@code
	// Open Graphics Resource driver
	TBool isDriverOpen = EFalse;
	if (SgDriver::Open() == KErrNone)
		{
		isDriverOpen = ETrue;
		}
	// Do some work in this thread
	DoThreadWork();
	// Close Graphics Resource driver
	if (isDriverOpen)
		{
		SgDriver::Close();
		}
@endcode

@pre If the process-local open count is one then there are no open handles to graphics
     resources in the calling process.
@post The process-local open count is decremented by one if greater than zero. If the
      count becomes zero, then the calling process is not able to use the Graphics
      Resource API any longer.
@panic SGRES 1 in debug builds if there still are any open handles to graphics resources
       in the calling process when the process termination tasks are carried out.
@see SgDriver::Open()
*/
EXPORT_C void SgDriver::Close()
	{
	if (gPls.iError != KErrNone)
		{
		return;
		}
	gPls.iMutex.Wait();
	if (gPls.iOpenCount > 0 && --gPls.iOpenCount == 0)
		{
		gPls.iDriver->Delete();
		gPls.iDriver = NULL;
#ifdef __WINS__
		gPls.iLibrary.Close();
#endif
		}
	gPls.iMutex.Signal();
	}


/**
@internalComponent
@test
*/
EXPORT_C TInt SgDriver::ResourceCount()
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt count = gPls.iDriver->ResourceCount();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return count;
	}


/**
@internalComponent
@test
*/
EXPORT_C void SgDriver::AllocMarkStart()
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	gPls.iDriver->AllocMarkStart();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	}


/**
@internalComponent
@test
*/
EXPORT_C void SgDriver::AllocMarkEnd(TInt aCount)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	gPls.iDriver->AllocMarkEnd(aCount);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	}


/**
@internalComponent
@test
*/
EXPORT_C void SgDriver::SetAllocFail(RAllocator::TAllocFail aType, TInt aRate)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	gPls.iDriver->SetAllocFail(aType, aRate);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	}
