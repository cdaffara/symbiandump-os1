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
// @file
// 
//

#ifndef TE_SMSCAPSSERVER_H
#define TE_SMSCAPSSERVER_H

#include <es_sock.h>

#include "smsstackbasetestserver.h"

class CSmsCapsTestServer : public CSmsStackTestServer
	{
public:
	static CSmsCapsTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	void ConstructL(const TDesC& aName);
	~CSmsCapsTestServer();
private:
	RSocketServ iSocketServer;
	RFs iFs;
	};

#endif // TE_SMSCAPSSERVER_H
