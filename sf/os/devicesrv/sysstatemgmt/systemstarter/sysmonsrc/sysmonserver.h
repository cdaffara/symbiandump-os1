// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/


#ifndef __SYSMONSERVER_H__
#define __SYSMONSERVER_H__

#include <e32std.h>
#include <e32base.h>

class CMonitor;
class CTimerList;

/*
SysMonServer is responsible for adding and canceling CMonitor objects.
Implemented using single linked list.
*/
class CSysMonServer: public CServer2
	{
public:
	static CSysMonServer* NewLC();
	void AddAndStartMonitorL(CMonitor* aMonitor);
	void CancelMonitor(const TProcessId& aProcessId);
	void CancelAllMonitors();
	TBool ShutDownInProgress() const;
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	CTimerList& TimerListL();
	~CSysMonServer();

private:
	CSysMonServer();
	void ConstructL();

private:
	TBool iShutDownInProgress;
	TSglQue<CMonitor> iMonitors;
	TSglQueIter<CMonitor> iIter;
	CTimerList* iTimerList;
	};

#endif // __SYSMONSERVER_H__
