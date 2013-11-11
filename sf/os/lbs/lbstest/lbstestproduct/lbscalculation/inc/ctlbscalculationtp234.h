/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __CT_LBS_CALCULATION_TP234_H__
#define __CT_LBS_CALCULATION_TP234_H__

//  INCLUDES
#include "ctlbscalculationstepbase.h"

// Literals used
_LIT(KLbsCalculationCalculateBearing, "LbsCalculationCalculateBearing");

// CLASS DECLARATION
class CT_LbsCalculationTP234 : public CT_LbsCalculationStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CT_LbsCalculationTP234(CT_LbsCalculationServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsCalculationTP234();

        static CT_LbsCalculationTP234* New(CT_LbsCalculationServer& aParent);

    public: // Functions from base classes

        TVerdict doTestStepL();

    private:
    
        void TestValuesL();

        void TestNotSupportedL();

        void LogTestError(const T_LbsCalculationCheck& aCalc, 
                          const TInt aError,
                          const TInt aRow=0);
    };

#endif      // __CT_LBS_CALCULATION_TP234_H__
            
// End of File
