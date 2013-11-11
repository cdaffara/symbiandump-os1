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

#ifndef __SSMSTARTSAFE_H__
#define __SSMSTARTSAFE_H__

#include <e32base.h>
#include <ssm/startandretrynotifications.h>

class CSsmStartupProperties;
class CStartAndRetry;
class CApaStarter;
/**
@internalTechnology
@released
*/
struct TSsmStartSafeEntry
	{
	CStartAndRetry* iStartAndRetry;
	TRequestStatus* iCallerTrs;
	TInt iCallerIndex;
	TBool iInUse;
	};

class MSsmLoadSysMon;

/**
Used to start a process or application reliably. 
Which means, if the starting fails after designated number of attempts then system restarts or an error is returned.
@internalTechnology
@released
*/
NONSHARABLE_CLASS( CSsmStartSafe ) : public CBase, public MStartAndRetryNotifications
	{
public:
	IMPORT_C static CSsmStartSafe* NewL();
	IMPORT_C static CSsmStartSafe* NewLC();
	~CSsmStartSafe();		
	
	IMPORT_C void StartL( const CSsmStartupProperties& aStartupProperties, RProcess& aProcess ); 	
	IMPORT_C void Start( const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TRequestStatus& aCommandTrs, TInt& aCallerIndex );
	IMPORT_C void StartAndMonitorL( const CSsmStartupProperties& aStartupProperties, RProcess& aProcess );
	IMPORT_C TInt InitAppArcServer();
	IMPORT_C void InitApparcServer( TRequestStatus& aCommandTrs );
	IMPORT_C void InitApparcServerCancel();
	IMPORT_C void StartCancel( TInt& aCallerIndex );

	// From MStartAndRetryNotifications
	void RendezvousComplete( TInt aComplete, TInt aCallerIndex );
	void CancellationComplete( TInt aComplete, TInt aCallerIndex );
	
private:	
	CSsmStartSafe();
	void ConstructL();
	
	TInt RegisterCall( TInt& aCallerIndex, TRequestStatus& aStatus );
	TInt DeRegisterCall(TInt aCallerIndex);
	void LoadApStartLibL();
	void LoadMonitorProxyLibL();
	
private:	
	RLibrary iApStartLib;
	CApaStarter* iApaStarter;
	RLibrary iSysMonProxyLib;
	MSsmLoadSysMon* iSysMonCli;
	RArray<TSsmStartSafeEntry> iStartSafeEntryArray;
	TInt iCallerCount;	
	};


#endif // __SSMSTARTSAFE_H__
