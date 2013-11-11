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



#ifndef __CT_LBS_CLIENT_POS_TP224_H__
#define __CT_LBS_CLIENT_POS_TP224_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 224
*
*/
class CT_LbsClientPosTp224 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp224(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp224();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

        /**
        * From CT_LbsPortedStepBase
        * Deinitializes the test.
        */
        void CloseTest();

	private: // New functions

		/**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
		void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);
    
        RPositionServer iServer;
        RPositioner     iPositioner;
        RPositioner     iPositioner2;
   
    };

#endif      // __CT_LBS_CLIENT_POS_TP224_H__

// End of File
