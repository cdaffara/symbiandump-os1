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

#ifndef __SYSMONMONITOR_H__
#define __SYSMONMONITOR_H__

#include <e32std.h>
#include <e32base.h>
#include <startup.hrh>

#include "logoncallback.h"
#include "logonbackofftimer.h"

class CSysMonServer;
class CStartupProperties;
class CLogonBackoffTimer;

/** 
This class is responsible for monitoring a single process.
@file
@internalComponent
@released
*/
class CMonitor:public CActive, public MLogonCallback
	{
public:
	static CMonitor* NewL(CSysMonServer& aServer, const TProcessId& aId, CStartupProperties* aStartupProperties, TBool aExecuteRecoveryMethodOnFailure);
	void Start();
	TProcessId ProcessId() const;
	~CMonitor();

public:
	// From MLogonCallback
	TInt DoProcessLogon();
	void ActivateSelf();
	
protected:
	static TInt Callback(TAny* aParent);
	void RestartProcessL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void CancelMonitor();

protected:
	CMonitor(CSysMonServer& aServer, const TProcessId& aId);	
	void ConstructL(CStartupProperties* aStartupProperties, TBool aExecuteRecoveryMethodOnFailure);

public : 
    static const TInt iOffset;
	
private:
	CSysMonServer& iSysMonServer;		
	TProcessId iProcessId;	
	RProcess iProcess;
	TTime iLoadTime;
	CStartupProperties* iStartupProperties;	
	TSglQueLink iSglQueLink;
	CLogonBackoffTimer* iLogonBackoffTimer;
	};

#endif
