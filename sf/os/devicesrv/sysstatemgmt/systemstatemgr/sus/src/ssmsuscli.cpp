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

#include <ssm/ssmsuscli.h>
#include "susutilclisrv.h"

/**
 Connect this session without pre-allocating any async message slots.
 This version of @c Connect signifies that the session has no dedicated pool of asynchronous message 
 objects but should use the global pool, reducing the kernel memory required to create the session. 
 This is the best value to use unless either:
	1. The session never sends asynchronous messages, in which case you should specify '0', OR
	2. The session makes guarantees on at least one asynchronous API that it cannot fail with KErrNoMemory - 
	   in which case you need to work out the maximum number of concurrent asynchronous messages that may 
	   be outstanding on a single session and use that value.
	   
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
 */
EXPORT_C TInt RSsmSusCli::Connect()
 	{
 	const TInt KTryAllocSlotsFromSystemPoolInRuntime = -1;
 	return Connect(KTryAllocSlotsFromSystemPoolInRuntime);
 	}

/**
 Connect this session and pre-allcocate the number of async message slots you will need.
 I.e. the max number of async calls to @c RequestLoadSup waiting for completion you will ever need.
 
 Use this version of @c Connect if you want to be quaranteed slots rather than relying 
 on slots being allocated in runtime from the system wide pool.
  
 @see RSessionBase::CreateSession
 @publishedPartner
 @released
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
*/
EXPORT_C TInt RSsmSusCli::Connect(TInt aAsyncMessageSlots)
	{
	return DoConnect(KSusUtilServerName, Version(), aAsyncMessageSlots);	
	}

/**
 This exists for testing purposes only.
 @internalComponent
 */
TInt RSsmSusCli::DoConnect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots)
	{
	if(!Handle())
		{
		return CreateSession(aServerName, aVersion, aAsyncMessageSlots);
		}
	
	return KErrAlreadyExists;
	}  //lint !e1746 suppress parameter 'aVersion' could be made const reference

/**
 Close this session.
 @publishedPartner
 @released
*/
EXPORT_C void RSsmSusCli::Close()
	{
	RSessionBase::Close();
	}
	
/**
*/		
TVersion RSsmSusCli::Version() const
	{
	return(TVersion(KSusUtilServMajorVersionNumber, KSusUtilServMinorVersionNumber, KSusUtilServBuildVersionNumber));
	}

/**
 Use this function to synchronously load a utility plugin. When this function returns,
 the plugin have been loaded, initialized and started in the Utility Server thread.
 
 @param aUtilityPluginDetails The information needed to identify and load the utility plugin.
 @return KErrNone or one of the system wide errorcodes.
 @publishedPartner
 @released
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
*/	
EXPORT_C TInt RSsmSusCli::RequestLoadSup(const TSsmSupInfo& aUtilityPluginDetails)
	{
	if(Handle())
		{
		const TPckgC<TSsmSupInfo> pckg(aUtilityPluginDetails);
		return SendReceive(ERequestLoadSup, TIpcArgs(&pckg));
		}
	
	return KErrDisconnected;
	}

/**
 Use this function to request asynchronous loading of a utility plugin.
 
 Usage pattern:
 @code
	RSsmSusCli session;
	User::LeaveIfError(fs.Connect());
	CleanupStackClosePushL(session);
	TPckgC<TSsmSupInfo>* infoBuf = new (ELeave) TPckgC<TSsmSupInfo>(info);
	session.RequestLoadSup(*infoBuf, status);
	...
	//It is expected that production code will use an active-object instead of User::WaitForRequest.
	//You might be interested in the technique employed in CSsmStartSafe. See activewaiter.h/cpp and its usage in ssmstartsafe.cpp
	User::WaitForRequest(status); 
	delete infoBuf;
	CleanupStack::PopAndDestroy(&session);
 @endcode
 
 @param aTSsmSupInfoPckgC 	A TSsmSupInfo wrapped in a TPckgC descriptor. Needs to be 
 						valid until this request (@c aStatus) has been completed.
 @param aStatus 		The status for this request. Will when completed contain 
 						KErrNone or one of the system wide errorcodes.
 @publishedPartner
 @released
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
*/	
EXPORT_C void RSsmSusCli::RequestLoadSup(const TDesC8& aTSsmSupInfoPckgC, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestLoadSup, TIpcArgs(&aTSsmSupInfoPckgC), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	} 

/**
 Cancel an asynchronous request to load a utility plugin. Note that a preceding asynchronous request 
 to load a plugin will still be handled before this request to cancel. 
 @publishedPartner
 @released
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
*/	
EXPORT_C void RSsmSusCli::RequestLoadSupCancel()
	{
	if(Handle())
		{
		SendReceive(ERequestLoadSupCancel);
		}
	} 

/**
 @internalComponent
 @released
 @capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
 */
TInt RSsmSusCli::RequestUnLoadSup(const TSsmSupInfo& aUtilityPluginDetails)
	{
	if(Handle())
		{
		const TPckgC<TSsmSupInfo> pckg(aUtilityPluginDetails);
		return SendReceive(ERequestUnLoadSup, TIpcArgs(&pckg));
		}
	
	return KErrDisconnected;
	} 
