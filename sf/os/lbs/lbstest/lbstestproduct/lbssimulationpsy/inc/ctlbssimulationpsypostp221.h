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



#ifndef __CT_LBS_SIMULATION_PSY_POS_TP221_H__
#define __CT_LBS_SIMULATION_PSY_POS_TP221_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Test procedure 221
*
*/
class CT_LbsSimulationPSYPosTp221 :public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsSimulationPSYPosTp221(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsSimulationPSYPosTp221();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();
        
        /**
		* Allways called when shut down test 
		*/
		void CloseTest();


	private: // New functions

        /**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
        void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg);

    private: // Data

    };

#endif      // __CT_LBS_SIMULATION_PSY_POS_TP221_H__

// End of File
