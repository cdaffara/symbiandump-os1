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
// tutilityplugin_srver.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TUTILITY_PLUGIN_TEST_SERVER_H__
#define __TUTILITY_PLUGIN_TEST_SERVER_H__

#include <test/testexecuteserverbase.h>

class CUtilityPluginTestServer : public CTestServer
	{
public:
	static CUtilityPluginTestServer * NewLC();

	//from CTestServer
	CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif		//__TUTILITY_PLUGIN_TEST_SERVER_H__
