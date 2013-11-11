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

#include "logonbackofftimer.h"
#include "shmadebug.h"

	
const TInt KMonitorLogonRetryFirstInterval = 100000; // 100ms
const TInt KMonitorLogonRetryMaxInterval = 10000000; // 10s
const TInt KMonitorLogonRetryIntervalMultiple = 2; 


	
/**
 Called from CMonitor
*/	
void CLogonBackoffTimer::ProcessLogon()
	{
		
	if( (iMonitor.DoProcessLogon() == KErrNoMemory) )
		{	
		iTimer.After( iStatus, iRetryInterval );
		SetActive();
		
		if( (iRetryInterval *= KMonitorLogonRetryIntervalMultiple) >= KMonitorLogonRetryMaxInterval ) 
			{
			iRetryInterval = KMonitorLogonRetryMaxInterval;	
			}
		

		DEBUGPRINT1( _L("SysMonMonitor: Logon failed owing to lack of memory ") );	
		}
	else
		{
		iMonitor.ActivateSelf();
		iRetryInterval = KMonitorLogonRetryFirstInterval;	
		}
	
	}
	


void CLogonBackoffTimer::RunL()
	{
		
	ProcessLogon();		
	}
	
	
	
void CLogonBackoffTimer::DoCancel()
	{
	
	iTimer.Cancel();	
	}
	
	

TInt CLogonBackoffTimer::RunError( TInt /*aError*/ )
	{
		
	iTimer.Cancel();
	
	return KErrNone;	
	}


	
CLogonBackoffTimer* CLogonBackoffTimer::NewL( MLogonCallback& aMonitor )
	{
	
	CLogonBackoffTimer* self = new(ELeave) CLogonBackoffTimer( aMonitor );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	


void CLogonBackoffTimer::ConstructL()
	{
	
	User::LeaveIfError( iTimer.CreateLocal() );	
	CActiveScheduler::Add( this );
	}
	
	
	
 CLogonBackoffTimer::CLogonBackoffTimer( MLogonCallback& aMonitor )
: CActive( EPriorityStandard ), iMonitor( aMonitor ), iRetryInterval( KMonitorLogonRetryFirstInterval )
 	{
 	}
 	
 	
 	
CLogonBackoffTimer::~CLogonBackoffTimer()
	{
	Cancel();
	iTimer.Close();	
	}
