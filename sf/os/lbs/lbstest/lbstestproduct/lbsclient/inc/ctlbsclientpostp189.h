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



#ifndef __CT_LBS_CLIENT_POS_TP189_H__
#define __CT_LBS_CLIENT_POS_TP189_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CONSTANTS

// CLASS DECLARATION

/**
*  Test procedure 189
*
*/
class CT_LbsClientPosTp189 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp189(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp189();
	
	public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

	private: // New functions
		
		/**
		* Prints the current Module status to log
		* @param aStatus The current status reported to write to log
		*/
		void CheckModuleStatusL(
		/* IN */ const TPositionModuleStatus& aStatus,
        /* IN */ const TBool aCheckStatus = EFalse
		);
		
		/**
		* Requests to be notified about certain events, wait for that event
		* and log the event
		* @param aEventToListenFor The event to listen for (EEventDataQualityStatus/EEventDeviceStatus)
		*/
		void RequestWaitAndLogEventL(
		/* IN */ const TInt aEventToListenFor
		);

	private: // Data
	
		RPositioner				iPositioner2;
        TPositionModuleStatus   iExpectedStatus;
    };

#endif      // __CT_LBS_CLIENT_POS_TP189_H__

// End of File
