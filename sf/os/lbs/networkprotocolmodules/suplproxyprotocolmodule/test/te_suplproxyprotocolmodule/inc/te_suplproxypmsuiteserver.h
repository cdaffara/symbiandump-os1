/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_suplproxypmsuiteserver.h
*/

#ifndef TESUPLPROXYPMSUITESERVER_H
#define TESUPLPROXYPMSUITESERVER_H

#include <test/testexecuteserverbase.h>


class CTe_SuplProxyPMSuite : public CTestServer
	{
public:
	static CTe_SuplProxyPMSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

// Please Add/modify your class members
private:
	};

#endif
