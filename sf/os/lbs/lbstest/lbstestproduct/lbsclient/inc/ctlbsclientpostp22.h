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



#ifndef __CT_LBS_CLIENT_POS_TP22_H__
#define __CT_LBS_CLIENT_POS_TP22_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATION
class CT_LbsClientPeriodicTester;

// CLASS DECLARATION

/**
*  Test procedure 22
*
*/
class CT_LbsClientPosTp22 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsClientPosTp22(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp22();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

		/**
        * From CT_LbsPortedStepBase
        * Called at close down of test
        */
		void CloseTest();
	
	private: // Data
		RArray<TInt> iRequestArray;
		CT_LbsClientPeriodicTester* iPeriodicTester1;
		CT_LbsClientPeriodicTester* iPeriodicTester2;
		
    };

#endif      // __CT_LBS_CLIENT_POS_TP22_H__

// End of File
