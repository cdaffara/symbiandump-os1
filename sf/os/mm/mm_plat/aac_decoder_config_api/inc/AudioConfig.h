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
* Description:  Audio configuration structure definition.
*
*/


#ifndef AUDIOCONFIG_H
#define AUDIOCONFIG_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
class TAudioConfig
    {
    public:
    // DATA TYPES
    enum TAudioObjectType
        {
        ENull    = 0,
        EAacMain = 1,
        EAacLc   = 2,
        EAacSsr  = 3,
        EAacLtp  = 4,
        ESbr     = 5
        };

    TAudioObjectType iAudioObjectType;
    };
    

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif      // AUDIOCONFIG_H
            
// End of File
