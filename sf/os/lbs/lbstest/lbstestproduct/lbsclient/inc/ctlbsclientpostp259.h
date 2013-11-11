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



#ifndef __CT_LBS_CLIENT_POS_TP259_H__
#define __CT_LBS_CLIENT_POS_TP259_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 259
*
*/
class CT_LbsClientPosTp259 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsClientPosTp259(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp259();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Deinitializes the test. 
        * Always called even if test leaves.
        */
        void CloseTest();

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

	private: // New functions
        
        /**
        * First call the RPosServer::CancelRequest method with inparameter 
        * EPositionerNotifyPositionUpdate and then call the RPositioner::CancelRequest
        * method with EPositionServerNotifyModuleStatusEvent
        */
        void TestPart1L();
        
        /**
        * Test some cases not included in TestPart1L
        */
        void TestPart2L();

    };

#endif      // __CT_LBS_CLIENT_POS_TP259_H__

// End of File
