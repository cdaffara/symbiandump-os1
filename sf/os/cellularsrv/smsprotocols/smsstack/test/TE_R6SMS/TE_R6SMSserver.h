// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


#if (!defined __TE_R6SMSSERVER_H__)
#define __TE_R6SMSSERVER_H__
#include "TE_R6SMSbase.h"
#include <test/testexecuteserverbase.h>

class CR6SMSTestServer : public CTestServer
	{
public:
	static CR6SMSTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
