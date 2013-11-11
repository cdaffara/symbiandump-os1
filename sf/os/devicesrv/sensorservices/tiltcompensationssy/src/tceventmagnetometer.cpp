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
* Description:  TTCEventMagnetometer class implementation.
*
*/


#include "tceventmagnetometer.h"
#include "trace.h"

TTCEventMagnetometer::TTCEventMagnetometer(
    const TSensrvMagnetometerAxisData& aData ):
    TTCEvent( EEventIdHandleMagnetometer ),
    iData( aData )
    {
    FUNC_LOG;
    }

// End of File
