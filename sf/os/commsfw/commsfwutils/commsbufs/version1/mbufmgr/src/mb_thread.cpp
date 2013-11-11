// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mb_thread.h>
#include <e32debug.h>
#include <es_mbman.h>

using namespace CommsFW;

EXPORT_C TInt RCFThread::Create(const TDesC& aName, TThreadFunction aOtherThreadFunction,
						TInt aStackSize, RHeap* aHeap, TAny* aModuleArgs)
/**
Derived implementation of RThread::Create, which processes some specific
data for the new thread before calling the RThread::Create.
@param aName Name of the new thread.
@param aOtherThreadFunction Main thread-function.
@param aStackSize Initial size of the thread stack.
@param aHeap The heap for the thread.
@param aModuleArgs Pointer given as argument to aOtherThreadFunction.
@see RThread
@see TThreadFunction
@see RHeap
@publishedPartner
@released
*/
	{
   	iStartupInfo.iMBufManager = CMBufManager::Context();
   	__CFLOG_STMT( iStartupInfo.iCFLogIf = CCFLogIf::Context());
	if( iStartupInfo.iMBufManager == NULL )
		return KErrNotFound;
	// Check to make sure the logger is available.
	#ifdef __CFLOG_ACTIVE
		if( !iStartupInfo.iCFLogIf )
			{
			RDebug::Print( _L( "RCFThread::Create - the log interface was not found.  This normally means that the logging version of commsfw.dll has been mixed with a stub version of cflog.dll.  See CommsDebugUtility How-To Document FAQ section for details on enabling logging in a release build." ) );

			return KErrNotFound;
			}
	#endif
	
   	iStartupInfo.iOtherThreadFunction = aOtherThreadFunction;
   	iStartupInfo.iModuleArgs = aModuleArgs;
   	return RThread::Create(aName, RCFThreadFunction, aStackSize, aHeap, &iStartupInfo);
   	};

EXPORT_C TInt RCFThread::Create(const TDesC& aName, TThreadFunction aOtherThreadFunction,
				TInt aStackSize, TInt aMinHeapSize, TInt aMaxHeapSize, TAny* aModuleArgs) 
/**
Derived implementation of RThread::Create, which processes some specific
data for the new thread before calling the RThread::Create
@param aName Name of the new thread.
@param aOtherThreadFunction Main thread-function.
@param aStackSize Initial size of the thread stack.
@param aMinHeapSize Minimum size of the heap that will be created for the thread.
@param aMaxHeapSize Maximum size of the heap that will be created for the thread.
@param aModuleArgs Pointer given as argument to aOtherThreadFunction.
@see RThread
@see TThreadFunction
@publishedPartner
@released
*/
	{
   	iStartupInfo.iMBufManager = CMBufManager::Context();
   	__CFLOG_STMT( iStartupInfo.iCFLogIf = CCFLogIf::Context());
	if( iStartupInfo.iMBufManager == NULL )
		return KErrNotFound;
	// Check to make sure the logger is available.
	#ifdef __CFLOG_ACTIVE
		if( !iStartupInfo.iCFLogIf )
			{
			RDebug::Print( _L( "RCFThread::Create - the log interface was not found.  This normally means that the logging version of commsfw.dll has been mixed with a stub version of cflog.dll.  See CommsDebugUtility How-To Document FAQ section for details on enabling logging in a release build." ) );

			return KErrNotFound;
			}
	#endif

   	iStartupInfo.iOtherThreadFunction = aOtherThreadFunction;
   	iStartupInfo.iModuleArgs = aModuleArgs;
   	return RThread::Create(aName, RCFThreadFunction, aStackSize, aMinHeapSize, aMaxHeapSize, &iStartupInfo);
   	};

TInt RCFThread::RCFThreadFunction(TAny* aStartupInfo )
/**
Intermediate function which masquerades as the main thread function in order to 
perform some specific actions for the new thread in the correct context before
calling the new thread's actual main thread function
@param aStartupInfo structure containing pointers to MBufMger and CFlog.
@see RCFThread::ThreadStartupInfo 
@internalComponent
*/
	{
	ThreadStartupInfo* startInfo = reinterpret_cast<ThreadStartupInfo*>(aStartupInfo);
	startInfo->iMBufManager->SetContext();
   	__CFLOG_STMT( startInfo->iCFLogIf->SetContext();)
	__CFLOG_OPEN;
   	TInt result = startInfo->iOtherThreadFunction(startInfo->iModuleArgs);
	__CFLOG_CLOSE;
   	return result;
   	};

