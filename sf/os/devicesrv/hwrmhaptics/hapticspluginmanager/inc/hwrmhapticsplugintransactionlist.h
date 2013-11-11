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
* Description:  Plugin transaction list header file.
*
*/


#ifndef C_HWRMHAPTICSPLUGINTRANSACTIONLIST_H
#define C_HWRMHAPTICSPLUGINTRANSACTIONLIST_H

#include <e32base.h>
#include "hwrmhapticsplugintransactionlistitem.h"


/**
 *  Haptics server side plugin transaction list handler
 *
 *  @lib hwrmhapticspluginmanager.lib
 *  @since S60 5.1
 */
class CHWRMHapticsPluginTransactionList : public CBase
    {
public:
    
    /**
     * Destructor.
     * Deletes all data instances still in the list, even though normally 
     * instances are deleted by using class. This is because after
     * list destruction there is no way to find the instances to delete any more.
     */
    virtual ~CHWRMHapticsPluginTransactionList();
    
public:

    /**
     * Finds a transaction from transaction list.
     *
     * @param aTransId   Unique transcation identifier
     * @param aRemove    If ETrue, transaction will be removed from the list.
     * @return Transaction data found or NULL if not found.
     */
    CHWRMHapticsPluginTransactionListItem* 
        FindTransaction( TUint8 aTransId, TBool aRemove);
        
    /**
     * Add transaction to transaction list
     *
     * @param aTransactionData Transaction data to add to list
     */
    void AddTransaction( 
        CHWRMHapticsPluginTransactionListItem* aTransactionData );
        
    /**
     * Removes first item from list. 
     * 
     * @return Pointer to removed item or NULL if empty
     */
    CHWRMHapticsPluginTransactionListItem* RemoveFirstItem();
        
    /**
     * Get first transaction data item in the list
     * That item can be then used to iterate further.
     *
     * @return First item in the list or NULL if empty.
     */
    inline CHWRMHapticsPluginTransactionListItem* FirstItem();

    /**
     * Get last transaction data item in the list
     *
     * @return Last item in the list or NULL if empty.
     */
    inline CHWRMHapticsPluginTransactionListItem* LastItem();
        
    /**
     * Get count of transactions in list
     */
    inline TInt Count() const;

public: 
    
    /**
     * C++ default constructor.
     */
    CHWRMHapticsPluginTransactionList();
    
private:   // data
    
    /**
     * Pointer to the next data in list.
     * User of the list is responsible for creation and deletion
     * of data. All existing items will be deleted automatically
     * in list destructor.
     */
    CHWRMHapticsPluginTransactionListItem* iTransactionData;
    
    /**
     * Last transaction data in list
     */
    CHWRMHapticsPluginTransactionListItem* iTransactionDataLast;
                                                               
    /**
     * Number of transactions in list
     */
    TInt iCount;
                
    };
    

#include "hwrmhapticsplugintransactionlist.inl"

#endif  // C_HWRMHAPTICSPLUGINTRANSACTIONLIST_H
            
// End of File
