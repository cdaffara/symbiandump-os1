/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file mmddtestserver.h
*/
#ifndef __MMDDTESTSERVER_H__
#define __MMDDTESTSERVER_H__

#include <simulprocserver.h> 
#include <e32base.h>

class CMMDDTestServer : public CSimulProcServer
	{
public:
	static CMMDDTestServer* NewL();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	CSimulProcTestStep* CreateTestStep(const TDesC& aTestStepName) const;
	~CMMDDTestServer();
	
private:
	CMMDDTestServer();
	TInt iSessionCount;
	TThreadId iClientThreadId;
	};
#endif

