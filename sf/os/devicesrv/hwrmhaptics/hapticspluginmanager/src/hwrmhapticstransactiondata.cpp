/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics transaction data implementation.
*
*/


#include "hwrmhapticstransactiondata.h"
#include "hwrmhapticstrace.h"


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsTransactionData::~CHWRMHapticsTransactionData()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsTransactionData::~CHWRMHapticsTransactionData()" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsTransactionData::~CHWRMHapticsTransactionData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsTransactionData::CHWRMHapticsTransactionData(
                                    CHWRMHapticsService* aCompletionCallback, 
                                    TUint8 aTransId, 
                                    TInt aCommandId,
                                    const TTime& aObsoletionTime )
        : CHWRMHapticsPluginTransactionListItem( aTransId, aCommandId ),
          iCompletionCallback( aCompletionCallback ),
          iObsoletionTime( aObsoletionTime )
    {
    }


// ---------------------------------------------------------------------------
// Returns time after which the transaction is obsolete.
// ---------------------------------------------------------------------------
//
TTime CHWRMHapticsTransactionData::TransactionObsoletionTime()
    {
    return iObsoletionTime;
    }
    
// ---------------------------------------------------------------------------
// Returns pointer to callback function.
// ---------------------------------------------------------------------------
//
CHWRMHapticsService* CHWRMHapticsTransactionData::CompletionCallback()
    {
    return iCompletionCallback;
    }
 
// End of File
