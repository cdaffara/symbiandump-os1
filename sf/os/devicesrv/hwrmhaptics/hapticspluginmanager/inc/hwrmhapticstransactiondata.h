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
* Description:  Haptics transaction data header file.
*
*/


#ifndef C_HWRMHAPTICSTRANSACTIONDATA_H
#define C_HWRMHAPTICSTRANSACTIONDATA_H

#include "hwrmhapticsservice.h"
#include "hwrmhapticsplugintransactionlist.h"

/**
 * Contains ongoing plugin operation data.
 */
class CHWRMHapticsTransactionData : 
    public CHWRMHapticsPluginTransactionListItem
    {
public:

    /**
     * Constructor.
     *
     * @param aCompletionCallback Service callback to be stored.
     * @param aTransId Transaction ID to be stored in base class.
     * @param aCommandId CommandID to be stored in base class.
     * @param aObsoletionTime Obsoletion time.
     */
    CHWRMHapticsTransactionData( CHWRMHapticsService* aCompletionCallback, 
                                 TUint8 aTransId, 
                                 TInt aCommandId,
                                 const TTime& aObsoletionTime );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsTransactionData();

    /**
     * Method to get time after which the transaction is obsolete.
     *
     * @return Time after which the transaction is obsolete.
     */
    TTime TransactionObsoletionTime();

    /**
     * Method to get pointer to callback function.
     *
     * @return Pointer to completion callback.
     */
    CHWRMHapticsService* CompletionCallback();
    
private:

    /**
     * Pointer to currently using service. Not owned.
     */
    CHWRMHapticsService* iCompletionCallback;

    /**
     * Time after which the transaction is obsolete.
     */
    TTime iObsoletionTime;
    };
    

#endif  // C_HWRMHAPTICSTRANSACTIONDATA_H
            
// End of File
