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
* Description:  TTCTransaction class declaration.
*
*/



#ifndef TCTRANSACTION_H
#define TCTRANSACTION_H

// INCLUDES
#include <e32std.h>

// DATA TYPES

/**
* Transaction IDs.
*/
enum TTCTransactionId
    {
    ETCTransactionIdNull,
    ETCTransactionIdOpenChannel,
    ETCTransactionIdCloseChannel,
    ETCTransactionIdStartChannelData,
    ETCTransactionIdStopChannelData,
    ETCTransactionIdFlushBuffer,
    TTCTransactionIdLast
    };
    
// CLASS DECLARATION

/**
*  Transaction base class.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCTransaction )
    {
    public:
    
        /**
        * Return transaction ID.
        *
        * @return TTCTransactionId
        */
        TTCTransactionId Id() const;
        
    protected:
    
        /**
        * Default C++ constructor.
        * 
        * @param aId Transaction ID.
        */
        TTCTransaction( TTCTransactionId aId );
        
    private: // Data
    
        /** Transaction ID */
        TTCTransactionId iId;
    };

#endif // TCTRANSACTION_H

// End of File
