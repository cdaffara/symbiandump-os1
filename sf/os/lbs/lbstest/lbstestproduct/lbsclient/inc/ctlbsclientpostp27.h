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



#ifndef __CT_LBS_CLIENT_POS_TP27_H__
#define __CT_LBS_CLIENT_POS_TP27_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 27
*
*/
class CT_LbsClientPosTp27 :public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp27(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp27();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Called after the tests
        */
        void CloseTest();

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();
        
	private:

		/**
		* Cancel a position request using the dialog plugin
		* (The request does not reach the PSY since the dialog plugin waits for a cancell call
		*/
		void MakeCancelRequestL(TTimeIntervalMicroSeconds32 aCancelTime, TTimeIntervalMicroSeconds aTimeLimit);

		/**
		* Perform a position request while cancelling two other 
		* notification requests and make sure that Epos does not hang!
		* Using a "slow" psy assures that the position request has reached the Psy
		*/
		void MakeCancelRequest2L(TTimeIntervalMicroSeconds32 aCancelTime, TTimeIntervalMicroSeconds aTimeLimit);

		/**
		* Check that a request that is cancelled when
		* the request has reached the PSY is logged in the requestlog.
		*/ 
		void CheckCancelRequestL();

        /**
        * Enables the specified PSY, does NOT disable any
        */
        void EnablePsyL(const TUid aPsyUid);

        /**
        * Disables all Psys
        */
        void DisableAllPsyL();
    };

#endif      // __CT_LBS_CLIENT_POS_TP27_H__
            
// End of File
