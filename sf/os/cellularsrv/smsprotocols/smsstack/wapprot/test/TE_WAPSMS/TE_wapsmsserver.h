// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#if (!defined __TE_WAPSMSSERVER_H__)
#define __TE_WAPSMSSERVER_H__
#include "TE_wapsmsbase.h"
#include <test/testexecuteserverbase.h>

class CWapSmsTestServer : public CTestServer
	{
public:
	static CWapSmsTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
