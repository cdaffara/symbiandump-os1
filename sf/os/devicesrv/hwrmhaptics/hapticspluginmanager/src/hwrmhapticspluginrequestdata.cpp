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
* Description:  Haptic plugin request data implementation.
*
*/


#include "hwrmhapticspluginrequestdata.h"
#include "hwrmhapticstrace.h"


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginRequestData::~CHWRMHapticsPluginRequestData()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginRequestData::~CHWRMHapticsPluginRequestData()" ) ) );
    delete iReqData;
    iReqData = NULL;
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPluginRequestData::~CHWRMHapticsPluginRequestData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPluginRequestData::CHWRMHapticsPluginRequestData(
                                            const RMessage2& aRequestMessage,
                                            TUint8 aTransId, 
                                            TInt aCommandId, 
                                            TBool aSplitCommand )
            : CHWRMHapticsPluginTransactionListItem( aTransId, aCommandId ),
              iRequestMessage(aRequestMessage), iSplitCommand( aSplitCommand )
    {
    }

// ---------------------------------------------------------------------------
// Returns plugin request message.
// ---------------------------------------------------------------------------
//
RMessage2 CHWRMHapticsPluginRequestData::RequestMessage()
    {
    return iRequestMessage;
    }

// ---------------------------------------------------------------------------
// Returns information is command split to two adaptation calls.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsPluginRequestData::CommandSplit()
    {
    return iSplitCommand;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPluginRequestData::SetRequestData( HBufC8* aRequestData )
    {
    iReqData = aRequestData;
    }
// End of File
