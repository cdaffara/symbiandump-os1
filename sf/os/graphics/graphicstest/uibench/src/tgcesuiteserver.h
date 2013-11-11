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
// *** For test case ID allocation update spreadsheet here in perforce: 				***
// *** //EPOC/DV3/team/2005/uif_graphics_doc/TS0002/Test/Test Case ID Reserve List.xls 	***
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TGCESUITESERVER_H__
#define __TGCESUITESERVER_H__

#include <test/testexecuteserverbase.h>

class CGceSuiteServer : public CTestServer
	{
public:
	static CGceSuiteServer* NewL();
	// from CTestServer
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};

#endif
