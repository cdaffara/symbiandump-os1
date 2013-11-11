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

#ifndef TE_SMSEMSPRTSERVER_H
#define TE_SMSEMSPRTSERVER_H

#include "smsstackbasetestserver.h"

class CSmsEmsPrtTestServer : public CSmsStackTestServer
	{
public:
	static CSmsEmsPrtTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // TE_SMSEMSPRTSERVER_H
