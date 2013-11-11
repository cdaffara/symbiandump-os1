// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CMmWimTsywithdispatcher.cpp.cpp
//

#ifdef USING_CTSY_DISPATCHER

/**
 * This file contains additional function definitions for the CMmWimTsy class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

#include "CMmWimTsy.h"

// ---------------------------------------------------------------------------
// CMmWimTsy::CompleteSendAPDUReq
// Completes a Send APDU request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmWimTsy::CompleteSendAPDUReq(
    CMmDataPackage* aDataPackage, 
    TInt aError )
    {
	// Complete request
    const TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(EWIMRequestTypeSendAPDUReq);

    if( reqHandle )
		{
		if ( KErrNone == aError )
	        {
	        //unpack the data
	        TDesC8* info = NULL;
	        TDesC8* data = NULL;
	
	        aDataPackage->UnPackData( &info, &data );
		    // request was successful, copy data to client side
		    if ( ( !iRetApduInfo && info ) || (!iRetApduData && data))
		    	{
		    	aError = KErrBadDescriptor;
		    	}
		    
		    if ( iRetApduInfo && info )
		        {
		        if (iRetApduInfo->Length() < info->Length())
		        	{
		        	aError = KErrOverflow;
		        	}
		        else
		        	{
		        	iRetApduInfo->Copy(*info);
		        	}
		        }
		
		    if ( iRetApduData && data )
		        {
		        if (iRetApduData->Length() < data->Length())
		        	{
		        	aError = KErrOverflow;
		        	}
		        else
		        	{
		        	iRetApduData->Copy(*data);
		        	}
		        }
		    }
		iMmCustomTsy->ReqCompleted( reqHandle, aError );
		}
    }

#endif //USING_CTSY_DISPATCHER
