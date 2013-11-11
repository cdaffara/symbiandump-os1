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
// lbsbtgpsconfig unit test: Thread to test config API
//

#ifndef TE_LBS_BTGPSCONFIGTHREAD_H
#define TE_LBS_BTGPSCONFIGTHREAD_H

#include "te_lbsbtgpsconfigstepbaseinternal.h"


class CTestBtGpsConfigThread : public CActive, public MLbsBtGpsConfigObserver
	{
public:
	static CTestBtGpsConfigThread* NewL();
	~CTestBtGpsConfigThread();
	
public:

	static TInt ThreadFunc(TAny* aAny);
	
	void Start();
	
	void Stop();

	void SetTaskAddDevice(const TBTDevAddr& aAddress, TInt aPosition);
	void SetTaskReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition);
	void SetTaskRemoveDevice(TLbsBtGpsEntryKey aKey);
	TInt GetTaskError();
	
	TInt GetDeviceList(RPointerArray<TLbsBtGpsDeviceInfo>& aDeviceList);
	
	// From MLbsBtGpsConfigObserver:
	void OnDeviceListUpdate();
	
protected:

	void ThreadMainL();
	
	// From CActive:
	void DoCancel();
	void RunL();
	
	CTestBtGpsConfigThread();
	void ConstructL();
	
private:

	// Owned by main thread:
	RThread iThread;
	RMutex iMutex;

	// Owned by created thread:
	RTimer iTimer;
	CActiveScheduler* iScheduler;

	
	CLbsBtGpsConfig* iConfig;

	RPointerArray<TLbsBtGpsDeviceInfo> iDeviceList;
	TInt iDeviceListError;

	enum Task
		{
		KTaskNone = 0,
		KTaskShutdown,
		KTaskAddDevice,
		KTaskReorderDevice,
		KTaskRemoveDevice
		};
	
	Task iTask;

	TBTDevAddr iDeviceAddress;
	TInt iDevicePosition;
	TLbsBtGpsEntryKey iDeviceKey;
	TInt iTaskError;
	
	};


#endif //TE_LBS_BTGPSCONFIGTHREAD_H
