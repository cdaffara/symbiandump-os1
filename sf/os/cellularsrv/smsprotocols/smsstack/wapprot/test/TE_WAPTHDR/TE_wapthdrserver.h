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

#if (!defined __TE_WAPTHDRSERVER_H__)
#define __TE_WAPTHDRSERVER_H__
#include "TE_wapthdrbase.h"
#include <test/testexecuteserverbase.h>

class CWapThdrTestServer : public CTestServer
	{
public:
	static CWapThdrTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
