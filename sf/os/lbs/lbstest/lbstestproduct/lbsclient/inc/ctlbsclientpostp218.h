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



#ifndef __CT_LBS_CLIENT_POS_TP218_H__
#define __CT_LBS_CLIENT_POS_TP218_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 218
*
*/
class CT_LbsClientPosTp218 :public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp218(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp218();

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

        /*
        * Setup the choosen default Psy
        */
        void SetupDefaultPsyL(const TUid aPsyUid);
    
    private:

        RPositioner iPositioner1;
        RPositioner iPositioner2;

    };

#endif      // __CT_LBS_CLIENT_POS_TP218_H__

// End of File
