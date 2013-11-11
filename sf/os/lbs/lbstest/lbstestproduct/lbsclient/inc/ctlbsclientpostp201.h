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



#ifndef __CT_LBS_CLIENT_POS_TP201_H__
#define __CT_LBS_CLIENT_POS_TP201_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATION

// CLASS DECLARATION

/**
*  Test procedure 201
*
*/
class CT_LbsClientPosTp201 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp201(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp201();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

    private:
        void CheckRequestResultL(TInt aExpectedErrorCode, TRequestStatus& aStatus);
    };

#endif      // __CT_LBS_CLIENT_POS_TP201_H__
            
// End of File
