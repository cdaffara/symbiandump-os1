/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CT_LBS_CLIENT_STEP_MULTI_INTGPS_FALLBACK_H__
#define __CT_LBS_CLIENT_STEP_MULTI_INTGPS_FALLBACK_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CLASS DECLARATION

/**
*  Test procedure 304
*/
class CT_LbsClientStepMultiIntGpsFallback : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientStepMultiIntGpsFallback(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientStepMultiIntGpsFallback();

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
        
        void SetupProxyPSYsL();
    };

#endif      // __CT_LBS_CLIENT_STEP_MULTI_INTGPS_FALLBACK_H__
            
// End of File
