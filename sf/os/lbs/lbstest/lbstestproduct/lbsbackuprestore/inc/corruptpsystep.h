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
 @file CorruptPSYStep.h
 @internalTechnology
*/
#ifndef __CORRUPTPSYSTEP_H__
#define __CORRUPTPSYSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsbackuprestoresuitestepbase.h"

class CCorruptPSYStep : public CTe_lbsbackuprestoreSuiteStepBase
    {
public:
    CCorruptPSYStep();
    ~CCorruptPSYStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
	static CCorruptPSYStep* NewL();
	static CCorruptPSYStep* NewLC();
	
    };

_LIT(KCorruptPSYStep,"CorruptPSYStep");

#endif
