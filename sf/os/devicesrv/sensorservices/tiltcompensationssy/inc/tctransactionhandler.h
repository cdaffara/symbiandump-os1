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
* Description:  MTCStatePool interface declaration.
*
*/



#ifndef TCTRANSACTIONHANDLER_H
#define TCTRANSACTIONHANDLER_H

// INCLUDES
#include <e32std.h>
#include <sensrvtypes.h>

#include "tctransaction.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Transaction handler. States can utilize transaction
*  handler to execute specific actions.
*  All transactions are asynchronous.
*
*  Transaction handler also provides needed data model for
*  state classes.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( MTCTransactionHandler )
    {
    public:
    
        /**
        * Process a transaction.
        *
        * @param aId Transaction ID.
        * @param aTransaction Transaction object.
        */
        virtual void ProcessTransactionL( TTCTransactionId aId,
            TTCTransaction* aTransaction = NULL ) = 0;

        /**
        * Checks if new transaction can be sent.
        *
        * @return ETrue if there is existing transaction being processed.
        */
        //virtual TBool ReadyForTransaction() const = 0;
        
        /**
        * Access to data buffer.
        * Fill the channel data into this buffer.
        *
        * @return TUint8*
        */
        virtual TUint8* DataBuffer() const = 0;

        /**
        * Access to data item count.
        * How many channel data items are requested.
        *
        * @return TInt
        */
        virtual TInt DataCount() const = 0;
        
        /**
        * Access to current channel properties.
        * 
        * @return const RSensrvPropertyList&
        */
        virtual const RSensrvPropertyList& Properties() const = 0;
};

#endif // TCTRANSACTIONHANDLER_H

// End of File
