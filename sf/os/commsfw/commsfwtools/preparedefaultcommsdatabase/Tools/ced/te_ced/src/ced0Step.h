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
#if (!defined __CED0_STEP_H__)
#define __CED0_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_cedSuiteStepBase.h"

/**
 * This case is to check that a given record with a given record number
 * (IAPRecord with recordNumber 3) in not yet in the database.
 */
class Cced0Step : public CTe_cedSuiteStepBase
	{
public:
	Cced0Step();
	~Cced0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Kced0Step,"ced0Step");

#endif
