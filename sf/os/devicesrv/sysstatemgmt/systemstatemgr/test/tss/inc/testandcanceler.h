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



#ifndef __TESTANDCANCELER_H__
#define __TESTANDCANCELER_H__

#include <e32base.h>
#include <e32def.h>

#include <ssm/ssmstartsafe.h>
#include "tss_step_base.h"
#include "ssconst.h"



class CStartAcknowledge : public CActive
	{
public:
	CStartAcknowledge( MSsTestAsyncNotifier* aSsTestAsyncNotifier );
	~CStartAcknowledge();
	
protected:
	void RunL();
	void DoCancel(){}
	
private:
	MSsTestAsyncNotifier* iSsTestAsyncNotifier;
	};



class CStartCanceler : public CActive
	{
public:
	CStartCanceler( TInt& aSsIndex, CSsmStartSafe& aSsmStartSafe, TInt aTimeout );
	~CStartCanceler(){ iTimer.Close(); }

protected:
	void RunL();
	void DoCancel(){}

private:
	CSsmStartSafe& iSsmStartSafe;
	TInt& iSsIndex;
	RTimer iTimer;
	TInt iTimeout;
	};



enum TSsTestDuffBehaviour
	{
	ESsTestDuffBehaviourGood = 0,
	ESsTestDuffBehaviourBadIndex
	};

class CTestAndCanceler : public CActive
	{
public:
	CTestAndCanceler( CSsmStartSafe& aSsmStartSafe, 
					 const CSsmStartupProperties &aStartupProperties, 
					 RProcess& aProcess, 
					 TInt aTimeout,
					 MSsTestAsyncNotifier* aSsTestAsyncNotifier = NULL );

	CTestAndCanceler( CSsmStartSafe& aSsmStartSafe, 
			 const CSsmStartupProperties &aStartupProperties, 
			 RProcess& aProcess, 
			 TInt aTimeout,
			 MSsTestAsyncNotifier* aSsTestAsyncNotifier, TSsTestDuffBehaviour aDuffBehaviour );
	
	~CTestAndCanceler();

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	TInt iCycle; 
	RTimer iTimer;
	CSsmStartSafe& iSsmStartSafe;
	const CSsmStartupProperties& iStartupProperties;
	RProcess& iProcess;
	TInt iTimeout;
	MSsTestAsyncNotifier* iSsTestAsyncNotifier; // Not owning
	TInt iSsIndex;
	CStartCanceler* iStartCanceler;
	TSsTestDuffBehaviour iDuff;
	};



#endif // __TESTANDCANCELER_H__
