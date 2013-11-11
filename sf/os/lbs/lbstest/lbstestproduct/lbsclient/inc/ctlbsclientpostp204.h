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



#ifndef __CT_LBS_CLIENT_POS_TP204_H__ 
#define __CT_LBS_CLIENT_POS_TP204_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
#include "mctlbsrequestobserver.h"

// FORWARD DECLARATIONS
class CBaBackupSessionWrapper;

// CLASS DECLARATION

/**
*  Test procedure 204
*
*/
class CT_LbsClientPosTp204 : public CT_LbsPortedStepBase, public MCT_LbsRequestObserver
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp204(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp204();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();
        
        /**
         * From MCT_LbsRequestObserver
         * Called when a CT_LbsRequester request has completed.
         */
        void RequestCompleteL();

        /**
        * From CT_LbsPortedStepBase
        * Called after the tests
        */
        void CloseTest();

	private: // New functions
        
    private:
        RPositionServer         iPosServer1;
        RPositioner             iPositioner1;
        CPeriodic*              iPeriodicTimer;
        CBaBackupSessionWrapper*    iBackupWrapper;
        TInt            		iCompletedRequests;
    };

#endif      // __CT_LBS_CLIENT_POS_TP204_H__

// End of File
