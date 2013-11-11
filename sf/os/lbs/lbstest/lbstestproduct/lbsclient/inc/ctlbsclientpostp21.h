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



#ifndef __CT_LBS_CLIENT_POS_TP21_H__
#define __CT_LBS_CLIENT_POS_TP21_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 21
*
*/
class CT_LbsClientPosTp21 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
    	CT_LbsClientPosTp21(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp21();

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
        * Setup privacy and notification override settings
        * @param aPsyUid    the uid of the psy
        * @param aDirective how to setup the PSY:s
        *                   Enum def in this file.
        */
        void SetupPsyL(const TUid aPsyUid,
                       TBool aChoice);

    };

#endif      // __CT_LBS_CLIENT_POS_TP21_H__
            
// End of File
