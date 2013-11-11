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



#ifndef __CT_LBS_CLIENT_POS_TP178_H__
#define __CT_LBS_CLIENT_POS_TP178_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 178
*
*/
class CT_LbsClientPosTp178 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp178(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp178();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

        /**
        * From CT_LbsPortedStepBase
        * Always called after the test, even if the test leaves
        */
        void CloseTest();

	private: // New functions

        /**
        * Test that the positioner does a timeout 
        * after the set update interval and that the request is not logged 
        * in the request log
        */
        void TestTimeoutL();

        /**
        * Test that it is possible to cancel
        * an outstanding request, and that the request is not logged 
        * in the request log
        */
        void TestCancelL();

        void TestOptionL();

	private:
    };

#endif      // __CT_LBS_CLIENT_POS_TP178_H__

// End of File
