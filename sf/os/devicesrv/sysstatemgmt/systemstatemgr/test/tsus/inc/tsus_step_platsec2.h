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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __TSUS_STEP_PLATSEC2_H__
#define __TSUS_STEP_PLATSEC2_H__

#include <test/testexecutestepbase.h>

_LIT(KTSusPlatsecStepProtSrvBegin, "SusPlatsecStepProtSrvBegin");
_LIT(KTSusPlatsecStepProtSrv,      "SusPlatsecStepProtSrv");
_LIT(KTSusPlatsecStepProtSrvEnd,   "SusPlatsecStepProtSrvEnd");

/**
 Tef step that starts the Utility Server with test-name
 */
class CSusPlatsecTestProtSrvBegin : public CTestStep
	{
public:
	CSusPlatsecTestProtSrvBegin();
	~CSusPlatsecTestProtSrvBegin();
protected:
	//from CTestStep
	TVerdict doTestStepL();
	};	

/**
 Test that Utility Server require its client to have ProtSrv
 */
class CSusPlatsecTestProtSrv : public CTestStep
	{
public:
	CSusPlatsecTestProtSrv();
	~CSusPlatsecTestProtSrv();
protected:
	//from CTestStep
	TVerdict doTestStepL();
	};	
	
/**
 Tef step that kills the Utility Server with test-name
 */
class CSusPlatsecTestProtSrvEnd : public CTestStep
	{
public:
	CSusPlatsecTestProtSrvEnd();
	~CSusPlatsecTestProtSrvEnd();
protected:
	//from CTestStep
	TVerdict doTestStepL();
	};	

#endif
