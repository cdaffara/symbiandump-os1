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
// Telephony Watchers Integration Test server header file.
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERSINTSERVER_H__
#define __TE_TELWATCHERSINTSERVER_H__

#include <test/testexecuteserverbase.h>
#include "TE_TelWatchersIntBase.h"

class CTelWatchersIntTestServer : public CTestServer
	{
public:
	static CTelWatchersIntTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};

#endif // __TE_TELWATCHERSINTSERVER_H__
