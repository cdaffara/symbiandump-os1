/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_CLIENT_POS_TP194_H__
#define __CT_LBS_CLIENT_POS_TP194_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATION

// CLASS DECLARATION

/**
*  Test procedure 194
*
*/
class CT_LbsClientPosTp194 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp194(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp194();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();
    };

#endif      // __CT_LBS_CLIENT_POS_TP194_H__
            
// End of File
