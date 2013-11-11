// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <startupproperties.h>

#include "startsafe.h"

#include "tstartsafe_asyncnotifier.h"

class CTestAndStopper : public CActive
	{
public:
	CTestAndStopper( CStartSafe& aStartSafe, 
			 const CStartupProperties &aStartupProperties, 
			 RProcess& aProcess, 
			 TInt aTimeout,
			 TInt& aNumTried,
			 MSsTestAsyncNotifier* aSsTestAsyncNotifier = NULL );	
	
	~CTestAndStopper();

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
	
private:
	RTimer iTimer;
	TBool iRunning;
	TBool iFinish;
	CStartSafe& iStartSafe;
	const CStartupProperties& iStartupProperties;
	RProcess& iProcess;
	TInt iTimeout;
	TInt& iNumTried;
	MSsTestAsyncNotifier* iSsTestAsyncNotifier; // Not owning
	};

#endif // __TESTANDSTOPPER_H__
