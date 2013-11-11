// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#ifndef CS_GLOB_H
#define CS_GLOB_H

#include "cs_thread.h"
#include "CS_STD.H"
#include <e32base.h>


class C32GlobalUtil
	{
public:
	static void InitC32GlobalsL(CC32WorkerThread* aWorker);
	static void ShutDown(void);
	static void NewSession();
	static void SessionClosing();
	};



class CC32ShutdownWatchDog;


class CC32Data : public CBase
// owned by CTLSRedirector and meant to be referenced via TLS. Holds various pointers
// to facilitate tricky scenarios such as shutdown and static logging with workerthread id
	{
public:
	static CC32Data* NewL();
	CC32WorkerThread* SelfWorker() const;

public:		
	CC32WorkerThread* iWorkerThread;           // pointer to the main class of the thread, not owned
	CC32ShutdownWatchDog* iShutdownWatchDog;   // loosely owned by workerthread: normally watchdog deletes itself when it fires
	TInt iNumSessions;						//< Keep a counter of the sessions
private:
	CC32Data();
	};



NONSHARABLE_CLASS(CTLSRedirector) : public CBase
// stored at TLS pointer and handles the two separate situations where TLS is used in C32:
// 1. inside C32, TLS is used for global data and CommTimer
// 2. outside C32, clients may call CommTimer which uses client's TLS
	{
public:
	static CTLSRedirector* NewL();
	inline CDeltaTimer* DeltaTimer() const;
	
	
	inline void SetDeltaTimer(CDeltaTimer* aTimer);

		
	inline CC32Data* C32GlobalData() const;
	
	inline void SetC32GlobalData(CC32Data* aC32Data);
		
	inline TBool IsC32GlobalDataSet() const;
	
	~CTLSRedirector();

private:
	CDeltaTimer* iTimer;	// The global delta timer for this thread. owned directly by this class
	CC32Data*    iC32Data;	// always owned by CWorkerThread. Never created in client thread situation.
	};




NONSHARABLE_CLASS(CC32ShutdownWatchDog) : public CTimer
/**
@internalComponent
*/
// There is some unclarity with regards to this watchdog, since it was copied from Esock along with most
// of the shutdown ideas. It appears it allows 
// the workerthread to delay the destruction of data structures until both the dealer and player classes
// have had a chance to shutdown - without which the destruction of the transport would panic due
// to commsfw seeing open channels.
// In practice the watchdog is often triggered for each thread when they run the shutdown for the
// dealer of the main thread (done by whoever gets there first). The watchdog allows the thread to yield
// and then it runs the player class shutdown, after which the watchdog runs.
// It is not known why a timer has been used rather than simply commence the data destruction after both
// dealer and player classes have shut down.
// The watchdog does not seem to be used to mitigate any inter-thread races.
//
	{
public:
	void Shutdown();
	static CC32ShutdownWatchDog* NewL();
#ifdef _DEBUG
	void SetWorkerId(TUint16 aId);
#endif
private:
	CC32ShutdownWatchDog();
	void RunL();
private:
#ifdef _DEBUG
	TInt iWorkerId;
#endif
	};
	
	
	
inline CDeltaTimer* CTLSRedirector::DeltaTimer() const
	{
	// non=-null not guaranteed, but this function is hardly used
	return iTimer;
	}


inline void CTLSRedirector::SetDeltaTimer(CDeltaTimer* aTimer)
	{
	iTimer = aTimer;
	}

	
inline CC32Data* CTLSRedirector::C32GlobalData() const
	{
	__ASSERT_DEBUG(iC32Data!=NULL,Fault(EBadState,_L8("CTLSRedirector: access of null C32GlobalData attempted")));
	return iC32Data;
	}
inline void CTLSRedirector::SetC32GlobalData(CC32Data* aC32Data)
	{
	__ASSERT_DEBUG(iC32Data==NULL,Fault(EBadState,_L8("CTLSRedirector: attempt to set already-set GlobalData")));
	iC32Data=aC32Data;
	}
	
inline TBool CTLSRedirector::IsC32GlobalDataSet() const
	{
	return iC32Data!=NULL;
	}


inline CTLSRedirector* CRedirectorInTls()
// provide a convenient way to access the tls redirector.
	{
	return static_cast<CTLSRedirector *>(Dll::Tls());
	}

inline CC32Data* CC32DataInTls()
// provide a convenient way to access the global data.
// Panics if the global data is not set
	{
	return CRedirectorInTls()->C32GlobalData();
	}	


#endif // CS_GLOB_H






