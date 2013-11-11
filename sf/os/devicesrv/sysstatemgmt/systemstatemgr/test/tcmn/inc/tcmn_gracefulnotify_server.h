// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Server for handling graceful notification incase of state change
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __TCMN_GRACEFUL_NOTIFY_SERVER_H__
#define __TCMN_GRACEFUL_NOTIFY_SERVER_H__

#include <test/testblockcontroller.h>
#include <test/testserver2.h>
#include <test/testexecuteserverbase.h>

class CAwareSessionTestBlock : public CTestBlockController
	{
public:
    CAwareSessionTestBlock() : CTestBlockController() {}
	~CAwareSessionTestBlock() {}

	CDataWrapper* CreateDataL(const TDesC& aData);
	};

	
class CAwareSessionTestServer : public CTestServer2
	{
public:
	CAwareSessionTestServer() {}
	~CAwareSessionTestServer() {}

	static CAwareSessionTestServer* NewL();
	CTestBlockController*	CreateTestBlock();	
	};

#endif //__TCMN_GRACEFUL_NOTIFY_SERVER_H__
