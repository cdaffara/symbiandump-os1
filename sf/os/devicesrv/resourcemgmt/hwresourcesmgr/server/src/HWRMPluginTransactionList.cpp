// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include "HWRMtrace.h"
#include "HWRMPluginTransactionList.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::CHWRMPluginTransactionList
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMPluginTransactionList::CHWRMPluginTransactionList()
    : iTransactionData(NULL),
    iTransactionDataLast(NULL)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::CHWRMPluginTransactionList()" ));
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::CHWRMPluginTransactionList - return " ));
    }

// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CHWRMPluginTransactionList* CHWRMPluginTransactionList::NewL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::NewL()" ));
    
    CHWRMPluginTransactionList* self = new( ELeave ) CHWRMPluginTransactionList();

    // No ConstructL as nothing to do there.
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginTransactionList::NewL - return 0x%x" ), self );

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPluginTransactionList::~CHWRMPluginTransactionList()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::~CHWRMPluginTransactionList()" ) );
    
    // delete any existing transaction data
    while ( iTransactionData )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginTransactionList::~CHWRMPluginTransactionList - Deleting transaction %d" ), iTransactionData->iTransId );
        THWRMPluginTransactionListItem* data = iTransactionData;
        iTransactionData = iTransactionData->iNextData;
        delete data;
        }
        
    // No need to delete iTransactionDataLast, obviously. Just NULL it to 
    iTransactionDataLast = NULL;    

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::~CHWRMPluginTransactionList - return " ));
    }


// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::FindTransaction
// Find transaction from list.
// Uses linear search, but there should rarely be more than one concurrent
// transaction so it won't be a problem.
// -----------------------------------------------------------------------------
//
THWRMPluginTransactionListItem* CHWRMPluginTransactionList::FindTransaction( TUint8 aTransId, TBool aRemove )
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPluginTransactionList::FindTransaction(0x%x, 0x%x)" ), aTransId, aRemove );
    
#ifdef COMPONENT_TRACE_FLAG
    // check the transaction list contents
    THWRMPluginTransactionListItem* checkData = iTransactionData;
    TInt count(0);
    if ( checkData )
        {
        while ( checkData )
            {        
            COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPluginTransactionList::FindTransaction - Data %d in list: 0x%x" ), count, checkData->iTransId );
            count++;
            checkData = checkData->iNextData;
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::FindTransaction - List is empty" ) );        
        }
#endif

    THWRMPluginTransactionListItem* data = iTransactionData;
    // comment to ignore Coverity false positive. See more detailed comment below.
    // coverity[assign_zero]
    THWRMPluginTransactionListItem* dataPrevious = NULL;

    TBool found(EFalse);
    
    while ( data && !found )
        {
        if ( data->iTransId == aTransId )
            {
            found = ETrue;
            
            // update pointers, if remove
            if ( aRemove )
                {                
                if ( iTransactionData == data )
                    {
                    iTransactionData = data->iNextData;
                    }
                else
                    {
                    // dataPrevious is always not NULL here.
                    // If the while loop IS on its first iteration iTransactionData == data 
                    // is true since data = iTransactionData is how data is initiated. If this is 
                    // NOT the first iteration dataPrevious = data in the else claues below will 
                    // have made sure dataPrevious != NULL. Comments to instruct Coverity to ignore
                    // this false positive are justified.
                    //
                    //coverity[var_deref_op]
                    dataPrevious->iNextData = data->iNextData;
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
            data = data->iNextData;
            }
        }
#ifdef COMPONENT_TRACE_FLAG
    if ( data )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginTransactionList::FindTransaction - return transaction 0x%x" ), data->iTransId );
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::FindTransaction - return NULL" ) );
        }
#endif
    
    return data;
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::AddTransaction
// Adds transaction to list
// -----------------------------------------------------------------------------
//
void CHWRMPluginTransactionList::AddTransaction( THWRMPluginTransactionListItem* aTransactionData )    
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginTransactionList::AddTransaction(0x%x)" ), aTransactionData );

    // if previous data exists, put it last in the list.
    if ( iTransactionData )
        {
        iTransactionDataLast->iNextData = aTransactionData;
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::AddTransaction - Added transaction to list" ) ); 
        }
    else
        {
        iTransactionData = aTransactionData;
        }
    
    iTransactionDataLast = aTransactionData;   
    iTransactionDataLast->iNextData = NULL;       
    
    iCount++;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::AddTransaction - return" ) );
    
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::RemoveFirstItem
// Removes first item from transaction list and adjust pointers accordingly
// -----------------------------------------------------------------------------
//
THWRMPluginTransactionListItem* CHWRMPluginTransactionList::RemoveFirstItem()    
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginTransactionList::RemoveFirstItem()" ) );

    THWRMPluginTransactionListItem* retval = iTransactionData;

    iTransactionData = iTransactionData->iNextData;

    iCount--;

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginTransactionList::RemoveFirstItem - return 0x%x" ), retval );
    
    return retval;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
