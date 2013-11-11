// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// lbsbtgpsconfig unit test: Test server
//

#ifndef TE_LBS_BTGPSCONFIGSERVER_H
#define TE_LBS_BTGPSCONFIGSERVER_H

#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>
#include <e32test.h>
#include <bttypes.h> 
#include <test/TestExecuteStepBase.h>
#include <test/TestExecuteServerBase.h>
//#include <lbsprocesssupervisor.h>


// The minimum limit to the number of devices that the list should support
const TInt KMinDeviceLimit = 10;


class CLbsBtGpsConfigTestServer : public CTestServer
	{
public:
	static CLbsBtGpsConfigTestServer* NewL();
	~CLbsBtGpsConfigTestServer();

public:
	
	CActiveScheduler* ActiveScheduler();
	
	TBTDevAddr GenerateUniqueBTDevAddr();
	
	CTestStep* CreateTestStep(const TDesC& aStepName);
	
private:
	void ConstructL(const TDesC& aName);
	
private:
	CActiveScheduler* iScheduler;
	
	TInt iUniqueAddr;
	
	};


#endif //LBS_TE_BTGPSCONFIGSERVER_H
