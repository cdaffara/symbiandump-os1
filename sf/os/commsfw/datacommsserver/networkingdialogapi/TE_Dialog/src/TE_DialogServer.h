/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TE_DialogServer.h
*/

#if (!defined __TE_DIALOGSERVER_H__)
#define __TE_DIALOGSERVER_H__

//Test Framework Header
#include <test/testexecuteserverbase.h>



class CDialogServer : public CTestServer
	{
public:
	static CDialogServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};
#endif  // TS_DIALOGSERVER_H

