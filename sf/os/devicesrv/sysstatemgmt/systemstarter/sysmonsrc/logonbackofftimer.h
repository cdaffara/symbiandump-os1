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

#ifndef __LOGONBACKOFFTIMER_H__
#define __LOGONBACKOFFTIMER_H__


#include "monitor.h"



class MLogonCallback;

/**
 In the event of CMonitor's call (via this class) to RProcess::Logon() completing its iStatus with 
 KErrNoMemory, this AO will retry the logon a number of times, each time after an 
 increased interval (up to a maximum). This is to ensure that if the low-memory 
 condition is cyclical in nature, we won't be unwittingly synchronised to it.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CLogonBackoffTimer) : public CActive
	{
	
public:
	static CLogonBackoffTimer* NewL( MLogonCallback& aMonitor );	
	~CLogonBackoffTimer();

	void ProcessLogon();		
	
protected:
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
		
private:
	CLogonBackoffTimer( MLogonCallback& aMonitor );
	void ConstructL();	
	
private:
	MLogonCallback& iMonitor;
	RTimer iTimer;
	TInt iRetryInterval;
	};

#endif // __LOGONBACKOFFTIMER_H__
