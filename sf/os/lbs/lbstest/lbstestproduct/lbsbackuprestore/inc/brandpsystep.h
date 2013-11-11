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
 @file BrandPSYStep.h
 @internalTechnology
*/
#ifndef __BRANDPSYSTEP_H__
#define __BRANDPSYSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsbackuprestoresuitestepbase.h"

class CBrandPSYStep : public CTe_lbsbackuprestoreSuiteStepBase
    {
public:
    CBrandPSYStep();
    ~CBrandPSYStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
	static CBrandPSYStep* NewL();
	static CBrandPSYStep* NewLC();

    };

_LIT(KBrandPSYStep,"BrandPSYStep");

#endif
