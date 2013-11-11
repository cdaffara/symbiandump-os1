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
* Description:  Haptics server plugin handler implementation
*
*/

#include "hwrmhapticstrace.h"
#include "hwrmhapticsplugintransactionlist.h"

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionList::CHWRMHapticsPluginTransactionList()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionList::~CHWRMHapticsPluginTransactionList()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::~CHWRMHapticsPluginTransactionList()" ) ) );
    
    // delete any existing transaction data
    while ( iTransactionData )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::~CHWRMHapticsPluginTransactionList - Deleting transaction %d" ), iTransactionData->TransactionId() ) );
        CHWRMHapticsPluginTransactionListItem* data = iTransactionData;
        iTransactionData = iTransactionData->NextItem();
        delete data;
        }
        
    // No need to delete iTransactionDataLast, obviously. Just NULL it to 
    iTransactionDataLast = NULL;    

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::~CHWRMHapticsPluginTransactionList - return " ) ) );
    }


// ---------------------------------------------------------------------------
// Find transaction from list.
// Uses linear search, but there should rarely be more than one concurrent
// transaction so it won't be a problem.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionListItem* 
CHWRMHapticsPluginTransactionList::FindTransaction( TUint8 aTransId, 
                                                    TBool aRemove )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::FindTransaction(0x%x, 0x%x)" ), aTransId, aRemove ) );
    
    CHWRMHapticsPluginTransactionListItem* data = iTransactionData;
    CHWRMHapticsPluginTransactionListItem* dataPrevious = NULL;

    TBool found( EFalse );
    
    while ( data && !found )
        {
        if ( data->TransactionId() == aTransId )
            {
            found = ETrue;
            
            // update pointers, if remove
            if ( aRemove )
                {                
                if ( iTransactionData == data )
                    {
                    iTransactionData = data->NextItem();
                    }
                else
                    {
                    // dataPrevious is always not NULL here, since first item
                    // in the list will use the other branch of this if.
                    dataPrevious->SetNextItem( data->NextItem() );
                    }
                    
                if ( iTransactionDataLast == data )
                    {
                    iTransactionDataLast = dataPrevious;
                    }

                iCount--;
                }
            }
        else
            {
            dataPrevious = data;
            data = data->NextItem();
            }
        }
    
    return data;
    }
    
// ---------------------------------------------------------------------------
// Adds transaction to list
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginTransactionList::AddTransaction( 
    CHWRMHapticsPluginTransactionListItem* aTransactionData )    
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::AddTransaction(0x%x)" ), aTransactionData ) );

    // if previous data exists, put it last in the list.
    if ( iTransactionData )
        {
        iTransactionDataLast->SetNextItem( aTransactionData );
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::AddTransaction - Added transaction to list" ) ) ); 
        }
    else
        {
        iTransactionData = aTransactionData;
        }
    
    iTransactionDataLast = aTransactionData;   
    iTransactionDataLast->SetNextItem( NULL );
    
    iCount++;

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::AddTransaction - return" ) ) );
    
    }
    
// ---------------------------------------------------------------------------
// Removes first item from transaction list and adjust pointers accordingly
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionListItem* 
CHWRMHapticsPluginTransactionList::RemoveFirstItem()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::RemoveFirstItem()" ) ) );

    CHWRMHapticsPluginTransactionListItem* retval = iTransactionData;

    iTransactionData = iTransactionData->NextItem();

    iCount--;

    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginTransactionList::RemoveFirstItem - return 0x%x" ), retval ) );
    
    return retval;
    }

//  End of File  
