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
* Description:  Plugin transaction list item implementation.
*
*/


#include "hwrmhapticstrace.h"
#include "hwrmhapticsplugintransactionlistitem.h"


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionListItem::~CHWRMHapticsPluginTransactionListItem()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsTransactionData::~CHWRMHapticsTransactionData()" ) ) );

    // iNextData not owned so not cleaned
    iNextData = NULL;

    COMPONENT_TRACE( ( _L( "CHWRMHapticsTransactionData::~CHWRMHapticsTransactionData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionListItem::CHWRMHapticsPluginTransactionListItem(
    TUint8 aTransId, TInt aCommandId )
    : iTransId( aTransId ),
      iCommandId( aCommandId ),
      iNextData( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Returns pointer to next item in list.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginTransactionListItem* 
CHWRMHapticsPluginTransactionListItem::NextItem()
    {
    return iNextData;
    }

// ---------------------------------------------------------------------------
// Sets next item of the list.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginTransactionListItem::SetNextItem( 
    CHWRMHapticsPluginTransactionListItem* aListItem )
    {
    iNextData = aListItem;
    }

// ---------------------------------------------------------------------------
// Sets ongoing transaction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginTransactionListItem::SetTransactionId( 
    TUint8 aTransactionId )
    {
    iTransId = aTransactionId;
    }

// ---------------------------------------------------------------------------
// Returns id of the ongoing transaction.
// ---------------------------------------------------------------------------
//
TUint8 CHWRMHapticsPluginTransactionListItem::TransactionId()
    {
    return iTransId;
    }

// ---------------------------------------------------------------------------
// Returns id of command being handled.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsPluginTransactionListItem::CommandId()
    {
    return iCommandId;
    }
// End of File
