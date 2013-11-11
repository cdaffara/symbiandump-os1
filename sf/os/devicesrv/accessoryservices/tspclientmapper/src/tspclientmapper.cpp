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
* Description:  This interface is used to add mappings between the process
*                that listens the remote control and is assosiated with
*                another function.
*
*/


// INCLUDE FILES
#include <e32property.h>
#include <tspclientmapper.h>
#include "RemConDebug.h"
#include "tspclientmapperimpl.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTspClientMapper::CTspClientMapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTspClientMapper::CTspClientMapper()
    {
    }

    
// -----------------------------------------------------------------------------
// CTspClientMapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTspClientMapper* CTspClientMapper::NewL()
	{
	COM_TRACE_( "[RemConTspCustomIf] CRemConTspCustomIf::NewL()" );
	
	CTspClientMapper* impl = NewLC();
	CleanupStack::Pop( impl );
	
	COM_TRACE_( "[RemConTspCustomIf] CRemConTspCustomIf::NewL() - return" );
	
	return impl;
	}

// -----------------------------------------------------------------------------
// CTspClientMapper::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//	
EXPORT_C CTspClientMapper* CTspClientMapper::NewLC()
	{
	COM_TRACE_( "[RemConTspCustomIf] CRemConTspCustomIf::NewLC()" );
	
	CTspClientMapper* impl = CTspClientMapperImpl::NewL();
	CleanupStack::PushL( impl );
	COM_TRACE_( "[RemConTspCustomIf] CRemConTspCustomIf::NewLC() - return" );
	
	return impl;
	}

// End of file
