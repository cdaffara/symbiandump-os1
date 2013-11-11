/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file man0Step.h
*/
#if (!defined __MAN0_STEP_H__)
#define __MAN0_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_manSuiteStepBase.h"
#include <lbs/epos_intgpshwstatus.h>

class Cman0Step : public CTe_manSuiteStepBase, public MPosIntGpsHwStatusObserver
	{
public:
	Cman0Step();
	~Cman0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// From MPosIntGpsHwStatusObserver
	virtual void OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError);

// Please add/modify your class members here:
private:
	TInt iStepCount;
	};

_LIT(Kman0Step,"man0Step");

#endif
