// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tadaptation_plugin.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __T_IADAPTATIONPLUGIN_H__
#define __T_IADAPTATIONPLUGIN_H__

#include <test/testexecuteserverbase.h>

class CAdaptationPluginTestServer : public CTestServer
	{
public:
	static CAdaptationPluginTestServer* NewLC();

	//from CTestServer
	CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // __T_ADAPTATIONPLUGIN_H__
