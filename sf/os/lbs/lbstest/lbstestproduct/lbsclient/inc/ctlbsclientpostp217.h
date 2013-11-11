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



#ifndef __CT_LBS_CLIENT_POS_TP217_H__
#define __CT_LBS_CLIENT_POS_TP217_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 217
*
*/
class CT_LbsClientPosTp217 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp217(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp217();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Deinitializes the test.
        */
        void CloseTest();

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
		void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);

        /**
        * Compares two TPositionInfoBase objects
        * @return returns true if equal, otherwise false
        */
        TBool ComparePositionL(TPositionInfo aPosInfo1, TPositionInfo aPosInfo2);

        /**
        * Test that it is not possible to set erroneous maxage and updateInterval values
        */
        void ExtendedTesting1();

        /**
        * Check that it is possible to change MaxAge
        * when the positioner already has a value
        */
        void ExtendedTesting2();
    
    private:
    
    };

#endif      // __CT_LBS_CLIENT_POS_TP217_H__

// End of File
