/*
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
 @file 
 @internalTechnology
 @test
*/

#ifndef __CT_LBSTESTSUITE_SERVER_H__
#define __CT_LBSTESTSUITE_SERVER_H__
#include <test/testexecuteserverbase.h>

#include "ctlbsserver.h"

class CTe_TestSuite : public CT_LbsServer
	{
public:
	static CTe_TestSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	virtual ~CTe_TestSuite();
	CTe_TestSuite();

protected:
	void ConstructL();
private:
	//CTe_SystemStarter iStarter;
	};

#endif //__CT_LBSTESTSUITE_SERVER_H__
