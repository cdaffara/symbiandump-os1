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
 

#ifndef __TSUS_STEP_PLATSEC_H__
#define __TSUS_STEP_PLATSEC_H__

#include <test/testexecutestepbase.h>

_LIT(KTSusPlatsecStep, "SusPlatsecStep");

/**
 Test that Utility Server require its client to have various capabilites
 */
class CSusPlatsecTest : public CTestStep
	{
public:
	CSusPlatsecTest();
	~CSusPlatsecTest();
protected:
	//from CTestStep
	TVerdict doTestStepL();
	};	

#endif
