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

#ifndef __TSTARTSAFE_APPTIMESUCC_H__
#define __TSTARTSAFE_APPTIMESUCC_H__


#include <test/testexecutestepbase.h>


//APPFWK-STARTSAFE-0006

_LIT(KCTestCaseAppTimeSucc, "AppTimeSucc");


class CAppFwkStartSafeTestStepAppTimeSucc : public CTestStep
	{
	
public:
	CAppFwkStartSafeTestStepAppTimeSucc();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	};

#endif
