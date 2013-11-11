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
* Description:  Panic inline function
*
*/


#ifndef __SUPLPSYPANIC_INL__
#define __SUPLPSYPANIC_INL__


//  INCLUDES
#include <e32std.h>

// FUNCTION PROTOTYPES
inline void Panic( TInt aPanicCode )
    {
    User::Panic( KSuplPanicGategory, aPanicCode );
    };

#endif // __SUPLPSYPANIC_INL__

// End of File


