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

/**
 @file
 @internalAll 
*/

#ifndef CRECEIVETHREADMAN_H
#define CRECEIVETHREADMAN_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>

#include "mltsyreceivethreadcallbackinterfaces.h"

class CReceiveThreadLifeWatcher;
class CReceiveThreadWaitLoopSignaller;

/**
 * Manager class for controlling a number of objects that are responsible for
 *  the spawning of a receive thread and then signalling back to the main CTSY thread
 *  when an event is received. This allows all LTSY processing to be performed on the 
 *  same thread of execution as CTSY and Etel run. 
 */
class CReceiveThreadManager : public CBase 
	{
public:
	static CReceiveThreadManager* NewL(MLtsyReceiveThreadEventStateHandler& aEventHandler, MLtsyReceiveThreadObserver& aLifeObserver);
	~CReceiveThreadManager();
	void StartReceiveThreadL();
	void StopReceiveThread();

protected:
	void ConstructL(MLtsyReceiveThreadEventStateHandler& aEventHandler, MLtsyReceiveThreadObserver& aLifeObserver);
	CReceiveThreadManager();
	
	void SpawnReceiveThreadL();
	static TInt StartReceiveThreadFn(TAny* aArg);
	
private:
	RThread iThread;
	CReceiveThreadLifeWatcher* iThreadLifeWatcher;
	CReceiveThreadWaitLoopSignaller* iReceiveThreadWaitLoopSignaller;
	};

/**
 * Class that watches for the life of the receive thread.  It uses RThread::Logon 
 * that will complete when the thread dies.
 */
class CReceiveThreadLifeWatcher : public CActive
	{
public:
	static CReceiveThreadLifeWatcher* NewL(MLtsyReceiveThreadObserver& aReceiveThreadLifeObserver);
	~CReceiveThreadLifeWatcher();
	void LogonL(TThreadId aID);

protected: //from CActive
	void RunL();
	void DoCancel();
protected:
	void ConstructL();	
	 CReceiveThreadLifeWatcher(MLtsyReceiveThreadObserver& aReceiveThreadLifeObserver);
	
private:
	RThread iThread;
	MLtsyReceiveThreadObserver& iReceiveThreadLifeObserver;
	};

/**
 * Class that signals the thread switch from the receive thread to the Etel/CTSY thread on receiving a signal
*/
class CReceiveThreadWaitLoopSignaller : CActive
	{
public:
	static CReceiveThreadWaitLoopSignaller* NewL(MLtsyReceiveThreadEventStateHandler& aEventHandler);
	~CReceiveThreadWaitLoopSignaller();
	void StartReceiver();
	TInt WaitLoop();
protected: //from CActive
	void RunL();
	void DoCancel();
protected:
	void ConstructL();	
	CReceiveThreadWaitLoopSignaller(MLtsyReceiveThreadEventStateHandler& aEventHandler);
	void RequestComplete(RThread& aThread, TRequestStatus& aStatus, TInt aError);
private:
	TInt ExecSync();
private:
	RThread iEtelThread;
	RThread iLtsyReceiveThread;
	TRequestStatus iLtsyReceiveThreadReqStatus;
	MLtsyReceiveThreadEventStateHandler& iEventHandler;
	};

#endif //CRECEIVETHREADMAN_H
