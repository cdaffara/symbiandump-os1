/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_lbspositioningstatusstepbase.h
*/

#ifndef TE_LBSPOSITIONINGSTATUSSTEPBASE_H
#define TE_LBSPOSITIONINGSTATUSSTEPBASE_H

#include <test/testexecuteserverbase.h>

_LIT(KStatusStep,"StatusStep");

class CTe_LbsStatusTestManager;

class CTe_LbsPositioningStatusStep : public CTestStep
    {
public:
    virtual ~CTe_LbsPositioningStatusStep();
    virtual TVerdict doTestStepPreambleL(); 
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    
    CTe_LbsPositioningStatusStep();
    
protected:
    CTe_LbsStatusTestManager* iTestManager;
    CActiveScheduler* iSched;
    };

#endif // TE_LBSPOSITIONINGSTATUSSTEPBASE_H
