/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file ced0Step.h
 @internalTechnology
*/
#if (!defined __CED_CHECK_H__)
#define __CED_CHECK_H__
#include <test/testexecutestepbase.h>
#include "Te_cedSuiteStepBase.h"

/**
 * This test step is to check whether the database fillup was successful or not.
 * This can be used after the CED command in each test script only the 
 * LOAD_SUITE te_cedSuite command should be called from the script.
 */
class CcedCheck : public CTe_cedSuiteStepBase
	{
public:
	CcedCheck();
	~CcedCheck();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KcedCheck,"cedCheck");


#endif
