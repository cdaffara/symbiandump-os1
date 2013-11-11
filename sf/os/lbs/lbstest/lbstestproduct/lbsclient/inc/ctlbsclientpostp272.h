/*
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



#ifndef __CT_LBS_CLIENT_POS_TP272_H__
#define __CT_LBS_CLIENT_POS_TP272_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CLASS DECLARATION

/**
*  Test procedure 272
*/
class CT_LbsClientPosTp272 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp272(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp272();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

        /**
        * From CT_LbsPortedStepBase
        * Always called, even if the test leaves
        */
        void CloseTest();

    private: // Functions
        
        void RequestL(const TInt& aRequestIndex, 
                      const TInt64& aMinTimeLimit,
                      const TInt64& aMaxTimeLimit,
                      const TInt& aPeriodicUpdateInterval=0);

        void SetPeriodicUpdateL(const TInt& aUpdateInterval);

        void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode=KErrGeneral);

    private: // data
             
    };

#endif      // __CT_LBS_CLIENT_POS_TP272_H__
            
// End of File
