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



#ifndef __CT_LBS_CLIENT_POS_TP208_H__ 
#define __CT_LBS_CLIENT_POS_TP208_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 208
*
*/
class CT_LbsClientPosTp208 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp208(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp208();

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

    protected: // New functions

        /**
        * 
        * Starts  nr of threads all connecting to the
        * Epos server. If aFullTest also does 
        * a number of position requests
        */
        TInt StartThreads(TBool aFullTest);

        /**
        *Fast connection only connects to epos no
        *tracing no nothing. Verification of EJSH-5WJG82
        */
        TInt DoFastConnectionL();
        
    private:
    
    	TInt iThreadCount;  

    };

#endif      // __CT_LBS_CLIENT_POS_TP208_H__
            
// End of File
