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
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __ASSTOPPER_H__
#define __ASSTOPPER_H__

#include <e32base.h>
#include <ssm/ssmstartsafe.h>


const TInt KAsStopperDelay = 2000000; //us
const TInt KSsAsStopperNominalTimeout = 2000000;

class MSsTestAsyncNotifier;
class CAsStopper : public CActive
	{
public:
	CAsStopper( TInt aTimeout );
	CAsStopper( TInt aTimeout, MSsTestAsyncNotifier* aNotify );
	~CAsStopper();
	
	TRequestStatus& Trs();
	TInt CompletionCode();
	
protected:
	void RunL();
	void DoCancel();
	
private:
	RTimer iTimer;
	TInt iTimeout; // us
	TBool iRunning;
	TInt iCompletionCode; // The first completion code
	MSsTestAsyncNotifier* iNotify; // Not owning
	};


	
class CAsPauseStopper : public CActive
	{
public:
CAsPauseStopper( TInt aTimeout );
	~CAsPauseStopper();
	
protected:
	void RunL();
	void DoCancel();
	
private:
	RTimer iTimer;
	TInt iTimeout; // us
	TBool iRunning;
	};	
	
	

class CSsPauseDeleter : public CActive
	{
public:
CSsPauseDeleter( TInt aTimeout, CSsmStartSafe* aSsmStartSafe );
	~CSsPauseDeleter();
	
protected:
	void RunL();
	void DoCancel();
	
private:
	RTimer iTimer;
	TInt iTimeout; // us
	CSsmStartSafe* iSsmStartSafe; // Not owning
	};	
	
	
#endif //__ASSTOPPER_H__



