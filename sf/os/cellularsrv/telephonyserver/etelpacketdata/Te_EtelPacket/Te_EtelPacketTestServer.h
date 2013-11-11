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

#ifndef __TE_EtelPacket_TESTSERVER_H__
#define __TE_EtelPacket_TESTSERVER_H__

#include <test/testexecuteserverbase.h>
#include <etelpckt.h>
#include <etel.h>

class CEtelPacketTestServer : public CTestServer
	{
public:
	static CEtelPacketTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	~CEtelPacketTestServer();
		
	
public:
	RTelServer iTelServer;
	RPhone iPhone;
	RPacketService iPacketService;
	RPacketContext iContext;
	TName iContextName;
	
	};

#endif
