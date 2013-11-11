/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TTCStateTilt class declaration.
*
*/



#ifndef TCSTATETILT_H
#define TCSTATETILT_H

// INCLUDES
#include "tcstate.h"

// CLASS DECLARATION

/**
*  Base class for all tilt channel (CTCChannelTilt) states.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCStateTilt ): public CTCState
    {
    protected:
    
        /**
        * Default C++ constructor.
        *
        * @param aId
        * @param aStatePool
        * @param aTransactionHandler
        */
        CTCStateTilt( TCStateId aId,
            MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    };

#endif // TCSTATETILT_H

// End of File
