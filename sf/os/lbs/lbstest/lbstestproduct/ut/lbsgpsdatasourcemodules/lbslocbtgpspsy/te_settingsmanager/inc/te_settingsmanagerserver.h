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
 @file te_settingsmanagerserver.h
 @internalTechnology
*/

#ifndef TE_SETTINGSMANAGER_SERVER_H
#define TE_SETTINGSMANAGER_SERVER_H

#include <test/TestExecuteServerBase.h>
#include "BTGPSLogging.h"
#include "te_settingsmanagerdefs.h"

class CTe_settingsmanager : public CTestServer
	{
public:
	static CTe_settingsmanager* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	void AddHeader();
private:
	};

#endif
