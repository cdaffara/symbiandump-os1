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
// te_lbsbtgpsconfigthread.h
// lbsbtgpsconfig unit test: Thread to test config API
//

#include "te_lbsbtgpsconfigthread.h"


const TTimeIntervalMicroSeconds32 timeout = 1000;

// Uncomment this to print thread activity for debugging 
//#define CTestBtGpsConfigThread_DEBUG


TInt CTestBtGpsConfigThread::ThreadFunc(TAny* aAny)
	{
	CTestBtGpsConfigThread* thread = static_cast<CTestBtGpsConfigThread*>(aAny);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,thread->ThreadMainL());
	delete cleanup;

	return err;
	}


CTestBtGpsConfigThread* CTestBtGpsConfigThread::NewL()
	{
	CTestBtGpsConfigThread* self = new (ELeave) CTestBtGpsConfigThread();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CTestBtGpsConfigThread::~CTestBtGpsConfigThread()
	{
	Stop();
	iThread.Close();
	iMutex.Close();
	iDeviceList.ResetAndDestroy();
	}


void CTestBtGpsConfigThread::Start()
	{
	TInt err = iThread.Create(KNullDesC, ThreadFunc, KDefaultStackSize, &User::Allocator(), this);
	iThread.Resume();
	}


void CTestBtGpsConfigThread::Stop()
	{
	TRequestStatus status;
	status = KRequestPending;
	iThread.Logon(status);

	// Signal the thread to shutdown
	iMutex.Wait();
	iTask = KTaskShutdown;
	iMutex.Signal();

	// Wait for the shutdown
	User::WaitForRequest(status);
	TInt err = status.Int();
	}


void CTestBtGpsConfigThread::SetTaskAddDevice(const TBTDevAddr& aAddress, TInt aPosition)
	{
	iMutex.Wait();

	iTask = KTaskAddDevice;
	iTaskError = 1;
	iDeviceAddress = aAddress;
	iDevicePosition = aPosition;
	
	iMutex.Signal();
	}


void CTestBtGpsConfigThread::SetTaskReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition)
	{
	iMutex.Wait();

	iTask = KTaskReorderDevice;
	iTaskError = 1;
	iDeviceKey = aKey;
	iDevicePosition = aNewPosition;
	
	iMutex.Signal();
	}


void CTestBtGpsConfigThread::SetTaskRemoveDevice(TLbsBtGpsEntryKey aKey)
	{
	iMutex.Wait();

	iTask = KTaskRemoveDevice;
	iTaskError = 1;
	iDeviceKey = aKey;
	
	iMutex.Signal();
	}


TInt CTestBtGpsConfigThread::GetTaskError()
	{
	return iTaskError;
	}


TInt CTestBtGpsConfigThread::GetDeviceList(RPointerArray<TLbsBtGpsDeviceInfo>& aDeviceList)
	{
	iMutex.Wait();

	TInt errReturn = iDeviceListError; 
	for (TInt index=0; index<iDeviceList.Count(); ++index)
		{		
		TLbsBtGpsDeviceInfo* devInfo = new (ELeave) TLbsBtGpsDeviceInfo();
		*devInfo = *iDeviceList[index];
		aDeviceList.Append(devInfo);
		}
	
	iMutex.Signal();
	
	return errReturn;
	}


void CTestBtGpsConfigThread::OnDeviceListUpdate()
	{
	iMutex.Wait();

	iDeviceList.ResetAndDestroy();
	TRAP(iDeviceListError, iConfig->GetDeviceListL(iDeviceList));

	#ifdef CTestBtGpsConfigThread_DEBUG
		RDebug::Printf("--- [%Lu] OnDeviceListUpdate count=%d err=%d ---", 
						iThread.Id().Id(), iDeviceList.Count(), iDeviceListError);
	#endif
	
	iMutex.Signal();
	}


void CTestBtGpsConfigThread::ThreadMainL()
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install(iScheduler);
	CActiveScheduler::Add(this);

	iTimer.CreateLocal();
	CleanupClosePushL(iTimer);
	iTimer.After(iStatus, timeout);
	SetActive();

	// Open the config API
	iConfig = CLbsBtGpsConfig::NewL(*this);
	CleanupStack::PushL(iConfig);

	CActiveScheduler::Start();

	// Cleanup
	
	CleanupStack::PopAndDestroy(iConfig);
	CleanupStack::PopAndDestroy(); // iTimer.Close()

	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iScheduler);
	}


void CTestBtGpsConfigThread::DoCancel()
	{
	iTimer.Cancel();
	}


void CTestBtGpsConfigThread::RunL()
	{
	iMutex.Wait();

	switch (iTask)
		{
	case KTaskShutdown:
		#ifdef CTestBtGpsConfigThread_DEBUG
			RDebug::Printf("--- [%Lu] Shutdown ---",iThread.Id().Id());
		#endif
		CActiveScheduler::Stop();
		break;

	case KTaskAddDevice:
		iTaskError = iConfig->AddDevice(iDeviceAddress, iDevicePosition, iDeviceKey);
		#ifdef CTestBtGpsConfigThread_DEBUG
			RDebug::Printf("--- [%Lu] AddDevice pos=%d key=%d err=%d ---",iThread.Id().Id(),iDevicePosition,iDeviceKey,iTaskError);
		#endif
		break;
		
	case KTaskReorderDevice:
		iTaskError = iConfig->ReorderDevice(iDeviceKey, iDevicePosition);
		#ifdef CTestBtGpsConfigThread_DEBUG
			RDebug::Printf("--- [%Lu] ReorderDevice key=%d pos=%d err=%d ---",iThread.Id().Id(),iDeviceKey,iDevicePosition,iTaskError);
		#endif	
		break;
		
	case KTaskRemoveDevice:
		iTaskError = iConfig->RemoveDevice(iDeviceKey);
		#ifdef CTestBtGpsConfigThread_DEBUG
			RDebug::Printf("--- [%Lu] RemoveDevice key=%d err=%d ---",iThread.Id().Id(),iDeviceKey,iTaskError);
		#endif
		break;
		}
	
	// Reset timer if we're not shutting down
	if (iTask != KTaskShutdown)
		{
		iTask = KTaskNone;
		iTimer.After(iStatus, timeout);
		SetActive();
		}

	iMutex.Signal();
	}


CTestBtGpsConfigThread::CTestBtGpsConfigThread()
	: CActive(CActive::EPriorityIdle)
	{

	}


void CTestBtGpsConfigThread::ConstructL()
	{
	iMutex.CreateLocal();
	}
