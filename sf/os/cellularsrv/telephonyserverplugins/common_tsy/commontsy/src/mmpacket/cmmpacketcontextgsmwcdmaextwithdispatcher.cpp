// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef USING_CTSY_DISPATCHER

/**
 * This file contains additional function definitions for the CMmPacketContextGsmWcdmaExt class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketcontextgsmwcdmaextwithdispatcherTraces.h"
#endif

#include "cmmpacketservicegsmwcdmaext.h"

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteInitialiseContextL
// This function starts context initialisation.
// @param aChannelId The ChannelId allocated by the LTSY.
// @param aResult Error code from LTSY
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteInitialiseContext(const TDesC& aChannelId,  
        													const TInt aResult ) // Result
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEINITIALISECONTEXT1_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteInitialiseContext. Status: aChannelId=%S, aResult=%d",aChannelId, aResult);

      if ( KErrNone == aResult )
        {
        iMmPacketContextTsy->CompleteNotifyStatusChange( 
            RPacketContext::EStatusInactive );

        if(iDataChannel != NULL)
        	{      	
        	__ASSERT_DEBUG(aChannelId.Length() <= KMaxName, User::Invariant());
        	iDataChannel->iChannelId.Copy(aChannelId.Left(KMaxName));
        	iDataChannel->iPort.Copy(iMmPacketContextTsy->ContextName());
        	iDataChannel = NULL;
        	}

        iIsActivateAllowed = ETrue;
        // Set Dynamic Caps
        iMmPacketServiceTsy->ActivePacketServiceExtension()->
            SetDynamicCapsFlag( RPacketService::KCapsActivate, ETrue );
        }
    else
        {
        iIsInitialiseAllowed = ETrue;
        }

      iMmPacketContextTsy->CompleteInitialiseContext( aResult );

    } // CMmPacketContextGsmWcdmaExt::CompleteInitialiseContext

#endif //USING_CTSY_DISPATCHER
