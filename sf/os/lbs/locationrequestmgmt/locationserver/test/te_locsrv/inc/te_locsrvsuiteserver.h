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
 @file Te_locsrvSuiteServer.h
 @internalTechnology
*/

#ifndef __TE_LOCSRV_SERVER_H__
#define __TE_LOCSRV_SERVER_H__
#include <test/testexecuteserverbase.h>
#include "te_systemstarter.h" 


class CTe_locsrvSuite : public CTestServer
	{
public:
	static CTe_locsrvSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	virtual ~CTe_locsrvSuite();

// Please Add/modify your class members
protected:
	void ConstructL();
private:
	CTe_SystemStarter iStarter;
	};

#endif
