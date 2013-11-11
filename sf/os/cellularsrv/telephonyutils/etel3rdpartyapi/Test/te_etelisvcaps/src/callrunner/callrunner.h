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
// Class defintion of test purpose utility application. 
// It establishes mobile orginated calls on the system. 
// The command line parameter accepts an integer parameter, 
// which specifies the number of calls to be created.  
// 
//

/**
 @file 
 @internalTechnology
*/
 
#ifndef __CALL_RUNNER_H__
#define __CALL_RUNNER_H__

#include <e32base.h>
#include <etel3rdparty.h>
#include <e32property.h>

/**
The CCallRunner class defines the simple interface to establish calls. 
It retrieves the command line parameter to identify the number of calls 
to create. After the calls are established it waits on RProperty until the 
client notifies to hang up requested calls.   
*/
class CCallRunner
{
public:
	static CCallRunner* NewL();
	virtual ~CCallRunner();
	void WaitToClose();
		
private:
	CCallRunner() {};
	void ConstructL();

private:
	CTelephony* iTelephony;
	RProperty iCallRunnerProperty;
	TInt iNumberOfCalls;
};

#endif //__CALL_RUNNER_H__
