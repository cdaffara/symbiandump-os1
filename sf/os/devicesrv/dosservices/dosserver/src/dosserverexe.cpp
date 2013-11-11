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
*    Executable module to start the process and load the DosSrv.dll
*
*/


#include "dosserver.h"

//
// ---------------------------------------------------------
// SetSignal
// ---------------------------------------------------------
//  

TInt SetSignal( CDosServer::TSignal& aSignal )
{
    TBuf<50> buf;
	User::CommandLine( buf );
    return aSignal.Set( buf );
}

//
// ---------------------------------------------------------
// E32Main
// ---------------------------------------------------------
//  

GLDEF_C TInt E32Main()
{
    CDosServer::TSignal signal;
    TInt err = SetSignal( signal );

    if ( !err )
        {
        err = CDosServer::ThreadStart( signal );
        }

    return err;
}
