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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#ifndef __TESTANDSTOPPER_H__
#define __TESTANDSTOPPER_H__

#include <e32base.h>
#include <e32def.h>

#include <ssm/ssmstartsafe.h>
#include "tss_step_base.h"

class CTestAndStopper : public CActive
	{
public:
	CTestAndStopper( CSsmStartSafe& aSsmStartSafe, 
			 const CSsmStartupProperties &aStartupProperties, 
			 RProcess& aProcess, 
			 TInt aTimeout,
			 MSsTestAsyncNotifier* aSsTestAsyncNotifier = NULL );	
	
	~CTestAndStopper();

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
	
private:
	void DoStartsL();

private:
	RTimer iTimer;
	TBool iRunning;
	TBool iFinish;
	CSsmStartSafe& iSsmStartSafe;
	const CSsmStartupProperties& iStartupProperties;
	RProcess& iProcess;
	TInt iTimeout;
	MSsTestAsyncNotifier* iSsTestAsyncNotifier; // Not owning
	};

// for inducing error condition
class CDuffTestAndStopper : public CActive
	{
public:
CDuffTestAndStopper( CSsmStartSafe& aSsmStartSafe, 
			 const CSsmStartupProperties &aStartupProperties, 
			 RProcess& aProcess, 
			 TInt aTimeout,
			 MSsTestAsyncNotifier* aSsTestAsyncNotifier = NULL );	
	
	~CDuffTestAndStopper();

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
	
private:
	void DoStartsL();

private:
	RTimer iTimer;
	TBool iRunning;
	TBool iFinish;
	CSsmStartSafe& iSsmStartSafe;
	const CSsmStartupProperties& iStartupProperties;
	RProcess& iProcess;
	TInt iTimeout;
	MSsTestAsyncNotifier* iSsTestAsyncNotifier; // Not owning
	};
	
	
class CTestTwoAndStopper : public CActive
	{
public:
CTestTwoAndStopper( CSsmStartSafe& aSsmStartSafe, 
			 const CSsmStartupProperties &aStartupProperties, 
			 RProcess& aProcess_0, RProcess& aProcess_1,
			 TInt aTimeout,
			 MSsTestAsyncNotifier* aSsTestAsyncNotifier = NULL );	
	
	~CTestTwoAndStopper();

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
	
private:
	void DoStartsL();

private:
	RTimer iTimer;
	TBool iRunning;
	TBool iFinish;
	CSsmStartSafe& iSsmStartSafe;
	const CSsmStartupProperties& iStartupProperties;
	RProcess& iProcess_0;
	RProcess& iProcess_1;
	TInt iTimeout;
	MSsTestAsyncNotifier* iSsTestAsyncNotifier; // Not owning
	};

#endif // __TESTANDSTOPPER_H__
