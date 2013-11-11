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



#ifndef __CT_LBS_CLIENT_POS_TP203_H__ 
#define __CT_LBS_CLIENT_POS_TP203_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
#include "mctlbsrequestobserver.h"

// CLASS DECLARATION

/**
*  Test procedure 203
*
*/
class CT_LbsClientPosTp203 : public CT_LbsPortedStepBase, public MCT_LbsRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp203(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp203();

    public: // Functions from base classes

        static TInt DisablePsyL(TAny* aSelf);

        /**
        * From CT_LbsPortedStepBase
        * Deinitializes the test.
        */
        void CloseTest();

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

        /**
        * From MCT_LbsRequestObserver
        * Called when a CRequester request has completed.
        */
        void RequestCompleteL();

    private: // Data
    
        CPeriodic* iPeriodicTimer;
    };

#endif      // __CT_LBS_CLIENT_POS_TP203_H__
            
// End of File
