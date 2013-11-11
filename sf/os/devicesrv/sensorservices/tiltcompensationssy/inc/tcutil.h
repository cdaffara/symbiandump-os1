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
* Description:  TCUtil class declaration.
*
*/



#ifndef TCUTIL_H
#define TCUTIL_H

// INCLUDES
#include <e32std.h>

#include "tcevent.h"
#include "tctransaction.h"

// FORWARD DECLARATIONS

class CTCState;

// CLASS DECLARATION

/**
*  Tilt SSY utility class.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TCUtil )
    {
    public:

        // Converts state type to descriptor
        static const TDesC& StateTypeAsDesC( CTCState* aState );

        // Converts state type to descriptor
        static const TDesC& StateIdAsDesC( CTCState* aState );
        
        // Converts event ID to descriptor
        static const TDesC& EventIdAsDesC( TTCEventId aId );
    
        // Converts transaction ID to descriptor
        static const TDesC& TransactionIdAsDesC( TTCTransactionId aId );

    private:
    
        // Deny creation
        TCUtil();
    };

#endif // TCUTIL_H

// End of File
