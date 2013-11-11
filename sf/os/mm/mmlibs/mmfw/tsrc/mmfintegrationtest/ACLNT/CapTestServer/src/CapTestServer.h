/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CapTestServer.h
*/
#ifndef __CAPTESTSERVER_H__
#define __CAPTESTSERVER_H__

#include <simulprocserver.h> 
#include <e32base.h>

class CCapTestServer : public CSimulProcServer
	{
public:
	static CCapTestServer* NewL();
	CSimulProcTestStep* CreateTestStep(const TDesC& aTestStepName) const;
	~CCapTestServer();
	
private:
	CCapTestServer();
	TInt iSessionCount;
	};
#endif

