// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "requestqueueoneshot.h"
#include "mdispatchercallback.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

const TInt KNoIpc = -1;

CRequestQueueOneShot::TIpcDataPackage::TIpcDataPackage()
	: iIpc(KNoIpc), 
	  iDataPackage(NULL), 
	  iResultCode(KErrNone), 
	  iCleanupOnly(EFalse), 
	  iDispatcherCallback(NULL) ,
	  iLink()
	{
	} // CRequestQueueOneShot::TIpcDataPackage::TIpcDataPackage

CRequestQueueOneShot::TIpcDataPackage::~TIpcDataPackage()
	{
	delete iDataPackage;
	iDataPackage = NULL;
	} // CRequestQueueOneShot::TIpcDataPackage::~TIpcDataPackage

CRequestQueueOneShot::CRequestQueueOneShot()
	: CAsyncOneShot(CActive::EPriorityStandard),
	  iIpcQueue(_FOFF(TIpcDataPackage,iLink))
	{
	} // CRequestQueueOneShot::CRequestQueueOneShot


CRequestQueueOneShot::~CRequestQueueOneShot()
	{
	TSYLOGENTRYEXIT;
	
	Cancel();

	TIpcDataPackage* ipcDataPackage;
	while (!iIpcQueue.IsEmpty())
		{
		ipcDataPackage = iIpcQueue.First();
		iIpcQueue.Remove(*ipcDataPackage);
		ipcDataPackage->iCleanupOnly = ETrue;
		// Callback to dispatcher to ensure that dispatcher cleans up any heap data
		// in the CMmDataPackage inside the TIpcDataPackage.
		ipcDataPackage->iDispatcherCallback->CallbackSync(*ipcDataPackage);
		}
	
	} // CRequestQueueOneShot::~CRequestQueueOneShot


CRequestQueueOneShot* CRequestQueueOneShot::NewLC()
	{
	CRequestQueueOneShot* self = new (ELeave)CRequestQueueOneShot();
	CleanupStack::PushL (self);
	self->ConstructL();
	return self;
	} // CRequestQueueOneShot::NewLC


CRequestQueueOneShot* CRequestQueueOneShot::NewL()
	{
	TSYLOGENTRYEXIT;
	CRequestQueueOneShot* self = CRequestQueueOneShot::NewLC();
	CleanupStack::Pop(self);
	return self;
	} // CRequestQueueOneShot::NewL


void CRequestQueueOneShot::ConstructL()
	{
	} // CRequestQueueOneShot::ConstructL


void CRequestQueueOneShot::QueueRequest(TIpcDataPackage& aIpcDataPackage)
/**
 * Add the request to the queue.
 * 
 * @param aIpcDataPackage The encapsulated request to add to the queue.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("IPC=%d,err=%d"), aIpcDataPackage.iIpc, aIpcDataPackage.iResultCode);

	__ASSERT_DEBUG(aIpcDataPackage.iDispatcherCallback != NULL, CtsyDispatcherPanic(EInvalidNullPtr)); 
	aIpcDataPackage.iCleanupOnly = EFalse;
	iIpcQueue.AddLast(aIpcDataPackage);
	
	// This object is only active if there is something in the queue
	if (!IsActive())
		{
		Call();
		}
	
	} // CRequestQueueOneShot::QueueRequest


void CRequestQueueOneShot::DoCancel()
	{
	} // CRequestQueueOneShot::DoCancel


void CRequestQueueOneShot::RunL()
/**
 * Completes the request at the head of the queue.
 */
	{
	TSYLOGENTRYEXIT;
	
	if (iStatus == KErrNone)
		{
		__ASSERT_DEBUG(!iIpcQueue.IsEmpty(), CtsyDispatcherPanic(ERequestNotFound));
		TIpcDataPackage* ipcDataPackage = iIpcQueue.First();
		iIpcQueue.Remove(*ipcDataPackage);
		ipcDataPackage->iDispatcherCallback->CallbackSync(*ipcDataPackage);
		}
	
	if (!iIpcQueue.IsEmpty())
		{
		Call(); // Restart active object to process next request at the head of the queue
		}
	
	} // CRequestQueueOneShot::RunL


