/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*      Manages session connections to Shared Data server
*  
*
*/


// INCLUDE FILES

#include <e32std.h>
#include "dosclishareddata.h"
#include "dosclientserver.h"
#include "dos_debug.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS ==========================================

// ---------------------------------------------------------
// RDosSharedData::Open
// ---------------------------------------------------------
//  
EXPORT_C TInt RDosSharedData::Open(RDosServer& aServer)
{   
	 COM_TRACE_( "RDosSharedData::Open(RDosServer& aServer)" );
     return CreateSubSession(aServer,EDosCreateSharedDataSubSession, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
}


// -----------------------------------------------------------------------------
// RDosSharedData::RequestFreeDiskSpace()
// -----------------------------------------------------------------------------

EXPORT_C void RDosSharedData::RequestFreeDiskSpace( const TInt aAmount) const
    {     
        COM_TRACE_( "RDosSharedData::RequestFreeDiskSpace" );
        TInt iRet = SendReceive(ERequestFreeDiskSpace, TIpcArgs(aAmount, TIpcArgs::ENothing, KAutoComplete));
    }

// -----------------------------------------------------------------------------
// RDosSharedData::RequestFreeDiskSpaceCancel()
// -----------------------------------------------------------------------------

EXPORT_C void RDosSharedData::RequestFreeDiskSpaceCancel() const
    {
		COM_TRACE_( "RDosSharedData::RequestFreeDiskSpaceCancel(), calling SendReceive" );
		SendReceive(ERequestFreeDiskSpaceCancel, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
    }

EXPORT_C void RDosSharedData::Close()
	{
		COM_TRACE_( "RDosSharedData::Close(), calling RequestFreeDiskSpaceCancel()" );
		RequestFreeDiskSpaceCancel();
		COM_TRACE_( "RDosSharedData::Close(), calling CloseSubSession(EDosCloseSubSession)" );
		CloseSubSession(EDosCloseSubSession);
	}

//  End of File  
