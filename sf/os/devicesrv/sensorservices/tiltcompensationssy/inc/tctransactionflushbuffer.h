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
* Description:  TTCTransactionFlushBuffer class declaration.
*
*/



#ifndef TCTRANSACTIONFLUSHBUFFER_H
#define TCTRANSACTIONFLUSHBUFFER_H

// INCLUDES
#include "tctransaction.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Flush buffer transaction. Flush buffer transaction
*  contains the amount of data objects written in
*  the current data buffer.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCTransactionFlushBuffer ): public TTCTransaction
    {
    public:
    
        /**
        * Default C++ constructor.
        */
        TTCTransactionFlushBuffer( TInt aDataCount );
        
    public:
    
        /** Written data count */
        TInt iDataCount;
    };

#endif // TCTRANSACTIONFLUSHBUFFER_H

// End of File
