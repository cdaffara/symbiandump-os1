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
 @file ced1Step.h
 @internalTechnology
*/
#if (!defined __CED1_STEP_H__)
#define __CED1_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_cedSuiteStepBase.h"

/**
 * This teststep is executed after appending an IAP record to the existing
 * database. \It tries to read the appended record.
 */
class Cced1Step : public CTe_cedSuiteStepBase
	{
public:
	Cced1Step();
	~Cced1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Kced1Step,"ced1Step");

#endif
