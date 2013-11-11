// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OPENWFCMONITORTHREAD_H_
#define OPENWFCMONITORTHREAD_H_

#include <e32base.h>

class COpenWfcJobManger;

class COpenWfcMonitorThread : public CBase
{
public:
	virtual ~COpenWfcMonitorThread();

	static COpenWfcMonitorThread* NewL(TInt aNumber, COpenWfcJobManger& aManager);
	static COpenWfcMonitorThread* NewLC(TInt aNumber, COpenWfcJobManger& aManager);
	static TInt Main(TAny *aSelf);
	void Start();
	void Resume();
	void Suspend();
	void Signal();
	TInt Run();
	void EndThread();
	
private:
	COpenWfcMonitorThread(COpenWfcJobManger& aManager);
	void ConstructL(TInt aScreenNumber);

private:
    volatile TBool iEndThread;
	RThread iThread;
	TRequestStatus iThreadStatus;
	COpenWfcJobManger& iManager;
    RSemaphore iSemaphore;
};

#endif /* OPENWFCMONITORTHREAD_H_ */
