/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSREQUESTORLISTENER_H
#define MPOSREQUESTORLISTENER_H

#include <e32base.h>
#include <lbs.h>

class TPositionInfoBase;

/**
* The MPosRequestorListener is the interface to receive location request result.
*/
class MPosRequestorListener
    {
    public: 
              
        /**
        * Notify the listener that location request has
        * been completed.
        *
        * @param aModuleId The module ID of this requestor
        * @param aErr The location request completion error code.
        * @param aPosInfo when aErr is not a error code(non-
        * negative number), this reference include the position
        * information.
        */
        virtual void LocationRequestCompleted( 
            TPositionModuleId aModuleId,
            TInt aErr, 
            const TPositionInfoBase& aPosInfo,
            TBool aIsPosInfoUpToDate ) = 0;

    };

#endif      // MPOSREQUESTORLISTENER_H  
            
// End of File
