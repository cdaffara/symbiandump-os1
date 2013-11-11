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

#if (!defined __TAMASTARTER_STEP_INVALIDSID_H__)
#define __TAMASTARTER_STEP_INVALIDSID_H__


#include <test/testexecutestepbase.h>


class CAppfwkAmaStarterTestStepInvalidSid : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepInvalidSid(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepInvalidSid();
	
	//from CTestStep	
	TVerdict doTestStepL();	

private:
	void LaunchExe(const TDesC& aFilename, const TDesC& aArguments, TInt aExitReason);
	};

_LIT(KAppfwkAmaStarterInvalidSID,"AmaStarterInvalidSID");

#endif


