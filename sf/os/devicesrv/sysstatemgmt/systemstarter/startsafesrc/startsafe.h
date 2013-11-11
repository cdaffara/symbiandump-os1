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


#ifndef __STARTSAFE_H__
#define __STARTSAFE_H__

#include <e32def.h>
#include <f32file.h>
#include <sysmonclisess.h>
#include <startup.hrh>


/**
@internalTechnology
@released
*/
const TInt KStartSafeNoTimeout = 0;


class CApaStarter;
class CStartupProperties;

/**
Used to start a process or application reliably. 
Which means, if the starting fails after designated number of attempts then system restarts or an error is returned.
@internalTechnology
@deprecated Instead of this use CSsmStartSafe
*/
NONSHARABLE_CLASS( CStartSafe ) : public CBase
	{
public:
	IMPORT_C static CStartSafe* NewL();
	virtual ~CStartSafe();		
	
	IMPORT_C void StartL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TInt& aNumRetried ); 	
	IMPORT_C void StartL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TRequestStatus& aCommandTrs );
	IMPORT_C void StartAndMonitorL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TInt& aNumRetried );
	IMPORT_C void InitAppArcServer( TRequestStatus& aStatus );
	
private:	
	CStartSafe();
	void ConstructL();
	
	void InstituteRestartL( const CStartupProperties& aStartupProperties );
	TInt AwaitRendezvous( RProcess& aProcess, TInt aTimeout );
	TInt AwaitProcessStartup( RProcess& aProcess, TInt aTimeout, TRequestStatus& aCommandTrs );
	void DoFireAndForgetL( const CStartupProperties& aStartupProperties, RProcess& aProcess );
	void DoWaitForStartL( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried );				
	TInt DoStartAppWithRetries( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried );
	void GetProcessHandleL( RProcess& aProcess, const TThreadId& aThreadId );
	void GetHandleAndWaitForProcessL( const CStartupProperties& aStartupProperties, RProcess& aProcess, TThreadId& aThreadId, TRequestStatus& aCommandTrs );
	TInt DoStartProcessWithRetries( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried );
	void DoStartAppL( const CStartupProperties& aStartupProperties, RProcess& aProcess );
	void DoStartProcessL( const CStartupProperties& aStartupProperties, RProcess& aProcess );
	void LoadApStartLibL();
	void ShootBadProcess( RProcess& aProcess, const TRequestStatus& aStatus );

private:	
	CApaStarter* iApaStarter;
	RLibrary iApStartLib;
	};


#endif
