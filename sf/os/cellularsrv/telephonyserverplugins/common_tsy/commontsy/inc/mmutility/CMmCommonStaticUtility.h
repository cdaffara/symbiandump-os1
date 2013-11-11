/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMCOMMONSTATICUTILITY_H
#define CMMCOMMONSTATICUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <e32math.h>
#include <etelpckt.h>
#include "CMmPrivateUtility.h"
#include <ctsy/serviceapi/cmmutility.h>


// CLASS DECLARATION

/**
 *  Common Static Utility methods
 */
class CMmCommonStaticUtility : public CBase
    {
    public:

        /**
         * Stores the core error code to lower 16 bits and adds the extended 
         * error code to higher 16 bits and returns the error code that can be
         * sent to ETel Server.
         *         
         * 
         * @param aCoreErrorCode Core error code
         * @param aExtendedErrorCode Extended error code
         * @return TInt Error code sent to ETEL
         */     
        static TInt EpocErrorCode( TInt aCoreErrorCode,
            TInt aExtendedErrorCode );
        
        /**
         * Returns higher 16 bits (extended error code) of the Epoc error code.
         *         
         * 
         * @param EpocErrorCode EPOC error code
         * @return TInt Extended error code
         */    
        static TInt ExtendedErrorCode( TInt EpocErrorCode );
    
    };

#endif // CMMSTATICUTILITY_H

//  End of file
