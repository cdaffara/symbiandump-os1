/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the RDosSubSession class.
*
*/


#include <DosSvrServices.h>
#include "dosclientserver.h"
#include "dos_debug.h"

// ---------------------------------------------------------
// RDosSubSession::Close
// ---------------------------------------------------------
//  
EXPORT_C void RDosSubSession::Close()
{
    API_TRACE_( "[DOSSERVER] RDosSubSession::Close()" );

	CloseSubSession(EDosCloseSubSession);
}

// Deprecated
TPtr8* RDosSubSession::NewPtr(TAny* /*aDataPtr*/,TInt /*aDataSize*/)
{
	TPtr8* result = NULL;

	return result;
}
