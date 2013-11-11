// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __TINTCONDITIONEVALUATOR_SERVER_H__
#define __TINTCONDITIONEVALUATOR_SERVER_H__

#include <test/testblockcontroller.h>
#include <test/testserver2.h>
#include <test/testexecuteserverbase.h>

class CCondCmdTestBlock : public CTestBlockController
	{
public:
	CCondCmdTestBlock() : CTestBlockController() {}
	~CCondCmdTestBlock() {}

	CDataWrapper* CreateDataL(const TDesC& aData);
	};

	
class CCondCmdTestServer : public CTestServer2
	{
public:
	CCondCmdTestServer() {}
	~CCondCmdTestServer() {}

	static CCondCmdTestServer* NewL();
	CTestBlockController*	CreateTestBlock();	
	};

#endif
