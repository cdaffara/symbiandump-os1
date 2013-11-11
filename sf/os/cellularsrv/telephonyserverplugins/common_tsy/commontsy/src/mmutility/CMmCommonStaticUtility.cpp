// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CMmCommonStaticUtilityTraces.h"
#endif

#include <etelmm.h>
#include <etelmmerr.h>
#include <exterror.h>
#include <ctsy/serviceapi/gsmerror.h>
#include "CMmCommonStaticUtility.h"

#include <in_sock.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMmCommonStaticUtility::EpocErrorCode
// Stores the core error code to lower 16 bits and adds the 
// extended error code to higher 16 bits and returns the error code that can 
// be sent to ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCommonStaticUtility::EpocErrorCode( 
    TInt aCoreErrorCode, // core error
    TInt aExtendedErrorCode ) // extended error
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCOMMONSTATICUTILITY_EPOCERRORCODE_1, "TSY: CMmStaticUtility::EpocErrorCode, Error mapping done, \Core error: %d, Extended error: %d", aCoreErrorCode, aExtendedErrorCode );

    TInt errorCode = ( aCoreErrorCode & 0x0000FFFF ) |
                     ( aExtendedErrorCode << 16 );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCOMMONSTATICUTILITY_EPOCERRORCODE_2, "TSY: CMmStaticUtility::EpocErrorCode. It was mapped \to the following Symbian OS error: %d", errorCode );

    return errorCode;
    }

// ---------------------------------------------------------------------------
// CMmCommonStaticUtility::EpocErrorCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCommonStaticUtility::ExtendedErrorCode( 
    TInt EpocErrorCode )
    {
    TInt temp = (EpocErrorCode & 0xFFFF0000) >> 16;
    if ( temp != KErrNone )
        {
        //add negative sign
        temp = temp | 0xFFFF0000;
        }
    return temp;
    }

//  End of File 
