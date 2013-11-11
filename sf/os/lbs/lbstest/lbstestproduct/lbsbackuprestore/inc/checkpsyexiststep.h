/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CheckPSYExistStep.h
 @internalTechnology
*/
#ifndef __CHECKPSYEXISTSTEP_H__
#define __CHECKPSYEXISTSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsbackuprestoresuitestepbase.h"

class CCheckPSYExistStep : public CTe_lbsbackuprestoreSuiteStepBase
    {
public:
    CCheckPSYExistStep();
    ~CCheckPSYExistStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
	static CCheckPSYExistStep* NewL();
	static CCheckPSYExistStep* NewLC();
	
    };

_LIT(KCheckPSYExistStep,"CheckPSYExistStep");

#endif
