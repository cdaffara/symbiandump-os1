/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SUPL PSY panic definition
*
*/


#ifndef __SUPLPSYPANIC_H__
#define __SUPLPSYPANIC_H__


//  INCLUDES
#include <e32std.h>

// CONSTANTS
_LIT( KSuplPanicGategory, "SUPL PSY" );


// DATA TYPES
enum TSuplPsyPanicReason
    {
    EPanicPositionRequestOngoing,       //There is already position request ongoing
    EPanicPositionerAlreadyRegistered,  //Positioner has already registered
    EPanicUnknownPositioningClass
    };

// FUNCTION PROTOTYPES
inline void Panic( TInt aPanicCode );

#include "suplpsypanic.inl"

#endif // __SUPLPSYPANIC_H__

// End of File


