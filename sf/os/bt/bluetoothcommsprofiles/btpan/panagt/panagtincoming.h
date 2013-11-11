// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANAGTINCOMING_H
#define PANAGTINCOMING_H

/**
@file
PAN agent incoming connection listener definition
@internalComponent
*/

#include <comms-infras/eintsock.h>
#include "pancommon.h"	// for KBnepPsm

class RInternalSocket;

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

const TUint KIncomingSockQueueSize = 8;
const TUint KPriorityIncomingListenerAo = CActive::EPriorityStandard;

/** Minimum time between attempts to reopen the listening and/or accepting socket(s) */
const TInt KMinSocketReopenAttemptInterval = 30*1000*1000;	// 30 secs
/** Maximum time between attempts to reopen the listening and/or accepting socket(s) */
const TInt KMaxSocketReopenAttemptInterval = 6*60*1000*1000;	// 6 mins 

class CPanIncomingSocketDelayedOpenHelper;
class MIncomingConnectionAcceptor;
NONSHARABLE_CLASS(CPanIncomingConnectionListener) : public CActive
/**
Listen for incoming connections from other Bluetooth devices
*/
	{
	friend class CPanIncomingSocketDelayedOpenHelper;
public:
	static CPanIncomingConnectionListener* NewL(MIncomingConnectionAcceptor& aAcceptor);
	~CPanIncomingConnectionListener();
	
	// CActive methods
	void RunL();
	void DoCancel();

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    void RestartIncomingConnectionListener(TInt aError);
#endif	

	inline TBool IsListeningSocketOpen() const;
	
private:
	CPanIncomingConnectionListener(MIncomingConnectionAcceptor& aAcceptor);
	void ConstructL();
	
	void OpenListeningSocketL();
	TBool IsAcceptingSocketOpen() const;

public:

private:
	::RInternalSocket iListeningSocket;	//< The listening socket
	::RInternalSocket iAcceptingSocket;	//< The current accepting socket
	
	CPanIncomingSocketDelayedOpenHelper* iDelayedOpenHelper;
	
	MIncomingConnectionAcceptor& iAcceptor;	//< The class to signal when a new connection is accepted
	TBool iListeningSocketIsOpen;
	};

inline TBool CPanIncomingConnectionListener::IsListeningSocketOpen() const
	{
	return iListeningSocketIsOpen;
	}
	
class MIncomingConnectionAcceptor
/**
An interface to allow the socket listener class to hand off new connections to a channel manager
*/
	{
public:
	virtual void CreateNewConnectionFromIncomingL(RInternalSocket& aConnectedSocket) = 0;
	
protected:
	virtual ~MIncomingConnectionAcceptor(){};
	};

NONSHARABLE_CLASS(CPanIncomingSocketDelayedOpenHelper) : public CTimer
/**
If it is not possible to reopen the listening or accepting sockets for any reason,
this class will perform a retry at regular intervals to attempt to recover.
@note This class uses an exponential backoff in the case of failure
*/
	{
public:
	static CPanIncomingSocketDelayedOpenHelper* NewL(CPanIncomingConnectionListener& aIncomingConnectionListener);
	~CPanIncomingSocketDelayedOpenHelper();
	
	// general methods
	void SocketOpenFailed();
	
	// CTimer (well, CActive actually) methods
	virtual void RunL();
	
private:
	CPanIncomingSocketDelayedOpenHelper(CPanIncomingConnectionListener& aIncomingConnectionListener);
	void ConstructL();
	
	void SetNewTimerPeriod();

public:

private:
	TTimeIntervalMicroSeconds32 iCurrentTimerPeriod;
	CPanIncomingConnectionListener& iIncomingConnectionListener;
	};

}	// PanAgent
#endif // PANINCOMING_H
