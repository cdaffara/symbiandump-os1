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



#ifndef __CT_LBS_CLIENT_POS_TP169_H__
#define __CT_LBS_CLIENT_POS_TP169_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 169
*
*/
class CT_LbsClientPosTp169 :public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp169(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp169();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

	private: // New functions

        /**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
        void AssertTrueL(TBool aCondition, const TDesC& errorMsg);

		/**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
		void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);

        /**
		* This testpart tests error report "ESLI-5W9D7M Poison4 - Sort order after full reset"
		* verifies that the Horizontal/Vertical accuracy 0 value is returned as NaN when reading
		* the value from the modulesdb, uses the panic psy which has values 0 for
		* horizontal and vertical accuracy
		*/
		void TestNaNFromPSY();
    };

#endif      // __CT_LBS_CLIENT_POS_TP169_H__

// End of File
