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

#ifndef __FIREANDFORGET_H__
#define __FIREANDFORGET_H__


#include <e32base.h>
#include <e32cmn.h>
#include <apastarter.h>
#include <e32debug.h>

#include "ssmloadsysmon.h"
#include "timeoutwaiternotification.h"
#include <ssm/ssmstartupproperties.h>
#include "ssconst.h"

/**
@internalTechnology
@released
*/

	


class CApaStarter;
class CTimeoutWaiter;

/**
An active object that can be used to start the process or application with Fire and Forget 
execution behaviour. This object deletes itself once the process has Rendezvous'd successfully, or 
when process's Rendezvous has not succeeded and retries(if any), have been exhausted.
*/    
NONSHARABLE_CLASS( CFireAndForget ): public CActive, public MTimeoutWaiterNotification
	{
public:
	static CFireAndForget* NewL( const CSsmStartupProperties& aStartupProperties );
	static CFireAndForget* NewLC( const CSsmStartupProperties& aStartupProperties );
	~CFireAndForget();
	void FireD();

	// From MTimeoutWaiterNotification
	void NotifyTimeout();

protected:
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
	
private:
	CFireAndForget();
	void ConstructL( const CSsmStartupProperties& aStartupProperties );
	void CleanupOrRetryL();
	void InvokeL();
	TBool GoodStart();
	TInt GetProcessHandle( RProcess& aProcess, const TThreadId& aThreadId );
	void LoadApStartLibL();
	void StartMonitorIfRequiredL();

	enum TFireAndForgetState
		{
		EFAndFIdle = 0,
		EFAndFStateRetrying
		};

private:
	CApaStarter* iApaStarter;
	CSsmStartupProperties* iStartupProperties;
	TInt iRetries;
	CTimeoutWaiter* iTimeoutWaiter;
	TFireAndForgetState iState;
	TThreadId iThreadId;
	RProcess iProcess;
	RLibrary iApStartLib;
	RLibrary iSysMonProxyLib;
	};


#endif // __FIREANDFORGET_H__
