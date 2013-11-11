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
 @file ReferenceLocationAltitudeStep.h
*/
#if (!defined __REFERENCELOCATIONALTITUDE_STEP_H__)
#define __REFERENCELOCATIONALTITUDE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"


class CReferenceLocationAltitudeStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
    CReferenceLocationAltitudeStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CReferenceLocationAltitudeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KReferenceLocationAltitudeStep,"ReferenceLocationAltitudeStep");

#endif
