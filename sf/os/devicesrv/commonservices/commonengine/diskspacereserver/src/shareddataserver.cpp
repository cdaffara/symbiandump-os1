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
*     Server class.
*
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include "shareddataserver.h"


/**
* To maintain binary compatibility.
*/
CSharedDataServer::~CSharedDataServer()
    {    
    }

/**
* To maintain binary compatibility.
*/
EXPORT_C TInt CSharedDataServer::ThreadStart( CSharedDataServer::TSignal& /* aSignal */ )
    {
    return( KErrNone );
    }

// End of File
