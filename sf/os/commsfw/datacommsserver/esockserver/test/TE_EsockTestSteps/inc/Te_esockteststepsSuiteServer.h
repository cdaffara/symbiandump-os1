/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_esockteststepsSuiteServer.h
*/

#if (!defined TE_ESOCKTESTSTEPS_SERVER_H)
#define TE_ESOCKTESTSTEPS_SERVER_H
#include <test/testexecuteserverbase.h>

class CCEsockTestBase;
class CTe_esockteststepsSuite : public CTestServer
	{
public:
	static CTe_esockteststepsSuite* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CTe_esockteststepsSuite();

protected:
	CTe_esockteststepsSuite();

private:
	const TPtrC ServerName() const;

private:
    CCEsockTestBase* iEsockTest;
	};

#endif

