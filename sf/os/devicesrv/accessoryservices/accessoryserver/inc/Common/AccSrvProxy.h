/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for describing ASY states
*
*/


#ifndef ACCSRVPROXY_H
#define ACCSRVPROXY_H

// INCLUDES
#include <e32def.h>

// CONSTANTS
const TUint KMaxImplementationInfoDataLength = 128;

// MACROS

// DATA TYPES
enum TASYProxyASYState
    {
    EASYProxyASYReset,
    EASYProxyASYInitial,
    EASYProxyASYThreadCreating,
    EASYProxyASYSynchronousLoadComplete,
    EASYProxyASYConnectionReady,
    EASYProxyASYThreadKills
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // ACCSRVPROXY_H

// End of File
