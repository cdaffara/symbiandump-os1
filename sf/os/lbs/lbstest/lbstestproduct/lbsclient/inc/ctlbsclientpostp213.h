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



#ifndef __CT_LBS_CLIENT_POS_TP213_H__
#define __CT_LBS_CLIENT_POS_TP213_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
#include "mctlbsrequestobserver.h"

// CONSTANTS

// FORWARD DECLARATION
class CT_LbsClientRequestor;
class CT_LbsClientEventTimer;
class CPosModules;

// CLASS DECLARATION

/**
*  Test procedure 213
*
*/
class CT_LbsClientPosTp213 : public CT_LbsPortedStepBase, public MCT_LbsRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp213(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp213();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

		/**
        * From CT_LbsPortedStepBase
        * Called after the tests
        */
        void CloseTest();

		/**
        * From MCT_LbsRequestObserver
        * Called when a CRequester request has completed.
        */
        void RequestCompleteL();
			
    private:

		void EnablePsyL(
			CPosModules&   aDb,
			TUid             aPsy);
			

		void DisablePsyL(
			CPosModules&   aDb,
			TUid             aPsy);
		
    private:

        void CheckRequestResultL(TInt aExpectedErrorCode, TRequestStatus& aStatus);

        CT_LbsClientRequestor*       iclient1;
		CT_LbsClientRequestor*       iclient2;
		CT_LbsClientRequestor*       iclient3;
		CT_LbsClientEventTimer*		    ieventtimer;
		TInt					iCompletedRequests;
		TBool					iNoFurtherEvents;
		TBool					iErrorDuringExecution;
    };

#endif      // __CT_LBS_CLIENT_POS_TP213_H__
            
// End of File
