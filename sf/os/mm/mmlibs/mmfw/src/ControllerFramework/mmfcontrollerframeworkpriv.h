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

#ifndef MMFCONTROLLERFRAMEWORKPRIV_H
#define MMFCONTROLLERFRAMEWORKPRIV_H

/**
@internalComponent

Used to monitor the death of the controller subthread created by the controller proxy.

@since 9.3
*/
NONSHARABLE_CLASS(CLogonMonitor) : public CActive
	{
public:
	/**
	Two phases constructor
	*/
	static CLogonMonitor* NewL(MLogonMonitorObserver* aLogonMonitorObserver);
	
	/**
	Destructor.
	*/
	~CLogonMonitor();
	
	/**
	Handle a request from the client to start monitoring a thread death

	@param  aThread
	        The thread to start monitoring
	*/
	void StartMonitoring(RThread& aThread);
	
	RServer2& Server();
private:
	/**
	Constructor
	*/
	CLogonMonitor(MLogonMonitorObserver* aLogonMonitorObserver);
	
	/**
	Second phase constructor
	*/
	void ConstructL();

private:
	/** 
	From CActive.
	*/
	void RunL();
	
	/** 
	From CActive.
	*/
	void DoCancel();
private:
	/** 
	A Pointer to the controller proxy. 
	*/
	MLogonMonitorObserver* iLogonMonitorObserver;
	/** 
	A Pointer to the monitored thread. This thread is owned by the controller proxy
	*/
	RThread* iThread;
	/**
	A Pointer to the currently installed active scheduler
	*/
	CActiveScheduler* iScheduler;
	
	RServer2 iServer;
	};
	
#endif // MMFCONTROLLERFRAMEWORKPRIV_H
