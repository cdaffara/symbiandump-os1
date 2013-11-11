// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Telephony Watchers Unit Test server header file.
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERSUNITSERVER_H__
#define __TE_TELWATCHERSUNITSERVER_H__

#include <test/testexecuteserverbase.h>
#include "TE_TelWatchersUnitBase.h"

class CTelWatchersUnitTestServer : public CTestServer
	{
public:
	static CTelWatchersUnitTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};

#endif // __TE_TELWATCHERSUNITSERVER_H__
