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
* Description:  Plugin transaction list item header file.
*
*/


#ifndef C_HWRMHAPTICSPLUGINTRANSACTIONLISTITEM_H
#define C_HWRMHAPTICSPLUGINTRANSACTIONLISTITEM_H

#include <e32base.h>

/**
* This class contains ongoing plugin operation data base class.
*
* @since S60 5.1
*/
class CHWRMHapticsPluginTransactionListItem : public CBase
    {
public:
    /**
     * Convenience constructor
     */
    CHWRMHapticsPluginTransactionListItem( TUint8 aTransId, TInt aCommandId );

    /**
     * Virtual destructor.
     */
    virtual ~CHWRMHapticsPluginTransactionListItem();

    /**
     * Returns pointer to next item in list.
     *
     * @return Pointer to next item.
     */
    CHWRMHapticsPluginTransactionListItem* NextItem();

    /**
     * Sets next item of the list.
     *
     * @param aListItem Pointer to next item of the list.
     */
    void SetNextItem( CHWRMHapticsPluginTransactionListItem* aListItem );

    /**
     * Sets ongoing transaction.
     *
     * @param aListItem Id of ongoing transaction.
     */
    void SetTransactionId( TUint8 aTransactionId );

    /**
     * Returns id of the ongoing transaction.
     *
     * @return Id of ongoing transaction.
     */
    TUint8 TransactionId();

    /**
     * Returns id of command being handled.
     *
     * @return Id of command being handled.
     */
    TInt CommandId();

private:
 
    /**
     * Id of the ongoing transaction.
     */   
    TUint8 iTransId;

    /**
     * Id of the command being handled.
     */
    TInt   iCommandId;

    /**
     * Next data in the queue. Not owned.
     */
    CHWRMHapticsPluginTransactionListItem* iNextData;
    };
    
#endif  // C_HWRMHAPTICSPLUGINTRANSACTIONLISTITEM_H
            
// End of File
